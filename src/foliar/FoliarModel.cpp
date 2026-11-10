/*
 * FoliarModel.cpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#include "../../include/foliar/FoliarModel.hpp"

void FoliarModel::initFoliarModel(	const unsigned int & numberOfFoliarModels,
									const unsigned int & numberOfSpectralBands,
									const unsigned int & numberOfDirections) {

	_leafAngleDistributionFunctionVector.resize(numberOfFoliarModels, std::vector<double>(NBMAX_TETAF + 1));
	_transmTransfertFunctionVector.resize(numberOfDirections + 1);

	_parametersForFoliarModels.resize(numberOfFoliarModels);
	for (unsigned int index = 0; index < numberOfFoliarModels; index++) {
		_parametersForFoliarModels[index].averageLeafAngle = 0.0;
		_parametersForFoliarModels[index].ellipsePrincipalAngle = 0.0;
		_parametersForFoliarModels[index].ellipseEccentricity = 0.0;
		_parametersForFoliarModels[index].omegaMin = 0.0;
		_parametersForFoliarModels[index].omegaMax = 0.0;
		_parametersForFoliarModels[index].clumpingA = 0.0;
		_parametersForFoliarModels[index].clumpingB = 0.0;
	}

	for (unsigned int j = 0; j < numberOfFoliarModels; j++) {
		_parametersForFoliarModels[j].diffuseTransmittanceAccelerationFactor = 0.;
		_parametersForFoliarModels[j].leafTransmittanceVector.resize(numberOfSpectralBands, 1.0);
		_parametersForFoliarModels[j].bottomLeafTransmittanceVector.resize(numberOfSpectralBands, 1.0);
		_parametersForFoliarModels[j].abaxialReflectanceVector.resize(numberOfSpectralBands, 1.0);
		_parametersForFoliarModels[j].adaxialReflectanceVector.resize(numberOfSpectralBands, 1.0);
	}

	_gFunctionForLadComputationFunctionVector.resize(numberOfFoliarModels, std::vector<double>(NBMAX_TETAF * 2 + 1));
}

const std::vector<std::vector<double> > & FoliarModel::getLeafAngleDistributionFunctionVector() {
	return _leafAngleDistributionFunctionVector;
}

const std::vector<double> & FoliarModel::getTransmTransfertFunctionVector() {
	return _transmTransfertFunctionVector;
}

void FoliarModel::setFoliarClumpingA(	const unsigned int& indice,
										const double& value) {
	_parametersForFoliarModels[indice].clumpingA = value;
}
void FoliarModel::setFoliarClumpingB(	const unsigned int& indice,
										const double& value) {
	_parametersForFoliarModels[indice].clumpingB = value;
}

void FoliarModel::setFoliarOmegaMin(const unsigned int& indice,
									const double& value) {
	_parametersForFoliarModels[indice].omegaMin = value;
}

void FoliarModel::setFoliarOmegaMax(const unsigned int& indice,
									const double& value) {
	_parametersForFoliarModels[indice].omegaMax = value;
}

double FoliarModel::getFoliarClumpingA(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].clumpingA;
}

double FoliarModel::getFoliarClumpingB(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].clumpingB;
}

double FoliarModel::getFoliarOmegaMin(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].omegaMin;
}

double FoliarModel::getFoliarOmegaMax(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].omegaMax;
}

void FoliarModel::setFoliarDimension(	const unsigned int& indice,
										const double& value) {
	_parametersForFoliarModels[indice].foliarDimension = value;
}

double FoliarModel::getFoliarDimension(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].foliarDimension;
}

double FoliarModel::getHotSpotFactor(const unsigned int& indice) const {
	return _parametersForFoliarModels[indice].hotSpotFactor;
}

void FoliarModel::setHotSpotFactor(const unsigned int& indice,
						const double& value) {
	_parametersForFoliarModels[indice].hotSpotFactor = value;
}

void FoliarModel::setFoliarAverageLeafAngle(const unsigned int& indice,
											const double& value) {
	_parametersForFoliarModels[indice].averageLeafAngle = value;
}

double FoliarModel::getFoliarAverageLeafAngle(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].averageLeafAngle;
}

void FoliarModel::setNumberOfAla(	const int& indice,
									const int& value) {
	_parametersForFoliarModels[indice].numberOfAla = value;
}
int FoliarModel::getNumberOfAla(const int& indice) {
	return _parametersForFoliarModels[indice].numberOfAla;
}
void FoliarModel::setNthAla(const int& indice,
							const int& value) {
	_parametersForFoliarModels[indice].nthAla = value;
}
int FoliarModel::getNthOfAla(const int& indice) {
	return _parametersForFoliarModels[indice].nthAla;
}

void FoliarModel::setFoliarEllipsePrincipalAngle(	const unsigned int& indice,
													const double& value) {
	_parametersForFoliarModels[indice].ellipsePrincipalAngle = value;
}
void FoliarModel::setFoliarEllipseEccentricity(	const unsigned int& indice,
												const double& value) {
	_parametersForFoliarModels[indice].ellipseEccentricity = value;
}

double FoliarModel::getFoliarEllipsePrincipalAngle(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].ellipsePrincipalAngle;
}
double FoliarModel::getFoliarEllipseEccentricity(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].ellipseEccentricity;
}

void FoliarModel::setFoliarTypeOfLeafAngleDistribution(	const unsigned int& indice,
														const int& value) {
	_parametersForFoliarModels[indice].typeOfLeafAngleDistribution = value;
}

int FoliarModel::getFoliarTypeOfLeafAngleDistribution(const unsigned int& indice) {
	return _parametersForFoliarModels[indice].typeOfLeafAngleDistribution;
}

void FoliarModel::setAdaxialReflectanceForFoliar(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
	_parametersForFoliarModels[indice].adaxialReflectanceVector[spectralBand] = value;
}

void FoliarModel::setAbaxialReflectanceForFoliar(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
	_parametersForFoliarModels[indice].abaxialReflectanceVector[spectralBand] = value;
}

void FoliarModel::setAbaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice,
														const double & value) {
	_parametersForFoliarModels[indice].bottomDiffuseTransmittanceAccelerationFactor = value;
}
void FoliarModel::setAdaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice,
														const double & value) {
	_parametersForFoliarModels[indice].diffuseTransmittanceAccelerationFactor = value;
}
double FoliarModel::getAbaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice) const {
	return _parametersForFoliarModels[indice].bottomDiffuseTransmittanceAccelerationFactor;
}
double FoliarModel::getAdaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice) const {
	return _parametersForFoliarModels[indice].diffuseTransmittanceAccelerationFactor;
}

void FoliarModel::setLeafTransmittanceForFoliar(const unsigned int& indice,
												const unsigned int& spectralBand,
												const double & value,
												const bool & isBottomFace) {
	if (isBottomFace) {
		_parametersForFoliarModels[indice].bottomLeafTransmittanceVector[spectralBand] = value;
	} else {
		_parametersForFoliarModels[indice].leafTransmittanceVector[spectralBand] = value;
	}
}

double FoliarModel::getAdaxialReflectanceForFoliar(	const unsigned int& indice,
													const unsigned int& spectralBand) {
	return _parametersForFoliarModels[indice].adaxialReflectanceVector[spectralBand];
}

double FoliarModel::getAbaxialReflectanceForFoliar(	const unsigned int& indice,
													const unsigned int& spectralBand) {
	return _parametersForFoliarModels[indice].abaxialReflectanceVector[spectralBand];
}

double FoliarModel::getLeafTransmittanceForFoliar(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const bool & isBottomFace) {
	if (isBottomFace) {
		return _parametersForFoliarModels[indice].bottomLeafTransmittanceVector[spectralBand];
	} else {
		return _parametersForFoliarModels[indice].leafTransmittanceVector[spectralBand];
	}
}

void FoliarModel::setEquivalentFrontLambertianIndex(const unsigned int foliarIndex, const unsigned int lambertianIndex) {
	_parametersForFoliarModels[foliarIndex].equivalentFrontLambertian = lambertianIndex;
}
void FoliarModel::setEquivalentBackLambertianIndex(const unsigned int foliarIndex, const unsigned int lambertianIndex) {
	_parametersForFoliarModels[foliarIndex].equivalentBackLambertian = lambertianIndex;
}

unsigned int FoliarModel::getEquivalentFrontLambertianIndex(const unsigned int foliarIndex) const {
	return _parametersForFoliarModels[foliarIndex].equivalentFrontLambertian;
}
unsigned int FoliarModel::getEquivalentBackLambertianIndex(const unsigned int foliarIndex) const {
	return _parametersForFoliarModels[foliarIndex].equivalentBackLambertian;
}
