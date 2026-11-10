/*
 * PhaseVegetation.cpp
 *
 *  Created on: 20 oct. 2026
 *      Author: lauretn
 */

#include "Phase.hpp"

void processVegetationVolumeMaterials(PhaseGeneralParametersDTO & genDTO) {
	FoliarModel foliarModel;
	foliarModel.initFoliarModel(PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands,
		PhaseProperty::_numberOfDirections);

	FoliarComputation foliarComputer(foliarModel);
	foliarComputer.initFoliarComputation(PhaseProperty::_numberOfDirections,
		PhaseProperty::_numberOfSpectralBands, PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfThreads,
		PhaseProperty::_threadEnd, foliarModel);

	LADComputation ladComputation(foliarModel);
	ladComputation.initLADComputation(PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands);

	computeLadData(genDTO, ladComputation, foliarModel);

	PhaseProperty::setParametersForPrintingProgress();
	PhaseProperty::resizePhaseParametersVectors();

	int iterationStepPhi = 10.0;
	int iterationStepTheta = 1.0;
	readMultiplicativeFactor(iterationStepPhi, iterationStepTheta, foliarModel, genDTO);
	printNumberOfFoliarModels();

	foliarComputer.computeFoliarCosinePsi(listDirections, iterationStepPhi, iterationStepTheta);
	vector<int> useSpectralIrradianceForReflectance(genDTO.numberOfSpectralBands);
	testIfUseSun(genDTO, useSpectralIrradianceForReflectance);
	writePhasePropertiesSpectralIrradiance(outCoeffSpecProperties, pkmg, genDTO);

	/* FOLIAR precomputing */
	if (PhaseProperty::getNumberOfFoliarModels() > 0) {
		cout << "	Processing Vegetation models" << endl;
	}
	FTFoliar fTFoliar = foliarPrecomputing(genDTO, listDirections, iterationStepTheta, iterationStepPhi, solidAnglesSum);
	PointsListsNCIntegration<DirectionCartesienne> pointsListSpec;
	PointsListsNCIntegration<DirectionCartesienne> pointsListDiff;

	if (PhaseProperty::getFluorescence()) {
		PhaseProperty::initYields();
		if (PhaseProperty::getNumberOfFoliarModels() > 0) {
			foliarComputer.initFluorescenceMatrix();
		}
	}

	static SphereDiscretizationMap sphereMap(1.0, fTFoliar.iterationStepPhi);
}



