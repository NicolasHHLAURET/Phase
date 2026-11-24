/*
 * FoliarComputation.cpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#include "../../include/foliar/FoliarComputation.hpp"

void FoliarComputation::initFoliarComputation(	const unsigned int & numberOfDirections,
												const unsigned int & numberOfSpectralBands,
												const unsigned int & numberOfFoliarModels,
												const unsigned int & numberOfThreads,
												std::vector<unsigned int> & threadEnd,
												FoliarModel & foliarModel) {
	_numberOfFoliarModels = numberOfFoliarModels;
	_numberOfDirections = numberOfDirections;
	_numberOfSpectralBands = numberOfSpectralBands;
	_numberOfThreads = numberOfThreads;
	_threadEnd = threadEnd;

	_nbDisplayedWarningMessages = 0;
//	MAX_WARNING_MESSAGE = 10;

	_albedoVector.resize(_numberOfSpectralBands);
	_albedoSpecVector.resize(_numberOfSpectralBands);

	_sineIntegrandVector.resize(NBMAX_TETAF);

	_clumpingIndex.resize(_numberOfDirections);
	_diffuseTransfertFunctionVector.resize(_numberOfSpectralBands);
	_specularTransfertFunctionVector.resize(_numberOfSpectralBands);
	_polTransfertFunctionVector.resize(_numberOfSpectralBands);
	_tIntdiffFeuille.resize(_numberOfSpectralBands);
	_tIntspecFeuille.resize(_numberOfSpectralBands);
	_tgOmegas.resize(_numberOfSpectralBands);
	_tgsOmegas.resize(_numberOfSpectralBands);
	_tgsOmegasNormalized.resize(_numberOfSpectralBands);

	_foliarPrecomputedNormalisation.resize(_numberOfDirections, std::vector<double>(_numberOfDirections));
	for (unsigned int i = 0; i < _numberOfSpectralBands; i++) {
		_diffuseTransfertFunctionVector[i].resize(_numberOfDirections, std::vector<double>(_numberOfDirections));
		_specularTransfertFunctionVector[i].resize(_numberOfDirections, std::vector<double>(_numberOfDirections));
		_polTransfertFunctionVector[i].resize(_numberOfDirections, std::vector<double>(_numberOfDirections));
		_tgOmegas[i].resize(_numberOfDirections);
		_tgsOmegas[i].resize(_numberOfDirections);
		_tgsOmegasNormalized[i].resize(_numberOfDirections);
	}

	// New precomputation
	_preDiffuseTransfertFunctionVector.resize(4);
	for (unsigned int i = 0; i < 4; i++) {
		_preDiffuseTransfertFunctionVector[i].resize(_numberOfDirections, std::vector<double>(_numberOfDirections));
	}
}

void FoliarComputation::initFluorescenceMatrix() {
	_fluorescenceDirectionTransferMatrixPSI.resize(_numberOfSpectralBands * _numberOfSpectralBands);
	_fluorescenceDirectionTransferMatrixPSII.resize(_numberOfSpectralBands * _numberOfSpectralBands);

	for (unsigned int i = 0; i < _numberOfSpectralBands * _numberOfSpectralBands; i++) {
		_fluorescenceDirectionTransferMatrixPSI[i].resize((_numberOfDirections) * (_numberOfDirections));
		_fluorescenceDirectionTransferMatrixPSII[i].resize((_numberOfDirections) * (_numberOfDirections));
	}
}

void FoliarComputation::reInitFoliarComputation() {
	for (unsigned int i = 0; i < _numberOfSpectralBands; i++) {
		for (int j = 0; j < _numberOfDirections; j++) {
			for (int k = 0; k < _numberOfDirections; k++) {
				_diffuseTransfertFunctionVector[i][j][k] = 0;
				_specularTransfertFunctionVector[i][j][k] = 0;
				_polTransfertFunctionVector[i][j][k] = 0;
			}
		}
	}
	if (PhaseProperty::getFluorescence()) {
		for (unsigned int i = 0; i < _numberOfSpectralBands; i++){
			for (unsigned int j = 0; j < _numberOfSpectralBands; j++){
				for (int k = 0; k < _numberOfDirections; k++){
					for (int l = 0; l < _numberOfDirections; l++) {
						_fluorescenceDirectionTransferMatrixPSI[i * _numberOfSpectralBands + j][k * (_numberOfDirections) + l] = 0;
						_fluorescenceDirectionTransferMatrixPSII[i * _numberOfSpectralBands + j][k * (_numberOfDirections) + l] = 0;
					}
				}
			}
		}
	}
}

void FoliarComputation::runComputeFoliarPerThread(	std::vector<FoliarProperty> & foliarProperty,
													const unsigned int & modelIndex,
													const FTFoliar & fTFoliar,
													OpticalPropertyParameters & foliarPropertyParameters,
													const std::vector<DirectionCartesienne> & listDirections,
													const SphereDiscretizationMap & sphereMap,
													const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
													const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff) {
	_listDirections = listDirections;
	precomputeFoliarNormalisation(listDirections, fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta);
	preComputeFRFoliar(fTFoliar, modelIndex, listDirections);
	if (fTFoliar.ifDiffIntegration || fTFoliar.ifSpecIntegration) {
		_precomputedCosinePsiSaVector.resize(listDirections.size());
		for (unsigned int outputDirection = 0; outputDirection < listDirections.size(); ++outputDirection)
			calcDotPSIIntegrateSA(pointsListDiff.getPointsMapCartesian(outputDirection), sphereMap, _precomputedCosinePsiSaVector[outputDirection], fTFoliar.iterationStepPhi);
	}
	boost::thread_group foliarThreads;
	for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
		boost::thread *t = new boost::thread(boost::bind(&FoliarComputation::computeFoliarPerThread, this, boost::ref(foliarProperty), modelIndex, boost::ref(fTFoliar), boost::ref(foliarPropertyParameters), boost::ref(sphereMap), boost::ref(pointsListSpec), boost::ref(pointsListDiff), threadNumber));
		foliarThreads.add_thread(t);
	}
	foliarThreads.join_all();
}

void FoliarComputation::computeFoliarPerThread(	std::vector<FoliarProperty> & foliarProperty,
												const unsigned int modelIndex,
												const FTFoliar & fTFoliar,
												OpticalPropertyParameters & foliarPropertyParameters,
												const SphereDiscretizationMap & sphereMap,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
												const unsigned int threadNumber) {
	unsigned int spectralBandIndex = 0;
	for (unsigned int i = 0; i < _threadEnd[threadNumber]; i++) {
		spectralBandIndex = threadNumber + _numberOfThreads * i;
		computeFoliarPerBand(foliarProperty[spectralBandIndex], spectralBandIndex, fTFoliar, foliarPropertyParameters, _listDirections, sphereMap, pointsListSpec, pointsListDiff);
	}
}

void FoliarComputation::computeFoliarPerBand(	FoliarProperty & foliarProperty,
												const unsigned int & spectralBandIndex,
												const FTFoliar & fTFoliar,
												OpticalPropertyParameters & foliarPropertyParameters,
												const std::vector<DirectionCartesienne> & listDirections,
												const SphereDiscretizationMap & sphereMap,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff) {

	shared_ptr<FoliarDatabaseParameters> foliarDatabaseParameters = ::static_pointer_cast<FoliarDatabaseParameters>(
		foliarPropertyParameters.getDatabaseParameters(FOLIAR_DATABASE));

	unsigned int modelIndex = foliarPropertyParameters.getModelIndex();

	_albedoVector[spectralBandIndex] = 0;
	_albedoSpecVector[spectralBandIndex] = 0;

	foliarProperty.typeOfLeafAngleDistribution = _parametersForCurrentModel.typeOfLeafAngleDistribution;
	foliarProperty.foliarDimension = _parametersForCurrentModel.foliarDimension;

	foliarProperty.reflectanceTop = 0.01 * foliarDatabaseParameters->getTopReflectanceForBand(spectralBandIndex)
			* _parametersForCurrentModel.adaxialReflectanceVector[spectralBandIndex];

	foliarProperty.reflectanceBottom = 0.01 * foliarDatabaseParameters->getBottomReflectanceForBand(spectralBandIndex)
			* _parametersForCurrentModel.abaxialReflectanceVector[spectralBandIndex];

	foliarProperty.transmittance = 0.01 * foliarDatabaseParameters->getTransmittanceForBand(spectralBandIndex)
			* _parametersForCurrentModel.leafTransmittanceVector[spectralBandIndex];

	foliarProperty.bottomTransmittance = 0.01 * foliarDatabaseParameters->getBottomTransmittanceForBand(spectralBandIndex)
			* _parametersForCurrentModel.bottomLeafTransmittanceVector[spectralBandIndex];

	if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
		foliarProperty.topPSIBackward.resize(_numberOfSpectralBands);
		foliarProperty.topPSIForward.resize(_numberOfSpectralBands);
		foliarProperty.topPSIIBackward.resize(_numberOfSpectralBands);
		foliarProperty.topPSIIForward.resize(_numberOfSpectralBands);
		foliarProperty.bottomPSIBackward.resize(_numberOfSpectralBands);
		foliarProperty.bottomPSIForward.resize(_numberOfSpectralBands);
		foliarProperty.bottomPSIIBackward.resize(_numberOfSpectralBands);
		foliarProperty.bottomPSIIForward.resize(_numberOfSpectralBands);

		if (_topFluorescenceComputation) {
			for (unsigned int emissionBandIndex = 0; emissionBandIndex < _numberOfSpectralBands; emissionBandIndex++) {
				foliarProperty.topPSIBackward[emissionBandIndex] = _frontFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][0];
				foliarProperty.topPSIForward[emissionBandIndex] = _frontFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][2];
				foliarProperty.topPSIIBackward[emissionBandIndex] = _frontFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][1];
				foliarProperty.topPSIIForward[emissionBandIndex] = _frontFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][3];
			}
		} else {
			for (unsigned int emissionBandIndex = 0; emissionBandIndex < _numberOfSpectralBands; emissionBandIndex++) {
				foliarProperty.topPSIBackward[emissionBandIndex] = 0.0;
				foliarProperty.topPSIForward[emissionBandIndex] = 0.0;
				foliarProperty.topPSIIBackward[emissionBandIndex] = 0.0;
				foliarProperty.topPSIIForward[emissionBandIndex] = 0.0;
			}
		}

		if (_bottomFluorescenceComputation) {
			for (unsigned int emissionBandIndex = 0; emissionBandIndex < _numberOfSpectralBands; emissionBandIndex++) {
				foliarProperty.bottomPSIBackward[emissionBandIndex] = _backFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][0];
				foliarProperty.bottomPSIForward[emissionBandIndex] = _backFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][2];
				foliarProperty.bottomPSIIBackward[emissionBandIndex] = _backFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][1];
				foliarProperty.bottomPSIIForward[emissionBandIndex] = _backFaceFluorescenceMatrix[spectralBandIndex][emissionBandIndex][3];
			}
		} else {
			for (unsigned int emissionBandIndex = 0; emissionBandIndex < _numberOfSpectralBands; emissionBandIndex++) {
				foliarProperty.bottomPSIBackward[emissionBandIndex] = 0.0;
				foliarProperty.bottomPSIForward[emissionBandIndex] = 0.0;
				foliarProperty.bottomPSIIBackward[emissionBandIndex] = 0.0;
				foliarProperty.bottomPSIIForward[emissionBandIndex] = 0.0;
			}
		}
	}

	computeFRFoliarSpecular(fTFoliar, spectralBandIndex, modelIndex, foliarProperty, listDirections, sphereMap, pointsListSpec, pointsListDiff);
	PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile("MultispectralOpticalFunctions.UnderstoryMultiFunctions.UnderstoryMulti[",
		modelIndex, -1.0, -1.0, -1.0);
	if (_albedoVector[spectralBandIndex] > 1.0001) {
		std::cerr << "Warning : foliar albedo > 1 for phase function " << modelIndex << " and band " << spectralBandIndex << " ["
				<< _albedoVector[spectralBandIndex] << "] "
				<< " ; Resetted to 1 to avoid problems in radiative transfer computation (multiple scattering)" << std::endl;
		_albedoVector[spectralBandIndex] = 1;
	}
	if (_albedoSpecVector[spectralBandIndex] > 1.0001) {

		std::cerr << "Warning : foliar specular albedo > 1 for phase function " << modelIndex << " and band " << spectralBandIndex
				<< " ; Resetted to 1 to avoid problems in radiative transfer computation (multiple scattering)" << std::endl;
		_albedoSpecVector[spectralBandIndex] = 1;
	}
}

void FoliarComputation::computeFRFoliarSpecular(const FTFoliar & fTFoliar,
												const unsigned int & spectralBandIndex,
												const unsigned int & modelIndex,
												const FoliarProperty & opticalProperties,
												const std::vector<DirectionCartesienne> & listDirections,
												const SphereDiscretizationMap & sphereMap,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
												const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff) {
	//	std::vector<std::vector<double> > cosinePsiSaVector;
	//	calcDotPSIIntegrateSA(pointsListDiff.getPointsMapCartesian(outputDirection), sphereMap, cosinePsiSaVector, fTFoliar.iterationStepPhi);
	double tdiSum = 0;
	double tsiSum = 0; // v visée // i incidente
	double tdvSum = 0; // td - diffuse // ts specular
	double tdv1Sum = 0;
	double tsvSum = 0;
	double simpleDiffAlbedo = (opticalProperties.reflectanceBottom + opticalProperties.reflectanceTop) * 0.5
			+ (opticalProperties.transmittance + opticalProperties.bottomTransmittance) * 0.5;
	for (int incidentDirection = 0; incidentDirection < _numberOfDirections; incidentDirection++) {
		tdvSum = 0;
		tdv1Sum = 0;
		tsvSum = 0;
		for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
			if (fTFoliar.ifDiffIntegration) {
				diffuseFoliarTransferFunctionFromIntegration(listDirections, incidentDirection, outputDirection, opticalProperties, fTFoliar,
					sphereMap, pointsListDiff, _precomputedCosinePsiSaVector[outputDirection], modelIndex, spectralBandIndex);
				if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
					fluorescenceTransferFunctionFromIntegration(listDirections, incidentDirection, outputDirection, opticalProperties, fTFoliar,
							sphereMap, pointsListDiff, _precomputedCosinePsiSaVector[outputDirection], modelIndex, spectralBandIndex);
				}
			} else {
				computeTransfertFunctionOfFoliarDiffuse(listDirections, incidentDirection, outputDirection, opticalProperties,
					fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta, modelIndex, spectralBandIndex);
				if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
					computeFluorescenceTransferFunction(listDirections, incidentDirection, outputDirection, opticalProperties,
							fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta, modelIndex, spectralBandIndex);
				}
			}
			if (listDirections[outputDirection].getProperty() != DirectionRoot::VIRTUAL) {
				tdv1Sum += _diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection];
			}
			if (opticalProperties.specularFoliarData._isSpecular) {
				if (fTFoliar.ifSpecIntegration) {
					computeSpecularPolTransferFoliarFromIntegration(listDirections, incidentDirection, outputDirection, opticalProperties,
						fTFoliar.casPDF, fTFoliar.numDivSpecPol, pointsListSpec, modelIndex, spectralBandIndex);
				} else {
					foliarPolaSpecTranferFunction(listDirections, incidentDirection, outputDirection, opticalProperties, fTFoliar.casPDF, modelIndex,
						spectralBandIndex);
				}
				if (listDirections[outputDirection].getProperty() != DirectionRoot::VIRTUAL) {
					tsvSum += _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection];
				}
			}
		}

		for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
			if (tdv1Sum > 0) {
				_diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection] *= (simpleDiffAlbedo) / tdv1Sum;

				if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
					for (unsigned int bandId = 0; bandId < _numberOfSpectralBands; ++bandId) {
						_fluorescenceDirectionTransferMatrixPSI[spectralBandIndex * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections + 1) + outputDirection] *= (simpleDiffAlbedo) / tdv1Sum;
						_fluorescenceDirectionTransferMatrixPSII[spectralBandIndex * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections + 1) + outputDirection] *= (simpleDiffAlbedo) / tdv1Sum;
					}
				}
			}
			if (listDirections[outputDirection].getProperty() != DirectionRoot::VIRTUAL) {
				tdvSum += _diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection];
			}
		}

		if (fTFoliar.monteCarloCheck > 0) {
			if (fTFoliar.ifDiffIntegration) {
				diffuseFoliarTransferFunctionFromIntegration(listDirections, incidentDirection, _numberOfDirections, opticalProperties, fTFoliar,
					sphereMap, pointsListDiff, _precomputedCosinePsiSaVector[_numberOfDirections], modelIndex, spectralBandIndex);
			} else {
				computeTransfertFunctionOfFoliarDiffuse(listDirections, incidentDirection, _numberOfDirections, opticalProperties,
					fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta, modelIndex, spectralBandIndex);
			}
			if (opticalProperties.specularFoliarData._isSpecular) {
				if (fTFoliar.ifSpecIntegration) {
					computeSpecularPolTransferFoliarFromIntegration(listDirections, incidentDirection, _numberOfDirections, opticalProperties,
						fTFoliar.casPDF, fTFoliar.numDivSpecPol, pointsListSpec, modelIndex, spectralBandIndex);
				} else {
					foliarPolaSpecTranferFunction(listDirections, incidentDirection, _numberOfDirections, opticalProperties, fTFoliar.casPDF,
						modelIndex, spectralBandIndex);
				}
			}
		}

		if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
			if (opticalProperties.specularFoliarData._isSpecular) {
				tsiSum += tsvSum * listDirections[incidentDirection].getSolidAngle();
			}
			tdiSum += tdvSum * listDirections[incidentDirection].getSolidAngle();
		}
	}

	_albedoVector[spectralBandIndex] = tdiSum / fTFoliar.solidAnglesSum;
	if (opticalProperties.specularFoliarData._isSpecular) {
		_albedoSpecVector[spectralBandIndex] = tsiSum / fTFoliar.solidAnglesSum;
	}
	computeAndStoreTgOmegas(listDirections, opticalProperties.specularFoliarData._isSpecular, modelIndex, spectralBandIndex);
}

void FoliarComputation::preComputeFRFoliar(	const FTFoliar & fTFoliar,
											const unsigned int & modelIndex,
											const std::vector<DirectionCartesienne> & listDirections) {
	if (!(fTFoliar.ifDiffIntegration)) {
		for (int incidentDirection = 0; incidentDirection < _numberOfDirections; incidentDirection++) {
			for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
				preComputeTransfertFunctionOfFoliarDiffuse(listDirections, incidentDirection, outputDirection, fTFoliar.iterationStepPhi,
					fTFoliar.iterationStepTheta, modelIndex);

			}

			if (fTFoliar.monteCarloCheck > 0) {
				preComputeTransfertFunctionOfFoliarDiffuse(listDirections, incidentDirection, _numberOfDirections, fTFoliar.iterationStepPhi,
					fTFoliar.iterationStepTheta, modelIndex);
			}
		}
	}
}

void FoliarComputation::preComputeTransfertFunctionOfFoliarDiffuse(	const std::vector<DirectionCartesienne> & listDirections,
																	const int & incidentDirection,
																	const int & outputDirection,
																	const int & iterationStepPhi,
																	const int & iterationStepTheta,
																	const unsigned int & modelIndex) {
	for (int idMaskFoliarProperty = 0; idMaskFoliarProperty < 4; ++idMaskFoliarProperty) {
		FoliarProperty maskedFoliarProperty;
		maskedFoliarProperty.reflectanceTop = idMaskFoliarProperty == 0 ? 1 : 0;
		maskedFoliarProperty.reflectanceBottom = idMaskFoliarProperty == 1 ? 1 : 0;
		maskedFoliarProperty.transmittance = idMaskFoliarProperty == 2 ? 1 : 0;
		maskedFoliarProperty.bottomTransmittance = idMaskFoliarProperty == 3 ? 1 : 0;

		double integranddTheta = 0;
		integranddTheta = computeIntegranddTheta(incidentDirection, outputDirection, maskedFoliarProperty, iterationStepPhi, iterationStepTheta,
			modelIndex);
		if (_transmTransfertFunctionVector[incidentDirection] < EPSILON) {
			double solidAngleCorrection = listDirections[outputDirection].getCosThetaDOmega()
					/ (std::cos(listDirections[outputDirection].getCenterTheta()) * listDirections[outputDirection].getSolidAngle());
			_preDiffuseTransfertFunctionVector[idMaskFoliarProperty][incidentDirection][outputDirection] = integranddTheta
					* listDirections[outputDirection].getSolidAngle() * solidAngleCorrection * _clumpingIndex[incidentDirection] * iterationStepPhi;
		} else {
			_preDiffuseTransfertFunctionVector[idMaskFoliarProperty][incidentDirection][outputDirection] = integranddTheta
					* _foliarPrecomputedNormalisation[incidentDirection][outputDirection];
		}
	}
}

void FoliarComputation::diffuseFoliarTransferFunctionFromIntegration(	const std::vector<DirectionCartesienne> & listDirections,
																		const int & incidentDirection,
																		const int & outputDirection,
																		const FoliarProperty & opticalProperties,
																		const FTFoliar & fTFoliar,
																		const SphereDiscretizationMap & sphereMap,
																		const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
																		const std::vector<std::vector<double> > & cosinePsiSaVector,
																		const unsigned int & modelIndex,
																		const unsigned int & reflectanceMode) {
	std::vector<DirectionCartesienne> outgoingDirList;
	std::vector<DirectionCartesienne>::iterator it;
	if (listDirections[outputDirection].getProperty() == DirectionRoot::OCCUPIED) {
		outgoingDirList.push_back(listDirections[outputDirection]);
		for (const auto & direction: listDirections) {
			if (direction.getProperty() == DirectionRoot::EMBEDDED) {
				windmillForOccupiedDirectionIntegration(outgoingDirList, direction);
			}
		}
	} else {
		outgoingDirList.push_back(listDirections[outputDirection]);
	}
	double output = 0;
	for (it = outgoingDirList.begin(); it < outgoingDirList.end(); it++) {
		std::vector<std::vector<double> > phaseMap;
		generatePhaseMapDiff(cosinePsiSaVector, pointsListDiff.getPointsMapCartesian(outputDirection).size(), pointsListDiff.getPointsMapCartesian(outputDirection)[0].size(), incidentDirection, opticalProperties, sphereMap, phaseMap, fTFoliar.iterationStepPhi,
			fTFoliar.iterationStepTheta, modelIndex);
		NewtonCotesIntegrationCalculator<double> integrationDiff(phaseMap, pointsListDiff.getDeltaCosTheta(outputDirection),
			pointsListDiff.getDeltaPhi(outputDirection));
		integrationDiff.setInput(phaseMap);
		integrationDiff.setDeltaX(pointsListDiff.getDeltaCosTheta(outputDirection));
		integrationDiff.setDeltaY(pointsListDiff.getDeltaPhi(outputDirection));
		integrationDiff.calculateIntegration2D();
		output += integrationDiff.getOutput();
	}
	_diffuseTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = output;
}

void FoliarComputation::fluorescenceTransferFunctionFromIntegration(	const std::vector<DirectionCartesienne> & listDirections,
																		const int & incidentDirection,
																		const int & outputDirection,
																		const FoliarProperty & opticalProperties,
																		const FTFoliar & fTFoliar,
																		const SphereDiscretizationMap & sphereMap,
																		const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
																		const std::vector<std::vector<double> > & cosinePsiSaVector,
																		const unsigned int & modelIndex,
																		const unsigned int & reflectanceMode) {
	std::vector<DirectionCartesienne> outgoingDirList;
	std::vector<DirectionCartesienne>::iterator it;
	if (listDirections[outputDirection].getProperty() == DirectionRoot::OCCUPIED) {
		outgoingDirList.push_back(listDirections[outputDirection]);
		for (const auto & direction: listDirections) {
			if (direction.getProperty() == DirectionRoot::EMBEDDED) {
				windmillForOccupiedDirectionIntegration(outgoingDirList, direction);
			}
		}
	} else {
		outgoingDirList.push_back(listDirections[outputDirection]);
	}

//	std::vector<std::vector<double> > cosinePsiSaVector;
//	calcDotPSIIntegrateSA(pointsListDiff.getPointsMapCartesian(outputDirection), sphereMap, cosinePsiSaVector, fTFoliar.iterationStepPhi);
	for (unsigned int emissionBandIndex = 0; emissionBandIndex < _numberOfSpectralBands; emissionBandIndex++) {
		double outputPSI = 0.0;
		double outputPSII = 0.0;
		for (it = outgoingDirList.begin(); it < outgoingDirList.end(); it++) {
			std::vector<std::vector<double> > phaseMapPSI;
			std::vector<std::vector<double> > phaseMapPSII;
			generatePhaseMapFluo(cosinePsiSaVector, pointsListDiff.getPointsMapCartesian(outputDirection).size(), pointsListDiff.getPointsMapCartesian(outputDirection)[0].size(), incidentDirection, opticalProperties, sphereMap, phaseMapPSI, phaseMapPSII, emissionBandIndex, fTFoliar.iterationStepPhi,
				fTFoliar.iterationStepTheta, modelIndex);
			NewtonCotesIntegrationCalculator<double> integrationPSI(phaseMapPSI, pointsListDiff.getDeltaCosTheta(outputDirection),
				pointsListDiff.getDeltaPhi(outputDirection));
			NewtonCotesIntegrationCalculator<double> integrationPSII(phaseMapPSII, pointsListDiff.getDeltaCosTheta(outputDirection),
				pointsListDiff.getDeltaPhi(outputDirection));
			integrationPSI.setInput(phaseMapPSI);
			integrationPSII.setInput(phaseMapPSII);
			integrationPSI.setDeltaX(pointsListDiff.getDeltaCosTheta(outputDirection));
			integrationPSII.setDeltaX(pointsListDiff.getDeltaCosTheta(outputDirection));
			integrationPSI.setDeltaY(pointsListDiff.getDeltaPhi(outputDirection));
			integrationPSII.setDeltaY(pointsListDiff.getDeltaPhi(outputDirection));
			integrationPSI.calculateIntegration2D();
			integrationPSII.calculateIntegration2D();
			outputPSI += integrationPSI.getOutput();
			outputPSII += integrationPSII.getOutput();
		}
		_fluorescenceDirectionTransferMatrixPSI[reflectanceMode * _numberOfSpectralBands + emissionBandIndex][incidentDirection * (_numberOfDirections + 1) + outputDirection] = outputPSI;
		_fluorescenceDirectionTransferMatrixPSII[reflectanceMode * _numberOfSpectralBands + emissionBandIndex][incidentDirection * (_numberOfDirections + 1) + outputDirection] = outputPSII;
	}
}

void FoliarComputation::computeTransfertFunctionOfFoliarDiffuse(const std::vector<DirectionCartesienne> & listDirections,
																const int & incidentDirection,
																const int & outputDirection,
																const FoliarProperty & opticalProperties,
																const int & iterationStepPhi,
																const int & iterationStepTheta,
																const unsigned int & modelIndex,
																const unsigned int & reflectanceMode) {
	// A + B + C + D
	double fReflectanceTop = opticalProperties.reflectanceTop * _preDiffuseTransfertFunctionVector[0][incidentDirection][outputDirection];
	double fReflectanceBottom = opticalProperties.reflectanceBottom
			* _preDiffuseTransfertFunctionVector[1][incidentDirection][outputDirection];
	double fTransmittance = opticalProperties.transmittance * _preDiffuseTransfertFunctionVector[2][incidentDirection][outputDirection];
	double fTransmittanceBottom = opticalProperties.bottomTransmittance
			* _preDiffuseTransfertFunctionVector[3][incidentDirection][outputDirection];

	_diffuseTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = fReflectanceTop + fReflectanceBottom + fTransmittance
			+ fTransmittanceBottom;
}

void FoliarComputation::computeFluorescenceTransferFunction(const std::vector<DirectionCartesienne> & listDirections,
															const int & incidentDirection,
															const int & outputDirection,
															const FoliarProperty & opticalProperties,
															const int & iterationStepPhi,
															const int & iterationStepTheta,
															const unsigned int & modelIndex,
															const unsigned int & reflectanceMode) {
//	double topAbsorption = (1. - opticalProperties.reflectanceTop - opticalProperties.transmittance);
//	double bottomAbsorption = (1. - opticalProperties.reflectanceBottom - opticalProperties.bottomTransmittance);
	for (unsigned int bandId = 0; bandId < _numberOfSpectralBands; ++bandId) {
		if (_topFluorescenceComputation) {
			_fluorescenceDirectionTransferMatrixPSI[reflectanceMode * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections) + outputDirection] += _preDiffuseTransfertFunctionVector[0][incidentDirection][outputDirection] * _frontFaceFluorescenceMatrix[reflectanceMode][bandId][0] + _preDiffuseTransfertFunctionVector[2][incidentDirection][outputDirection] * _frontFaceFluorescenceMatrix[reflectanceMode][bandId][2];
			_fluorescenceDirectionTransferMatrixPSII[reflectanceMode * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections) + outputDirection] += _preDiffuseTransfertFunctionVector[0][incidentDirection][outputDirection] * _frontFaceFluorescenceMatrix[reflectanceMode][bandId][1] + _preDiffuseTransfertFunctionVector[2][incidentDirection][outputDirection] * _frontFaceFluorescenceMatrix[reflectanceMode][bandId][3];
		}
		if (_bottomFluorescenceComputation) {
			_fluorescenceDirectionTransferMatrixPSI[reflectanceMode * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections) + outputDirection] += _preDiffuseTransfertFunctionVector[1][incidentDirection][outputDirection] * _backFaceFluorescenceMatrix[reflectanceMode][bandId][0] + _preDiffuseTransfertFunctionVector[3][incidentDirection][outputDirection] * _backFaceFluorescenceMatrix[reflectanceMode][bandId][2];
			_fluorescenceDirectionTransferMatrixPSII[reflectanceMode * _numberOfSpectralBands + bandId][incidentDirection * (_numberOfDirections) + outputDirection] += _preDiffuseTransfertFunctionVector[1][incidentDirection][outputDirection] * _backFaceFluorescenceMatrix[reflectanceMode][bandId][1] + _preDiffuseTransfertFunctionVector[3][incidentDirection][outputDirection] * _backFaceFluorescenceMatrix[reflectanceMode][bandId][3];
		}
	}
}

double FoliarComputation::computeIntegranddTheta(	const int & incidentDirection,
													const int & outputDirection,
													const FoliarProperty & opticalProperties,
													const int & iterationStepPhi,
													const int & iterationStepTheta,
													const unsigned int & modelIndex) {
	double integrand = 0;
	int x = 0;
	int n = 90 / iterationStepTheta;
	double h = iterationStepTheta / 3.;
	integrand = functiondTheta(x, incidentDirection, outputDirection, opticalProperties, iterationStepPhi, iterationStepTheta, modelIndex);
	for (int i = 2; i < n; i += 2) {
		integrand += 2. * functiondTheta(i, incidentDirection, outputDirection, opticalProperties, iterationStepPhi, iterationStepTheta, modelIndex);
	}
	for (int i = 1; i < n; i += 2) {
		integrand += 4. * functiondTheta(i, incidentDirection, outputDirection, opticalProperties, iterationStepPhi, iterationStepTheta, modelIndex);
	}
	integrand += functiondTheta(n, incidentDirection, outputDirection, opticalProperties, iterationStepPhi, iterationStepTheta, modelIndex);

	return integrand * h;
}

double FoliarComputation::functiondTheta(	const int & thetaF,
											const int & incidentDirection,
											const int & outputDirection,
											const FoliarProperty & opticalProperties,
											const int & iterationStepPhi,
											const int & iterationStepTheta,
											const unsigned int & modelIndex) {
	double integranddPhi = computeIntegranddPhi(thetaF, incidentDirection, outputDirection, opticalProperties, iterationStepPhi);
	return integranddPhi * _leafAngleDistributionFunctionVector[thetaF * iterationStepTheta];
}

/**
 * 513 instruction/appels
 */
