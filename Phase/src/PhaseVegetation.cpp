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
 * Index of the direction set used by the vegetation models. Every
 * UnderstoryMulti references a direction set through its DirectionSetLink;
 * until the computation engine supports one direction set per model, the
 * first vegetation model's link drives the shared precomputing.
 */
static unsigned int getVegetationDirectionSetIndex(const PhaseGeneralParametersDTO & genDTO) {
	for (unsigned int modelIndex = 0; modelIndex < genDTO._volumesMaterials.understoryMultiFunctions.size(); ++modelIndex) {
		const unsigned int indexDirectionSetLink = genDTO._volumesMaterials.understoryMultiFunctions[modelIndex]->directionSetLink.indexDirectionSetLink;
		if (indexDirectionSetLink < genDTO._directionSet.size()) {
			return indexDirectionSetLink;
		}
	}
	return 0;
}

VegetationVolumeMaterials processVegetationVolumeMaterials(PhaseGeneralParametersDTO & genDTO) {
	VegetationVolumeMaterials vegetation;

	const unsigned int numberOfFoliarModels = PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels;
	const unsigned int directionSetIndex = getVegetationDirectionSetIndex(genDTO);
	const vector<DirectionCartesienne> & listDirections = genDTO._directionSet[directionSetIndex]._directions;
	const unsigned int numberOfDirections = listDirections.size();

	vegetation.foliarModel.reset(new FoliarModel());
	vegetation.foliarModel->initFoliarModel(numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands,
			numberOfDirections);

	vegetation.foliarComputation.reset(new FoliarComputation(*vegetation.foliarModel));
	vegetation.foliarComputation->initFoliarComputation(numberOfDirections,
			PhaseProperty::_numberOfSpectralBands, numberOfFoliarModels, PhaseProperty::_numberOfThreads,
			PhaseProperty::_threadEnd, *vegetation.foliarModel);

	vegetation.ladComputation.reset(new LADComputation(*vegetation.foliarModel));
	vegetation.ladComputation->initLADComputation(numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands);

	computeLadData(genDTO, *vegetation.ladComputation, *vegetation.foliarModel);

    PhaseProperty::setParametersForPrintingProgress();
    PhaseProperty::resizePhaseParametersVectors();

    int iterationStepPhi = 10.0;
    int iterationStepTheta = 1.0;
	readMultiplicativeFactor(iterationStepPhi, iterationStepTheta, *vegetation.foliarModel, genDTO);
    printNumberOfFoliarModels();

	vegetation.foliarComputation->computeFoliarCosinePsi(listDirections, iterationStepPhi, iterationStepTheta);
	testIfUseSun(genDTO, vegetation.useSpectralIrradianceForReflectance);

    /* FOLIAR precomputing */
    if (PhaseProperty::getNumberOfFoliarModels() > 0) {
		cout << "\\tProcessing Vegetation models" << endl;
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

	return vegetation;
 }
