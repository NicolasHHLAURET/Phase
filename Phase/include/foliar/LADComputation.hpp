/*
 * LADComputation.hpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_FOLIAR_LADCOMPUTATION_HPP_
#define INCLUDE_FOLIAR_LADCOMPUTATION_HPP_

#define POLY_LEGENDRE   32  // Coefficient du Polynome de Legendre
#define MAX_WARNING_MESSAGE	10 // Nombre maximal de message d'erreurs (anti spam)

#include "FoliarModel.hpp"
#include "../PhaseUtils.hpp"

/**
 * Computes the leaf angle distribution (LAD) of the model currently being
 * treated. The LAD and g-function vectors live in FoliarModel as single
 * slots (the previous model's data is simply overwritten), so this class
 * holds no per-model state of its own: only a scratch PDF buffer.
 */
class LADComputation {
private:
	int _nbDisplayedWarningMessages;

	std::vector<std::vector<double> > _probabilityDensityFunctionVector;

	std::vector<double> & _gFunctionForLadComputationFunctionVector;
	std::vector<double> & _leafAngleDistributionFunctionVector;

public:
	LADComputation(FoliarModel & foliarModel) :
			_gFunctionForLadComputationFunctionVector(foliarModel.getRefGFunctionForLadComputationFunctionVector()), _leafAngleDistributionFunctionVector(
				foliarModel.getRefLeafAngleDistributionFunctionVecto()), _nbDisplayedWarningMessages(0) {
	}

	~LADComputation() {}

	void initLADComputation();

	void computeEllipsoidalLadDistribution(	const double & ALA);

	void computeVerticalHorizontalLadDistribution(	const int specifiedAngle);

	void computeBoundedUniformLadDistribution(	const int & numberOfAla,
												const int & nthAla);

	void computeClassicalLadDistribution(	const double & m,
											const double & b0,
											const double & b1,
											const double & b2);

	void computeEllipticalLadDistribution(	const double & thetaM,
											const double & eccentricity);

	void readLadFile(	const std::string & leadAngleDistributionFilename);

	void writeLadFile(	std::string & leadAngleDistributionFilename);

	void getLadFile(const int & numberTypeOfLad);

	double computeClassicalLadWithA(const double & angle,
									const double & m,
									const double & b0,
									const double & b1,
									const double & b2,
									const double & a);

	double computeEllipsoidalLadWithPrecompute(	const double & angle,
												const double & ALA);

	void writeLad(	const unsigned int & modelIndex,
					std::ofstream & ofstreamLAD,
					const int & numberTypeOfLad);

	void writeEllipticalLad(	std::ofstream & ofstreamElliptical);

	void normalizeLeafAngleDistribution();

	void computeLeafAngleDistributionFromBunnick(	const int & typeOfLad);

	bool isFileReadableCheck(std::string & file);

	void getBunnickCoefficients(double & ag,
								double & bg,
								double & cg,
								double & dg,
								const int & typeOfLad);

	double computeBunnickLadWithPrecompute(	const double & angle,
											const double & ag,
											const double & bg,
											const double & cg,
											const double & dg);

	double normalisationFactor(	const double & B1,
								const double & B2,
								const double & B0,
								const int & M);

	double computeEllipticalLad(const double angle,
								const double & thetaM,
								const double & eccentricity);

	double computeBoundedUniformLad(const double angle,
									const int & numberOfAla,
									const int & nthAla);

	double computeVerticalHorizontalLad(const double angle,
								const int specifiedAngle);

	void testNormalisation();

	void gaussLegendre(	double *Z,
						double *WF);

	int initialisationLadProbabilityDensityFunctions();
};

#endif /* INCLUDE_FOLIAR_LADCOMPUTATION_HPP_ */
