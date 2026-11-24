/*
 * PhaseDTOs.hpp
 *
 *  Created on: Oct 10, 2013
 *      Author: yint
 */

#ifndef PHASEDTOS_HPP_
#define PHASEDTOS_HPP_

#include <vector>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "DartSAParameters.h"
#include "containers/DirectionCartesienne.hpp"
#include "FichierProperties.hpp"

struct PhaseGeneralParametersDTO {
	static const short int FLUXTRACKING = 0;
	static const short int MONTECARLO = 1;
	static const short int LIDAR = 2;

	// Temperature determination modes
	static const unsigned int MODE_DETERMINATION_REALTIME = 5;
	static const unsigned int MODE_DETERMINATION_VERTICAL_1D = 2;
	static const unsigned int MODE_DETERMINATION_MATRICE_3D = 1;
	static const unsigned int MODE_DETERMINATION_ECLAIREMENT = 0;

	// Spectral modes
	static const short int ModeR = 0;
	static const short int ModeRPlusT = 1;
	static const short int ModeT = 2;
	// -----------------------------------------------------------------------------
	/**
	 * Parametres de Phase
	 *
	 * @param int numberOfSpectralBands 			: nombre de bandes spectrales.
	 * @param std::vector<double> lowerBandWavelength 	: vecteur contenant les valeurs des bandes spectrales minimum.
	 * @param std::vector<double> upperBandWavelength 	: vecteur contenant les valeurs des bandes spectrales maximales.
	 * @param double dAS 					: angle azimuthal solaire
	 * @param double dZS 					: angle zenithal solaire
	 * @param string sfich_angle 		: fichier contenant toutes directions.
	 * @param int mode 						: mode dans lequel va se lancer la simulation.
	 * @param int MonteCarlo 				: 1 si la simulation tourne en mode Monte Carlo.
	 * @param int Specul 					: 1 si effet speculaire demande.
	 *
	 * @return 1 si okay
	 */

	unsigned int numberOfSpectralBands;
	std::vector<double> lowerBandWavelength;
	std::vector<double> upperBandWavelength;
	std::vector<int> spectralDomain;
	std::vector<int> emissionLaw;
	std::vector<int> yamlSpectralBandIndex;
	bool isThermalEmissionEnabled;
	unsigned int temperatureDeterminationMode;
	std::string temperatureFile;
	double atmosphericApparentTemperature;

	bool isAskedForWritingLadResultsCheck;
	bool generateEquivalentLambertianPhaseFunctionsFromVegetation;
	bool isSimulatedSolarNoiseCheck;
	short int SNrunningmode;
	unsigned int accelerationEngine;
	unsigned int atmosphereRTmode;
	int radiativeTransferMethod;
	int computationMethodTOAtoBOA;
	int sunCalMethod; //0: downward angle; 1: exact date; 2: viewing angle
	int dayOfTheYear;
	double meanTemperature;
	unsigned int numberOfThreads;
	DirectionCartesienne solarDirection;
	DirectionCartesienne lidarDirection;
	DirectionCartesienne sourceDirection;

	bool penumbraEffects;
	double sunDistance;
	double sunRadius;

	FichierProperties luxCoreRenderSceneProperties;
	FichierProperties luxCoreRenderConfigProperties;
	FichierProperties luxCoreLightProperties;

	std::map<unsigned int, unsigned int> dartIDtoLuxCoreID;
	std::queue<unsigned int> availableLightIDs;
	std::unordered_set<unsigned int> unavailableLightIDs;
	unsigned int lightCounter;

	PhaseGeneralParametersDTO() :
			numberOfSpectralBands(0), isAskedForWritingLadResultsCheck(false), generateEquivalentLambertianPhaseFunctionsFromVegetation(false), isSimulatedSolarNoiseCheck(false), SNrunningmode(-1),accelerationEngine(0), atmosphereRTmode(0), radiativeTransferMethod(0), sunCalMethod(2), dayOfTheYear(-1), meanTemperature(300.), numberOfThreads(1),
			isThermalEmissionEnabled(false), temperatureDeterminationMode(0), temperatureFile(""), atmosphericApparentTemperature(0.), lightCounter(0) {}

	~PhaseGeneralParametersDTO() {}

	void useSolarDirection() {
		sourceDirection = solarDirection;
	}

	void useLidarDirection() {
		sourceDirection = lidarDirection;
	}
};

#endif /* PHASEDTOS_HPP_ */
