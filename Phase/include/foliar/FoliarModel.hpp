/*
 * FoliarModel.hpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_FOLIAR_FOLIARMODEL_HPP_
#define INCLUDE_FOLIAR_FOLIARMODEL_HPP_

#include "libDartEnv.h"
#include "../PhaseDTOs.hpp"

class FoliarModel {
public:
	struct FoliarModelParameters {
		int typeOfLeafAngleDistribution;
		int numberOfAla;
		int nthAla;

		unsigned int equivalentFrontLambertian;
		unsigned int equivalentBackLambertian;

		double foliarDimension;
		double hotSpotFactor;
		double omegaMin;
		double omegaMax;
		double clumpingA;
		double clumpingB;
		double averageLeafAngle;
		double ellipsePrincipalAngle;
		double ellipseEccentricity;

		std::vector<double> adaxialReflectanceVector; // == TOP
		std::vector<double> abaxialReflectanceVector; // == BOTTOM
		std::vector<double> leafTransmittanceVector;
		std::vector<double> bottomLeafTransmittanceVector;
	};

private:
	FoliarModelParameters _parametersForCurrentModel; // current model only (single slot)
	std::vector<double> _transmTransfertFunctionVector; // common to all models (one direction set)
	std::vector<double> _leafAngleDistributionFunctionVector; // current model only (single slot)
	std::vector<double> _gFunctionForLadComputationFunctionVector; // current model only (single slot)

public:
	void initFoliarModel(	const unsigned int & numberOfSpectralBands,
							const unsigned int & numberOfDirections);

	const std::vector<double> & getLeafAngleDistributionFunctionVector();
	const std::vector<double> & getTransmTransfertFunctionVector();

	FoliarModelParameters & getRefParametersForCurrentModel() {
		return _parametersForCurrentModel;
	}
	std::vector<double> & getRefTransmTransfertFunctionVector() {
		return _transmTransfertFunctionVector;
	}

	std::vector<double> & getRefLeafAngleDistributionFunctionVecto() {
		return _leafAngleDistributionFunctionVector;
	}

	std::vector<double> & getRefGFunctionForLadComputationFunctionVector() {
		return _gFunctionForLadComputationFunctionVector;
	}

	void setFoliarClumpingA(const double& value);

	void setFoliarClumpingB(const double& value);

	void setFoliarOmegaMin(const double& value);

	void setFoliarOmegaMax(const double& value);

	double getFoliarClumpingA();

	double getFoliarClumpingB();

	double getFoliarOmegaMin();

	double getFoliarOmegaMax();

	void setFoliarDimension(const double& value);

	double getFoliarDimension();

	double getHotSpotFactor() const;

	void setHotSpotFactor(const double& value);

	void setFoliarAverageLeafAngle(const double& value);

	double getFoliarAverageLeafAngle();

	void setNumberOfAla(const int& value);

	int getNumberOfAla();

	void setNthAla(const int& value);

	int getNthOfAla();

	void setFoliarEllipsePrincipalAngle(const double& value);

	void setFoliarEllipseEccentricity(const double& value);

	double getFoliarEllipsePrincipalAngle();

	double getFoliarEllipseEccentricity();

	void setFoliarTypeOfLeafAngleDistribution(const int& value);

	int getFoliarTypeOfLeafAngleDistribution();

	void setAdaxialReflectanceForFoliar(	const unsigned int& spectralBand,
										const double & value);

	void setAbaxialReflectanceForFoliar(	const unsigned int& spectralBand,
										const double & value);

	void setLeafTransmittanceForFoliar(	const unsigned int& spectralBand,
										const double & value,
										const bool & isBottomFace);

	void setEquivalentFrontLambertianIndex(const unsigned int lambertianIndex);
	void setEquivalentBackLambertianIndex(const unsigned int lambertianIndex);
	unsigned int getEquivalentFrontLambertianIndex() const;
	unsigned int getEquivalentBackLambertianIndex() const;

};

#endif /* INCLUDE_FOLIAR_FOLIARMODEL_HPP_ */