double FoliarComputation::computeIntegranddPhi(	const int & thetaF,
												const int & incidentDirection,
												const int & outputDirection,
												const FoliarProperty & opticalProperties,
												const int & iterationStepPhi) {
	double integrand = 0;
	int x = 0;
	int n = PHI_MAX / iterationStepPhi;
	integrand = functionOfFoliarDiffuse(thetaF, x, incidentDirection, outputDirection, opticalProperties);
	for (int i = 2; i < n; i += 2) {
		integrand += 2. * functionOfFoliarDiffuse(thetaF, i, incidentDirection, outputDirection, opticalProperties);
	}
	for (int i = 1; i < n; i += 2) {
		integrand += 4. * functionOfFoliarDiffuse(thetaF, i, incidentDirection, outputDirection, opticalProperties);
	}
	integrand += functionOfFoliarDiffuse(thetaF, n, incidentDirection, outputDirection, opticalProperties);

	return integrand * 0.333333333;
}

/**
 * 28.5 instructions/appels
 */
double FoliarComputation::functionOfFoliarDiffuse(	const int & thetaF,
													const int & phiF,
													const int & incidentDirection,
													const int & outputDirection,
													const FoliarProperty & foliarProperty) {
	double prodCosPsi = _foliarCosinePsiVector[incidentDirection][thetaF][phiF] * _foliarCosinePsiVector[outputDirection][thetaF][phiF];
	double fd = 0.0;
	if (_foliarCosinePsiVector[incidentDirection][thetaF][phiF] < 0) {
		if (_foliarCosinePsiVector[outputDirection][thetaF][phiF] < 0) {
			// C -> top transmittance
			fd = foliarProperty.transmittance;
		} else {
			// A -> top reflectance
			fd = foliarProperty.reflectanceTop;
		}
	} else {
		if (_foliarCosinePsiVector[outputDirection][thetaF][phiF] < 0) {
			// B -> bottom réflectance
			fd = foliarProperty.reflectanceBottom;
		} else {
			// D -> bottom transmittance
			fd = foliarProperty.bottomTransmittance;
		}
	}

	return fabs(prodCosPsi) * fd;
}

