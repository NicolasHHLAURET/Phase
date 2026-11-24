/*
 * FoliarModel.cpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#include "../../include/foliar/FoliarModel.hpp"

void FoliarModel::initFoliarModel(	const unsigned int & numberOfSpectralBands,
									const unsigned int & numberOfDirections) {

	_leafAngleDistributionFunctionVector.resize(NBMAX_TETAF + 1);
	_transmTransfertFunctionVector.resize(numberOfDirections + 1);
	_gFunctionForLadComputationFunctionVector.resize(NBMAX_TETAF * 2 + 1);

	_parametersForCurrentModel.averageLeafAngle = 0.0;
	_parametersForCurrentModel.ellipsePrincipalAngle = 0.0;
	_parametersForCurrentModel.ellipseEccentricity = 0.0;
	_parametersForCurrentModel.omegaMin = 0.0;
	_parametersForCurrentModel.omegaMax = 0.0;
	_parametersForCurrentModel.clumpingA = 0.0;
	_parametersForCurrentModel.clumpingB = 0.0;
	_parametersForCurrentModel.leafTransmittanceVector.assign(numberOfSpectralBands, 1.0);
	_parametersForCurrentModel.bottomLeafTransmittanceVector.assign(numberOfSpectralBands, 1.0);
	_parametersForCurrentModel.abaxialReflectanceVector.assign(numberOfSpectralBands, 1.0);
	_parametersForCurrentModel.adaxialReflectanceVector.assign(numberOfSpectralBands, 1.0);
}

const std::vector<double> & FoliarModel::getLeafAngleDistributionFunctionVector() {
	return _leafAngleDistributionFunctionVector;
}

const std::vector<double> & FoliarModel::getTransmTransfertFunctionVector() {
	return _transmTransfertFunctionVector;
}

void FoliarModel::setFoliarClumpingA(const double& value) {
	_parametersForCurrentModel.clumpingA = value;
}
void FoliarModel::setFoliarClumpingB(const double& value) {
	_parametersForCurrentModel.clumpingB = value;
}

void FoliarModel::setFoliarOmegaMin(const double& value) {
	_parametersForCurrentModel.omegaMin = value;
}

void FoliarModel::setFoliarOmegaMax(const double& value) {
	_parametersForCurrentModel.omegaMax = value;
}

double FoliarModel::getFoliarClumpingA() {
	return _parametersForCurrentModel.clumpingA;
}

double FoliarModel::getFoliarClumpingB() {
	return _parametersForCurrentModel.clumpingB;
}

double FoliarModel::getFoliarOmegaMin() {
	return _parametersForCurrentModel.omegaMin;
}

double FoliarModel::getFoliarOmegaMax() {
	return _parametersForCurrentModel.omegaMax;
}

void FoliarModel::setFoliarDimension(const double& value) {
	_parametersForCurrentModel.foliarDimension = value;
}

double FoliarModel::getFoliarDimension() {
	return _parametersForCurrentModel.foliarDimension;
}

double FoliarModel::getHotSpotFactor() const {
	return _parametersForCurrentModel.hotSpotFactor;
}

void FoliarModel::setHotSpotFactor(const double& value) {
	_parametersForCurrentModel.hotSpotFactor = value;
}

void FoliarModel::setFoliarAverageLeafAngle(const double& value) {
	_parametersForCurrentModel.averageLeafAngle = value;
}

double FoliarModel::getFoliarAverageLeafAngle() {
	return _parametersForCurrentModel.averageLeafAngle;
}

void FoliarModel::setNumberOfAla(const int& value) {
	_parametersForCurrentModel.numberOfAla = value;
}
int FoliarModel::getNumberOfAla() {
	return _parametersForCurrentModel.numberOfAla;
}
void FoliarModel::setNthAla(const int& value) {
	_parametersForCurrentModel.nthAla = value;
}
int FoliarModel::getNthOfAla() {
	return _parametersForCurrentModel.nthAla;
}

void FoliarModel::setFoliarEllipsePrincipalAngle(const double& value) {
	_parametersForCurrentModel.ellipsePrincipalAngle = value;
}
void FoliarModel::setFoliarEllipseEccentricity(const double& value) {
	_parametersForCurrentModel.ellipseEccentricity = value;
}

double FoliarModel::getFoliarEllipsePrincipalAngle() {
	return _parametersForCurrentModel.ellipsePrincipalAngle;
}
double FoliarModel::getFoliarEllipseEccentricity() {
	return _parametersForCurrentModel.ellipseEccentricity;
}

void FoliarModel::setFoliarTypeOfLeafAngleDistribution(const int& value) {
	_parametersForCurrentModel.typeOfLeafAngleDistribution = value;
}

int FoliarModel::getFoliarTypeOfLeafAngleDistribution() {
	return _parametersForCurrentModel.typeOfLeafAngleDistribution;
}

void FoliarModel::setAdaxialReflectanceForFoliar(	const unsigned int& spectralBand,
													const double & value) {
	_parametersForCurrentModel.adaxialReflectanceVector[spectralBand] = value;
}

void FoliarModel::setAbaxialReflectanceForFoliar(	const unsigned int& spectralBand,
													const double & value) {
	_parametersForCurrentModel.abaxialReflectanceVector[spectralBand] = value;
}

void FoliarModel::setLeafTransmittanceForFoliar(	const unsigned int& spectralBand,
												const double & value,
												const bool & isBottomFace) {
	if (isBottomFace) {
		_parametersForCurrentModel.bottomLeafTransmittanceVector[spectralBand] = value;
	} else {
		_parametersForCurrentModel.leafTransmittanceVector[spectralBand] = value;
	}
}

void FoliarModel::setEquivalentFrontLambertianIndex(const unsigned int lambertianIndex) {
	_parametersForCurrentModel.equivalentFrontLambertian = lambertianIndex;
}
void FoliarModel::setEquivalentBackLambertianIndex(const unsigned int lambertianIndex) {
	_parametersForCurrentModel.equivalentBackLambertian = lambertianIndex;
}

unsigned int FoliarModel::getEquivalentFrontLambertianIndex() const {
	return _parametersForCurrentModel.equivalentFrontLambertian;
}
unsigned int FoliarModel::getEquivalentBackLambertianIndex() const {
	return _parametersForCurrentModel.equivalentBackLambertian;
}
