/*
 * Hapke1981MaterialWriter.cpp
 *
 *  Created on: 2 nov. 2026
 *      Author: lauretn
 */

#include "materials/Hapke1981MaterialWriter.hpp"
#include "materials/LuxCoreMaterialOutput.hpp"

#include "phaseExtern/MarmitComputation.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/LambertianDatabaseParameters.hpp"
#include "database/model/HapkeDatabaseParameters.hpp"
#include "database/model/TransmittanceDatabaseParameters.hpp"

namespace phase {

std::string Hapke1981MaterialWriter::getMaterialName(unsigned int indexFunction) {
	return "Hapke" + std::to_string(indexFunction);
}
Hapke1981MaterialWriter::Hapke1981MaterialWriter(	const FluorescenceService & fluorescenceService,
													const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}

bool Hapke1981MaterialWriter::treat(const phsdata::SurfaceFunction & sf,
                                     PhaseGeneralParametersDTO & genDTO,
                                     const std::vector<bool> & useSpectralIrradianceForReflectance,
                                     unsigned int materialIndex,
                                     PhaseKeyMappingGenerator & pkmg,
                                     std::ofstream & outCoeffSpecProperties) const {
    const phsdata::Hapke1981 & h = dynamic_cast<const phsdata::Hapke1981 &>(sf);
    OpticalPropertyParameters opticalProperty(HAPKE_PROPERTY, h.indexFunction, h.functionName);
    if (!prepareDatabase(h, genDTO, opticalProperty))
        return false;

    for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
        PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

    pkmg.setTypeFonctionDePhase(globalReferences::HAPKE_SPEC);

    std::vector<PropertyHapke> properties(genDTO.numberOfSpectralBands);
    applyMultiplicativeFactors(h, opticalProperty, properties);

    writeLuxCoreMaterial(h, genDTO, properties, materialIndex);
    return true;
}

bool Hapke1981MaterialWriter::prepareDatabase(const phsdata::Hapke1981 & h,
											  const PhaseGeneralParametersDTO & genDTO,
											  OpticalPropertyParameters & opticalProperty) const {
	if (!h.databaseName.is_initialized() || !h.modelName.is_initialized()) {
		// No Hapke database/model defined: nothing to process.
		return false;
	}

	auto hapkeDatabaseParameters = std::make_shared<HapkeDatabaseParameters>(
		*h.databaseName, DartEnv::getDatabasePath(*h.databaseName), *h.modelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(hapkeDatabaseParameters);

	auto transmittanceDatabaseParameters = std::make_shared<TransmittanceDatabaseParameters>(
		h.transmittanceDatabaseName, DartEnv::getDatabasePath(h.transmittanceDatabaseName),
		h.transmittanceModelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);
	return true;
}

void Hapke1981MaterialWriter::applyMultiplicativeFactors(	const phsdata::Hapke1981 & h,
															OpticalPropertyParameters & opticalProperty,
															std::vector<PropertyHapke> & properties) const {
	auto hapkeDatabaseParameters = std::static_pointer_cast < HapkeDatabaseParameters > (opticalProperty.getDatabaseParameters(HAPKE_DATABASE));
	auto transmittanceParameters = std::static_pointer_cast < TransmittanceDatabaseParameters > (opticalProperty.getDatabaseParameters(TRANSMITTANCE_DATABASE));

	for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
		properties[sbi].directTransmittance = 0.01 * transmittanceParameters->getDirectTransmittanceForBand(sbi);
		properties[sbi].diffuseTransmittance = 0.01 * transmittanceParameters->getDiffuseTransmittanceForBand(sbi);

		properties[sbi].hapkeParameterW = hapkeDatabaseParameters->getHapkeWForBand(sbi);
		properties[sbi].hapkeParameterC1 = hapkeDatabaseParameters->getHapkeC1ForBand(sbi);
		properties[sbi].hapkeParameterC2 = hapkeDatabaseParameters->getHapkeC2ForBand(sbi);
		properties[sbi].hapkeParameterC3 = hapkeDatabaseParameters->getHapkeC3ForBand(sbi);
		properties[sbi].hapkeParameterC4 = hapkeDatabaseParameters->getHapkeC4ForBand(sbi);
		properties[sbi].hapkeParameterH1 = hapkeDatabaseParameters->getHapkeH1ForBand(sbi);
		properties[sbi].hapkeParameterH2 = hapkeDatabaseParameters->getHapkeH2ForBand(sbi);
	}

	if (h.nodeMultiplicativeFactor.is_initialized()) {
		// Same XML semantics as for Lambertian (see NOTE ON XML SEMANTICS in
		// LambertianMaterialWriter::applyMultiplicativeFactors): `factors` always
		// contains exactly one HapkeMultiplicativeFactorForLUT per spectral band,
		// so useSameFactorForAllBands can be ignored and factors applied one-to-one.
		const phsdata::MultiplicativeFactorList & mfl = *h.nodeMultiplicativeFactor;
		for (unsigned int sbi = 0; sbi < properties.size() && sbi < mfl.factors.size(); ++sbi) {
			auto hmf = std::static_pointer_cast<const phsdata::Hapke1981MultiplicativeFactor>(mfl.factors[sbi]);
			properties[sbi].directTransmittance *= hmf->directTransmittanceFactor;
			properties[sbi].diffuseTransmittance *= hmf->diffuseTransmittanceFactor;
			properties[sbi].hapkeParameterW *= hmf->hapkeParameterW;
			properties[sbi].hapkeParameterC1 *= hmf->hapkeParameterC1;
			properties[sbi].hapkeParameterC2 *= hmf->hapkeParameterC2;
			properties[sbi].hapkeParameterC3 *= hmf->hapkeParameterC3;
			properties[sbi].hapkeParameterC4 *= hmf->hapkeParameterC4;
			properties[sbi].hapkeParameterH1 *= hmf->hapkeParameterH1;
			properties[sbi].hapkeParameterH2 *= hmf->hapkeParameterH2;
		}
	}

	for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
        checkAlbedo(h.functionName, properties[sbi]);
    }
}