void FoliarComputation::computeAndStoreTgOmegas(const std::vector<DirectionCartesienne> & listDirections,
												int specularCheck,
												unsigned int modelIndex,
												unsigned int spectralBandIndex) {
	double tgsSum = 0;
	double tspecSum = 0;
	double totalSum = 0;
	if (!specularCheck) {
		for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
			_tgOmegas[spectralBandIndex][outputDirection] = 0;
			for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
				if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
					_tgOmegas[spectralBandIndex][outputDirection] += _diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
							* listDirections[incidentDirection].getSolidAngle() * _transmTransfertFunctionVector[incidentDirection];
				}
			}
		}
	} else {
		for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
			_tgOmegas[spectralBandIndex][outputDirection] = 0;
			for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
				if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
					_tgOmegas[spectralBandIndex][outputDirection] += (_diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
							+ _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]) * listDirections[incidentDirection].getSolidAngle()
							* _transmTransfertFunctionVector[incidentDirection];
				}
			}
		}

		_tgsOmegas[spectralBandIndex].resize(listDirections.size());
		tgsSum = 0;
		for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
			_tgsOmegas[spectralBandIndex][outputDirection] = 0;
			for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
				if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
					_tgsOmegas[spectralBandIndex][outputDirection] += _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
							* listDirections[incidentDirection].getSolidAngle() * _transmTransfertFunctionVector[incidentDirection];
				}
			}

			if (listDirections[outputDirection].getProperty() != DirectionRoot::VIRTUAL) {
				tgsSum += _tgsOmegas[spectralBandIndex][outputDirection];
			}
		}

		for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
			tspecSum = totalSum = 0;
			for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
				if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
					tspecSum += _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection] * listDirections[incidentDirection].getSolidAngle();
					totalSum += (_diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
							+ _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]) * listDirections[incidentDirection].getSolidAngle();
				}
			}
		}

		for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
			_tgsOmegasNormalized[spectralBandIndex][outputDirection] = _tgsOmegas[spectralBandIndex][outputDirection] / tgsSum;
		}
	}
}
void FoliarComputation::computeSpecularPolTransferFoliarFromIntegration(const std::vector<DirectionCartesienne> & listDirections,
																		const int & incidentDirection,
																		const int & outputDirection,
																		const FoliarProperty & opticalProperties,
																		const int & casPDF,
																		const unsigned int & numDiv,
																		const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
																		const unsigned int & modelIndex,
																		const unsigned int & reflectanceMode) {
	std::vector<DirectionCartesienne> outgoingDirList;
	std::vector<DirectionCartesienne>::iterator it;
	double Q = 8.0 * M_PI * _transmTransfertFunctionVector[incidentDirection];
	if (listDirections[outputDirection].getProperty() == DirectionRoot::OCCUPIED) {
		outgoingDirList.push_back(listDirections[outputDirection]);
		for (const auto & direction: listDirections) {
			if (direction.getProperty() == DirectionRoot::EMBEDDED) {
				windmillForOccupiedDirectionIntegration(outgoingDirList, direction);
			}
		}
	} else {
		outgoingDirList.push_back(listDirections[outputDirection]);
	}
	double outputSpec = 0;
	double outputPol = 0;
	std::vector<std::vector<Doublet<double> > > pointsMap = pointsListSpec.getPointsMap(outputDirection);
	std::vector<std::vector<double> > phaseSpecMap, phasePolMap;
	unsigned int n = pointsMap.size();
	phaseSpecMap.resize(n, std::vector<double>(n));
	phasePolMap.resize(n, std::vector<double>(n));
	unsigned int resultFlag = 0;
	double totalFTSpec_temp = 0;
	double totalFTPol_temp = 0;
	double distSum = 0;
	int index = 0;
	double PSIfs = 0;
	double ZFs = 0;
	double cosPSIfs = 0;
	double refractionIndex = 0;
	double K = 0;
	double roughness = 0;
	double roughnessCoefficient = 0;
	double Rspe = 0;
	double Rpola = 0;
	double gfs = 0;
	for (it = outgoingDirList.begin(); it < outgoingDirList.end(); it++) {
		resultFlag = 0;
		totalFTSpec_temp = 0;
		totalFTPol_temp = 0;
		distSum = 0;
		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < n; j++) {
				tempAngleSpecularFoliar(listDirections[incidentDirection], pointsMap[i][j], PSIfs, ZFs);
				cosPSIfs = cos(PSIfs);
				if (cosPSIfs < 0) {
					refractionIndex = opticalProperties.specularFoliarData.refractionIndexTop;
					roughness = opticalProperties.specularFoliarData.roughnessTop;
					roughnessCoefficient = opticalProperties.specularFoliarData.roughnessCoefficientTop;
				} else {
					refractionIndex = opticalProperties.specularFoliarData.refractionIndexBottom;
					roughness = opticalProperties.specularFoliarData.roughnessBottom;
					roughnessCoefficient = opticalProperties.specularFoliarData.roughnessCoefficientBottom;
				}
				computeFresnelCoefficientsSpecPol(Rspe, Rpola, PSIfs, refractionIndex);
				K = PhaseUtils::computeRoughness(PSIfs, roughness, roughnessCoefficient);
				index = (int) floor(ZFs);
				if (index == 0) {
					gfs = 0.;
				} else {
					gfs = _leafAngleDistributionFunctionVector[index] / PhaseUtils::getStaticSine(index * 2);
				}


				phaseSpecMap[i][j] = gfs * K * Rspe;
				phasePolMap[i][j] = gfs * K * Rpola;
			}

		}

		if (resultFlag == 1) {
			outputSpec += totalFTSpec_temp / distSum;
			outputPol += totalFTPol_temp / distSum;
		} else {
			NewtonCotesIntegrationCalculator<double> integrationSpec(phaseSpecMap, pointsListSpec.getDeltaCosTheta(outputDirection),
				pointsListSpec.getDeltaPhi(outputDirection));
			integrationSpec.calculateIntegration2D();
			outputSpec += integrationSpec.getOutput();
			integrationSpec.setInput(phasePolMap);
			integrationSpec.setDeltaX(pointsListSpec.getDeltaCosTheta(outputDirection));
			integrationSpec.setDeltaY(pointsListSpec.getDeltaPhi(outputDirection));
			integrationSpec.calculateIntegration2D();
			outputPol += integrationSpec.getOutput();
		}
	}

	_specularTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = outputSpec / Q;
	_polTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = outputPol / Q;
}

