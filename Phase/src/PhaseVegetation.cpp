/*
 * PhaseVegetation.cpp
 *
 *  Created on: 20 oct. 2026
 *      Author: lauretn
 */

#include "Phase.hpp"

/**
 * Decides, for each spectral band, whether the reflectance values must be
 * weighted by the sun spectral irradiance: only the bands located in the
 * reflective (non thermal-only) domain use the sun irradiance.
 */
static void testIfUseSun(const PhaseGeneralParametersDTO & genDTO,
		std::vector<bool> & useSpectralIrradianceForReflectance) {
	useSpectralIrradianceForReflectance.assign(genDTO.numberOfSpectralBands, false);
	for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; ++bandIndex) {
		useSpectralIrradianceForReflectance[bandIndex] =
				(genDTO.spectralDomain[bandIndex] != PhaseGeneralParametersDTO::Thermal);
	}
}

/**
 * Direction set used by one vegetation model: its own DirectionSetLink when
 * valid, else the first direction set.
 */
unsigned int getUnderstoryDirectionSetIndex(const PhaseGeneralParametersDTO & genDTO, const unsigned int modelPosition) {
	const unsigned int indexDirectionSetLink = genDTO._volumesMaterials.understoryMultiFunctions[modelPosition]->directionSetLink.indexDirectionSetLink;
	if (indexDirectionSetLink < genDTO._directionSet.size()) {
		return indexDirectionSetLink;
	}
	return 0;
}

VegetationVolumeMaterials processVegetationVolumeMaterials(PhaseGeneralParametersDTO & genDTO) {
	VegetationVolumeMaterials vegetation;

	vegetation.foliarModel.reset(new FoliarModel());
	vegetation.foliarComputation.reset(new FoliarComputation(*vegetation.foliarModel));
	vegetation.ladComputation.reset(new LADComputation(*vegetation.foliarModel));

	testIfUseSun(genDTO, vegetation.useSpectralIrradianceForReflectance);

	return vegetation;
}

/**
 * (Re)initialises the shared foliar objects for one direction set and runs the
 * direction-set-dependent precomputing. Called by VegetationMaterialWriter
 * whenever the direction set changes between models (the per-model data filled
 * by readMultiplicativeFactor is reset by initFoliarModel, so it is redone
 * here as well; the per-model LAD is computed later in treat(), per model).
 */
void prepareVegetationDirectionSet(PhaseGeneralParametersDTO & genDTO, VegetationVolumeMaterials & vegetation, const unsigned int directionSetIndex) {
	const unsigned int numberOfFoliarModels = genDTO._volumesMaterials.understoryMultiFunctions.size();
	const vector<DirectionCartesienne> & listDirections = genDTO._directionSet[directionSetIndex]._directions;
	const unsigned int numberOfDirections = listDirections.size();

	vegetation.foliarModel->initFoliarModel(numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands,
			numberOfDirections);

	vegetation.foliarComputation->initFoliarComputation(numberOfDirections,
			PhaseProperty::_numberOfSpectralBands, numberOfFoliarModels, PhaseProperty::_numberOfThreads,
			PhaseProperty::_threadEnd, *vegetation.foliarModel);

	vegetation.ladComputation->initLADComputation();

    PhaseProperty::setParametersForPrintingProgress();
    PhaseProperty::resizePhaseParametersVectors();

    int iterationStepPhi = 10.0;
    int iterationStepTheta = 1.0;
	readMultiplicativeFactor(iterationStepPhi, iterationStepTheta, *vegetation.foliarModel, genDTO);
    printNumberOfFoliarModels();

	vegetation.foliarComputation->computeFoliarCosinePsi(listDirections, iterationStepPhi, iterationStepTheta);

    /* FOLIAR precomputing */
    if (PhaseProperty::getNumberOfFoliarModels() > 0) {
		cout << "\tProcessing Vegetation models" << endl;
    }
	double solidAnglesSum = 0.;
	numberOfAngularSectors(listDirections, solidAnglesSum);
	vegetation.fTFoliar = foliarPrecomputing(genDTO, listDirections, iterationStepTheta, iterationStepPhi, solidAnglesSum);

    if (PhaseProperty::getFluorescence()) {
        PhaseProperty::initYields();
        if (PhaseProperty::getNumberOfFoliarModels() > 0) {
			vegetation.foliarComputation->initFluorescenceMatrix();
        }
    }
}

/**
 * Builds the OpticalPropertyParameters (FOLIAR_PROPERTY) of one vegetation
 * model from its phsdata storage: the top/bottom database+model names (the
 * database name is the prospect-generated uuid when prepareProspect() ran),
 * with the temp output folder and self-destruct flag in that case. Shared by
 * createDatabasesReadingProperties() and VegetationMaterialWriter.
 */
OpticalPropertyParameters buildFoliarOpticalPropertyParameters(const phsdata::UnderstoryMulti & understory, const unsigned int numberOfBands) {
	const phsdata::UnderstoryModel * topModel = understory.getTopModel();
	const phsdata::UnderstoryModel * bottomModel = understory.getBottomModel();

	OpticalPropertyParameters opticalProperty(FOLIAR_PROPERTY, understory.indexFunction, understory.functionName, -1);

	bool useProspectGen = (topModel->prospectExternalModule.useProspectExternalModule
			|| topModel->prospectExternalModule.isFluorescent)
			&& topModel->prospectExternalModule.externParameters.is_initialized();
	std::string databaseName = topModel->databaseName;
	std::string modelName = topModel->modelName;
	std::string databasePath;
	if (useProspectGen) {
		databasePath = DartEnv::getOutputTempDatabasePath(true);
		// Maybe change the model name in the future, but this modelName are not findable by the user.
		// Spaces and special caracters are not allowed in modelName so ...
		modelName = "Model";
	} else {
		databasePath = DartEnv::getDatabasePath(databaseName);
	}

	// Bottom foliar model
	std::string bottomDatabaseName;
	std::string bottomDatabasePath;
	std::string bottomModelName;
	bool bottomProspectGen = false;
	if (bottomModel != nullptr) {
		bottomProspectGen = (bottomModel->prospectExternalModule.useProspectExternalModule
				|| bottomModel->prospectExternalModule.isFluorescent)
				&& bottomModel->prospectExternalModule.externParameters.is_initialized();
		bottomDatabaseName = bottomModel->databaseName;
		bottomModelName = bottomModel->modelName;
		if (bottomProspectGen) {
			bottomDatabasePath = DartEnv::getOutputTempDatabasePath(true);
			bottomModelName = "Model";
		} else {
			bottomDatabasePath = DartEnv::getDatabasePath(bottomDatabaseName);
		}
	}

	shared_ptr<FoliarDatabaseParameters> foliarDatabaseParameters(
		new FoliarDatabaseParameters(databaseName, databasePath, modelName, numberOfBands, understory.hasDifferentModelForBottom,
				bottomDatabaseName, bottomDatabasePath, bottomModelName));
	if (useProspectGen) {
		foliarDatabaseParameters.get()->setSelfDestructDatabase(true);
	}
	if (bottomProspectGen) {
		foliarDatabaseParameters.get()->setSelfDestructBottomDatabase(true);
	}
	opticalProperty.addDatabaseToProperty(foliarDatabaseParameters);

	return opticalProperty;
}
