/*
 * Hapke2012MaterialWriter.cpp
 *
 *  Created on: 5 nov. 2026
 *      Author: lauretn
 */

#include "materials/Hapke2012MaterialWriter.hpp"
#include "materials/LuxCoreMaterialOutput.hpp"

#include "phaseExtern/MarmitComputation.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/LambertianDatabaseParameters.hpp"
#include "database/model/HapkeDatabaseParameters.hpp"
#include "database/model/TransmittanceDatabaseParameters.hpp"

namespace phase {

Hapke2012MaterialWriter::Hapke2012MaterialWriter(	const FluorescenceService & fluorescenceService,
												const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}
bool Hapke2012MaterialWriter::treat(const phsdata::SurfaceFunction & sf,
									 PhaseGeneralParametersDTO & genDTO,
									 const std::vector<bool> & useSpectralIrradianceForReflectance,
									 unsigned int materialIndex,
									 PhaseKeyMappingGenerator & pkmg,
									 std::ofstream & outCoeffSpecProperties) const {
	const phsdata::Hapke2012 & h = dynamic_cast<const phsdata::Hapke2012 &>(sf);
	OpticalPropertyParameters opticalProperty(HAPKE2_PROPERTY, h.indexFunction, h.functionName);
	if (!prepareDatabase(h, genDTO, opticalProperty))
		return false;

	for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
		PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE2_SPEC);

	std::vector<PropertyHapke2> properties(genDTO.numberOfSpectralBands);
	applyMultiplicativeFactors(h, opticalProperty, properties);

	writeLuxCoreMaterial(h, genDTO, properties, materialIndex);
	return true;
}

bool Hapke2012MaterialWriter::prepareDatabase(const phsdata::Hapke2012 & h,
											  const PhaseGeneralParametersDTO & genDTO,
											  OpticalPropertyParameters & opticalProperty) const {
	if (!h.databaseName.is_initialized() || !h.modelName.is_initialized()) {
		// No Hapke database/model defined: nothing to process.
		return false;
	}

	auto hapkeDatabaseParameters = std::make_shared<Hapke2DatabaseParameters>(
		*h.databaseName, DartEnv::getDatabasePath(*h.databaseName), *h.modelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(hapkeDatabaseParameters);

	auto transmittanceDatabaseParameters = std::make_shared<TransmittanceDatabaseParameters>(
		h.transmittanceDatabaseName, DartEnv::getDatabasePath(h.transmittanceDatabaseName),
		h.transmittanceModelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);
	return true;
}

void Hapke2012MaterialWriter::applyMultiplicativeFactors(const phsdata::Hapke2012 & h,
                                                         OpticalPropertyParameters & opticalProperty,
                                                         std::vector<PropertyHapke2> & properties) const {
    auto hapkeDatabaseParameters = std::static_pointer_cast<Hapke2DatabaseParameters>(
        opticalProperty.getDatabaseParameters(HAPKE2_DATABASE));
    auto transmittanceParameters = std::static_pointer_cast<TransmittanceDatabaseParameters>(
        opticalProperty.getDatabaseParameters(TRANSMITTANCE_DATABASE));

    for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
        properties[sbi].directTransmittance = 0.01 * transmittanceParameters->getDirectTransmittanceForBand(sbi);
        properties[sbi].diffuseTransmittance = 0.01 * transmittanceParameters->getDiffuseTransmittanceForBand(sbi);

        properties[sbi].hapkeParameterW  = hapkeDatabaseParameters->getHapke2WForBand(sbi);
        properties[sbi].hapkeParameterC  = hapkeDatabaseParameters->getHapke2CForBand(sbi);
        properties[sbi].hapkeParameterB  = hapkeDatabaseParameters->getHapke2BForBand(sbi);
        properties[sbi].hapkeParameterH  = hapkeDatabaseParameters->getHapke2HForBand(sbi);
        properties[sbi].hapkeParameterB0 = hapkeDatabaseParameters->getHapke2B0ForBand(sbi);
        properties[sbi].hapkeParameterTh = M_PI / 180. * hapkeDatabaseParameters->getHapke2ThForBand(sbi);  // deg -> rad
    }

    if (h.nodeMultiplicativeFactor.is_initialized()) {
        // Same XML semantics as Lambertian/Hapke1981: `factors` always contains exactly
        // one Hapke2MultiplicativeFactorForLUT per spectral band.
        const phsdata::MultiplicativeFactorList & mfl = *h.nodeMultiplicativeFactor;
        for (unsigned int sbi = 0; sbi < properties.size() && sbi < mfl.factors.size(); ++sbi) {
            auto hmf = std::static_pointer_cast<const phsdata::Hapke2012MultiplicativeFactor>(mfl.factors[sbi]);
            properties[sbi].directTransmittance *= hmf->directTransmittanceFactor;
            properties[sbi].diffuseTransmittance *= hmf->diffuseTransmittanceFactor;
            properties[sbi].hapkeParameterW  *= hmf->hapkeParameterW;
            properties[sbi].hapkeParameterC  *= hmf->hapkeParameterC;
            properties[sbi].hapkeParameterB  *= hmf->hapkeParameterB;
            properties[sbi].hapkeParameterH  *= hmf->hapkeParameterH;
            properties[sbi].hapkeParameterB0 *= hmf->hapkeParameterB0;
            properties[sbi].hapkeParameterTh *= hmf->hapkeParameterTh;
        }
    }

    for (unsigned int sbi = 0; sbi < properties.size(); ++sbi)
        checkAlbedo(h.functionName, properties[sbi]);
}