void FoliarComputation::foliarPolaSpecTranferFunction(	const std::vector<DirectionCartesienne> & listDirections,
														const int & incidentDirection,
														const int & outputDirection,
														const FoliarProperty & opticalProperties,
														const int & casPDF,
														const unsigned int & modelIndex,
														const unsigned int & reflectanceMode) {
	double PSIfs = 0;
	double ZFs = 0;
	double refractionIndex = 0;
	double roughness = 0, roughnessIndex = 0, Rspe = 0, Rpola = 0;
	Doublet<double> inputV(listDirections[outputDirection].getCenterTheta(), listDirections[outputDirection].getCenterPhi());
	tempAngleSpecularFoliar(listDirections[incidentDirection], inputV, PSIfs, ZFs);
	double cosPSIfs = cos(PSIfs);
	if (cosPSIfs < 0) {
		refractionIndex = opticalProperties.specularFoliarData.refractionIndexTop;
		roughness = opticalProperties.specularFoliarData.roughnessTop;
		roughnessIndex = opticalProperties.specularFoliarData.roughnessCoefficientTop;
	} else {
		refractionIndex = opticalProperties.specularFoliarData.refractionIndexBottom;
		roughness = opticalProperties.specularFoliarData.roughnessBottom;
		roughnessIndex = opticalProperties.specularFoliarData.roughnessCoefficientBottom;
	}
	computeFresnelCoefficientsSpecPol(Rspe, Rpola, PSIfs, refractionIndex);
	double K = PhaseUtils::computeRoughness(PSIfs, roughness, roughnessIndex);
	int index = (int) floor(ZFs);
	double gfs = 0;
	if (index != 0) {
		double gfs = _leafAngleDistributionFunctionVector[index] / PhaseUtils::getStaticSine(index * 2);
	}
	double Q = 8.0 * M_PI * _transmTransfertFunctionVector[incidentDirection];
	if (casPDF == REELLE || fabs(gfs) < EPSILON) {
		_specularTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = gfs * K * Rspe
				* listDirections[outputDirection].getSolidAngle() / Q;
		_polTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = gfs * K * Rpola
				* listDirections[outputDirection].getSolidAngle() / Q;
	} else {
		_specularTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = K * Rspe / Q;
		_polTransfertFunctionVector[reflectanceMode][incidentDirection][outputDirection] = K * Rpola / Q;
	}
}

