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
#include <unordered_map>
#include <queue>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "inputs/storage/EmissionMaterials.hpp"
#include "inputs/storage/DirectionSet.hpp"
#include "inputs/storage/opticalMaterials/SurfacesMaterials.hpp"

#include "DartSAParameters.h"
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
	enum SpectralBandDomain {
		Visible = 0,
		VisibleAndThermal = 1,
		Thermal = 2,
	};
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

	unsigned int numberOfSpectralBands = 1;
	std::vector<double> lowerBandWavelength;
	std::vector<double> upperBandWavelength;
	std::vector<int> spectralDomain;
	std::vector<int> emissionLaw;
	std::vector<int> yamlSpectralBandIndex;
	bool isThermalEmissionEnabled;
	unsigned int temperatureDeterminationMode;
	std::string temperatureFile;
	double atmosphericApparentTemperature;

	phsdata::SurfacesMaterials _surfaceMaterials;
	phsdata::EmissionMaterials _emissionMaterials;
	std::vector<phsdata::DirectionSet> _directionSet;

	phsdata::CellDimensions globalCellDimensions;

	bool isAskedForWritingLadResultsCheck;
	bool generateEquivalentLambertianPhaseFunctionsFromVegetation;
	unsigned int atmosphereRTmode;
	int radiativeTransferMethod;
	int computationMethodTOAtoBOA = 0;
	int sunCalMethod; //0: downward angle; 1: exact date; 2: viewing angle
	int dayOfTheYear;
	double meanTemperature;
	unsigned int numberOfThreads;
	DirectionCartesienne solarDirection;
	DirectionCartesienne sourceDirection;

	bool penumbraEffects = false;
	double sunDistance = 149600000000;
	double sunRadius = 695500000;

	FichierProperties luxCoreRenderSceneProperties;
	FichierProperties luxCoreRenderConfigProperties;
	FichierProperties luxCoreLightProperties;

	std::map<unsigned int, unsigned int> dartIDtoLuxCoreID;
	std::queue<unsigned int> availableLightIDs;
	std::unordered_set<unsigned int> unavailableLightIDs;
	unsigned int lightCounter;

	std::unordered_map<std::string, std::string> _fileNameToTextureMap;

	PhaseGeneralParametersDTO() : isAskedForWritingLadResultsCheck(false), generateEquivalentLambertianPhaseFunctionsFromVegetation(false), atmosphereRTmode(0), radiativeTransferMethod(0), sunCalMethod(2), dayOfTheYear(-1), meanTemperature(300.), numberOfThreads(1),
			isThermalEmissionEnabled(false), temperatureDeterminationMode(0), temperatureFile(""), atmosphericApparentTemperature(0.), lightCounter(0) {}

	~PhaseGeneralParametersDTO() {}

	void useSolarDirection() {
		sourceDirection = solarDirection;
	}
};

#endif /* PHASEDTOS_HPP_ */
