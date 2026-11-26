/*
 * SpecularMaterialWriter.cpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#include "materials/SpecularMaterialWriter.hpp"
#include "materials/LuxCoreMaterialOutput.hpp"

#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/RefractiveIndexDatabaseParameters.hpp"

namespace phase {

SpecularMaterialWriter::SpecularMaterialWriter(	const FluorescenceService & fluorescenceService,
												const OpticalFactorsTextureWriter & opticalFactorsWriter) :
	_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}

bool SpecularMaterialWriter::prepareDatabase(const phsdata::Specular & s,
											 const PhaseGeneralParametersDTO & genDTO,
											 OpticalPropertyParameters & opticalProperty) const {
	if (!s.databaseName.is_initialized()) {
		// No refractive-index database defined: nothing to process.
		return false;
	}

	// The v6 SpecularMulti carries the database name plus the ni/nt model names
	// (e.g. "air"/"glass"); the old v5 DAO read of those names is gone.
	auto refractiveIndexDatabaseParameters = std::make_shared<RefractiveIndexDatabaseParameters>(
		*s.databaseName, DartEnv::getDatabasePath(*s.databaseName), s.ni, genDTO.numberOfSpectralBands, true, s.nt);
	opticalProperty.addDatabaseToProperty(refractiveIndexDatabaseParameters);
	return true;
}

void SpecularMaterialWriter::applyMultiplicativeFactors(const phsdata::Specular & s,
														OpticalPropertyParameters & opticalProperty,
														std::vector<PropertySpecular> & properties,
														std::string & baseMaterial) const {
	auto refractiveIndexDatabaseParameters = std::static_pointer_cast<RefractiveIndexDatabaseParameters>(
		opticalProperty.getDatabaseParameters(REFRACTIVEINDEX_DATABASE));

	// materialCharacter==2 (glossy coating + diffuse base) links the base BSDF
	// through OpticalLink; the other characters have no base.
	int baseBSDF = 0;
	int baseBSDFIndex = 0;
	if (s.materialCharacter == 2 && s.opticalLink.is_initialized()) {
		baseBSDF = s.opticalLink->typeOpticalLink;
		baseBSDFIndex = s.opticalLink->indexOpticalLink;
	}
	// Same base-material mapping as the legacy treatSpecular (the
	// "notImpementedYet" literal is emitted as-is, kept for output parity).
	if (baseBSDF == 0) { // lambertian
		baseMaterial = "lambertian";
	} else if (baseBSDF == 4) { // RPV
		baseMaterial = "RPV";
	} else if (baseBSDF == 2) { // Hapke
		baseMaterial = "notImpementedYet";
		baseBSDFIndex = 0;
	} else if (baseBSDF == 22) { // mixed
		baseMaterial = "notImpementedYet";
		baseBSDFIndex = 0;
	} else {
		baseMaterial = "lambertian";
		baseBSDFIndex = 0;
	}

	for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
		properties[sbi].ni = refractiveIndexDatabaseParameters->getRefractionIndex(sbi);
		properties[sbi].nt = refractiveIndexDatabaseParameters->getRefractionIndex2(sbi);
		// opticalDepth plays the role of the old per-model tau.
		properties[sbi].tau = s.opticalDepth;
		// No database for the intensities: the multiplicative factors are the values.
		properties[sbi].intensityR = 1.;
		properties[sbi].intensityT = 1.;
		properties[sbi].roughness = s.roughnessFactor;
		properties[sbi].type = s.materialCharacter;
		properties[sbi].baseBSDF = baseBSDF;
		properties[sbi].baseBSDFIndex = baseBSDFIndex;
	}

	if (s.nodeMultiplicativeFactor.is_initialized()) {
		// Same XML semantics as the other materials: `factors` always contains exactly
		// one SpecularMultiplicativeFactorForLUT per spectral band.
		const phsdata::MultiplicativeFactorList & mfl = *s.nodeMultiplicativeFactor;
		for (unsigned int sbi = 0; sbi < properties.size() && sbi < mfl.factors.size(); ++sbi) {
			auto smf = std::static_pointer_cast<const phsdata::SpecularMultiplicativeFactor>(mfl.factors[sbi]);
			properties[sbi].ni *= smf->niFactor;
			properties[sbi].nt *= smf->ntFactor;
			properties[sbi].tau *= smf->opticalDepthFactor;
			properties[sbi].intensityR *= smf->reflectionIntensityFactor;
			properties[sbi].intensityT *= smf->transmissionIntensityFactor;
		}
	}
}

bool SpecularMaterialWriter::treat(const phsdata::SurfaceFunction & sf,
								   PhaseGeneralParametersDTO & genDTO,
								   const std::vector<bool> & useSpectralIrradianceForReflectance,
								   unsigned int materialIndex,
								   PhaseKeyMappingGenerator & pkmg,
								   std::ofstream & outCoeffSpecProperties) const {
	const phsdata::Specular & s = dynamic_cast<const phsdata::Specular &>(sf);
	OpticalPropertyParameters opticalProperty(SPECULAR_PROPERTY, s.indexFunction, s.functionName);
	if (!prepareDatabase(s, genDTO, opticalProperty))
		return false;

	for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
		PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

	std::vector<PropertySpecular> properties(genDTO.numberOfSpectralBands);
	std::string baseMaterial;
	applyMultiplicativeFactors(s, opticalProperty, properties, baseMaterial);

	writeLuxCoreMaterial(s, genDTO, properties, baseMaterial, materialIndex);
	return true;
}

void SpecularMaterialWriter::writeLuxCoreMaterial(const phsdata::Specular & s,
												  PhaseGeneralParametersDTO & genDTO,
												  const std::vector<PropertySpecular> & properties,
												  const std::string & baseMaterial,
												  unsigned int materialID) const {
	const unsigned int nbBands = genDTO.numberOfSpectralBands;
	const std::string keyRoot = "scene.materials.specular" + std::to_string(s.indexFunction) + ".";
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialID));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opIndex", std::to_string(s.indexFunction));

	auto niValue = [&properties](unsigned int sbi) { return properties[sbi].ni; };
	auto ntValue = [&properties](unsigned int sbi) { return properties[sbi].nt; };
	auto krValue = [&properties](unsigned int sbi) { return properties[sbi].intensityR; };
	auto ktValue = [&properties](unsigned int sbi) { return properties[sbi].intensityT; };
	auto tauValue = [&properties](unsigned int sbi) { return properties[sbi].tau; };

	std::ostringstream valueNi, valueNt, valueR, valueT, valueTau, emissivity;
	valueNi << setprecision(15);
	valueNi << niValue(0);
	luxcore::appendPerBandDuplicated(valueNi, nbBands, niValue);

	valueNt << setprecision(15);
	valueNt << ntValue(0);
	luxcore::appendPerBandDuplicated(valueNt, nbBands, ntValue);

	valueR << setprecision(15);
	valueR << krValue(0);
	luxcore::appendPerBandDuplicated(valueR, nbBands, krValue);

	valueT << setprecision(15);
	valueT << ktValue(0);
	luxcore::appendPerBandDuplicated(valueT, nbBands, ktValue);

	valueTau << setprecision(15);
	valueTau << tauValue(0);
	luxcore::appendPerBandDuplicated(valueTau, nbBands, tauValue);

	// Emissivity is forced to 1 (the real emissivity is computed in DART); it is zero for the extra simulated bands (Q,U,V and PSI,PSII).
	emissivity << setprecision(15);
	emissivity << 1.;
	luxcore::appendPerBandZeroPadded(emissivity, nbBands, [](unsigned int) { return 1.; });

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "ni", valueNi.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "nt", valueNt.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kr", valueR.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kt", valueT.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "tau", valueTau.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "emission.gain", emissivity.str());

	// roughness / material type are band-independent in the v6 XML.
	const double roughness = properties[0].roughness;
	const int materialType = properties[0].type;

	if (materialType == 0) { // interface
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "inbounce", "0");
		if (roughness < 1e-3) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "specular");
		} else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "glossy");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "roughness", std::to_string(roughness));
		}
	} else if (materialType == 1) { // thin layer
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "inbounce", "1");
		if (roughness < 1e-3) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "specular");
		} else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "glossy");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "roughness", std::to_string(roughness));
		}
	} else if (materialType == 2) { // glossy coating + diffuse base
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "glossydiffuse");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "roughness", std::to_string(roughness));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "base", baseMaterial + std::to_string(properties[0].baseBSDFIndex));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "baseType", std::to_string(properties[0].baseBSDF));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "baseIndex", std::to_string(properties[0].baseBSDFIndex));
	}

	if (s.nodeMultiplicativeFactor.is_initialized()) {
		const phsdata::MultiplicativeFactorList & mfl = *s.nodeMultiplicativeFactor;
		bool useOpticalFactorMatrix = mfl.useSameOpticalFactorMatrixForAllBands && mfl.bandCommonOpticalFactors.is_initialized();
		if (!useOpticalFactorMatrix) {
			const unsigned int nbFactors = std::min<unsigned int>(nbBands, mfl.factors.size());
			for (unsigned int sbi = 0; sbi < nbFactors && !useOpticalFactorMatrix; sbi++)
				useOpticalFactorMatrix = mfl.factors[sbi]->useOpticalFactorMatrix && mfl.factors[sbi]->opticalFactorMatrix.is_initialized();
		}

		if (useOpticalFactorMatrix) {
			// "specular" prefix: the legacy code reused the Lambertian "l" prefix, risking
			// name collisions with Lambertian-generated optical factors.
			const std::string textureName = "specular" + std::to_string(s.indexFunction) + "OF";
			const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
			if (outputDir.empty())
				throw std::runtime_error("Output directory path is empty");
			if (_opticalFactorsWriter.writeTexture(genDTO, textureName, mfl, outputDir.string()))
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opticalFactors", textureName);
		}
	}
}
}