void FoliarComputation::tempAngleSpecularFoliar(const DirectionCartesienne & incidentDir,
												const Doublet<double> & thePhi,
												double & PSIfs,
												double & ZFs) {
	double CosIV = 0;
	double CosZFs = 0;
	double cosZI = 0;
	double cosZV = 0;
	Vecteur<double> xYZ = sphericalToCartesian(thePhi.getX(), thePhi.getY());
	CosIV = incidentDir.getDirectionCartesienne().scalaire(xYZ);
	if (CosIV > 1.0) {
		CosIV = 1.0;
	} else if (CosIV < -1.0) {
		CosIV = -1.0;
	}

	PSIfs = acos(-CosIV) / 2.0;
	if (incidentDir.getCenterTheta() == thePhi.getX() && incidentDir.getCenterPhi() == thePhi.getY()) {
		if (incidentDir.getCenterTheta() < 90.0) {
			ZFs = 90.0 - incidentDir.getCenterTheta();
		} else {
			ZFs = incidentDir.getCenterTheta() - 90.0;
		}
	} else {
		cosZI = incidentDir.getZ();
		cosZV = xYZ.getZ();
		CosZFs = fabs(cosZI - cosZV) / (2.0 * cos(PSIfs));
		if (CosZFs > 1) {
			CosZFs = 1.0;
		}
		if (CosZFs < 0) {
			CosZFs = 0.0;
		}
		ZFs = acos(CosZFs) / CONV_DEG_RADIAN;
	}
}

Vecteur<double> FoliarComputation::sphericalToCartesian(const double & zenith,
														const double & azimuth) {
	double cosineZenith = cos(zenith);
	double cosineAzimuth = cos(azimuth);
	double sineZenith = sin(zenith);
	double sineAzimuth = sin(azimuth);
	Vecteur<double> cartesien(sineZenith * cosineAzimuth, sineZenith * sineAzimuth, cosineZenith);
	cartesien.normaliserEgal();
	return cartesien;
}

void FoliarComputation::computeFresnelCoefficientsSpecPol(	double & Rspe,
															double & Rpola,
															double & PSIfs,
															double & refractionIndex) {
	double n2 = pow(refractionIndex, 2);
	double aa = sqrt(n2 - pow(sin(PSIfs), 2));
	double cosPSIfs = cos(PSIfs);
	double Rh = pow((cosPSIfs - aa), 2) / pow((cosPSIfs + aa), 2);
	double Rv = pow((n2 * cosPSIfs - aa), 2) / pow((n2 * cosPSIfs + aa), 2);
	Rspe = (Rh + Rv) * 0.5;
	Rpola = fabs(Rh - Rv) * 0.5;
}

void FoliarComputation::windmillForOccupiedDirectionIntegration(std::vector<DirectionCartesienne> & dirList,
																const DirectionRoot & emDir) {
	std::vector<DirectionCartesienne>::iterator it;
	std::vector<DirectionCartesienne> addedDirections;
	for (it = dirList.begin(); it < dirList.end(); ++it) {
		DirectionRoot joint;
		if (it->findJoint(emDir, joint)) {
			if (joint.getBeginTheta() > (*it).getBeginTheta()) {
				DirectionCartesienne sub1((*it).getBeginTheta(), joint.getBeginTheta(), joint.getBeginPhi(), (*it).getEndPhi());
				if (sub1.getSolidAngle() > 0)
					addedDirections.push_back(sub1);

			}
			if (joint.getEndPhi() != (*it).getEndPhi()) {
				DirectionCartesienne sub2(joint.getBeginTheta(), (*it).getEndTheta(), joint.getEndPhi(), (*it).getEndPhi());
				if (sub2.getSolidAngle() > 0)
					addedDirections.push_back(sub2);

			}
			if (joint.getEndTheta() < (*it).getEndTheta()) {
				DirectionCartesienne sub3(joint.getEndTheta(), (*it).getEndTheta(), (*it).getBeginPhi(), joint.getEndPhi());
				if (sub3.getSolidAngle() > 0)
					addedDirections.push_back(sub3);

			}
			if (joint.getBeginPhi() != (*it).getBeginPhi()) {
				DirectionCartesienne sub4((*it).getBeginTheta(), joint.getEndTheta(), (*it).getBeginPhi(), joint.getBeginPhi());
				if (sub4.getSolidAngle() > 0)
					addedDirections.push_back(sub4);

			}
			dirList.erase(it);
			it--;
		}

	}

	for (const auto & addDir: addedDirections) {
		dirList.push_back(addDir);
	}
}

void FoliarComputation::calcDotPSIIntegrateSA(	const std::vector<std::vector<Vecteur<double> > > & dirsTPMatrix,
												const SphereDiscretizationMap & sphereMap,
												std::vector<std::vector<double> > & cosinePsiSaVector,
												const double & DELTA_AF) {
	if (dirsTPMatrix.size() == 0 || dirsTPMatrix[0].size() == 0) {
		std::cerr << "Error: calcCosPSIIntegrateSA: /n The input has 0 dimension." << std::endl;
		abort();
	}
	unsigned int numTheta = dirsTPMatrix.size();
	unsigned int numPhi = dirsTPMatrix[0].size();
	cosinePsiSaVector.resize(numTheta * numPhi, vector<double>(sphereMap.getSizeTheta() * sphereMap.getSizePhi()));
	for (unsigned int i = 0; i < numTheta; i++) {
		for (unsigned int j = 0; j < numPhi; j++) {
			sphereMap.generateScalerProductMap(cosinePsiSaVector[j * numTheta + i], dirsTPMatrix[i][j]);
		}
	}

}

void FoliarComputation::generatePhaseMapDiff(	const std::vector<std::vector<double> > & cosinePsiSaVector,
												const unsigned int numberOfThetaSubdivisions,
												const unsigned int numberOfPhiSubdivisions,
												const int & incidentDirection,
												const FoliarProperty & opticalProperties,
												const SphereDiscretizationMap & sphereMap,
												std::vector<std::vector<double> > & phaseMap,
												const double & iterationStepPhi,
												const double & iterationStepTheta,
												const unsigned int & modelIndex) {
	if (cosinePsiSaVector.size() == 0 || cosinePsiSaVector[0].size() == 0) {
		std::cerr << "Error: calcCosPSIIntegrateSA: /n The input has 0 dimension." << std::endl;
		abort();
	}
	phaseMap.resize(numberOfThetaSubdivisions, std::vector<double>(numberOfPhiSubdivisions));
	unsigned int numberOfZenith = sphereMap.getSizeTheta();
	unsigned int numberOfAzimuth = sphereMap.getSizePhi();
	double cosPsifs = 0.0;
	double cosPsifvPoint = 0.0;
	double prod_cosPsi = 0.0;
	double fd = 0.0;
	double yy = 0.0;
	for (unsigned int i = 0; i < numberOfThetaSubdivisions; i++) {
		for (unsigned int j = 0; j < numberOfPhiSubdivisions; j++) {
			cosPsifs = 0.0;
			cosPsifvPoint = 0.0;
			prod_cosPsi = 0.0;
			fd = 0.0;
			yy = 0.0;
			for (unsigned int zenithAngle = 0; zenithAngle < numberOfZenith; zenithAngle++) {
				for (unsigned int azimuthAngle = 0; azimuthAngle < numberOfAzimuth; azimuthAngle++) {
					cosPsifs = _foliarCosinePsiVector[incidentDirection][zenithAngle][azimuthAngle];
					cosPsifvPoint = cosinePsiSaVector[j * numberOfThetaSubdivisions + i][azimuthAngle * numberOfZenith + zenithAngle];
					prod_cosPsi = cosPsifs * cosPsifvPoint;
					if (cosPsifs > 0) {
						if (cosPsifvPoint > 0) {
							// C -> top transmittance
							fd = opticalProperties.transmittance;
						} else {
							// A -> top reflectance
							fd = opticalProperties.reflectanceTop;
						}
					} else {
						if (cosPsifvPoint > 0) {
							// B -> bottom réflectance
							fd = opticalProperties.reflectanceBottom;
						} else {
							// D -> bottom transmittance
							fd = opticalProperties.bottomTransmittance;
						}
					}
					yy += fabs(prod_cosPsi) * fd * _leafAngleDistributionFunctionVector[zenithAngle];
				}
			}
			if (_transmTransfertFunctionVector[incidentDirection] < EPSILON) {
				yy *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta;
			} else {
				yy *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta
						/ (_transmTransfertFunctionVector[incidentDirection] * 360.0 * M_PI);
			}
			phaseMap[i][j] = yy;
		}
	}
}