void Hapke2012MaterialWriter::checkAlbedo(const std::string & modelName,
                                          PropertyHapke2 & opticalProperties) const {
    if (opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance > 1.000001) {
        const double sum = opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance;
        cerr << " HAPKE2012 Warning : hapke W + DiffuseTransmittance = " << sum
             << " > 1 !!! (hapke2 phase function : " << modelName << ") -> Scaled back to 1 " << endl;
        opticalProperties.hapkeParameterW /= sum;
        opticalProperties.diffuseTransmittance /= sum;
    }
    if (opticalProperties.hapkeParameterW < 0) {
        cerr << " HAPKE2012 Warning : hapke W < 0 !!! (hapke2 phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.hapkeParameterW = 0;
    }
    if (opticalProperties.diffuseTransmittance < 0) {
        cerr << " Warning : Diffuse Transmittance < 0 !!! (hapke2 phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.diffuseTransmittance = 0;
    }
    if (opticalProperties.directTransmittance < 0) {
        cerr << " Warning : Direct Transmittance < 0 !!! (hapke2 phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.directTransmittance = 0;
    }
}

void Hapke2012MaterialWriter::writeLuxCoreMaterial(const phsdata::Hapke2012 & h,
                                                   PhaseGeneralParametersDTO & genDTO,
                                                   const std::vector<PropertyHapke2> & hapke2Properties,
                                                   unsigned int materialID) const {
    const unsigned int nbBands = genDTO.numberOfSpectralBands;
    const std::string keyRoot = "scene.materials.Hapke2_" + std::to_string(h.indexFunction) + ".";

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "Hapke2012");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialID));
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opIndex", std::to_string(h.indexFunction));

    auto wValue     = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterW; };
    auto cValue     = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterC; };
    auto bValue     = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterB; };
    auto hValue     = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterH; };
    auto b0Value     = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterB0; };
    auto thetaValue = [&hapke2Properties](unsigned int sbi) { return hapke2Properties[sbi].hapkeParameterTh; };

    // Transmittance (kt): values below 1e-8 are clamped to 0 to avoid numerical noise.
    // W == 1 would cause a division by zero; kt defaults to 0 in that case.
    auto ktValue = [&hapke2Properties](unsigned int sbi) {
        const double dt = hapke2Properties[sbi].diffuseTransmittance;
        const double w = hapke2Properties[sbi].hapkeParameterW;
        if (dt <= 1e-8 || w == 1.)
            return 0.;
        return dt / (1. - w);
    };

    std::ostringstream valueW, valueC, valueB, valueH, valueB0, valueTh, valueKt, emissivity;
    valueW << setprecision(15);
    valueW << wValue(0);
    luxcore::appendPerBandDuplicated(valueW, nbBands, wValue);

    valueC << setprecision(15);
    valueC << cValue(0);
    luxcore::appendPerBandDuplicated(valueC, nbBands, cValue);

    valueB << setprecision(15);
    valueB << bValue(0);
    luxcore::appendPerBandDuplicated(valueB, nbBands, bValue);

    valueH << setprecision(15);
    valueH << hValue(0);
    luxcore::appendPerBandDuplicated(valueH, nbBands, hValue);

    valueB0 << setprecision(15);
    valueB0 << b0Value(0);
    luxcore::appendPerBandDuplicated(valueB0, nbBands, b0Value);

    valueTh << setprecision(15);
    valueTh << thetaValue(0);
    luxcore::appendPerBandDuplicated(valueTh, nbBands, thetaValue);

    valueKt << setprecision(15);
    valueKt << ktValue(0);
    luxcore::appendPerBandDuplicated(valueKt, nbBands, ktValue);

    // Emissivity is forced to 1 (the real emissivity is computed in DART); it is zero
    // for the extra simulated bands (Q,U,V and PSI,PSII).
    emissivity << setprecision(15);
    emissivity << 1.;
    luxcore::appendPerBandZeroPadded(emissivity, nbBands, [](unsigned int) { return 1.; });

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "w", valueW.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "c", valueC.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "b", valueB.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "h", valueH.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "b0", valueB0.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "theta", valueTh.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kt", valueKt.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "emission.gain", emissivity.str());

    if (h.nodeMultiplicativeFactor.is_initialized()) {
        const phsdata::MultiplicativeFactorList & mfl = *h.nodeMultiplicativeFactor;
        bool useOpticalFactorMatrix = mfl.useSameOpticalFactorMatrixForAllBands && mfl.bandCommonOpticalFactors.is_initialized();
        if (!useOpticalFactorMatrix) {
            const unsigned int nbFactors = std::min<unsigned int>(nbBands, mfl.factors.size());
            for (unsigned int sbi = 0; sbi < nbFactors && !useOpticalFactorMatrix; sbi++)
                useOpticalFactorMatrix = mfl.factors[sbi]->useOpticalFactorMatrix && mfl.factors[sbi]->opticalFactorMatrix.is_initialized();
        }

        if (useOpticalFactorMatrix) {
            // "h2012" prefix for Hapke2012 (the legacy code reused the Lambertian "l" prefix,
            // risking name collisions with Lambertian-generated optical factors).
            const std::string textureName = "h2012" + std::to_string(h.indexFunction) + "OF";
            const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
            if (outputDir.empty())
                throw std::runtime_error("Output directory path is empty");
            if (_opticalFactorsWriter.writeTexture(genDTO, textureName, mfl, outputDir.string()))
                genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opticalFactors", textureName);
        }
    }
}
}
