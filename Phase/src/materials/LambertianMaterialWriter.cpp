/*
 * LambertianMaterialWriter.cpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#include "materials/LambertianMaterialWriter.hpp"
#include "materials/LuxCoreMaterialOutput.hpp"

#include "phaseExtern/MarmitComputation.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/LambertianDatabaseParameters.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace phase {

std::string LambertianMaterialWriter::getMaterialName(unsigned int indexFunction) {
	return "lambertian" + std::to_string(indexFunction);
}
LambertianMaterialWriter::LambertianMaterialWriter(	const FluorescenceService & fluorescenceService,
													const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}

bool LambertianMaterialWriter::treat(	const phsdata::SurfaceFunction & sf,
										PhaseGeneralParametersDTO & genDTO,
										const std::vector<bool> & useSpectralIrradianceForReflectance,
										unsigned int materialIndex,
										PhaseKeyMappingGenerator & pkmg,
										std::ofstream & outCoeffSpecProperties) const {
	const phsdata::LambertianMulti & lm = dynamic_cast<const phsdata::LambertianMulti &>(sf);
    OpticalPropertyParameters opticalProperty(LAMBERTIAN_PROPERTY, lm.indexFunction, lm.functionName);
    if (!prepareDatabase(lm, genDTO, opticalProperty))
        return false;

    for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
        PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

    FluorescenceResults fluorescence = computeFluorescence(lm, genDTO);   // FIRST: decides material type

    std::vector<PropertyLambertian> properties(genDTO.numberOfSpectralBands);
    applyMultiplicativeFactors(lm, opticalProperty, properties);

    writeLuxCoreMaterial(lm, genDTO, properties, fluorescence, materialIndex);
    _fluorescenceService.writeSimulationProperties(fluorescence, pkmg, outCoeffSpecProperties);
    return true;
}

bool LambertianMaterialWriter::prepareDatabase(	const phsdata::LambertianMulti & lm,
												const PhaseGeneralParametersDTO & genDTO,
												OpticalPropertyParameters & opticalProperty) const {
    std::string databaseName, databasePath, modelName;
    bool selfDestruct = false;

    if (lm.lambertian.is_initialized()) {
        databaseName = lm.lambertian->databaseName;
        modelName = lm.lambertian->modelName;
        databasePath = DartEnv::getDatabasePath(databaseName);
        const phsdata::ProspectExternalModule & pem = lm.lambertian->prospectExternalModule;
        if ((pem.useProspectExternalModule || pem.isFluorescent) && pem.externParameters.is_initialized()) {
            // prepareProspect() generated this database in the temp output folder (and overwrote databaseName)
            databasePath = DartEnv::getOutputTempDatabasePath(true);
            selfDestruct = true;
        }
    } else if (lm.marmit.is_initialized()) {
        // Generate a unique database name (unique for multithreading / will be deleted after)
        databaseName = boost::lexical_cast<std::string>(boost::uuids::random_generator()()) + ".db";

        MarmitComputation marmit(lm.marmit->smc, lm.marmit->d, lm.marmit->marmitModel, databaseName);
        if (!marmit.runMarmitDatabaseGeneration()) {
        	cerr << "Marmit run failed on lambertian phase function : " << lm.functionName << " using " << lm.marmit->marmitModel << ". Phase function skipped." << endl;
            return false;
        }
        if (lm.marmit->keepOutputFile)
            marmit.runMarmitOutputFileGeneration();
        // Model name is not visible to the user; spaces and special characters are not allowed.
        modelName = "Model";
        databasePath = DartEnv::getDatabasePath(databaseName); // Marmit writes to the local temp database dir, found via the database search order
        selfDestruct = true;
    } else {
        // Neither a Lambertian database nor a Marmit configuration: nothing to process.
        return false;
    }

    auto lambertianDatabaseParameters = std::make_shared<LambertianDatabaseParameters>(databaseName, databasePath, modelName, genDTO.numberOfSpectralBands);
    lambertianDatabaseParameters->setSelfDestructDatabase(selfDestruct);
    opticalProperty.addDatabaseToProperty(lambertianDatabaseParameters);
    return true;
}

FluorescenceResults LambertianMaterialWriter::computeFluorescence(	const phsdata::LambertianMulti & lm,
																	const PhaseGeneralParametersDTO & genDTO) const {
	FluorescenceResults fluorescence;
	if (!PhaseProperty::getFluorescence() || !lm.lambertian.is_initialized())
		return fluorescence;
	const phsdata::ProspectExternalModule & pem = lm.lambertian->prospectExternalModule;
	if (pem.isFluorescent && pem.externParameters.is_initialized() && pem.fluorescence.is_initialized()) {
		InterpolatedMatrices interpolatedMatrices(genDTO.numberOfSpectralBands);
		_fluorescenceService.computeMatrices(fluorescence, interpolatedMatrices, globalReferences::LAMBERTIEN, lm.indexFunction, *pem.externParameters, *pem.fluorescence, genDTO, 0);
	}
	return fluorescence;
}

void LambertianMaterialWriter::applyMultiplicativeFactors(const phsdata::LambertianMulti & lm,
								  OpticalPropertyParameters & opticalProperty,
								  std::vector<PropertyLambertian> & properties) const {
	shared_ptr<LambertianDatabaseParameters> lambertianDatabaseParameters = ::static_pointer_cast<LambertianDatabaseParameters>(opticalProperty.getDatabaseParameters(LAMBERTIAN_DATABASE));

	for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
		properties[sbi].reflectance = 0.01 * lambertianDatabaseParameters->getReflectanceForBand(sbi);
		properties[sbi].diffuseTransmittance = 0.01 * lambertianDatabaseParameters->getDiffuseTransmittanceForBand(sbi);
		properties[sbi].directTransmittance = 0.01 * lambertianDatabaseParameters->getDirectTransmittanceForBand(sbi);
	}

	if (lm.nodeMultiplicativeFactor.is_initialized()) {
		// NOTE ON XML SEMANTICS (to be reworked — see TODO in writeOpticalFactorsFile):
		// The XML producer guarantees that `factors` always contains exactly one
		// LambertianMultiplicativeFactorForLUT node per spectral band, regardless of
		// the useSameFactorForAllBands flag. When that flag is true, the producer
		// duplicates the first band's factor values into all other bands, so the
		// one-factor-per-band application below is valid in both cases and the flag
		// does not need to be read here.
		//
		// Contrast with useSameOpticalFactorMatrixForAllBands, which behaves
		// differently: when true, a single common OpticalFactorMatrix is stored in
		// bandCommonOpticalFactors (no per-band OFM nodes); when false, each factor
		// carries its own optional OpticalFactorMatrix. The two flags therefore have
		// similar names but different storage layouts, which is an XML design flaw
		// to be fixed in the producer.
		const phsdata::MultiplicativeFactorList & mfl = *lm.nodeMultiplicativeFactor;
		for (unsigned int sbi = 0; sbi < properties.size() && sbi < mfl.factors.size(); ++sbi) {
			auto lmf = std::static_pointer_cast<const phsdata::LambertianMultiplicativeFactor>(mfl.factors[sbi]);
			properties[sbi].reflectance *= lmf->reflectanceFactor;
			properties[sbi].diffuseTransmittance *= lmf->diffuseTransmittanceFactor;
			properties[sbi].directTransmittance *= lmf->directTransmittanceFactor;
		}
	}

	for (unsigned int sbi = 0; sbi < properties.size(); ++sbi) {
		checkAlbedo(lm.functionName, properties[sbi]);
	}
}

void LambertianMaterialWriter::writeLuxCoreMaterial(const phsdata::LambertianMulti & lm,
							  PhaseGeneralParametersDTO & genDTO,
							  const std::vector<PropertyLambertian> & lambertianProperties,
							  const FluorescenceResults & fluorescenceResults,
							  unsigned int materialID) const {
	writeLuxCoreMaterial(genDTO, lm.indexFunction, materialID, lambertianProperties, fluorescenceResults);

	if (lm.nodeMultiplicativeFactor.is_initialized()) {
		const phsdata::MultiplicativeFactorList & mfl = *lm.nodeMultiplicativeFactor;
		bool useOpticalFactorMatrix = mfl.useSameOpticalFactorMatrixForAllBands && mfl.bandCommonOpticalFactors.is_initialized();
		if (!useOpticalFactorMatrix) {
			const unsigned int nbFactors = std::min<unsigned int>(genDTO.numberOfSpectralBands, mfl.factors.size());
			for (unsigned int sbi = 0; sbi < nbFactors && !useOpticalFactorMatrix; sbi++)
				useOpticalFactorMatrix = mfl.factors[sbi]->useOpticalFactorMatrix && mfl.factors[sbi]->opticalFactorMatrix.is_initialized();
		}

		if (useOpticalFactorMatrix) {
			const std::string textureName = "l" + std::to_string(lm.indexFunction) + "OF";
			const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
			if (outputDir.empty())
				throw std::runtime_error("Output directory path is empty");
			if (_opticalFactorsWriter.writeTexture(genDTO, textureName, mfl, outputDir.string()))
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.materials.lambertian" + std::to_string(lm.indexFunction) + ".opticalFactors", textureName);
		}
	}
}

void LambertianMaterialWriter::writeLuxCoreMaterial(	PhaseGeneralParametersDTO & genDTO,
							unsigned int materialIndex,
							unsigned int materialID,
							const std::vector<PropertyLambertian> & lambertianProperties,
							const FluorescenceResults & fluorescenceResults) const {
	const unsigned int nbBands = genDTO.numberOfSpectralBands;
	const std::string keyRoot = "scene.materials." + getMaterialName(materialIndex) + ".";

   // Fluorescent only if the matrices were actually computed and written successfully.
	const bool fluorescentMaterial = PhaseProperty::getFluorescence() && fluorescenceResults.isFluorescent;

	// Translucent materials (i.e. where transmittance != 0) are slower. We don't use them unless it's necessary.
	const bool translucent = std::any_of(lambertianProperties.begin(), lambertianProperties.end(), [](const PropertyLambertian & p) {return p.diffuseTransmittance > 0.;});

	std::string materialType;
	if (fluorescentMaterial)
		materialType = "fluorescent";
	else
		materialType = translucent ? "mattetranslucent" : "matte";

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", materialType);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialID));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "opIndex", std::to_string(materialIndex));

	const bool hasDirectTransmittance = std::any_of(lambertianProperties.begin(), lambertianProperties.end(), [](const PropertyLambertian & p) {return p.directTransmittance > 0.;});
	if (hasDirectTransmittance) {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "transparency.angularStep", "90");
		std::vector<double> directTransmittances(nbBands);
		for (unsigned int sbi = 0; sbi < nbBands; sbi++)
			directTransmittances[sbi] = lambertianProperties[sbi].directTransmittance;
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "transparency.directionalFile", luxcore::writeLuxCoreDirectTransmittance(directTransmittances));
	}

	// Reflectance (kr or kd) and emissivity
	std::ostringstream valueKr, valueKt, emissivity;
	valueKr << setprecision(15);
	valueKr << lambertianProperties[0].reflectance;
	luxcore::appendPerBandDuplicated(valueKr, nbBands, [&lambertianProperties](unsigned int sbi) { return lambertianProperties[sbi].reflectance; });

	emissivity << setprecision(15);
	// Values below 1e-15 are clamped to 0 to avoid tiny emissivities such as -2.22044604925031e-16.
	auto emissivityValue = [&lambertianProperties](unsigned int sbi) {
		const double e = 1 - lambertianProperties[sbi].reflectance - lambertianProperties[sbi].diffuseTransmittance;
		return (std::abs(e) < 1e-15) ? 0.0 : e;
	};
	emissivity << emissivityValue(0);
	// Emissivity is zero for the extra simulated bands (Q,U,V and PSI,PSII): they carry
	// the optical properties, not thermal emission.
	luxcore::appendPerBandZeroPadded(emissivity, nbBands, emissivityValue);

	if (materialType == "mattetranslucent" or materialType == "fluorescent")
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kr", valueKr.str());
	else
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kd", valueKr.str());

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "emission.gain", emissivity.str());

	// Transmittance (kt), only needed for translucent or fluorescent materials
	if (translucent or fluorescentMaterial) {
		valueKt << setprecision(15);
		// Guard against division by zero when reflectance is exactly 1.
		auto ktValue = [&lambertianProperties](unsigned int sbi) {
			if (lambertianProperties[sbi].reflectance == 1)
				return 0.;
			return lambertianProperties[sbi].diffuseTransmittance / (1. - lambertianProperties[sbi].reflectance);
		};
		valueKt << ktValue(0);
		luxcore::appendPerBandDuplicated(valueKt, nbBands, ktValue);
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "kt", valueKt.str());
	}

	if (fluorescentMaterial) {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "fluorescencefiles", fluorescenceResults.fileName);
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "useBioClimaticProfileEta", fluorescenceResults.useBioClimaticProfileEta ? "1" : "0");
		if (fluorescenceResults.useBioClimaticProfileEta)
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "bioClimaticFilePath", fluorescenceResults.bioClimaticFileRelativePath);
	}

}

void LambertianMaterialWriter::checkAlbedo(	const string & modelName,
											PropertyLambertian & opticalProperties) const {
	if (opticalProperties.reflectance + opticalProperties.diffuseTransmittance > 1.000001) {
		const double sum = opticalProperties.reflectance + opticalProperties.diffuseTransmittance;
		cerr << " LAMBERTIAN Warning : Reflectance + DiffuseTransmittance = " << sum << " > 1 !!! (lambertian phase function : " << modelName << ") -> Scaled back to 1 " << endl;
		opticalProperties.reflectance /= sum;
		opticalProperties.diffuseTransmittance /= sum;
	}
	if (opticalProperties.reflectance < 0) {
		cerr << " Warning : Reflectance < 0 !!! (lambertian phase function : " << modelName << " ) -> fixed to 0" << endl;
		opticalProperties.reflectance = 0;
	}
	if (opticalProperties.diffuseTransmittance < 0) {
	    cerr << " Warning : Diffuse Transmittance < 0 !!! (lambertian phase function : " << modelName << " ) -> fixed to 0" << endl;
	    opticalProperties.diffuseTransmittance = 0;
	}
	if (opticalProperties.directTransmittance < 0) {
	    cerr << " Warning : Direct Transmittance < 0 !!! (lambertian phase function : " << modelName << " ) -> fixed to 0" << endl;
	    opticalProperties.directTransmittance = 0;
	}
}
}