void FoliarComputation::generatePhaseMapFluo(	const std::vector<std::vector<double> > & cosinePsiSaVector,
												const unsigned int numberOfThetaSubdivisions,
												const unsigned int numberOfPhiSubdivisions,
												const int & incidentDirection,
												const FoliarProperty & opticalProperties,
												const SphereDiscretizationMap & sphereMap,
												std::vector<std::vector<double> > & phaseMapPSI,
												std::vector<std::vector<double> > & phaseMapPSII,
												const unsigned int & emissionBandInex,
												const double & iterationStepPhi,
												const double & iterationStepTheta,
												const unsigned int & modelIndex) {
	if (cosinePsiSaVector.size() == 0 || cosinePsiSaVector[0].size() == 0) {
		std::cerr << "Error: calcCosPSIIntegrateSA: /n The input has 0 dimension." << std::endl;
		abort();
	}
//	unsigned int numberOfDivisionsOfPhiTheta = cosinePsiSaVector.size();
	phaseMapPSI.resize(numberOfThetaSubdivisions, std::vector<double>(numberOfPhiSubdivisions));
	phaseMapPSII.resize(numberOfThetaSubdivisions, std::vector<double>(numberOfPhiSubdivisions));

	unsigned int numberOfZenith = sphereMap.getSizeTheta();
	unsigned int numberOfAzimuth = sphereMap.getSizePhi();
	double cosPsifs = 0.0;
	double cosPsifvPoint = 0.0;
	double prod_cosPsi = 0.0;
	double fdPSI = 0.0;
	double yyPSI = 0.0;
	double fdPSII = 0.0;
	double yyPSII = 0.0;
	for (unsigned int i = 0; i < numberOfThetaSubdivisions; i++) {
		for (unsigned int j = 0; j < numberOfPhiSubdivisions; j++) {
			cosPsifs = 0.0;
			cosPsifvPoint = 0.0;
			prod_cosPsi = 0.0;
			fdPSI = 0.0;
			yyPSI = 0.0;
			fdPSII = 0.0;
			yyPSII = 0.0;
			for (unsigned int zenithAngle = 0; zenithAngle < numberOfZenith; zenithAngle++) {
				for (unsigned int azimuthAngle = 0; azimuthAngle < numberOfAzimuth; azimuthAngle++) {
					cosPsifs = _foliarCosinePsiVector[incidentDirection][zenithAngle][azimuthAngle];
					cosPsifvPoint = cosinePsiSaVector[j * numberOfThetaSubdivisions + i][azimuthAngle * numberOfZenith + zenithAngle];
					prod_cosPsi = cosPsifs * cosPsifvPoint;
					if (cosPsifs > 0) {
						if (cosPsifvPoint > 0) {
							// C -> top transmittance
							fdPSI = opticalProperties.topPSIForward[emissionBandInex];
							fdPSII = opticalProperties.topPSIIForward[emissionBandInex];
						} else {
							// A -> top reflectance
							fdPSI = opticalProperties.topPSIBackward[emissionBandInex];
							fdPSII = opticalProperties.topPSIIBackward[emissionBandInex];
						}
					} else {
						if (cosPsifvPoint > 0) {
							// B -> bottom réflectance
							fdPSI = opticalProperties.bottomPSIBackward[emissionBandInex];
							fdPSII = opticalProperties.bottomPSIIBackward[emissionBandInex];
						} else {
							// D -> bottom transmittance
							fdPSI = opticalProperties.bottomPSIForward[emissionBandInex];
							fdPSII = opticalProperties.bottomPSIIForward[emissionBandInex];
						}
					}
					yyPSI += fabs(prod_cosPsi) * fdPSI * _leafAngleDistributionFunctionVector[zenithAngle];
					yyPSII += fabs(prod_cosPsi) * fdPSII * _leafAngleDistributionFunctionVector[zenithAngle];
				}
			}
			if (_transmTransfertFunctionVector[incidentDirection] < EPSILON) {
				yyPSI *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta;
				yyPSII *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta;
			} else {
				yyPSI *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta
						/ (_transmTransfertFunctionVector[incidentDirection] * 360.0 * M_PI);
				yyPSII *= _clumpingIndex[incidentDirection] * iterationStepPhi * iterationStepTheta
						/ (_transmTransfertFunctionVector[incidentDirection] * 360.0 * M_PI);
			}
			phaseMapPSI[i][j] = yyPSI;
			phaseMapPSII[i][j] = yyPSII;
		}
	}
}

void FoliarComputation::testAlbedo(	const unsigned int & spectralBandIndex,
									const unsigned int & modelIndex) {
	if (_albedoVector[spectralBandIndex] > 1.0001) {
		std::cerr << "Warning : foliar albedo > 1 for phase function " << modelIndex << " and band " << spectralBandIndex << " ["
				<< _albedoVector[spectralBandIndex] << "] "
				<< " ; Resetted to 1 to avoid problems in radiative transfer computation (multiple scattering)" << std::endl;
		_albedoVector[spectralBandIndex] = 1;
	}
	if (_albedoSpecVector[spectralBandIndex] > 1.0001) {
		std::cerr << "Warning : foliar specular albedo > 1 for phase function " << modelIndex << " and band " << spectralBandIndex
				<< " ; Resetted to 1 to avoid problems in radiative transfer computation (multiple scattering)" << std::endl;
		_albedoSpecVector[spectralBandIndex] = 1;
	}
}

double FoliarComputation::getAlbedo(const unsigned int & spectralBandIndex) {
	return _albedoVector[spectralBandIndex];
}

double FoliarComputation::getAlbedoSpec(const unsigned int & spectralBandIndex) {
	return _albedoSpecVector[spectralBandIndex];
}

void FoliarComputation::writeFoliar(const unsigned int& modelIndex,
									const unsigned int& spectralBandIndex,
									const std::string & modelName,
									PhaseKeyMappingGenerator & pkmg,
									std::ofstream & outCoeffSpecProperties,
									std::vector<FoliarProperty> & foliarPropertyVector) {
	pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	pkmg.setBandeSpectrale(spectralBandIndex);
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << std::endl;
	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << std::endl;
	outCoeffSpecProperties << pkmg.cleOmegaMin() << _foliarModel.getFoliarOmegaMin() << std::endl;
	outCoeffSpecProperties << pkmg.cleOmegaMax() << _foliarModel.getFoliarOmegaMax() << std::endl;
	outCoeffSpecProperties << pkmg.cleClumpingA() << _foliarModel.getFoliarClumpingA() << std::endl;
	outCoeffSpecProperties << pkmg.cleClumpingB() << _foliarModel.getFoliarClumpingB() << std::endl;
	outCoeffSpecProperties << pkmg.cleLAD() << foliarPropertyVector[spectralBandIndex].typeOfLeafAngleDistribution << std::endl;

	if (foliarPropertyVector[spectralBandIndex].typeOfLeafAngleDistribution == globalReferences::ELLIPS) {
		outCoeffSpecProperties << pkmg.cleALA() << _foliarModel.getFoliarAverageLeafAngle() << std::endl;
	} else if (foliarPropertyVector[spectralBandIndex].typeOfLeafAngleDistribution == globalReferences::ELLIPT) {
		outCoeffSpecProperties << pkmg.cleThetaM() << _foliarModel.getFoliarEllipsePrincipalAngle() << std::endl;
		outCoeffSpecProperties << pkmg.cleEccentricity() << _foliarModel.getFoliarEllipseEccentricity() << std::endl;
	}
	outCoeffSpecProperties << pkmg.cleDimensionFoliaire() << foliarPropertyVector[spectralBandIndex].foliarDimension << std::endl;
	outCoeffSpecProperties << pkmg.cleRoSuperieur() << foliarPropertyVector[spectralBandIndex].reflectanceTop << std::endl;
	outCoeffSpecProperties << pkmg.cleRoInferieur() << foliarPropertyVector[spectralBandIndex].reflectanceBottom << std::endl;
	outCoeffSpecProperties << pkmg.cleTo() << foliarPropertyVector[spectralBandIndex].transmittance << std::endl;
	outCoeffSpecProperties << pkmg.cleToInf() << foliarPropertyVector[spectralBandIndex].bottomTransmittance << std::endl;
	outCoeffSpecProperties << pkmg.isSpecularKey() << foliarPropertyVector[spectralBandIndex].specularFoliarData._isSpecular << std::endl;
	if (foliarPropertyVector[spectralBandIndex].specularFoliarData._isSpecular) {
		outCoeffSpecProperties << pkmg.cleNinf() << foliarPropertyVector[spectralBandIndex].specularFoliarData.refractionIndexBottom << std::endl;
		outCoeffSpecProperties << pkmg.cleNsup() << foliarPropertyVector[spectralBandIndex].specularFoliarData.refractionIndexTop << std::endl;
		outCoeffSpecProperties << pkmg.cleKoInf() << foliarPropertyVector[spectralBandIndex].specularFoliarData.roughnessBottom << std::endl;
		outCoeffSpecProperties << pkmg.cleIndiceRugositeInf() << foliarPropertyVector[spectralBandIndex].specularFoliarData.roughnessCoefficientBottom
				<< std::endl;
		outCoeffSpecProperties << pkmg.cleKoSup() << foliarPropertyVector[spectralBandIndex].specularFoliarData.roughnessTop << std::endl;
		outCoeffSpecProperties << pkmg.cleIndiceRugositeSup() << foliarPropertyVector[spectralBandIndex].specularFoliarData.roughnessCoefficientTop
				<< std::endl;
	} else {
		outCoeffSpecProperties << pkmg.cleNinf() << 0 << std::endl;
		outCoeffSpecProperties << pkmg.cleNsup() << 0 << std::endl;
		outCoeffSpecProperties << pkmg.cleKoInf() << 0 << std::endl;
		outCoeffSpecProperties << pkmg.cleIndiceRugositeInf() << 0 << std::endl;
		outCoeffSpecProperties << pkmg.cleKoSup() << 0 << std::endl;
		outCoeffSpecProperties << pkmg.cleIndiceRugositeSup() << 0 << std::endl;
	}
	outCoeffSpecProperties << pkmg.cleAlbedo() << getAlbedo(spectralBandIndex) << std::endl;
	if (foliarPropertyVector[spectralBandIndex].specularFoliarData._isSpecular) {
		outCoeffSpecProperties << pkmg.cleAlbedoSpec() << getAlbedoSpec(spectralBandIndex) << std::endl;
	}
}

void FoliarComputation::precomputeFoliarNormalisation(	const std::vector<DirectionCartesienne> & listDirections,
														const double & iterationStepPhi,
														const double & iterationStepTheta) {
	for (int incidentDirection = 0; incidentDirection < _numberOfDirections + 1; incidentDirection++) {
		for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
			double solidAngleCorrection = listDirections[outputDirection].getCosThetaDOmega()
					/ (std::cos(listDirections[outputDirection].getCenterTheta()) * listDirections[outputDirection].getSolidAngle());
			_foliarPrecomputedNormalisation[incidentDirection][outputDirection] = listDirections[outputDirection].getSolidAngle()
					* solidAngleCorrection * _clumpingIndex[incidentDirection] * iterationStepPhi
					/ (_transmTransfertFunctionVector[incidentDirection] * 360.0 * M_PI);
		}
	}
}

