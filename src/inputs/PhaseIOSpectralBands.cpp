/*
 * PhaseIOSpectralBands.cpp
 *
 *  Created on: 25 sept. 2026
 *      Author: lauretn
 */

#include "PhaseDAO.hpp"

void PhaseDAO::readSpectralBandsXML(PhaseGeneralParametersDTO & genDTO) {
	DartSAParameters::openFile("spectralBands.xml");
	genDTO.numberOfSpectralBands = DartSAParameters::getNbOf("Phase.DartInputParameters.SpectralIntervals.SpectralIntervalsProperties");
	double lowerBandWavelength;
	for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
		double meanLambda = 0.0;
		double deltaLambda = 0.0;
		int domaineSpectralCourant = 0, loiEmissionCourante = -1;
		ostringstream oss;
		oss << i;
		string temp = "Phase.DartInputParameters.SpectralIntervals.SpectralIntervalsProperties[" + oss.str();
		DartSAParameters::get(temp + "].meanLambda", meanLambda);
		DartSAParameters::get(temp + "].deltaLambda", deltaLambda);
		DartSAParameters::get(temp + "].spectralDartMode", domaineSpectralCourant);
		genDTO.spectralDomain.push_back(domaineSpectralCourant);

		lowerBandWavelength = meanLambda - deltaLambda / 2;
		if (lowerBandWavelength < 0.25) {
			lowerBandWavelength = 0.25;
		}
		genDTO.lowerBandWavelength.push_back(lowerBandWavelength);
		genDTO.upperBandWavelength.push_back(meanLambda + deltaLambda / 2);
		if (domaineSpectralCourant == 2) {
			DartSAParameters::get(temp + "].emissionLawNode.emissionLaw", loiEmissionCourante);
		} else if (domaineSpectralCourant == 1) {
			loiEmissionCourante = PLANCK;
		}
		genDTO.emissionLaw.push_back(loiEmissionCourante);
	}
	genDTO.isThermalEmissionEnabled = false;
	for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
		if (genDTO.spectralDomain[bandIndex] == PhaseGeneralParametersDTO::Thermal || genDTO.spectralDomain[bandIndex] == PhaseGeneralParametersDTO::VisibleAndThermal) { // mode T+R or T
			genDTO.isThermalEmissionEnabled = true;
		}
	}
	DartSAParameters::terminate();
}