void Hapke1981MaterialWriter::checkAlbedo(const std::string & modelName,
                                          PropertyHapke & opticalProperties) const {
    if (opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance > 1.000001) {
        const double sum = opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance;
        cerr << " HAPKE Warning : hapke W + DiffuseTransmittance = " << sum
             << " > 1 !!! (hapke phase function : " << modelName << ") -> Scaled back to 1 " << endl;
        opticalProperties.hapkeParameterW /= sum;
        opticalProperties.diffuseTransmittance /= sum;
    }
    if (opticalProperties.hapkeParameterW < 0) {
        cerr << " HAPKE Warning : hapke W < 0 !!! (hapke phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.hapkeParameterW = 0;
    }
    if (opticalProperties.diffuseTransmittance < 0) {
        cerr << " Warning : Diffuse Transmittance < 0 !!! (hapke phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.diffuseTransmittance = 0;
    }
    if (opticalProperties.directTransmittance < 0) {
        cerr << " Warning : Direct Transmittance < 0 !!! (hapke phase function : " << modelName
             << " ) -> fixed to 0" << endl;
        opticalProperties.directTransmittance = 0;
    }
}

void Hapke1981MaterialWriter::writeLuxCoreMaterial(const phsdata::Hapke1981 & h,
                                                   PhaseGeneralParametersDTO & genDTO,
                                                   const std::vector<PropertyHapke> & hapkeProperties,
                                                   unsigned int materialID) const {
    const unsigned int nbBands = genDTO.numberOfSpectralBands;
    const std::string keyRoot = "scene.materials." + getMaterialName(h.indexFunction) + ".";

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "Hapke1981");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialID));
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opIndex", std::to_string(h.indexFunction));

    // Hapke parameters, per band
    auto wValue  = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterW; };
    auto c1Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterC1; };
    auto c2Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterC2; };
    auto c3Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterC3; };
    auto c4Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterC4; };
    auto h1Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterH1; };
    auto h2Value = [&hapkeProperties](unsigned int sbi) { return hapkeProperties[sbi].hapkeParameterH2; };

    // Transmittance (kt): values below 1e-8 are clamped to 0 to avoid numerical noise.
    // W == 1 would cause a division by zero; kt defaults to 0 in that case.
    auto ktValue = [&hapkeProperties](unsigned int sbi) {
        const double dt = hapkeProperties[sbi].diffuseTransmittance;
        const double w = hapkeProperties[sbi].hapkeParameterW;
        if (dt <= 1e-8 || w == 1.)
            return 0.;
        return dt / (1. - w);
    };

    std::ostringstream valueW, valueC1, valueC2, valueC3, valueC4, valueH1, valueH2, valueKt, emissivity;
    valueW << setprecision(15);
    valueW << wValue(0);
    luxcore::appendPerBandDuplicated(valueW, nbBands, wValue);

    valueC1 << setprecision(15);
    valueC1 << c1Value(0);
    luxcore::appendPerBandDuplicated(valueC1, nbBands, c1Value);

    valueC2 << setprecision(15);
    valueC2 << c2Value(0);
    luxcore::appendPerBandDuplicated(valueC2, nbBands, c2Value);

    valueC3 << setprecision(15);
    valueC3 << c3Value(0);
    luxcore::appendPerBandDuplicated(valueC3, nbBands, c3Value);

    valueC4 << setprecision(15);
    valueC4 << c4Value(0);
    luxcore::appendPerBandDuplicated(valueC4, nbBands, c4Value);

    valueH1 << setprecision(15);
    valueH1 << h1Value(0);
    luxcore::appendPerBandDuplicated(valueH1, nbBands, h1Value);

    valueH2 << setprecision(15);
    valueH2 << h2Value(0);
    luxcore::appendPerBandDuplicated(valueH2, nbBands, h2Value);

    valueKt << setprecision(15);
    valueKt << ktValue(0);
    luxcore::appendPerBandDuplicated(valueKt, nbBands, ktValue);

    // Emissivity is forced to 1 (the real emissivity is computed in DART); it is zero
    // for the extra simulated bands (Q,U,V and PSI,PSII).
    emissivity << setprecision(15);
    emissivity << 1.;
    luxcore::appendPerBandZeroPadded(emissivity, nbBands, [](unsigned int) { return 1.; });

    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "w", valueW.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "c1", valueC1.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "c2", valueC2.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "c3", valueC3.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "c4", valueC4.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "h1", valueH1.str());
    genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "h2", valueH2.str());
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
        	// "h1981" prefix for Hapke1981 (the legacy code reused the Lambertian "l"
            // prefix, risking name collisions with Lambertian-generated optical factors).
            const std::string textureName = "h1981" + std::to_string(h.indexFunction) + "OF";
            const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
            if (outputDir.empty())
                throw std::runtime_error("Output directory path is empty");
            if (_opticalFactorsWriter.writeTexture(genDTO, textureName, mfl, outputDir.string()))
                genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opticalFactors", textureName);
        }
    }
}
}