void FoliarComputation::computeFoliarCosinePsi(	const std::vector<DirectionCartesienne> & listOfDirections,
												const int & iterationStepPhi,
												const int & iterationStepTheta) {
	double tempDouble = 0;
	_foliarCosinePsiVector.resize(_numberOfDirections + 1, std::vector<std::vector<double> >(
	NBMAX_TETAF / iterationStepTheta + 1, std::vector<double>((PHI_MAX / iterationStepPhi) + 1)));
	for (int direction = 0; direction < _numberOfDirections + 1; direction++) {
		//Pour chaque zenith foliaire
		for (int zenithal_foliaire = 0; zenithal_foliaire < NBMAX_TETAF; zenithal_foliaire += iterationStepTheta) {
			//Pour chaque azimuth foliaire
			for (int azimuthal_foliaire = 0; azimuthal_foliaire <= PHI_MAX; azimuthal_foliaire += iterationStepPhi) {
				// on integre entre 0 et 90 par pas de 1degre, or, l'integrale a ete divisee en deux parties, il faut donc prendre l'angle zenithal foliaire+0.5
				tempDouble = PhaseUtils::conv_direction((double) zenithal_foliaire + 0.5, (double) azimuthal_foliaire).scalaire(
					listOfDirections[direction].getDirectionCartesienne());
				_foliarCosinePsiVector[direction][zenithal_foliaire / iterationStepTheta][azimuthal_foliaire / iterationStepPhi] = tempDouble;
			}
		}
	}
}

void FoliarComputation::computeFoliarHotSpotDirectionalFactor(	unsigned int modelIndex,
																const std::vector<DirectionCartesienne> & listDirections,
																const int & iterationStepPhi,
																const int & iterationStepTheta) {
	double sumB, sumC, gfs, hotSpotBFactor = 0., hotSpotCFactor = 0.;
	_hotSpotFactor.resize(_numberOfDirections);
	for (int dirIndex = 0; dirIndex < _numberOfDirections; ++dirIndex) {
		hotSpotBFactor = 0.;
		hotSpotCFactor = 0.;
		for (int thetaLeaf = 0; thetaLeaf < NBMAX_TETAF; thetaLeaf += iterationStepTheta) {
//			unsigned int thetaLADIndex = floor(thetaLeaf);
			gfs = (_gFunctionForLadComputationFunctionVector[thetaLeaf * iterationStepTheta]) * M_PI / 180;

			sumB = 0;
			sumC = 0;
			for (int phiLeaf = 0; phiLeaf <= PHI_MAX; phiLeaf += iterationStepPhi) {
				double sunLeafCosinePsi = _foliarCosinePsiVector[_numberOfDirections][thetaLeaf / iterationStepTheta][phiLeaf / iterationStepPhi];
				double viewLeafCosinePsi = _foliarCosinePsiVector[dirIndex][thetaLeaf / iterationStepTheta][phiLeaf / iterationStepPhi];
				sumB += abs(sunLeafCosinePsi * sunLeafCosinePsi * viewLeafCosinePsi);
				sumC += abs(sunLeafCosinePsi * viewLeafCosinePsi);
			}
			sumB *= gfs / (PHI_MAX / iterationStepPhi);
			sumC *= gfs / (PHI_MAX / iterationStepPhi);
			hotSpotBFactor += sumB;
			hotSpotCFactor += sumC;
		}
		_hotSpotFactor[dirIndex] = (hotSpotBFactor * (1. - 2 * _transmTransfertFunctionVector[_numberOfDirections]) + hotSpotCFactor * (2. * _transmTransfertFunctionVector[_numberOfDirections] * _transmTransfertFunctionVector[_numberOfDirections] - 1)) / (_transmTransfertFunctionVector[_numberOfDirections] * (_transmTransfertFunctionVector[_numberOfDirections] - 1.) * _transmTransfertFunctionVector[dirIndex]);
	}
}

void FoliarComputation::computeFRFoliarSpecularForModelIndex(	PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
																PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
																const std::vector<DirectionCartesienne> & listDirections,
																FTFoliar & fTFoliar,
																unsigned int modelIndex,
																const FoliarProperty & foliarProperty) {
	for (int incidentDirection = 0; incidentDirection < listDirections.size() + 1; incidentDirection++) {
		_transmTransfertFunctionVector[incidentDirection] = 0;
	}
	fTFoliar.ifDiffIntegration = false;
	if (fTFoliar.listDirectionsSize <= 80 || fTFoliar.casPDF == 2) {
		fTFoliar.ifDiffIntegration = true;
	}
	fTFoliar.ifSpecIntegration = false;
	if (foliarProperty.specularFoliarData._isSpecular && (fTFoliar.casPDF == REELLE)) {
		fTFoliar.ifSpecIntegration = true;
	}
	if (fTFoliar.ifSpecIntegration) {
		std::cout << "    Number of points used for Newton-Cotes integration (Specular and Polarized Phase Function): " << fTFoliar.numDivSpecPol
				<< std::endl;
		pointsListSpec = PointsListsNCIntegration<DirectionCartesienne>(listDirections, fTFoliar.numDivSpecPol,true);
	}
	if (fTFoliar.ifDiffIntegration) {
		std::cout << "    Number of points used for Newton-Cotes integration (Diffused Phase Function): " << fTFoliar.numDivDiff << std::endl;
		pointsListDiff = PointsListsNCIntegration<DirectionCartesienne>(listDirections, fTFoliar.numDivDiff,true);
	}
	double zenithAngleInDegrees = 0;
	double azimuthAngleInDegrees = 0;
	for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
		if (foliarProperty.typeOfLeafAngleDistribution == SPHERIC_CASE) //CESB-TT-201:correction typeOfLeafAngleDistribution spherique
		{
			_transmTransfertFunctionVector[incidentDirection] = 0.5;
		} else if (foliarProperty.typeOfLeafAngleDistribution == globalReferences::VERTI) {
			_transmTransfertFunctionVector[incidentDirection] = fabs(2. / M_PI * sin(listDirections[incidentDirection].getCenterTheta()));
		} else if (foliarProperty.typeOfLeafAngleDistribution == globalReferences::HORIZ) {
			_transmTransfertFunctionVector[incidentDirection] = fabs(cos(listDirections[incidentDirection].getCenterTheta()));
		} else {
			const DirectionCartesienne & inDir = listDirections[incidentDirection];
			zenithAngleInDegrees = _rad2Deg * (inDir.getCenterTheta());
			azimuthAngleInDegrees = _rad2Deg * (inDir.getCenterPhi());
			transmissionTransferFunction(zenithAngleInDegrees, azimuthAngleInDegrees, incidentDirection, modelIndex);
		}
	}
}

