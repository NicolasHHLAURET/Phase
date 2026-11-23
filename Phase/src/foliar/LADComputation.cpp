/*
 * LADComputation.cpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#include "../../include/foliar/LADComputation.hpp"

void LADComputation::initLADComputation() {
	_nbDisplayedWarningMessages = 0;
	_probabilityDensityFunctionVector.resize(NBMAX_TETAF, std::vector<double>(2));
}

void LADComputation::computeEllipsoidalLadDistribution(const double & ALA) {
	double alaInRad = PhaseUtils::getStaticDeg2Rad() * (ALA);
	if (alaInRad == 0) {
		computeVerticalHorizontalLadDistribution(0);
	} else {
		for (unsigned int angle = 0; angle <= 90; angle++) {
			_leafAngleDistributionFunctionVector[angle] = PhaseUtils::getStaticSine(2 * angle)
					* computeEllipsoidalLadWithPrecompute(angle, alaInRad);
			_gFunctionForLadComputationFunctionVector[2 * angle] = _leafAngleDistributionFunctionVector[angle];
		}
		for (double theta_f = 0.5; theta_f <= 90; theta_f++) {
			_gFunctionForLadComputationFunctionVector[2 * theta_f] = PhaseUtils::getStaticSine(2 * theta_f)
					* computeEllipsoidalLadWithPrecompute(theta_f, alaInRad);
		}
		testNormalisation();
		normalizeLeafAngleDistribution();
	}
}

void LADComputation::computeVerticalHorizontalLadDistribution(const int specifiedAngle) {
	for (unsigned int i = 0; i < NBMAX_TETAF; i++) {
		_leafAngleDistributionFunctionVector[i] = 0.0;
		_gFunctionForLadComputationFunctionVector[2 * i] = 0;
		_gFunctionForLadComputationFunctionVector[2 * i + 1] = 0;
	}
	_gFunctionForLadComputationFunctionVector[180] = 0;
	_leafAngleDistributionFunctionVector[90] = 0;
	_leafAngleDistributionFunctionVector[specifiedAngle] = 1080. / M_PI;
	_gFunctionForLadComputationFunctionVector[2 * specifiedAngle] = 1080. / M_PI;
	testNormalisation();
	normalizeLeafAngleDistribution();
}

void LADComputation::computeBoundedUniformLadDistribution(const int & numberOfAla,
															const int & nthAla) {
	for (unsigned int angle = 0; angle <= 90; angle++) {
		_leafAngleDistributionFunctionVector[angle] = computeBoundedUniformLad(angle, numberOfAla, nthAla);
		_gFunctionForLadComputationFunctionVector[2 * angle] = _leafAngleDistributionFunctionVector[angle];
	}
	for (double theta_f = 0.5; theta_f <= 90; theta_f++) {

		_gFunctionForLadComputationFunctionVector[2 * theta_f] = computeBoundedUniformLad(theta_f, numberOfAla, nthAla);
	}
	testNormalisation();
	normalizeLeafAngleDistribution();
}

void LADComputation::computeClassicalLadDistribution(const double & m,
														const double & b0,
														const double & b1,
														const double & b2) {

	double a = normalisationFactor(b1, b2, b0, m);
	for (unsigned int theta = 0; theta <= 90; theta++) {
		_leafAngleDistributionFunctionVector[theta] = PhaseUtils::getStaticSine(2 * theta)
				* computeClassicalLadWithA(theta, m, b0, b1, b2, a);
		_gFunctionForLadComputationFunctionVector[2 * theta] = _leafAngleDistributionFunctionVector[theta];
	}
	for (double theta_f = 0.5; theta_f <= 90; theta_f += 0.5) {
		_gFunctionForLadComputationFunctionVector[2 * theta_f] = PhaseUtils::getStaticSine(2 * theta_f)
				* computeClassicalLadWithA(theta_f, m, b0, b1, b2, a);
	}
	normalizeLeafAngleDistribution();
	testNormalisation();
}

void LADComputation::computeEllipticalLadDistribution(const double & thetaM,
														const double & eccentricity) {
	if (eccentricity > 1)
		std::cout << "error eccentricity > 1" << std::endl;
	else {
		for (unsigned int angle = 0; angle <= 90; angle++) {
			_leafAngleDistributionFunctionVector[angle] = PhaseUtils::getStaticSine(2 * angle)
					* computeEllipticalLad(angle, thetaM, eccentricity);
			_gFunctionForLadComputationFunctionVector[2 * angle] = _leafAngleDistributionFunctionVector[angle];
		}
		for (double theta_f = 0.5; theta_f <= 90; theta_f++) {

			_gFunctionForLadComputationFunctionVector[2 * theta_f] = PhaseUtils::getStaticSine(2 * theta_f)
					* computeEllipticalLad(theta_f, thetaM, eccentricity);
		}
		testNormalisation();
		normalizeLeafAngleDistribution();
	}
}

void LADComputation::readLadFile(	const std::string & leadAngleDistributionFilename) {
	std::ifstream IN;
	IN.open(leadAngleDistributionFilename.c_str());
	double angle = 0;
	double gValue = 0;
	std::string header = "";
	IN >> header;
	IN >> header;
	for (unsigned int i = 0; i <= 180; i++) {
		IN >> angle;
		IN >> gValue;
		if (i % 2 == 0) {
			_leafAngleDistributionFunctionVector[int(i * 0.5)] = gValue;
		}
		_gFunctionForLadComputationFunctionVector[i] = gValue;
	}

	IN.close();
	normalizeLeafAngleDistribution();
	testNormalisation();
}

void LADComputation::writeLadFile(	std::string & leadAngleDistributionFilename) {
	std::ofstream writeOutput;
	writeOutput.open(leadAngleDistributionFilename.c_str());
	writeOutput.precision(20);
	writeOutput.setf(std::ios::scientific);
	writeOutput << "Thetaf ";
	writeOutput << "g_sinThetaf ";
	writeOutput << std::endl;
	for (int i = 0; i <= 180; i++) {
		writeOutput << i * 0.5 << " ";
		writeOutput << _gFunctionForLadComputationFunctionVector[i] << " ";
		writeOutput << std::endl;
	}
	writeOutput.close();
}

void LADComputation::getLadFile(const int & numberTypeOfLad) {
	std::string leadAngleDistributionFilename = DartEnv::getLocalHomePath() + "database/LAD_";
	std::string ladName = "";
	if (numberTypeOfLad == globalReferences::UNIFORM) {
		ladName = "UNIFORM";
	} else if (numberTypeOfLad == globalReferences::BOUNDEDUNI) {
		ladName = "BOUNDEDUNI";
	} else if (numberTypeOfLad == globalReferences::SPHERIC) {
		ladName = "SPHERIC";
	} else if (numberTypeOfLad == globalReferences::ERECTO) {
		ladName = "ERECTO";
	} else if (numberTypeOfLad == globalReferences::PLANO) {
		ladName = "PLANO";
	} else if (numberTypeOfLad == globalReferences::EXTREMO) {
		ladName = "EXTREMO";
	} else if (numberTypeOfLad == globalReferences::PLAGIO) {
		ladName = "PLAGIO";
	}
	leadAngleDistributionFilename = leadAngleDistributionFilename + ladName + ".txt";
	std::ifstream IN_LAD;
	if (isFileReadableCheck(leadAngleDistributionFilename)) {
		readLadFile(leadAngleDistributionFilename);
	} else {
		computeLeafAngleDistributionFromBunnick(numberTypeOfLad);
		writeLadFile(leadAngleDistributionFilename);
	}
}

double LADComputation::computeClassicalLadWithA(const double & angle,
												const double & m,
												const double & b0,
												const double & b1,
												const double & b2,
												const double & a) {
	double b3 = (M_PI05 * (angle - b0)) / (b2 - b0); // hapkeParameterb2!=b0 tjs vrai
	double d1 = fabs(a);
	return PhaseUtils::getStaticDeg2Rad() * (d1 * pow(fabs(cos(b3)), m));

}

double LADComputation::computeEllipsoidalLadWithPrecompute(	const double & angle,
															const double & ALA) {
	if (ALA == 0) {
		return computeVerticalHorizontalLad(angle, 0);
		return 0;
	} else {
		double xi = pow(ALA / 9.65, -1.0 / 1.65) - 3.;
		double lambda;
		double epsilon;
		if (xi == 1) {
			lambda = 2;
		} else if (xi < 1) {
			epsilon = sqrt(1 - xi * xi);
			lambda = xi + asin(epsilon) / epsilon;
		} else {
			epsilon = sqrt(1 - 1 / xi / xi);
			lambda = xi + log((1 + epsilon) / (1 - epsilon)) / (2 * xi * epsilon);
		}
		double numerator = 2 * (pow(xi, 3));
		double denominator = lambda
				* pow(
					PhaseUtils::getStaticCosine(2 * angle) * PhaseUtils::getStaticCosine(2 * angle)
							+ PhaseUtils::getStaticSine(2 * angle) * PhaseUtils::getStaticSine(2 * angle) * xi * xi, 2);
		if (denominator != 0) {
			return numerator / denominator;
		} else {
			return 0;
		}
	}
}

void LADComputation::writeLad(	const unsigned int & modelIndex,
								std::ofstream & ofstreamLAD,
								const int & numberTypeOfLad) {
	std::stringstream modelIndexSS;
	modelIndexSS << modelIndex;

	std::string modelIndexString = modelIndexSS.str();
	std::string fileName = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator
			+ "leafAngleDistributionFunctionVector[" + modelIndexString + "].txt";
	ofstreamLAD.open(fileName.c_str(), std::ofstream::app);
	ofstreamLAD.precision(20);
	ofstreamLAD.setf(std::ios::scientific);
	if (ofstreamLAD) {
		for (int i = 0; i < NBMAX_TETAF; i++) {
			ofstreamLAD << numberTypeOfLad << _tab_ << i << _space_ << _leafAngleDistributionFunctionVector[i] << std::endl;
		}
		ofstreamLAD.close();
	} else {
		if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
			_nbDisplayedWarningMessages++;
			std::cerr << "Impossible d'ouvrir le flux outLAD representant le fichier : " << fileName << std::endl;
		}
	}
}

void LADComputation::writeEllipticalLad(	std::ofstream & ofstreamElliptical) {
	std::string fileName = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + "/LAD_ELLIPT.txt";
	ofstreamElliptical.open(fileName.c_str(), std::ofstream::app);
	ofstreamElliptical.precision(20);
	ofstreamElliptical.setf(std::ios::scientific);
	if (ofstreamElliptical) {
		for (int i = 0; i < NBMAX_TETAF; i++) {
			ofstreamElliptical << _leafAngleDistributionFunctionVector[i] << std::endl;
		}
	}
	ofstreamElliptical.close();
}

void LADComputation::normalizeLeafAngleDistribution() {
	double normalisation = 0;
	for (int i = 0; i <= 90; i++) {
		normalisation += _leafAngleDistributionFunctionVector[i];
	}
	if (normalisation != 0) {
		for (int i = 0; i <= 90; i++) {
			_leafAngleDistributionFunctionVector[i] /= normalisation;
		}
	}

}

void LADComputation::computeLeafAngleDistributionFromBunnick(const int & typeOfLad) {
	double ag = 0.0;
	double bg = 0.0;
	double cg = 0.0;
	double dg = 0.0;
	getBunnickCoefficients(ag, bg, cg, dg, typeOfLad);
	for (unsigned int theta = 0; theta <= 90; theta++) {
		_leafAngleDistributionFunctionVector[theta] = computeBunnickLadWithPrecompute(theta, ag, bg, cg, dg);
		_gFunctionForLadComputationFunctionVector[2 * theta] = _leafAngleDistributionFunctionVector[theta];
	}
	for (double theta_f = 0.5; theta_f <= 90; theta_f += 1) {
		_gFunctionForLadComputationFunctionVector[2 * theta_f] = computeBunnickLadWithPrecompute(theta_f, ag, bg, cg, dg);
	}
	normalizeLeafAngleDistribution();
	testNormalisation();
}

bool LADComputation::isFileReadableCheck(std::string & file) {
	std::ifstream fichier(file.c_str());
	return !fichier.fail();
}

void LADComputation::getBunnickCoefficients(double & ag,
											double & bg,
											double & cg,
											double & dg,
											const int & typeOfLad) {
	if (typeOfLad == globalReferences::UNIFORM) {
		bg = cg = dg = 0;
		ag = 1;
	} else if (typeOfLad == globalReferences::SPHERIC) {
		ag = bg = cg = 0;
		dg = 1;
	} else if (typeOfLad == globalReferences::ERECTO) {
		ag = cg = 1;
		bg = -1.0;
		dg = 0;
	} else if (typeOfLad == globalReferences::PLANO) {
		ag = bg = cg = 1;
		dg = 0;
	} else if (typeOfLad == globalReferences::EXTREMO) {
		ag = bg = 1;
		cg = 2;
		dg = 0;
	} else if (typeOfLad == globalReferences::PLAGIO) {
		ag = 1;
		bg = -1;
		cg = 2;
		dg = 0;
	}
}

double LADComputation::computeBunnickLadWithPrecompute(	const double & angle,
														const double & ag,
														const double & bg,
														const double & cg,
														const double & dg) {
	double d1 = ag + bg * PhaseUtils::getStaticCosine(2 * 2 * cg * angle);
	return 2. / M_PI * d1 + dg * PhaseUtils::getStaticSine(2 * angle);
}

double LADComputation::normalisationFactor(	const double & B1,
											const double & B2,
											const double & B0,
											const int & M) {
	double FN = 0;
	double X1 = 0;
	double X2 = 0;
	double X = 0;
	double B3 = 0;
	double *AB;
	double *PD;
	AB = dvector(0, POLY_LEGENDRE);
	PD = dvector(0, POLY_LEGENDRE);
	if (M == 0) // Distribution uniforme
		FN = (B2 - B1) * CONV_DEG_RADIAN;
	else if (M == 1) {
		X1 = (M_PI / 2) * (B1 - B0) / (B2 - B0); // B2 != hapkeParameterB0 tjs vrai
		X2 = (B2 - B0) / 90; // X2 = (2 / M_PI) * (B2 - hapkeParameterB0) * (M_PI / 180)
		FN = X2 * (1 - sin(X1));
	} else {
		FN = 0.;
		gaussLegendre(AB, PD);
		for (int i = 1; i < POLY_LEGENDRE + 1; i++) {
			B3 = (B2 - B1) * AB[i] + (B2 + B1) / 2;
			X1 = (M_PI / 2) * (B3 - B0) / (B2 - B0);
			X = fabs(cos(X1));
			X2 = pow(X, M);
			FN += (B2 - B1) * CONV_DEG_RADIAN * PD[i] * X2 / 2;
		}
	}
	if (fabs(FN) > EPSILON)
		FN = 1 / FN;

	free_dvector(AB, 0);
	free_dvector(PD, 0);
	return FN;
}

double LADComputation::computeEllipticalLad(const double angle,
											const double & thetaM,
											const double & eccentricity) {
	double differenceThetafThetamRadian = (angle - thetaM) * M_PI / 180;
	double cosine = cos(differenceThetafThetamRadian);
	double root = 1. - eccentricity * eccentricity * cosine * cosine;
	if (root > 0) {
		return 1. / sqrt(root);
	} else {
		return 0;
	}
}

double LADComputation::computeBoundedUniformLad(const double angle,
												const int & numberOfAla,
												const int & nthAla) {
	if (90 * (nthAla - 1) / numberOfAla <= angle and angle < 90 * (nthAla) / numberOfAla and numberOfAla != nthAla) {
		return numberOfAla / 90.;
	} else if (90 * (nthAla - 1) / numberOfAla <= angle and angle <= 90 * (nthAla) / numberOfAla and numberOfAla == nthAla) {
		return numberOfAla / 90.;
	} else {
		return 0;
	}
}

double LADComputation::computeVerticalHorizontalLad(const double angle,
													const int specifiedAngle) {
	if (specifiedAngle == angle) {
		return 1080. / M_PI;
	} else {
		return 0;
	}
}

void LADComputation::testNormalisation() {

	double sum = 0;
	double trapeze = 0;
	for (double theta = 0; theta < 90; theta++) {
		trapeze = _gFunctionForLadComputationFunctionVector[2 * theta]
				+ 4 * _gFunctionForLadComputationFunctionVector[2 * theta + 1]
				+ _gFunctionForLadComputationFunctionVector[2 * theta + 2];
		sum += trapeze;
	}
	sum *= (M_PI / 1080.);
	for (unsigned int i = 0; i <= 180; i++) {
		_gFunctionForLadComputationFunctionVector[i] /= sum;
	}
	if (sum > 1.1 || sum < 0.9) {
		std::cout << "    Normalisation of leaf angle distribution function vector = " << sum << std::endl;
	}
}

void LADComputation::gaussLegendre(	double *Z,
									double *WF) {
	double y = 0;
	unsigned int nb_racine = 0;
	int nCount = 0;
	double xInc = 0;
	double xNow = 0;
	double pDir = 0;
	double coeff1[POLY_LEGENDRE + 1];
	double coeff2[POLY_LEGENDRE + 1];
	double P[POLY_LEGENDRE + 2];
	P[1] = 1;
	int k = 0;
	for (unsigned int i = 2; i <= POLY_LEGENDRE; i++) {
		y = i;
		coeff1[i] = (2 * y - 1) / y;
		coeff2[i] = (y - 1) / y;
	}
	nb_racine = (POLY_LEGENDRE + 1) / 2;
	for (unsigned int i = 1; i <= nb_racine; i++) {
		k = POLY_LEGENDRE - i + 1;
		nCount = 0;
		xInc = 1;
		// Utilisation de la methode de NEWTON pour le calcul des racines.
		xNow = cos((i * M_PI - (M_PI / 4)) / (POLY_LEGENDRE + 0.5));
		while ((fabs(xInc) > 1E-10) && (nCount <= 10)) {
			// Calcul de Pn(x) par une formule de recurrence.
			P[2] = xNow;
			for (int j = 2; j <= POLY_LEGENDRE; j++)
				P[j + 1] = coeff1[j] * xNow * P[j] - coeff2[j] * P[j - 1];
			// Calcul de la derivee de Pn(x) a partir de Pn(x) et Pn-1(x).
			pDir = POLY_LEGENDRE * (P[POLY_LEGENDRE] - xNow * P[POLY_LEGENDRE + 1]) / (1 - xNow * xNow);
			xInc = -P[POLY_LEGENDRE + 1] / pDir;
			xNow += xInc;
			++nCount;
		}
		// Valeurs des abscisses (+Z), (-Z) et du poids correspondant WF.
		Z[k] = xNow;
		Z[i] = -Z[k];
		WF[k] = 2 / ((1 - pow(Z[k], 2)) * pow(pDir, 2));
		WF[i] = WF[k];
	}
}

int LADComputation::initialisationLadProbabilityDensityFunctions() {
	int casPDF = REELLE;
	for (int zenithal_foliaire = 0; zenithal_foliaire < NBMAX_TETAF; zenithal_foliaire++) {
		_probabilityDensityFunctionVector[zenithal_foliaire][1] = _leafAngleDistributionFunctionVector[zenithal_foliaire];
		_probabilityDensityFunctionVector[zenithal_foliaire][0] = zenithal_foliaire;
		if (fabs(_probabilityDensityFunctionVector[zenithal_foliaire][1] - 1.) < EPSILON) {
			if (zenithal_foliaire == 0) {
				casPDF = 1;
			} else if (zenithal_foliaire == NBMAX_TETAF - 1) {
				casPDF = 2;
			} else {
//				std::cerr << "PDF error in Phase.cpp (Init_PDF)" << std::endl;
//				abort();
			}
		}
	}
	return casPDF;
}
