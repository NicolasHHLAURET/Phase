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
	struct ParametersForFoliarModels {
		bool hasDifferentModelForBottom;

		int typeDiffPhase;
		int typeOfLeafAngleDistribution;
		int numberOfAla;
		int nthAla;

		unsigned int equivalentFrontLambertian;
		unsigned int equivalentBackLambertian;

		double tMinLeaf;
		double tMaxLeaf;
		double standardDeviationVector;
		double foliarDimension;
		double hotSpotFactor;
		double omegaMin;
		double omegaMax;
		double clumpingA;
		double clumpingB;
		double averageLeafAngle;
		double ellipsePrincipalAngle;
		double ellipseEccentricity;

		double diffuseTransmittanceAccelerationFactor;
		double bottomDiffuseTransmittanceAccelerationFactor;
		std::vector<double> adaxialReflectanceVector; // == TOP
		std::vector<double> abaxialReflectanceVector; // == BOTTOM
		std::vector<double> leafTransmittanceVector;
		std::vector<double> bottomLeafTransmittanceVector;
	};

private:
	std::vector<FoliarModel::ParametersForFoliarModels> _parametersForFoliarModels;
	std::vector<double> _transmTransfertFunctionVector;
	std::vector<std::vector<double> > _leafAngleDistributionFunctionVector;
	std::vector<std::vector<double> > _hotSpotFactorDistributionFunctionVector;
	std::vector<std::vector<double> > _gFunctionForLadComputationFunctionVector;

public:
	void initFoliarModel(	const unsigned int & numberOfFoliarModels,
							const unsigned int & numberOfSpectralBands,
							const unsigned int & numberOfDirections);

	const std::vector<std::vector<double> > & getLeafAngleDistributionFunctionVector();
	const std::vector<double> & getTransmTransfertFunctionVector();

	std::vector<FoliarModel::ParametersForFoliarModels> & getRefParametersForFoliarModels() {
		return _parametersForFoliarModels;
	}
	std::vector<double> & getRefTransmTransfertFunctionVector() {
		return _transmTransfertFunctionVector;
	}

	std::vector<std::vector<double> > & getRefLeafAngleDistributionFunctionVecto() {
		return _leafAngleDistributionFunctionVector;
	}

	std::vector<std::vector<double> > & getRefGFunctionForLadComputationFunctionVector() {
		return _gFunctionForLadComputationFunctionVector;
	}

	const std::vector<std::vector<double> >& getHotSpotFactorDistributionFunctionVector() const {
		return _hotSpotFactorDistributionFunctionVector;
	}

	void setHotSpotFactorDistributionFunctionVector(const std::vector<std::vector<double> >& hotSpotFactorDistributionFunctionVector) {
		_hotSpotFactorDistributionFunctionVector = hotSpotFactorDistributionFunctionVector;
	}

	void setFoliarClumpingA(const unsigned int& indice,
							const double& value);

	void setFoliarClumpingB(const unsigned int& indice,
							const double& value);

	void setFoliarOmegaMin(	const unsigned int& indice,
							const double& value);

	void setFoliarOmegaMax(	const unsigned int& indice,
							const double& value);

	double getFoliarClumpingA(const unsigned int& indice);

	double getFoliarClumpingB(const unsigned int& indice);

	double getFoliarOmegaMin(const unsigned int& indice);

	double getFoliarOmegaMax(const unsigned int& indice);

	void setFoliarDimension(const unsigned int& indice,
							const double& value);

	double getFoliarDimension(const unsigned int& indice);
	//

	double getHotSpotFactor(const unsigned int& indice) const;

	void setHotSpotFactor(const unsigned int& indice,
							const double& value);

	void setFoliarAverageLeafAngle(	const unsigned int& indice,
									const double& value);

	double getFoliarAverageLeafAngle(const unsigned int& indice);

	void setNumberOfAla(const int& indice,
						const int& value);

	int getNumberOfAla(const int& indice);

	void setNthAla(	const int& indice,
					const int& value);

	int getNthOfAla(const int& indice);

	void setFoliarEllipsePrincipalAngle(const unsigned int& indice,
										const double& value);

	void setFoliarEllipseEccentricity(	const unsigned int& indice,
										const double& value);

	double getFoliarEllipsePrincipalAngle(const unsigned int& indice);

	double getFoliarEllipseEccentricity(const unsigned int& indice);

	void setFoliarTypeOfLeafAngleDistribution(	const unsigned int& indice,
												const int& value);

	int getFoliarTypeOfLeafAngleDistribution(const unsigned int& indice);


	void setAbaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice,
															const double & value);
	void setAdaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice,
															const double & value);
	double getAbaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice) const;
	double getAdaxialDiffuseTransmittanceAccelerationForFoliar(	const unsigned int& indice) const;

	void setAdaxialReflectanceForFoliar(const unsigned int& indice,
										const unsigned int& spectralBand,
										const double & value);

	void setAbaxialReflectanceForFoliar(const unsigned int& indice,
										const unsigned int& spectralBand,
										const double & value);

	void setLeafTransmittanceForFoliar(	const unsigned int& indice,
										const unsigned int& spectralBand,
										const double & value,
										const bool & isBottomFace);

	double getAdaxialReflectanceForFoliar(	const unsigned int& indice,
											const unsigned int& spectralBand);

	double getAbaxialReflectanceForFoliar(	const unsigned int& indice,
											const unsigned int& spectralBand);

	double getLeafTransmittanceForFoliar(	const unsigned int& indice,
											const unsigned int& spectralBand,
											const bool & isBottomFace);

	void setEquivalentFrontLambertianIndex(const unsigned int foliarIndex, const unsigned int lambertianIndex);
	void setEquivalentBackLambertianIndex(const unsigned int foliarIndex, const unsigned int lambertianIndex);
	unsigned int getEquivalentFrontLambertianIndex(const unsigned int foliarIndex) const;
	unsigned int getEquivalentBackLambertianIndex(const unsigned int foliarIndex) const;

};

#endif /* INCLUDE_FOLIAR_FOLIARMODEL_HPP_ */