void FoliarComputation::writeTransmittance(	const FTFoliar & fTFoliar,
											const std::vector<DirectionCartesienne> & listDirections,
											const int & rtm,
											const unsigned int & accelEngine,
											const bool &isSpecular,
											const string & pathPhaseTransm,
											const string & pathPhaseDiff,
											const string & pathPhaseSpec,
											const string & pathHotSpot,
											const string & pathFluorescence) {
	std::fstream _ofstreamTransmittance, _ofstreamDiffuseTransmittance, _ofstreamSpecularTransmittance, ofstreamFluorescence;
	std::fstream _ofstreamSpecularTransmittance_mc, _ofstreamDiffuseTransmittance_mc, _ofstreamDiffuseHotSpot, ofstreamFluorescence_mc;
	// Writer buffer
	std::vector<double> vecBuff;

	_ofstreamTransmittance.open(pathPhaseTransm.c_str(), std::fstream::out | std::fstream::binary);
	if (accelEngine != 2) {
		_ofstreamDiffuseTransmittance.open(pathPhaseDiff.c_str(), std::fstream::out | std::fstream::binary);
		_ofstreamDiffuseHotSpot.open(pathHotSpot.c_str(), std::fstream::out);
	}

	if (isSpecular) {
		_ofstreamSpecularTransmittance.open(pathPhaseSpec.c_str(), std::fstream::out | std::fstream::binary);
	}
	if ((_topFluorescenceComputation || _bottomFluorescenceComputation) && accelEngine != 2) {
		ofstreamFluorescence.open(pathFluorescence.c_str(), std::fstream::out | std::fstream::binary);
	}
	if (rtm > 0 || accelEngine == 2) {
		string pathPhaseSpecmc = pathPhaseSpec + "_mc";
		string pathPhaseDiffMc = pathPhaseDiff + "_mc";

		if (accelEngine != 2) {
			_ofstreamSpecularTransmittance_mc.open(pathPhaseSpecmc.c_str(), std::fstream::out | std::fstream::binary);
		}
		_ofstreamDiffuseTransmittance_mc.open(pathPhaseDiffMc.c_str(), std::fstream::out | std::fstream::binary);
		if (!_ofstreamSpecularTransmittance_mc || !_ofstreamDiffuseTransmittance_mc) {
			cerr << "Impossible d'ouvrir les fichiers : " << pathPhaseSpec << endl << pathPhaseDiff << endl << " du repertoire lib_phase" << endl;
			exit(EXIT_FAILURE);
		}
		if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
			ofstreamFluorescence_mc.open((pathFluorescence + "_mc").c_str(), std::fstream::out | std::fstream::binary);
		}
	}

	for (const auto & value : _hotSpotFactor) {
		_ofstreamDiffuseHotSpot << value << endl;
	}
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
		if (spectralBandIndex == 0) {
			_ofstreamTransmittance.write((char *) _transmTransfertFunctionVector.data(), sizeof(double)*(_numberOfDirections+1));
		}
		_ofstreamDiffuseTransmittance.write((char *) _diffuseTransfertFunctionVector[spectralBandIndex][_numberOfDirections].data(), sizeof(double)*_numberOfDirections);

		if ((_topFluorescenceComputation || _bottomFluorescenceComputation) && accelEngine != 2) {
			// Solar direction
			for (unsigned int exitSpectralBandIndex = 0; exitSpectralBandIndex < _numberOfSpectralBands; exitSpectralBandIndex++) {
				vecBuff.clear();
				vecBuff.resize(2*_numberOfDirections);
				for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
					vecBuff[2*outputDirection] = _fluorescenceDirectionTransferMatrixPSI[spectralBandIndex * _numberOfSpectralBands + exitSpectralBandIndex][_numberOfDirections * (_numberOfDirections + 1) + outputDirection];
					vecBuff[2*outputDirection+1] = _fluorescenceDirectionTransferMatrixPSII[spectralBandIndex * _numberOfSpectralBands + exitSpectralBandIndex][_numberOfDirections * (_numberOfDirections + 1) + outputDirection];
				}
				ofstreamFluorescence.write((char *) vecBuff.data(), sizeof(double)*2*_numberOfDirections);
			}
		}
		if (isSpecular) {
			_ofstreamSpecularTransmittance.write((char *) _specularTransfertFunctionVector[spectralBandIndex][_numberOfDirections].data(), sizeof(double)*_numberOfDirections);
			_ofstreamSpecularTransmittance.write((char *) _polTransfertFunctionVector[spectralBandIndex][_numberOfDirections].data(),sizeof(double)*_numberOfDirections);
		}

		if (fTFoliar.monteCarloCheck > 0 || accelEngine == 2) {
			for (int incidentDirection = 0; incidentDirection <= _numberOfDirections; incidentDirection++) {
				_ofstreamDiffuseTransmittance_mc.write((char *) _diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection].data(), sizeof(double)*(1+_numberOfDirections));
			}
			if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
				for (unsigned int exitSpectralBandIndex = 0; exitSpectralBandIndex < _numberOfSpectralBands; exitSpectralBandIndex++) {
					for (int incidentDirection = 0; incidentDirection <= _numberOfDirections; incidentDirection++) {
						vecBuff.clear();
						vecBuff.resize(2*_numberOfDirections);
						for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
							vecBuff[2*outputDirection] = _fluorescenceDirectionTransferMatrixPSI[spectralBandIndex * _numberOfSpectralBands + exitSpectralBandIndex][incidentDirection * (_numberOfDirections + 1) + outputDirection];
							vecBuff[2*outputDirection+1] = _fluorescenceDirectionTransferMatrixPSII[spectralBandIndex * _numberOfSpectralBands + exitSpectralBandIndex][incidentDirection * (_numberOfDirections + 1) + outputDirection];
						}
						ofstreamFluorescence_mc.write((char *) vecBuff.data(), sizeof(double)*(2*_numberOfDirections));
					}
				}
			}
			if (isSpecular) {
				for (int incidentDirection = 0; incidentDirection <= _numberOfDirections; incidentDirection++) {
					_ofstreamSpecularTransmittance_mc.write((char *) _specularTransfertFunctionVector[spectralBandIndex][incidentDirection].data(), sizeof(double)*(1+_numberOfDirections));
				}
				for (int incidentDirection = 0; incidentDirection <= _numberOfDirections; incidentDirection++) {
					_ofstreamSpecularTransmittance_mc.write((char *) _polTransfertFunctionVector[spectralBandIndex][incidentDirection].data(), sizeof(double)*(1+_numberOfDirections));
				}
			}
		}
		if (!isSpecular) {
			_ofstreamDiffuseTransmittance.write((char *) _tgOmegas[spectralBandIndex].data(), sizeof(double)*_numberOfDirections);
		} else {
			for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
				_tgOmegas[spectralBandIndex][outputDirection] = 0;
			}
			_ofstreamSpecularTransmittance.write((char *) _tgOmegas[spectralBandIndex].data(), sizeof(double)*_numberOfDirections);
			_ofstreamSpecularTransmittance.write((char *) _tgsOmegas[spectralBandIndex].data(), sizeof(double)*_numberOfDirections);

			vecBuff.clear();
			vecBuff.resize(_numberOfDirections);
			for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
				vecBuff[outputDirection] = _specularTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]
																													/ (_specularTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]
																															+ _diffuseTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]);
			}
			_ofstreamSpecularTransmittance.write((char *) vecBuff.data(), sizeof(double)*_numberOfDirections);
			double tspecSum = 0;
			double totalSum = 0;
			double ratio = 0;
			vecBuff.clear();
			vecBuff.resize(_numberOfDirections);
			for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
				tspecSum = totalSum = 0;
				for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
					if (listDirections[incidentDirection].getProperty() != DirectionRoot::VIRTUAL) {
						tspecSum += _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
								* listDirections[incidentDirection].getSolidAngle();
						totalSum += (_diffuseTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection]
								+ _specularTransfertFunctionVector[spectralBandIndex][incidentDirection][outputDirection])
								* listDirections[incidentDirection].getSolidAngle();
					}
				}
				vecBuff[outputDirection] = tspecSum / totalSum;
				//ratio = tspecSum / totalSum;
			}
			_ofstreamSpecularTransmittance.write((char *) vecBuff.data(), sizeof(double)*listDirections.size());

			_ofstreamSpecularTransmittance.write((char *) _tgsOmegasNormalized[spectralBandIndex].data(), sizeof(double)*_numberOfDirections);
			vecBuff.clear();
			vecBuff.resize(_numberOfDirections);
			for (int outputDirection = 0; outputDirection < _numberOfDirections; outputDirection++) {
				vecBuff[outputDirection] = _polTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]
						/ (_specularTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]
								+ _diffuseTransfertFunctionVector[spectralBandIndex][_numberOfDirections][outputDirection]);
			}
			_ofstreamSpecularTransmittance.write((char *) vecBuff.data(), sizeof(double)*_numberOfDirections);
		}
	}

	_ofstreamTransmittance.close();
	_ofstreamDiffuseTransmittance.close();
	if (isSpecular) {
		_ofstreamSpecularTransmittance.close();
	}
	if (rtm > 0) {
		_ofstreamSpecularTransmittance_mc.close();
		_ofstreamDiffuseTransmittance_mc.close();
		if (_topFluorescenceComputation || _bottomFluorescenceComputation) {
			ofstreamFluorescence_mc.close();
		}
	}
}

void FoliarComputation::computeClumpingIndex(	const std::vector<DirectionCartesienne> & listDirections,
								const DirectionCartesienne & sunDirection) {
	double theta = 0;
	for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
		const DirectionCartesienne & inDir = incidentDirection == listDirections.size() ? sunDirection: listDirections[incidentDirection];
		theta = inDir.getCenterTheta();
		if (fabs(_foliarModel.getFoliarOmegaMin() - _foliarModel.getFoliarOmegaMax()) < EPSILON
				&& fabs(_foliarModel.getFoliarOmegaMax() - _foliarModel.getFoliarClumpingA()) < EPSILON) {
			_clumpingIndex[incidentDirection] = _foliarModel.getFoliarClumpingA() + _foliarModel.getFoliarClumpingB() * theta;
			_transmTransfertFunctionVector[incidentDirection] *= _clumpingIndex[incidentDirection];
		} else {
			if (theta > 90.) {
				theta = 180 - theta;
			}
			_clumpingIndex[incidentDirection] = _foliarModel.getFoliarOmegaMin()
					+ _foliarModel.getFoliarOmegaMax()
							/ (1 + exp(-_foliarModel.getFoliarClumpingA() * (theta - _foliarModel.getFoliarClumpingB())));
			_transmTransfertFunctionVector[incidentDirection] *= _clumpingIndex[incidentDirection];
		}
	}
}

void FoliarComputation::transmissionTransferFunction(	const double & theta,
														const double & phi,
														const int & incidentDirection,
														const int modelIndex) {
	_transmTransfertFunctionVector[incidentDirection] = integrationWithSimpsonMethod_dPhif(theta, modelIndex);
}

double FoliarComputation::integrationWithSimpsonMethod_dPhif(	const double & theta,
																const unsigned int & modelIndex) {
	double sum = 0;
	double AireTrapeze = 0;
	double t2 = 0;
	double t12 = 0;
	for (double phi_f = 0; phi_f < 360; phi_f++) {
		t2 = phi_f + 1;
		t12 = phi_f + 0.5;
		AireTrapeze = integrationWithSimpsonMethod_dTheta_f(theta, phi_f, modelIndex) + integrationWithSimpsonMethod_dTheta_f(theta, t2, modelIndex)
				+ 4 * integrationWithSimpsonMethod_dTheta_f(theta, t12, modelIndex);
		sum += AireTrapeze;
	}
	sum *= (1. / 2160.);
	return sum;
}

double FoliarComputation::integrationWithSimpsonMethod_dTheta_f(const double & theta,
																const double & phi_f,
																const unsigned int & modelIndex) {
	double sum = 0;
	double AireTrapeze = 0;
	double t2 = 0;
	double t12 = 0;
	for (double theta_f = 0; theta_f < 90; theta_f++) {
		t2 = theta_f + 1;
		t12 = theta_f + .5;
		AireTrapeze = integrandOf_gTheta(theta, theta_f, phi_f, modelIndex) + integrandOf_gTheta(theta, t2, phi_f, modelIndex)
				+ 4 * integrandOf_gTheta(theta, t12, phi_f, modelIndex);
		sum += AireTrapeze;
	}
	sum *= (M_PI / 1080.);
	return sum;
}

double FoliarComputation::integrandOf_gTheta(	const double & theta,
												const double & theta_f,
												const double & phi_f,
												const unsigned int & modelIndex) {
	return scalarProductOfDirections(theta, theta_f, phi_f) * _foliarModel.getRefGFunctionForLadComputationFunctionVector()[2 * theta_f];
}

double FoliarComputation::scalarProductOfDirections(const double & theta,
													const double & theta_f,
													const double & phi_f) {
	return fabs(
		PhaseUtils::getStaticCosine(2 * theta) * PhaseUtils::getStaticCosine(2 * theta_f)
				+ PhaseUtils::getStaticSine(2 * theta) * PhaseUtils::getStaticSine(2 * theta_f) * PhaseUtils::getStaticCosine(2 * phi_f));
}

void FoliarComputation::resetBackFaceFluorescenceMatrix(unsigned nbSpectralBand) {
	// Resize (if necessary), plus reset back all values to 0.
	_backFaceFluorescenceMatrix.resize(nbSpectralBand);
	for (auto & level1 : _backFaceFluorescenceMatrix) {
		level1.resize(nbSpectralBand);
		for (auto & level2: level1) {
			level2.resize(4);
			level2[0] = 0.;
			level2[1] = 0.;
			level2[2] = 0.;
			level2[3] = 0.;
		}
	}
}
void FoliarComputation::resetFrontFaceFluorescenceMatrix(unsigned nbSpectralBand) {
	// Resize (if necessary), plus reset back all values to 0.
	_frontFaceFluorescenceMatrix.resize(nbSpectralBand);
	for (auto & level1 : _frontFaceFluorescenceMatrix) {
		level1.resize(nbSpectralBand);
		for (auto & level2: level1) {
			level2.resize(4);
			level2[0] = 0.;
			level2[1] = 0.;
			level2[2] = 0.;
			level2[3] = 0.;
		}
	}
}

double FoliarComputation::getLeafAngleDistributionForAngle(const double & theta) const {
	double resultat = 0;
	int tmp = (int) theta;
	if (tmp == theta) {
		resultat = _leafAngleDistributionFunctionVector[tmp];
	}
	else {
		int n1 = floor(theta), n2 = n1 + 1;
		double y1 = _leafAngleDistributionFunctionVector[n1];
		double y2 = _leafAngleDistributionFunctionVector[n2];
		double slope = (y2 - y1)/(n2 - n1);
		double intercept = y1 - slope * n1;
		resultat = slope * theta + intercept;
	}
	return resultat;
}
