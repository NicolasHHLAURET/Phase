/*
 * RPVMaterialWriter.cpp
 *
 *  Created on: 9 nov. 2026
 *      Author: lauretn
 */

#include "materials/RPVMaterialWriter.hpp"
#include "materials/LuxCoreMaterialOutput.hpp"

#include "phaseExtern/MarmitComputation.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/LambertianDatabaseParameters.hpp"
#include "database/model/RpvDatabaseParameters.hpp"
#include "database/model/TransmittanceDatabaseParameters.hpp"

namespace phase {

RPVMaterialWriter::RPVMaterialWriter(	const FluorescenceService & fluorescenceService,
												const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}
bool RPVMaterialWriter::prepareDatabase(const phsdata::RPVMulti & r,
										 const PhaseGeneralParametersDTO & genDTO,
										 OpticalPropertyParameters & opticalProperty) const {
	if (!r.databaseName.is_initialized() || !r.modelName.is_initialized()) {
		// No RPV database/model defined: nothing to process.
		return false;
	}

	auto rpvDatabaseParameters = std::make_shared<RpvDatabaseParameters>(
		*r.databaseName, DartEnv::getDatabasePath(*r.databaseName), *r.modelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(rpvDatabaseParameters);

	auto transmittanceDatabaseParameters = std::make_shared<TransmittanceDatabaseParameters>(
		r.transmittanceDatabaseName, DartEnv::getDatabasePath(r.transmittanceDatabaseName),
		r.transmittanceModelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);
	return true;
}

void RPVMaterialWriter::applyMultiplicativeFactors(const phsdata::RPVMulti & r,
                                                   OpticalPropertyParameters & opticalProperty,
                                                   std::vector<PropertyRpv> & properties) const {
    auto rpvDatabaseParameters = std::static_pointer_cast<RpvDatabaseParameters>(
        opticalProperty.getDatabaseParameters(RPV_DATABASE));
    auto transmittanceParameters = std::static_pointer_cast<TransmittanceDatabaseParameters>(
        opticalProperty.getDatabaseParameters(TRANSMITTANCE_DATABASE));

    for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
        properties[sbi].reflectance = 0.01 * rpvDatabaseParameters->getReflectanceRho0ForBand(sbi);

        // Database oddity: K, G and H are set between 0 and 100. They're actually neither fraction or percentage in the RPV model
        properties[sbi].rpvParameterK = 0.01 * rpvDatabaseParameters->getRpvKForBand(sbi);
        properties[sbi].rpvParameterG = 0.01 * rpvDatabaseParameters->getRpvGForBand(sbi);
        properties[sbi].rpvParameterH = 0.01 * rpvDatabaseParameters->getRpvHForBand(sbi);

        properties[sbi].directTransmittance = 0.01 * transmittanceParameters->getDirectTransmittanceForBand(sbi);
        properties[sbi].diffuseTransmittance = 0.01 * transmittanceParameters->getDiffuseTransmittanceForBand(sbi);

        // Band-independent RPV-specific parameter (formerly read via PhaseDAO::readCoeffDiffParametersForRPVThetaMax at every band).
        properties[sbi].theta_max = r.theta_max;
    }

    if (r.nodeMultiplicativeFactor.is_initialized()) {
        // Same XML semantics as the other materials: `factors` always contains exactly
        // one RPVMultiplicativeFactorForLUT per spectral band.
        const phsdata::MultiplicativeFactorList & mfl = *r.nodeMultiplicativeFactor;
        for (unsigned int sbi = 0; sbi < properties.size() && sbi < mfl.factors.size(); ++sbi) {
            auto rmf = std::static_pointer_cast<const phsdata::RPVMultiplicativeFactor>(mfl.factors[sbi]);
            properties[sbi].reflectance *= rmf->rhoZero_RPVMultiplicativeFactor;
            properties[sbi].rpvParameterK *= rmf->k_RPVMultiplicativeFactor;
            properties[sbi].rpvParameterG *= rmf->g_RPVMultiplicativeFactor;
            properties[sbi].rpvParameterH *= rmf->h_RPVMultiplicativeFactor;
            properties[sbi].directTransmittance *= rmf->directTransmittanceFactor;
            properties[sbi].diffuseTransmittance *= rmf->diffuseTransmittanceFactor;
        }
    }

    for (unsigned int sbi = 0; sbi < properties.size(); ++sbi)
        checkAlbedo(r.functionName, properties[sbi]);
}

void RPVMaterialWriter::checkAlbedo(const std::string & modelName,
                                    PropertyRpv & opticalProperties) const {
    // Specular handling has been moved to the dedicated Specular material; the specular
    // branches of the legacy testRPVAlbedo are dead code here.
    if (opticalProperties.reflectance + opticalProperties.diffuseTransmittance > 1.000001) {
        const double sum = opticalProperties.reflectance + opticalProperties.diffuseTransmittance;
        cerr << " RPV Warning : Rho0 + DiffuseTransmittance = " << sum
             << " > 1 !!! (RPV phase function : " << modelName << ") -> Scaled back to 1 " << endl;
        opticalProperties.reflectance /= sum;
        opticalProperties.diffuseTransmittance /= sum;
    }
    if (opticalProperties.reflectance < 0) {
        cerr << " RPV Warning : Rho0 < 0 !!! (RPV phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.reflectance = 0;
    }
    if (opticalProperties.diffuseTransmittance < 0) {
        cerr << " Warning : Diffuse Transmittance < 0 !!! (RPV phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.diffuseTransmittance = 0;
    }
    if (opticalProperties.directTransmittance < 0) {
        cerr << " Warning : Direct Transmittance < 0 !!! (RPV phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.directTransmittance = 0;
    }
}

bool RPVMaterialWriter::treat(const phsdata::SurfaceFunction & sf,
                              PhaseGeneralParametersDTO & genDTO,
                              const std::vector<bool> & useSpectralIrradianceForReflectance,
                              unsigned int materialIndex,
                              PhaseKeyMappingGenerator & pkmg,
                              std::ofstream & outCoeffSpecProperties) const {
    const phsdata::RPVMulti & r = dynamic_cast<const phsdata::RPVMulti &>(sf);
    OpticalPropertyParameters opticalProperty(RPV_PROPERTY, r.indexFunction, r.functionName);
    if (!prepareDatabase(r, genDTO, opticalProperty))
        return false;

    for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
        PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

    pkmg.setTypeFonctionDePhase(globalReferences::RPV_PHASE);

    std::vector<PropertyRpv> properties(genDTO.numberOfSpectralBands);
    applyMultiplicativeFactors(r, opticalProperty, properties);

    writeLuxCoreMaterial(r, genDTO, properties, materialIndex);
    return true;
}

void RPVMaterialWriter::writeLuxCoreMaterial(const phsdata::RPVMulti & r,
                                             PhaseGeneralParametersDTO & genDTO,
                                             const std::vector<PropertyRpv> & rpvProperties,
                                             unsigned int materialID) const {
    const unsigned int nbBands = genDTO.numberOfSpectralBands;
    const std::string keyRoot = "scene.materials.RPV" + std::to_string(r.indexFunction) + ".";

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "RPV");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialID));
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opIndex", std::to_string(r.indexFunction));

    auto r0Value = [&rpvProperties](unsigned int sbi) { return rpvProperties[sbi].reflectance; };  // named Rho0 in RAMI definition
    auto kValue  = [&rpvProperties](unsigned int sbi) { return rpvProperties[sbi].rpvParameterK; };
    auto gValue  = [&rpvProperties](unsigned int sbi) { return rpvProperties[sbi].rpvParameterG; };
    // Named H in DART-FT while it is named Rc in the RAMI definition.
    auto rcValue = [&rpvProperties](unsigned int sbi) { return rpvProperties[sbi].rpvParameterH; };

    // Transmittance (kt): values below 1e-8 are clamped to 0 to avoid numerical noise.
    // Rho0 == 1 would cause a division by zero; kt defaults to 0 in that case.
    auto ktValue = [&rpvProperties](unsigned int sbi) {
        const double dt = rpvProperties[sbi].diffuseTransmittance;
        const double r0 = rpvProperties[sbi].reflectance;
        if (dt <= 1e-8 || r0 == 1.)
            return 0.;
        return dt / (1. - r0);
    };

    // theta_max is band-independent but written once per band, like the legacy code.
    auto thetaMaxValue = [&rpvProperties](unsigned int sbi) { return rpvProperties[sbi].theta_max; };

    std::ostringstream valueR0, valueK, valueG, valueRc, valueKt, valueThetaMax, emissivity;
    valueR0 << setprecision(15);
    valueR0 << r0Value(0);
    luxcore::appendPerBandDuplicated(valueR0, nbBands, r0Value);

    valueK << setprecision(15);
    valueK << kValue(0);
    luxcore::appendPerBandDuplicated(valueK, nbBands, kValue);

    valueG << setprecision(15);
    valueG << gValue(0);
    luxcore::appendPerBandDuplicated(valueG, nbBands, gValue);

    valueRc << setprecision(15);
    valueRc << rcValue(0);
    luxcore::appendPerBandDuplicated(valueRc, nbBands, rcValue);

    valueKt << setprecision(15);
    valueKt << ktValue(0);
    luxcore::appendPerBandDuplicated(valueKt, nbBands, ktValue);

    valueThetaMax << setprecision(15);
    valueThetaMax << thetaMaxValue(0);
    luxcore::appendPerBandDuplicated(valueThetaMax, nbBands, thetaMaxValue);

    // Emissivity is forced to 1 (the real emissivity is computed in DART); it is zero for the extra simulated bands (Q,U,V and PSI,PSII).
    emissivity << setprecision(15);
    emissivity << 1.;
    luxcore::appendPerBandZeroPadded(emissivity, nbBands, [](unsigned int) { return 1.; });

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "r0", valueR0.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "k", valueK.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "g", valueG.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "rc", valueRc.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kt", valueKt.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "theta_max", valueThetaMax.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "emission.gain", emissivity.str());

    if (r.nodeMultiplicativeFactor.is_initialized()) {
        const phsdata::MultiplicativeFactorList & mfl = *r.nodeMultiplicativeFactor;
        bool useOpticalFactorMatrix = mfl.useSameOpticalFactorMatrixForAllBands && mfl.bandCommonOpticalFactors.is_initialized();
        if (!useOpticalFactorMatrix) {
            const unsigned int nbFactors = std::min<unsigned int>(nbBands, mfl.factors.size());
            for (unsigned int sbi = 0; sbi < nbFactors && !useOpticalFactorMatrix; sbi++)
                useOpticalFactorMatrix = mfl.factors[sbi]->useOpticalFactorMatrix && mfl.factors[sbi]->opticalFactorMatrix.is_initialized();
        }

        if (useOpticalFactorMatrix) {
            // "rpv" prefix (the legacy code reused the Lambertian "l" prefix, risking name collisions with Lambertian-generated optical factors).
            const std::string textureName = "rpv" + std::to_string(r.indexFunction) + "OF";
            const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
            if (outputDir.empty())
                throw std::runtime_error("Output directory path is empty");
            if (_opticalFactorsWriter.writeTexture(genDTO, textureName, mfl, outputDir.string()))
                genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opticalFactors", textureName);
        }
    }
}
}
