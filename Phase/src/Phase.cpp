/*! Time-stamp: <@(#) Phase.c  10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : Phase.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description : Calcul des fonctions de phase
 *
 *********************************************************************
 * Historique :
 *
 * V 4.5.0 15/10/2008  ELG : correction angle minimal
 * V 4.5.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.5.0 21/04/2008  MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture/ecriture)
 * V 4.4.0 18/07/2008  Pierre Chalier : Suppression des appels aux fonctions systemes
 * V 4.3.0 03/12/2007  MAG-VBT : Correction caractere non lisible
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 17/02/2006  CLF : Revision
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.3 15/09/2005  EMA-VBT : Correction d'un bug de l'ancien DART sur le clumping index.
 * V 4.0.3 15/09/2005  EMA-VBT : Correction d'un bug de l'ancien DART sur les variables averageLeafAngle, ellipsePrincipalAngle, ellipseEccentricity
 * V 4.0.3 15/09/2005  EMA-VBT : Correction d'un bug de l'ancien DART sur le calcul des fonctions de phase.
 * V 4.0.2 17/08/2005  VBT : Correction d'un bug de l'ancien DART sur Tab_diracn.
 * V 4.0.1 08/07/2005  VBT : Correction du bug sur la mise a jour du coeff_diff.xml
 * V 4.0.1 07/07/2005  VBT : Correction du bug sur la creation des phase*.src
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.1   08/03/2005  VBT : MAG-TT-004 : Implementation nouvelles fonctions de lecture des parametres Phase
 * V 1.0   07/03/2005  VBT : First Revision
 *
 *********************************************************************
 */

/************************************************************/
/*                                                          */
/*        MODELISATION D'UN MILIEU NATUREL FORESTIER        */
/*        __________________________________________        */
/*                                                          */
/*    CALCUL DES CARACTERISTIQUES OPTIQUES DES ELEMENTS     */
/*                                                          */
/*      Version  4.5.0 compatible UNIX/LINUX/WINDOWS        */
/*                                                          */
/*          Derniere Modification : 27/06/2008              */
/*                                                          */
/************************************************************/
#include "../include/Phase.hpp"
#include "../../libDartGeometry/include/Plan.hpp"
#include "../../libDartBase/include/Lib_physic.h"

#include <stdlib.h>
#include <sstream>

#if defined(USE_GPERFTOOLS) && defined(ENABLE_TCMALLOC_PROFILER)
	#include "../../gperftools/src/gperftools/heap-profiler.h"
	#include "../../gperftools/src/gperftools/profiler.h"
#endif
#include <new>

#define _ONE_GIGABYTE (1024. * 1024. * 1024.)

#if defined(_WIN32)
#include <windows.h>
static double getTotalSystemMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return (double) status.ullTotalPhys;
}
static double getAvailableSystemMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return (double) status.ullAvailPhys;
}
#else
#include <unistd.h>
static double getTotalSystemMemory() {
	const long pages = sysconf(_SC_PHYS_PAGES);
	const long pageSize = sysconf(_SC_PAGESIZE);
	return pages > 0 ? (double) pages * (double) pageSize : 0.;
}
static double getAvailableSystemMemory() {
	const long pages = sysconf(_SC_AVPHYS_PAGES);
	const long pageSize = sysconf(_SC_PAGESIZE);
	return pages > 0 ? (double) pages * (double) pageSize : 0.;
}
#endif

extern "C" int sqlite3_initialize(void);
#define TYPE_MODELISATION_LAMBERTIEN 	4
#define TYPE_MODELISATION_SPEC 			7
#define TYPE_MODELISATION_RPV_SPEC 	    10
#define TYPE_MODELISATION_TURBIDE 		10 // Fonction de phase d'1 milieu turbide
#define NBSPECTRALPARAMTOREAD	10 // Nombre de parametres spectraux a lire dans la database
// Logger specifique a l'outil Phase.
//static Logger phaseLogger = Logger::getInstance(LOG4CPLUS_TEXT("Phase"));

#include "materials/SurfaceMaterialProcessor.hpp"
#include "materials/LambertianMaterialWriter.hpp"
#include "materials/Hapke1981MaterialWriter.hpp"
#include "materials/Hapke2012MaterialWriter.hpp"
#include "materials/RPVMaterialWriter.hpp"
#include "materials/SpecularMaterialWriter.hpp"
#include "materials/VegetationMaterialWriter.hpp"

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

void executePhase(	string const& fich,
					bool const& fromStream) {
	// Sqlite3 manage its own mutex for concurrent database access.
	// The allocation of this mutex is handled once, during the first call to sqlite3_initialize().
	// => as phase essentially does concurrent access to databases,
	//    we preinitialize here the sqlite3 engine in order to prevent this function from being called concurrently...
	//    (otherwise it may randomly result in a segmentation fault if two or more threads ends-up creating the mutex at the same time)
	sqlite3_initialize();

	PhaseGeneralParametersDTO genDTO;
	DartEnv::initCurrentSimulationPathFromSimulationName(fich);
	PhaseDAO::initPhase(fromStream);
	PhaseUtils::initStaticVectors();
	EXIT_IF(!PhaseDAO::readPhaseParameters(genDTO), "");
	string outputDir = DartEnv::getCurrentSimulationOutputPath() + DartEnv::getSeparator();
	string outputPhaseDir;
	outputPhaseDir = outputDir + "lib_phase";
	while (isDir(outputPhaseDir)) {
		removeDirectory(outputPhaseDir);
	}
	outputPhaseDir = outputDir + "lib_phase_SolarNoise";
	while (isDir(outputPhaseDir)) {
		removeDirectory(outputPhaseDir);
	}
	outputPhaseDir = outputDir + "lib_phase_LIDAR";
	while (isDir(outputPhaseDir)) {
		removeDirectory(outputPhaseDir);
	}
	while (isDir(DartEnv::getOutputTempDatabasePath(false))) {
		removeDirectory(DartEnv::getOutputTempDatabasePath(false));
	}

//	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
//			|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO) {
		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
		PhaseUtils::setOutputFolder("lib_phase");
		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties phasePropert(fileNameProperty.c_str());
		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
		// Each DART module creates its own "exclusive" ".scn" file.
		// The user defined scene file is "scene.scn". The latter is never modified by any program.
		// dart.exe will load the scn files in this order: phase.scn, maket.scn, atmos.scn and scene.scn
		// Any redundant key will be overwritten by the last file to be loaded (meaning scene.scn has the most priority).
		std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
		// This file is deleted on start-up by the module
		if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
			boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
		}
		// Create "phase.scn"
		std::ofstream tmp(luxCoreRenderSceneFilePath.c_str(), ios::out);
		if (tmp) {
			tmp.close();
		}
		genDTO.luxCoreRenderSceneProperties.changerFichierProperties(luxCoreRenderSceneFilePath, "=");
//			removePreviousPhaseKeys(genDTO.luxCoreRenderSceneProperties);
//			genDTO.luxCoreRenderSceneProperties.effacerLignesCommencantParClef("scene.simPropertiesPath");

		std::string luxCoreRenderConfigFilePath = DartEnv::getFilePathForEdit(OUTPUT_DIR, "scene.cfg");
		if (luxCoreRenderConfigFilePath == "") {
			luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
		}
		if (!boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
			std::ofstream tmp(luxCoreRenderConfigFilePath.c_str(), ios::out);
			if (tmp) {
				tmp.close();
			}
		}
		genDTO.luxCoreRenderConfigProperties.changerFichierProperties(luxCoreRenderConfigFilePath, "=");
		genDTO.luxCoreRenderConfigProperties.effacerLignesCommencantParClef("mode.");
		genDTO.luxCoreRenderConfigProperties.ajouterCle("scene.file", "scene.scn");

		// Light description file
		std::string luxCoreLightPropertiesFilePath = DartEnv::getFilePathForEdit(OUTPUT_DIR, "lights.properties");
		if (luxCoreLightPropertiesFilePath == "") {
			luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
		}
		if (!boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
			std::ofstream tmp(luxCoreLightPropertiesFilePath.c_str(), ios::out);
			if (tmp) {
				tmp.close();
			}
		}
		genDTO.luxCoreLightProperties.changerFichierProperties(luxCoreLightPropertiesFilePath, "=");
		reorganiseLightsId(genDTO, genDTO.luxCoreLightProperties);
		genDTO.useSolarDirection();
		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
		execute_batchPhase(genDTO, fileNameProperty);
//	} else if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == false) {
//		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
//		PhaseUtils::setOutputFolder("lib_phase");
//		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
//		FichierProperties phasePropert(fileNameProperty.c_str());
//		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
//		if(genDTO.accelerationEngine == 2){ //luxcorerender
//			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
//			// This file is deleted on start-up by the module
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
//			}
//			// Create "phase.scn"
//			std::ofstream tmp(luxCoreRenderSceneFilePath.c_str(), ios::out);
//			if (tmp) {
//				tmp.close();
//			}
//			genDTO.luxCoreRenderSceneProperties.changerFichierProperties(luxCoreRenderSceneFilePath, "=");
//
////			removePreviousPhaseKeys(genDTO.luxCoreRenderSceneProperties);
////			genDTO.luxCoreRenderSceneProperties.effacerLignesCommencantParClef("scene.simPropertiesPath");
//
//			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
//			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
//				std::ofstream tmp(luxCoreRenderConfigFilePath.c_str(), ios::out);
//				if (tmp) {
//					tmp.close();
//				}
//			}
//			genDTO.luxCoreRenderConfigProperties.changerFichierProperties(luxCoreRenderConfigFilePath, "=");
//			genDTO.luxCoreRenderConfigProperties.ajouterCle("scene.file", "scene.scn");
//
//			// Light description file
//			std::string luxCoreLightPropertiesFilePath = DartEnv::getFilePathForEdit(OUTPUT_DIR, "lights.properties");
//			if (luxCoreLightPropertiesFilePath == "") {
//				luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
//			}
//			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
//				std::ofstream tmp(luxCoreLightPropertiesFilePath.c_str(), ios::out);
//				if (tmp) {
//					tmp.close();
//				}
//			}
//			genDTO.luxCoreLightProperties.changerFichierProperties(luxCoreLightPropertiesFilePath, "=");
//			reorganiseLightsId(genDTO, genDTO.luxCoreLightProperties);
//			readLiDAR();
//		} else {
//			// Clean up the folder of LuxCore's Files
//			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
//			}
//			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderConfigFilePath));
//			}
//			std::string luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreLightPropertiesFilePath));
//			}
//		}
//		genDTO.useLidarDirection();
//		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
//		execute_batchPhase(genDTO, fileNameProperty);
//	} else if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == true) {
//		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
//		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
//		FichierProperties phasePropert(fileNameProperty.c_str());
//		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
//		if(genDTO.accelerationEngine == 2){ //luxcorerender
//			genDTO.lightCounter = 0;
//			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
//			// This file is deleted on start-up by the module
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
//			}
//			// Create "phase.scn"
//			std::ofstream tmp(luxCoreRenderSceneFilePath.c_str(), ios::out);
//			if (tmp) {
//				tmp.close();
//			}
//			genDTO.luxCoreRenderSceneProperties.changerFichierProperties(luxCoreRenderSceneFilePath, "=");
//
////			removePreviousPhaseKeys(genDTO.luxCoreRenderSceneProperties);
////			genDTO.luxCoreRenderSceneProperties.effacerLignesCommencantParClef("scene.simPropertiesPath");
//
//			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
//			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
//				std::ofstream tmp(luxCoreRenderConfigFilePath.c_str(), ios::out);
//				if (tmp) {
//					tmp.close();
//				}
//			}
//			genDTO.luxCoreRenderConfigProperties.changerFichierProperties(luxCoreRenderConfigFilePath, "=");
//			genDTO.luxCoreRenderConfigProperties.ajouterCle("scene.file", "scene.scn");
//
//			// Light description file
//			std::string luxCoreLightPropertiesFilePath = DartEnv::getFilePathForEdit(OUTPUT_DIR, "lights.properties");
//			if (luxCoreLightPropertiesFilePath == "") {
//				luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
//			}
//			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
//				std::ofstream tmp(luxCoreLightPropertiesFilePath.c_str(), ios::out);
//				if (tmp) {
//					tmp.close();
//				}
//			}
//			genDTO.luxCoreLightProperties.changerFichierProperties(luxCoreLightPropertiesFilePath, "=");
//			reorganiseLightsId(genDTO, genDTO.luxCoreLightProperties);
//			readLiDAR();
//		} else {
//			// Clean up the folder of LuxCore's Files
//			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
//			}
//			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderConfigFilePath));
//			}
//			std::string luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
//			if (boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
//				boost::filesystem::remove(boost::filesystem::path(luxCoreLightPropertiesFilePath));
//			}
//		}
//		cout << "- Phase of LIDAR with Solar Noise: " << endl;
//		cout << "- SolarNoise Phase Calculation: " << endl;
//		genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::FLUXTRACKING;
//		genDTO.useSolarDirection();
//		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
//		PhaseUtils::setOutputFolder("lib_phase_SolarNoise");
//		outputPhaseDir = outputDir + PhaseUtils::getOutputFolder();
//		PhaseKeyMappingGenerator::useNodeSolarNoiseSimuSN();
//		execute_batchPhase(genDTO, fileNameProperty);
//		PhaseProperty::reinit();
//		PhaseDAO::initPhase(fromStream); //????????????????????????????????????
//		cout << "- LIDAR Phase Calculation: " << endl;
//		genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::LIDAR;
//		genDTO.useLidarDirection();
//		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
//		PhaseUtils::setOutputFolder("lib_phase_LIDAR");
//		outputPhaseDir = outputDir + PhaseUtils::getOutputFolder();
//		PhaseKeyMappingGenerator::useNodeSolarNoiseSimuLIDAR();
//		execute_batchPhase(genDTO, fileNameProperty);
//	}
	// Remove temp databases at the end of phase.exe
	while (isDir(DartEnv::getOutputTempDatabasePath(false))) {
		removeDirectory(DartEnv::getOutputTempDatabasePath(false));
	}
}

void removePreviousPhaseKeys(FichierProperties & luxCoreRenderSceneProperties) {
	static const std::string materialRootKey = "scene.materials.";
	std::vector<std::string> materialNameList;

	static const std::string textureRootKey = "scene.textures.";
	std::vector<std::string> textureNameList;

	static const std::string objectRootKey = "scene.objects.";
	std::vector<std::string> objectNameList;

	static const std::string volumeRootKey = "scene.volumes.";
	std::vector<std::string> volumeNameList;

	static const std::string temperatureRootKey = "scene.temperatureproperties";
	std::vector<std::string> temperatureNameList;

	static const std::string volumeOpRootKey = "scene.op.volumes";
	std::vector<std::string> volumeOpNameList;

	static const std::string lightRootKey = "scene.lights";
	std::vector<std::string> lightNameList;

	static const std::string generatorKey = ".generator";
	static const std::string generatorValue = "phase";

	for (const auto & keyValue : luxCoreRenderSceneProperties.getPropertyList()) {
		if (keyValue.second == generatorValue && boost::ends_with(keyValue.first, generatorKey)) {
			// We keep the final "." in the substring to avoid potential confusions between "element1" and "element10" further away.
			if (boost::starts_with(keyValue.first, materialRootKey)) {
				materialNameList.push_back(keyValue.first.substr(materialRootKey.length(), keyValue.first.size() - materialRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, textureRootKey)) {
				textureNameList.push_back(keyValue.first.substr(textureRootKey.length(), keyValue.first.size() - textureRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, objectRootKey)) {
				objectNameList.push_back(keyValue.first.substr(objectRootKey.length(), keyValue.first.size() - objectRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, volumeRootKey)) {
				volumeNameList.push_back(keyValue.first.substr(volumeRootKey.length(), keyValue.first.size() - volumeRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, temperatureRootKey)) {
				temperatureNameList.push_back(keyValue.first.substr(temperatureRootKey.length(), keyValue.first.size() - temperatureRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, volumeOpRootKey)) {
				volumeOpNameList.push_back(keyValue.first.substr(volumeOpRootKey.length(), keyValue.first.size() - volumeOpRootKey.length() - generatorKey.size() + 1));
			} else if (boost::starts_with(keyValue.first, lightRootKey)) {
				lightNameList.push_back(keyValue.first.substr(lightRootKey.length(), keyValue.first.size() - lightRootKey.length() - generatorKey.size() + 1));
			}
		}
	}

	auto iterator = luxCoreRenderSceneProperties.getPropertyList().begin();
	const auto endIterator = luxCoreRenderSceneProperties.getPropertyList().end();
	std::string elementName;
	bool remove = true;
	while (iterator != endIterator) {
		remove = false;
		if (boost::starts_with(iterator->first, materialRootKey)) {
			elementName = iterator->first.substr(materialRootKey.length());
			for (const auto & name : materialNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, textureRootKey)) {
			elementName = iterator->first.substr(textureRootKey.length());
			for (const auto & name : textureNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, objectRootKey)) {
			elementName = iterator->first.substr(objectRootKey.length());
			for (const auto & name : objectNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, volumeRootKey)) {
			elementName = iterator->first.substr(volumeRootKey.length());
			for (const auto & name : volumeNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, temperatureRootKey)) {
			elementName = iterator->first.substr(temperatureRootKey.length());
			for (const auto & name : temperatureNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, volumeOpRootKey)) {
			elementName = iterator->first.substr(volumeOpRootKey.length());
			for (const auto & name : volumeOpNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		} else if (boost::starts_with(iterator->first, lightRootKey)) {
			elementName = iterator->first.substr(lightRootKey.length());
			for (const auto & name : lightNameList) {
				if (boost::starts_with(elementName, name)) {
					remove = true;
					break;
				}
			}
		}

		++iterator;
		if (remove) {
			luxCoreRenderSceneProperties.getPropertyList().erase(std::prev(iterator, 1));
		}
	}
}

void reorganiseLightsId(PhaseGeneralParametersDTO & genDTO,
						FichierProperties & luxCoreLightProperties) {
	static const std::string lightRootKey = "light.";

	// Find all lights
	std::unordered_set<std::string> lightNames;
	std::string elementName;
	for (const auto & keyValue : luxCoreLightProperties.getPropertyList()) {
		if (boost::starts_with(keyValue.first, lightRootKey)) {
			elementName = keyValue.first.substr(lightRootKey.size(), keyValue.first.size() - lightRootKey.size());
			lightNames.emplace(elementName.substr(0, elementName.find(".")));
		}
	}

	// Retrieve the lights previously generated by phase
	std::vector<std::string> phasePreviousLightNames;
	for (const auto & lightName: lightNames) {
		if (luxCoreLightProperties.exists(lightRootKey + lightName + ".generator") && luxCoreLightProperties.valeurProperties<std::string>(lightRootKey + lightName + ".generator") == "phase") {
			phasePreviousLightNames.push_back(lightName);

			if (luxCoreLightProperties.exists(lightRootKey + lightName + ".lcrID")) {
				// We will remove the previous "Phase" lights. As such, their IDs will be available
				genDTO.availableLightIDs.push(luxCoreLightProperties.valeurProperties<unsigned int>(lightRootKey + lightName + ".lcrID"));
			}
		} else {
			if (luxCoreLightProperties.exists(lightRootKey + lightName + ".lcrID")) {
				// IDs set by other modules should not be used by Phase, unless they match the same type (see correspondence table)
				genDTO.unavailableLightIDs.emplace(luxCoreLightProperties.valeurProperties<unsigned int>(lightRootKey + lightName + ".lcrID"));
			}
		}
	}

	// Remove the previous "Phase" lights
	if (phasePreviousLightNames.size() > 0) {
		auto iterator = luxCoreLightProperties.getPropertyList().begin();
		const auto endIterator = luxCoreLightProperties.getPropertyList().end();
		bool remove = true;
		while (iterator != endIterator) {
			remove = false;
			if (boost::starts_with(iterator->first, lightRootKey)) {
				elementName = iterator->first.substr(lightRootKey.length());
				for (const auto & name : phasePreviousLightNames) {
					if (boost::starts_with(elementName, name + ".")) {
						remove = true;
						break;
					}
				}
			}

			++iterator;
			if (remove) {
				luxCoreLightProperties.getPropertyList().erase(std::prev(iterator, 1));
			}
		}

		for (const auto & phaseLightName : phasePreviousLightNames) {
			lightNames.erase(phaseLightName);
		}
	}

	// Prepare the correspondence table of "DART IDs" to "LuxCore IDs".
	for (const auto & lightName : lightNames) {
		genDTO.dartIDtoLuxCoreID[luxCoreLightProperties.valeurProperties<unsigned int>(lightRootKey + lightName + ".id")] = luxCoreLightProperties.valeurProperties<unsigned int>(lightRootKey + lightName + ".lcrID");
	}
}

int main(	int argc,
			char *argv[]) {
	// Init DartEnv
	DartEnv::initDARTVersion();
	// Configuration du logger.
	//PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(DartEnv::getRootLogPath() + "log4cplus.properties"));
	// Gestion des options
	GestOption gest;
	gest.setProgramDescription(
		"Pre-compute the phase functions (albedo, transmittance, BRDF, ...) of the various elements of the scene for all the directions computed by the directions.exe module.");
	string pathOptionDescription =
			"simulationName is the relative path of the simulation from the 'simulations' directory stored in your 'dart_local' directory:\n";
	pathOptionDescription.append(
		"         if the absolute path to your simulation is 'c:\\dart_local\\simulations\\test\\rootSim', then the simulation name is 'test\\rootSim'");
	gest.ajouterType("path", true, pathOptionDescription, 1, vector<string>(1, "simulationName"));
#if defined(USE_GPERFTOOLS) && defined(ENABLE_TCMALLOC_PROFILER)
	gest.ajouterType("profile_memory", false, "Enable memory profiling. Env variables and doc => https://gperftools.github.io/gperftools/heapprofile.html", 0, vector<string>(0));
	gest.ajouterType("profile_cpu", false, "Enable cpu profiling. Env variables and doc => https://gperftools.github.io/gperftools/cpuprofile.html", 0, vector<string>(0));
#endif
	if (!gest.parser(argc, argv)) {
		//LOG4CPLUS_ERROR(phaseLogger, gest.getErreur());
		cout << "Echec lancement phase:" << endl;
		cout << gest.getErreur();
		cout << gest.getMan() << endl;
		return EXIT_FAILURE;
	}

#if defined(USE_GPERFTOOLS) && defined(ENABLE_TCMALLOC_PROFILER)
	if(gest.estPresent("profile_memory")) HeapProfilerStart("");
	if(gest.estPresent("profile_cpu")) ProfilerStart("");
#endif

	// multiplicativeFactorForSpecularReflectance partir d'ici, il suffira d'appeler les macros LOG4CPLUS_XXXXX (phaseLogger, message)
	// pour logger un message (ou XXXXX est le niveau du message : INFO, DEBUG, WARN, ERROR ou FATAL)
	cout << endl << "               PHASE Module   " << DartEnv::getFullDARTversion() << endl;
	int returnCode = EXIT_SUCCESS;
	try {
		std::cout << std::endl << " - System memory (free/total) : " << std::fixed << std::setprecision(3)
				<< (double) getAvailableSystemMemory() / (double) _ONE_GIGABYTE << "GB / " << (double) getTotalSystemMemory() / (double) _ONE_GIGABYTE
				<< "GB" << std::endl;
		string fich = gest.getValeurS("path", 0);
		executePhase(fich, false);
	} catch (IndexErrorException & iee) {
		cout << "Echec lancement phase:" << endl;
		cout << gest.getErreur();
		cout << gest.getMan() << endl;
		returnCode = EXIT_FAILURE;
	} catch (std::bad_alloc& ba) {
		std::cerr << "Memory error: " << ba.what() << std::endl;
		returnCode = EXIT_FAILURE;
	} catch (std::exception & e) {
		cerr << e.what() << endl;
		returnCode = EXIT_FAILURE;
	}

#if defined(USE_GPERFTOOLS) && defined(ENABLE_TCMALLOC_PROFILER)
	if(gest.estPresent("profile_memory")) HeapProfilerStop();
	if(gest.estPresent("profile_cpu")) ProfilerStop();
#endif

	cout << endl << _space_ << "**************************************************" << endl << endl;
	return returnCode;
}
/**
 * Swig ne peut pas creer un wrapper du main donc une methode run est definie.
 * @see http://www.swig.org/Doc2.0/SWIG.html#SWIG_nn50
 */
int run(int argc,
		char *argv[]) {
	/*
	 Configuration du logger.
	 PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(DartEnv::getRootLogPath() + "log4cplus.properties"));
	 Gestion des options
	 */
	GestOption gest;
	gest.setProgramDescription(
		"Pre-compute the phase functions (albedo, transmittance, BRDF, ...) of the various elements of the scene for all the directions computed by the directions.exe module.");
	string pathOptionDescription =
			"simulationName is the relative path of the simulation from the 'simulations' directory stored in your 'dart_local' directory:\n";
	pathOptionDescription.append(
		"         if the absolute path to your simulation is 'c:\\dart_local\\simulations\\test\\rootSim', then the simulation name is 'test\\rootSim'");
	gest.ajouterType("path", true, pathOptionDescription, 1, vector<string>(1, "simulationName"));
	if (!gest.parser(argc, argv)) {
		cout << "Echec lancement phase:" << endl;
		cout << gest.getErreur();
		cout << gest.getMan() << endl;
		return EXIT_FAILURE;
	}
	/*
	 multiplicativeFactorForSpecularReflectance partir d'ici, il suffira d'appeler les macros LOG4CPLUS_XXXXX (phaseLogger, message)
	 pour logger un message (ou XXXXX est le niveau du message : INFO, DEBUG, WARN, ERROR ou FATAL)
	 */
	cout << endl << "               PHASE Module   " << DartEnv::getFullDARTversion() << endl;
	try {
		string fich = gest.getValeurS("path", 0);
		executePhase(fich, true);
	} catch (...) {
		cout << "Echec lancement phase:" << endl;
		cout << gest.getErreur();
		exit(EXIT_FAILURE);
	}
	cout << endl << _space_ << "**************************************************" << endl << endl;
	return EXIT_SUCCESS;
}

unsigned int getLightIDFromType(PhaseGeneralParametersDTO & genDTO, unsigned int lightTypeId, const std::string & lightName) {
	unsigned int lcrID = 0;

	const auto & it = genDTO.dartIDtoLuxCoreID.find(lightTypeId);
	if (it == genDTO.dartIDtoLuxCoreID.end()) {
		// If the light type is not already referenced
		// First, find an available light ID
		if (genDTO.availableLightIDs.size() > 0) {
			lcrID = genDTO.availableLightIDs.front();
			genDTO.availableLightIDs.pop();
		} else {
			while (genDTO.unavailableLightIDs.count(lcrID) > 0) {
				++lcrID;
			}
		}

		// Second, reference the new light ID
		genDTO.dartIDtoLuxCoreID[lightTypeId] = lcrID;
		genDTO.unavailableLightIDs.emplace(lcrID);
		std::string lightKeyName = "phaseLight" + std::to_string(lcrID); // Just needs to be unique
		genDTO.luxCoreLightProperties.ajouterCle("light." + lightKeyName + ".id", std::to_string(lightTypeId));
		genDTO.luxCoreLightProperties.ajouterCle("light." + lightKeyName + ".lcrID", std::to_string(lcrID));
		genDTO.luxCoreLightProperties.ajouterCle("light." + lightKeyName + ".name", lightName);
		genDTO.luxCoreLightProperties.ajouterCle("light." + lightKeyName + ".generator", "phase");
	} else {
		lcrID = it->second;
	}
	return lcrID;
}

void writePhasePropertiesSpectralIrradiance(ofstream & outCoeffSpecProperties,
											PhaseKeyMappingGenerator & pkmg,
											PhaseGeneralParametersDTO & genDTO) {

	vector<double> Es_BOA; // It is not equal to the physical quantity BOA irradiance: E_BOA;
	                       // it is computed from TOA solar constant Es_TOA without considering the costheta;
	                       // it is rather E_BOA/costheta. In case that costheta < 0, this value might be positive
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		pkmg.setBandeSpectrale(spectralBandIndex);
		outCoeffSpecProperties << pkmg.cleLambdaMin() << PhaseProperty::getLowerBandWavelength(spectralBandIndex) << endl;
		outCoeffSpecProperties << pkmg.cleLambdaMax() << PhaseProperty::getUpperBandWavelength(spectralBandIndex) << endl;
		outCoeffSpecProperties << pkmg.cleSpectralDomain() << genDTO.spectralDomain[spectralBandIndex] << endl;
		outCoeffSpecProperties << pkmg.cleLoiEmission() << genDTO.emissionLaw[spectralBandIndex] << endl;
		outCoeffSpecProperties << pkmg.cleTOA() << PhaseProperty::getMeanIrradianceTopOfAtmosphere(spectralBandIndex) << endl;
		outCoeffSpecProperties << pkmg.cleBOA() << PhaseProperty::getMeanIrradianceBottomOfAtmosphere(spectralBandIndex) << endl;
		outCoeffSpecProperties << pkmg.cleSKYL() << PhaseProperty::getMeanSKYL(spectralBandIndex) << endl;
		outCoeffSpecProperties << pkmg.cleAtmosphereTemperatureAbsorption() << PhaseProperty::getTemperatureAtmosphereAbsorption(spectralBandIndex) << endl;

		// initialize the Es_BOA
		if(cosf(PhaseProperty::getSunZenithAngleInRadians()) > 0.0){ // during the night
			if(genDTO.sunCalMethod == 1){ // the simulation Earth-Sun force the night irradiance 0
				Es_BOA.push_back(0.0);
			}else{ // the simulation extraterrestrial planet keep the solar direct irradiance during the night, the diffuse irradiance is set to zero
				Es_BOA.push_back(PhaseProperty::getMeanIrradianceBottomOfAtmosphere(spectralBandIndex));
			}

		}else{
			Es_BOA.push_back(PhaseProperty::getMeanIrradianceBottomOfAtmosphere(spectralBandIndex));
		}
	}

	std::string spectrumKey = "scene.spectrum.";
	std::ostringstream emissionLawStream, lambdaMinStream, lambdaMaxStream, spectralDomainStream;
	for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
		emissionLawStream << genDTO.emissionLaw[i] << " ";
	}
	for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
		lambdaMinStream << genDTO.lowerBandWavelength[i] << " ";
	}
	for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
		lambdaMaxStream << genDTO.upperBandWavelength[i] << " ";
	}
	for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
		spectralDomainStream << genDTO.spectralDomain[i] << " ";
	}
	genDTO.luxCoreRenderSceneProperties.ajouterCle(spectrumKey + "emissionLaw", emissionLawStream.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(spectrumKey + "wavelength.min", lambdaMinStream.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(spectrumKey + "wavelength.max", lambdaMaxStream.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(spectrumKey + "spectralDomain", spectralDomainStream.str());

//	if (genDTO.accelerationEngine == 2 && (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
//			|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO) && genDTO.SNrunningmode != 2) {
		if (genDTO.computationMethodTOAtoBOA != 2 || genDTO.atmosphereRTmode == 1 /* DART-Lux MC atmosphere RT method need a solar light*/) {
			bool noSKYL = true;
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				if (PhaseProperty::getMeanSKYL(spectralBandIndex) > 0.0) {
					noSKYL = false;
					break;
				}
			}
			// Thermal emission of the atmosphere in "NO ATMOSPHERE RT" mode.
			if (genDTO.isThermalEmissionEnabled && genDTO.atmosphericApparentTemperature > 0. && genDTO.computationMethodTOAtoBOA == 0) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.type", "sky_DART");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.isotropic", "1");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.generator", "phase");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.id", std::to_string(getLightIDFromType(genDTO, globalReferences::SKY, "Sky")));

				std::ostringstream value;
				double precision = 0.001;
				unsigned int maxNumberOfIterations = 1000;
				bool thermalEmission = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::Thermal || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::VisibleAndThermal;
				value << (thermalEmission ? M_PI * temperatureToLuminanceMeanOnSpectralBand(genDTO.atmosphericApparentTemperature, PLANCK, genDTO.lowerBandWavelength[0] * 1E-6, genDTO.upperBandWavelength[0] * 1E-6, precision, maxNumberOfIterations) : 0.f);
				for (unsigned int sbIndex = 1; sbIndex < genDTO.numberOfSpectralBands; sbIndex++) {
					thermalEmission = genDTO.spectralDomain[sbIndex] == PhaseGeneralParametersDTO::Thermal || genDTO.spectralDomain[sbIndex] == PhaseGeneralParametersDTO::VisibleAndThermal;
					value << " " << (thermalEmission ? M_PI * temperatureToLuminanceMeanOnSpectralBand(genDTO.atmosphericApparentTemperature, PLANCK, genDTO.lowerBandWavelength[sbIndex] * 1E-6, genDTO.upperBandWavelength[sbIndex] * 1E-6, precision, maxNumberOfIterations) : 0.f);
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 3; spectralBandIndex++) {
						value << " 0";
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The sky does not illuminate in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 2; spectralBandIndex++) {
						value << " 0";
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.color", value.str());
			}

			if (noSKYL || genDTO.computationMethodTOAtoBOA == 2) {
				//only solar direction illumination
				//sunlight type
				if (genDTO.penumbraEffects) {
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.type", "sun");
					if (genDTO.sunDistance > 81000) {
						// We "move" the sun closer in order to diminish floating point precision issues in LuxCore (technically Embree)
						genDTO.sunRadius = genDTO.sunRadius * 81000 / genDTO.sunDistance;
						genDTO.sunDistance = 81000;
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.radius", boost::lexical_cast<std::string>(genDTO.sunRadius));
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.meanDistance", boost::lexical_cast<std::string>(genDTO.sunDistance));
				} else {
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.type", "sharpdistant");
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.id", std::to_string(getLightIDFromType(genDTO, globalReferences::SUN, "Sun")));
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.generator", "phase");
				//irradiance
				std::ostringstream value;
				bool sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::VisibleAndThermal;
				value << (sunIllumination ? ((genDTO.atmosphereRTmode == 1 && genDTO.computationMethodTOAtoBOA == 2) ? PhaseProperty::getMeanIrradianceTopOfAtmosphere(0) : Es_BOA[0]) : 0.0);
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::VisibleAndThermal;
					value << " " << (sunIllumination ? (genDTO.atmosphereRTmode == 1 ? PhaseProperty::getMeanIrradianceTopOfAtmosphere(spectralBandIndex) : Es_BOA[spectralBandIndex]) : 0.0);
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 3; spectralBandIndex++) {
						value << " 0";
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The sun does not illuminate in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 2; spectralBandIndex++) {
						value << " 0";
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.color", value.str());

				//direction
				value.str("");
				if (genDTO.penumbraEffects) {
					value << (-genDTO.solarDirection.getX()) << " " << (-genDTO.solarDirection.getY()) << " " << (-genDTO.solarDirection.getZ());
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.dir", value.str());
				} else {
					value << (genDTO.solarDirection.getX()) << " " << (genDTO.solarDirection.getY()) << " " << (genDTO.solarDirection.getZ());
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.direction", value.str());
				}
			} else {					//with skyl
				bool noSolar = true;
				for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					if (PhaseProperty::getMeanSKYL(spectralBandIndex) < 1.0) {
						noSolar = false;
						break;
					}
				}
				//sunlight type
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.type", "sky_DART");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.isotropic", "1");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.generator", "phase");
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.id", std::to_string(getLightIDFromType(genDTO, globalReferences::SKY, "Sky")));

				//sky irradiance
				std::ostringstream value;
				// theta_s is downward angle (theta_s > 90 in the day, theta_s < 90 in the night).
				// if daytime: costheta = abs(cos(theta_s))
				// if nighttime: costheta = 0
				double costheta = cosf(PhaseProperty::getSunZenithAngleInRadians()) <= 0.0 ? abs(cosf(PhaseProperty::getSunZenithAngleInRadians())) : 0.0;

				// this BOA radiance did not consider the solar direction. need to multiply the costheta
				double valueToAdd = 0.;
				double precision = 0.001;
				unsigned int maxNumberOfIterations = 1000;
				bool sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::VisibleAndThermal;
				bool skyThermalEmission = false;
				for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					valueToAdd = 0.;
					sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::VisibleAndThermal;
					if (sunIllumination) {
						valueToAdd += Es_BOA[spectralBandIndex] * PhaseProperty::getMeanSKYL(spectralBandIndex) * costheta;
					}
					skyThermalEmission = genDTO.atmosphericApparentTemperature > 0. && (genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::Thermal || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::VisibleAndThermal);
					if (skyThermalEmission) {
						valueToAdd += M_PI * temperatureToLuminanceMeanOnSpectralBand(genDTO.atmosphericApparentTemperature, PLANCK, genDTO.lowerBandWavelength[spectralBandIndex] * 1E-6, genDTO.upperBandWavelength[spectralBandIndex] * 1E-6, precision, maxNumberOfIterations) * PhaseProperty::getTemperatureAtmosphereAbsorption(spectralBandIndex);
					}
					if (spectralBandIndex > 0) {
						value << " ";
					}
					value << valueToAdd;

				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 3; spectralBandIndex++) {
						value << " 0";
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The sky does not illuminate in these bands
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 2; spectralBandIndex++) {
						value << " 0";
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.color", value.str());

//				if (genDTO.isSimulatedSolarNoiseCheck) {
//					value.str("");
//					value << genDTO.lightCounter;
//					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sky.id", value.str());
//				}

				if (!noSolar) {					//add second light source
					//sunlight type
					if (genDTO.penumbraEffects) {
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.type", "sun");
						if (genDTO.sunDistance > 81000) {
							genDTO.sunRadius = genDTO.sunRadius * 81000 / genDTO.sunDistance;
							genDTO.sunDistance = 81000;
						}
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.radius", boost::lexical_cast<std::string>(genDTO.sunRadius));
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.meanDistance", boost::lexical_cast<std::string>(genDTO.sunDistance));
					} else {
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.type", "sharpdistant");
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.id", std::to_string(getLightIDFromType(genDTO, globalReferences::SUN, "Sun")));
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.generator", "phase");
					//irradiance
					std::ostringstream value;
					sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::VisibleAndThermal;
					value << (sunIllumination ? Es_BOA[0] * (1.0 - PhaseProperty::getMeanSKYL(0)) : 0.);
					for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::Visible || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::VisibleAndThermal;
						value << " " << (sunIllumination ? Es_BOA[spectralBandIndex] * (1.0 - PhaseProperty::getMeanSKYL(spectralBandIndex)) : 0.f);
					}

					if (PhaseProperty::getPolarization()) {
						// 3 * numberOfSpectralBands are added to simulate Q, U, V
						// The optical properties are repeated in these bands
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 3; spectralBandIndex++) {
							value << " 0";
						}
					}

					if (PhaseProperty::getFluorescence()) {
						// 2 * numberOfSpectralBands are added to simulate PSI and PSII
						// The sun does not illuminate in these bands
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands * 2; spectralBandIndex++) {
							value << " 0";
						}
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.color", value.str());

					//direction
					value.str("");
					if (genDTO.penumbraEffects) {
						value << (-genDTO.solarDirection.getX()) << " " << (-genDTO.solarDirection.getY()) << " " << (-genDTO.solarDirection.getZ());
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.dir", value.str());
					} else {
						value << (genDTO.solarDirection.getX()) << " " << (genDTO.solarDirection.getY()) << " " << (genDTO.solarDirection.getZ());
						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.direction", value.str());
					}

//					if (genDTO.isSimulatedSolarNoiseCheck) {
//						value.str("");
//						value << genDTO.lightCounter;
//						genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.id", value.str());
//					}
				}
			}
		}
//		genDTO.lightCounter++;
//	}
}

/*  Compute the LAD depending on the choice of the user */
void printNumberOfFoliarModels() {
	if (PhaseProperty::getNumberOfPhaseExternModels() > 0) {
		if (PhaseProperty::getNumberOfPhaseExternModels() > 1) {
			cout << "    -> " << PhaseProperty::getNumberOfPhaseExternModels() << " extern phase functions." << endl;
		} else {
			cout << "    -> " << PhaseProperty::getNumberOfPhaseExternModels() << " extern phase function." << endl;
		}
	}

	if (PhaseProperty::getNumberOfFoliarModels() > 1) {
		cout << "    -> " << PhaseProperty::getNumberOfFoliarModels() << " vegetation phase functions." << endl;
	} else {
		cout << "    -> " << PhaseProperty::getNumberOfFoliarModels() << " vegetation phase function." << endl;
	}
}

void writePhasePropertiesNumberOfModels(ofstream & outCoeffSpecProperties,
										PhaseKeyMappingGenerator & pkmg,
										bool generateEquivalentLambertianFromVegetation) {
	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE_SPEC);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfHapkeModels() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE2_SPEC);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfHapke2Models() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::PHASE_EXTERN);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfPhaseExternModels() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::RPV_PHASE);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfRPVModels() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfFoliarModels() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::WATER_INTERFACE);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << PhaseProperty::getNumberOfWaterInterfaceModels() << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase();
	if (generateEquivalentLambertianFromVegetation) {
		outCoeffSpecProperties << (PhaseProperty::getNumberOfLambertianModels() + PhaseProperty::getNumberOfEquivalentLambertianModels());
	} else {
		outCoeffSpecProperties << PhaseProperty::getNumberOfLambertianModels();
	}
	outCoeffSpecProperties << endl;
	pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN_SPEC);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << 0 << endl;

	// If available, write the UI corresponding indices

}

void readMultiplicativeFactor(const PhaseGeneralParametersDTO & genDTO) {
	PhaseProperty::resizeForMultiplicativeFactorforLUT();
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorLambertien(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorSpecular(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorHapke(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorHapke2(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorRPV(genDTO), "");
}

void printTimeAtmosphere(	time_t & tMiddle,
							const time_t & tInit) {
	int processingTimeMiddle = 0;
	time(&tMiddle);
	processingTimeMiddle = tMiddle - tInit;
	cout << "  -> Atmosphere Processing Time : " << processingTimeMiddle / 3600 << " hours " << (processingTimeMiddle % 3600) / 60 << " minutes "
			<< processingTimeMiddle % 60 << "  seconds" << endl;
	cout << " - End of Atmosphere processing" << endl;
}

void cleanAndCreateDirectory(bool isAirFunctionEmpty) {
	/* The precomputation of the Fresnel coefficients for the specular reflection on facets could not be used */
	bool isSpecular = false;
	if (PhaseProperty::getNumberOfFoliarModels() > 0 || !isAirFunctionEmpty || isSpecular || PhaseProperty::getNumberOfPhaseExternModels() > 0) {
		createOutputDirectory(DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator);
	}
	string cleanLib_phase = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + "LAD.txt";
	//DartEnv::getCurrentSimulationOutputPath() +	"lib_phase" + pathSeparator + "typeOfLeafAngleDistribution.txt";
	removeFile(cleanLib_phase);
	cleanLib_phase = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + "LAD_ELLIPT.txt";
	//DartEnv::getCurrentSimulationOutputPath() +	"lib_phase" + pathSeparator + "LAD_ELLIPT.txt";
	removeFile(cleanLib_phase);
}

void computeSpecular(	SpecularData & specularProperty,
						const double & specularIntensity,
						const unsigned int & spectralBandIndex,
						shared_ptr<SpecularDatabaseParameters> & specularDatabaseParameters) {

	if (specularProperty.isSpecular) {
		specularProperty._useRefraction = specularDatabaseParameters->isUseRefraction();

		specularProperty._specularRadianceDistribution = specularDatabaseParameters->getRadianceDistribution();

		specularProperty.refraction_index_down = specularDatabaseParameters->getRefractionIndexDownForBand(spectralBandIndex);
		specularProperty.refraction_index_up = specularDatabaseParameters->getRefractionIndexUpForBand(spectralBandIndex);

		specularProperty.intensity_factor_down_to_up = specularDatabaseParameters->getIntensityFactorDownToUpForBand(spectralBandIndex)
				* specularIntensity;
		specularProperty.intensity_factor_up_to_down = specularDatabaseParameters->getIntensityFactorUpToDownForBand(spectralBandIndex)
				* specularIntensity;

		specularProperty.angular_divergence_down_to_up = specularDatabaseParameters->getAngularDivergenceDownToUpForBand(spectralBandIndex);
		specularProperty.angular_divergence_up_to_down = specularDatabaseParameters->getAngularDivergenceUpToDownForBand(spectralBandIndex);

		specularProperty.k_power_down_to_up = specularDatabaseParameters->getKPowerDownToUpForBand(spectralBandIndex);
		specularProperty.k_power_up_to_down = specularDatabaseParameters->getKPowerUpToDownForBand(spectralBandIndex);

		specularProperty.albedo_down_to_up = PhaseProperty::computeSpecularAlbedo(specularProperty.intensity_factor_down_to_up,
			specularProperty.refraction_index_down, specularProperty.refraction_index_up, 100.0) * specularIntensity;
		specularProperty.albedo_up_to_down = PhaseProperty::computeSpecularAlbedo(specularProperty.intensity_factor_up_to_down,
			specularProperty.refraction_index_up, specularProperty.refraction_index_down, 100.0) * specularIntensity;

		specularProperty.albedo_refraction_down_to_up = (1
				- PhaseProperty::computeSpecularAlbedo(specularProperty.intensity_factor_down_to_up, specularProperty.refraction_index_down,
					specularProperty.refraction_index_up, 100.0)) * specularIntensity;

		specularProperty.albedo_refraction_up_to_down = (1
				- PhaseProperty::computeSpecularAlbedo(specularProperty.intensity_factor_up_to_down, specularProperty.refraction_index_up,
					specularProperty.refraction_index_down, 100.0)) * specularIntensity;
	}
}

void writeSpecular(	SpecularData & specularProperty,
					ofstream & outCoeffSpecProperties,
					PhaseKeyMappingGenerator & pkmg) {

	outCoeffSpecProperties << pkmg.isSpecularKey() << specularProperty.isSpecular << endl;

	if (specularProperty.isSpecular) {
		outCoeffSpecProperties << pkmg.cleUseRefraction() << specularProperty._useRefraction << endl;

		outCoeffSpecProperties << pkmg.cleSpecularRadianceDistribution() << specularProperty._specularRadianceDistribution << endl;

		outCoeffSpecProperties << pkmg.cleRefractionIndexUp() << specularProperty.refraction_index_up << endl;
		outCoeffSpecProperties << pkmg.cleRefractionIndexDown() << specularProperty.refraction_index_down << endl;

		outCoeffSpecProperties << pkmg.cleIntensityFactorDownToUp() << specularProperty.intensity_factor_down_to_up << endl;
		outCoeffSpecProperties << pkmg.cleIntensityFactorUpToDown() << specularProperty.intensity_factor_up_to_down << endl;

		outCoeffSpecProperties << pkmg.cleAngularDivergenceDownToUp() << specularProperty.angular_divergence_down_to_up << endl;
		outCoeffSpecProperties << pkmg.cleAngularDivergenceUpToDown() << specularProperty.angular_divergence_up_to_down << endl;

		outCoeffSpecProperties << pkmg.cleKPowerDownToUp() << specularProperty.k_power_down_to_up << endl;
		outCoeffSpecProperties << pkmg.cleKPowerUpToDown() << specularProperty.k_power_up_to_down << endl;

		outCoeffSpecProperties << pkmg.cleAlbedoRefractionSpecularDownToUp() << specularProperty.albedo_refraction_down_to_up << endl;
		outCoeffSpecProperties << pkmg.cleAlbedoRefractionSpecularUpToDown() << specularProperty.albedo_refraction_up_to_down << endl;

		outCoeffSpecProperties << pkmg.cleAlbedoSpecularDownToUp() << specularProperty.albedo_down_to_up << endl;
		outCoeffSpecProperties << pkmg.cleAlbedoSpecularUpToDown() << specularProperty.albedo_up_to_down << endl;
	} else {
		outCoeffSpecProperties << pkmg.cleUseRefraction() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleSpecularRadianceDistribution() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleRefractionIndexUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleRefractionIndexDown() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleIntensityFactorDownToUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleIntensityFactorUpToDown() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleAngularDivergenceDownToUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleAngularDivergenceUpToDown() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleKPowerDownToUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleKPowerUpToDown() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleAlbedoRefractionSpecularDownToUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleAlbedoRefractionSpecularUpToDown() << 0 << endl;

		outCoeffSpecProperties << pkmg.cleAlbedoSpecularDownToUp() << 0 << endl;
		outCoeffSpecProperties << pkmg.cleAlbedoSpecularUpToDown() << 0 << endl;
	}
}

/* ----------------------------------------------------------------------------------------------------------
 * 									 	Phase Extern
 */
void treatPhaseExtern(OpticalPropertyParameters & PhaseExternPropertyParameters,
						const vector<bool> & useSpectralIrradianceForReflectance,
						PhaseKeyMappingGenerator & pkmg,
						ofstream & outCoeffSpecProperties,
						PhaseGeneralParametersDTO & genDTO,
						const vector<DirectionCartesienne> & listDirections) {

	pkmg.setTypeFonctionDePhase(globalReferences::PHASE_EXTERN);

//	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
//		PhaseExternProperty PhaseExternProperty;
//
//		computeOpticalPropertiesPhaseExtern(spectralBandIndex, PhaseExternPropertyParameters, listDirections, PhaseExternProperty);
//
//
//		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile(
//			"MultispectralOpticalFunctions.PhaseExternMultiFunctions.PhaseExtern[", PhaseExternPropertyParameters.getModelIndex(), -1.0, -1.0,
//			-1.0);
//
//		writeTreatPhaseExtern(outCoeffSpecProperties, pkmg, PhaseExternProperty, PhaseExternPropertyParameters.getModelName(), spectralBandIndex,
//			PhaseExternPropertyParameters.getModelIndex());
//	}

	std::ostringstream keyRoot;

	keyRoot << "scene.materials." << "PhaseExtern" <<  PhaseExternPropertyParameters.getModelIndex()<< ".";
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "PhaseExtern");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "dataBase",
			std::to_string(PhaseExternPropertyParameters.getModelIndex()) + ".phaseExternData");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
}


void computeOpticalPropertiesPhaseExtern(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & PhaseExternPropertyParameters,
										const vector<DirectionCartesienne> & listDirections,
										PhaseExternProperty & PhaseExternProperty) {

//	shared_ptr<PhaseExternDatabaseParameters> PhaseExternDatabaseParameters = ::static_pointer_cast<PhaseExternDatabaseParameters>(
//		PhaseExternPropertyParameters.getDatabaseParameters(PHASE_EXTERN_DATABASE));
//	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
//		PhaseExternPropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
//	shared_ptr<TransmittanceDatabaseParameters> hapkeTransmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
//		PhaseExternPropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));
//	shared_ptr<SpecularDatabaseParameters> kapkeSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
//		PhaseExternPropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
//	unsigned int modelIndex = PhaseExternPropertyParameters.getModelIndex();
}



void writeTreatPhaseExtern(	ofstream & outCoeffSpecProperties,
					PhaseKeyMappingGenerator & pkmg,
					PhaseExternProperty & PhaseExternProperty,
					const string & modelName,
					const unsigned int & spectralBandIndex,
					const unsigned int & modelIndex) {
//	pkmg.setNumeroFonctionDePhase(modelIndex);
//	pkmg.setBandeSpectrale(spectralBandIndex);
//	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
//	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
}

void readLiDAR(){
	PhaseDAO::readLiDARXML();
}

void treatMixedMaterial(PhaseGeneralParametersDTO & genDTO,	unsigned int opIndex, unsigned int pfIndex) {
	// LuxCore
	unsigned int model1Type, model1Index, model2Type, model2Index;
	std::string phaseName, model1Name, model2Name;
	double percentOfModel1;
	EXIT_IF(!PhaseDAO::readCoeffDiffParametersForMixedMaterial(pfIndex, phaseName, model1Type, model1Index, model1Name, model2Type, model2Index, model2Name, percentOfModel1), "");

	std::string material1Name = getMaterialName(model1Type, model1Index);
	std::string material2Name = getMaterialName(model2Type, model2Index);

	std::string mixedMaterialRootKey = "scene.materials.mixedMat" + boost::lexical_cast<std::string>(pfIndex) + ".";

	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "type", "mix");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "id", std::to_string(opIndex));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "amount", boost::lexical_cast<std::string>(percentOfModel1 / 100.));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material1", material1Name);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material1Type", boost::lexical_cast<std::string>(model1Type));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material1Index", boost::lexical_cast<std::string>(model1Index));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material2", material2Name);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material2Type", boost::lexical_cast<std::string>(model2Type));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "material2Index", boost::lexical_cast<std::string>(model2Index));
}

std::string getMaterialName(unsigned int modelType, unsigned int modelIndex) {
	std::string materialName = "";
	switch (modelType) {
		case globalReferences::LAMBERTIEN:
			if (modelIndex >= PhaseProperty::getNumberOfLambertianModels()) {
				cerr << "Lambertian optical property " << modelIndex << " referenced by a mixed material doesn't exists." << endl;
				exit(-7);
			}
			materialName = "lambertian";
			break;
		case globalReferences::RPV_PHASE:
			if (modelIndex >= PhaseProperty::getNumberOfRPVModels()) {
				cerr << "RPV optical property " << modelIndex << " referenced by a mixed material doesn't exists." << endl;
				exit(-7);
			}
			materialName = "RPV";
			break;
		case globalReferences::AIR_FP:
		case globalReferences::FOLIAIRE:
		case globalReferences::MIX_VOLUME:
			cerr << "Can not create a mixed material out of a volume." << endl;
			exit(-6);
		case globalReferences::SPECULAR_LUX:
			if (modelIndex >= PhaseProperty::getNumberOfSpecularModels()) {
				cerr << "Specular optical property " << modelIndex << " referenced by a mixed material doesn't exists." << endl;
				exit(-7);
			}
			materialName = "specular";
			break;
		case globalReferences::MIX_MATERIAL:
			materialName = "mixedMat";
			break;
		default:
			materialName = "notImplementedYet";
			break;
	}
	materialName += boost::lexical_cast<std::string>(modelIndex) /*+ "_0"*/;
	return materialName;
}

FTFoliar foliarPrecomputing(const PhaseGeneralParametersDTO & genDTO,
							const vector<DirectionCartesienne> & listDirections,
							const unsigned int & iterationStepTheta,
							const unsigned int & iterationStepPhi,
							const double & solidAnglesSum) {
	FTFoliar fTFoliar;

	if (PhaseProperty::getNumberOfFoliarModels() > 0) {
		cout << "	Pre-compute vegetation models ..." << std::endl;
	}
	fTFoliar.listDirectionsSize = listDirections.size();
	fTFoliar.monteCarloCheck = genDTO.radiativeTransferMethod;
	fTFoliar.iterationStepPhi = iterationStepPhi;
	fTFoliar.iterationStepTheta = iterationStepTheta;
	fTFoliar.numDivDiff = round(sqrt(810.0 / fTFoliar.listDirectionsSize));
	fTFoliar.numDivSpecPol = round(sqrt(10001.0 / fTFoliar.listDirectionsSize));
	if (fTFoliar.numDivDiff < 2) {
		fTFoliar.numDivDiff = 2;
	} else if (fTFoliar.numDivDiff > 100) {
		fTFoliar.numDivDiff = 100;
	}
	if (fTFoliar.numDivSpecPol < 2) {
		fTFoliar.numDivSpecPol = 2;
	} else if (fTFoliar.numDivSpecPol > 100) {
		fTFoliar.numDivSpecPol = 100;
	}
	fTFoliar.solidAnglesSum = solidAnglesSum;

	return fTFoliar;
}

/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
void treatFluid(PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				vector<AirPhaseFunction> & airPhaseFunctions,
				PhaseGeneralParametersDTO & genDTO) {
	pkmg.setTypeFonctionDePhase(globalReferences::AIR_FP);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << airPhaseFunctions.size() << endl;
	if (!airPhaseFunctions.empty()) {
		cout << " - Processing of Air Phase Functions" << endl;
		for (AirPhaseFunction & apf : airPhaseFunctions) {
			std::ostringstream keyRootVol, keyRootMat;
			keyRootVol << "scene.op.volumes." << "fluid_" <<  apf.getIndex()  << ".";
			apf.readFileData(genDTO.lowerBandWavelength, genDTO.upperBandWavelength);
			const shared_ptr<AirLaw> airLaw = apf.getAirLaw();
			const unsigned int airLawType = airLaw->getType();
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "generator", "phase");
			if (airLawType == 0) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "type", "homogeneous_TTHG");
				const shared_ptr<AirHenleyGreenstein> henyeyGreensteinLaw = std::dynamic_pointer_cast<AirHenleyGreenstein>(airLaw);
				const vector<CoefficientHenleyGreenstein> coefficients = henyeyGreensteinLaw->getCoefficients();
				std::ostringstream aCoeffStream, g1CoeffStream, g2CoeffStream, absorptionStream, scatteringStream;
				for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
					aCoeffStream << coefficients[sb].getCoefficientA() << " ";
					g1CoeffStream << coefficients[sb].getCoefficientG1() << " ";
					g2CoeffStream << -coefficients[sb].getCoefficientG2() << " ";
					const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
					const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
					absorptionStream << (1.0 - albedo) * sigma << " ";
					scatteringStream << albedo * sigma << " ";
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
						for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
							aCoeffStream << coefficients[sb].getCoefficientA() << " ";
							g1CoeffStream << coefficients[sb].getCoefficientG1() << " ";
							g2CoeffStream << -coefficients[sb].getCoefficientG2() << " ";
							const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
							const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
							absorptionStream << (1.0 - albedo) * sigma << " ";
							scatteringStream << albedo * sigma << " ";
						}
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
							aCoeffStream << coefficients[sb].getCoefficientA() << " ";
							g1CoeffStream << coefficients[sb].getCoefficientG1() << " ";
							g2CoeffStream << -coefficients[sb].getCoefficientG2() << " ";
							const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
							const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
							absorptionStream << (1.0 - albedo) * sigma << " ";
							scatteringStream << albedo * sigma << " ";
						}
					}
				}

				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "asymmetry1", g1CoeffStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "asymmetry2", g2CoeffStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "factor", aCoeffStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "multiscattering", "1");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "absorption", absorptionStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "scattering", scatteringStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "ior", "1");
			}
			else {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "type", "homogeneous_Rayleigh");
				const shared_ptr<AirRayleigh> rayleighLaw = std::dynamic_pointer_cast<AirRayleigh>(airLaw);
				const vector<CoefficientRayleigh> coefficients = rayleighLaw->getCoefficients();
				std::ostringstream aCoeffStream, cCoeffStream, absorptionStream, scatteringStream;
				for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
					aCoeffStream << coefficients[sb].getCoefficientA() << " ";
					cCoeffStream << coefficients[sb].getCoefficientC() << " ";
					const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
					const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
					absorptionStream << (1.0 - albedo) * sigma << " ";
					scatteringStream << albedo * sigma << " ";
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
						for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
							aCoeffStream << coefficients[sb].getCoefficientA() << " ";
							cCoeffStream << coefficients[sb].getCoefficientC() << " ";
							const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
							const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
							absorptionStream << (1.0 - albedo) * sigma << " ";
							scatteringStream << albedo * sigma << " ";
						}
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int sb = 0; sb < genDTO.numberOfSpectralBands; sb++) {
							aCoeffStream << coefficients[sb].getCoefficientA() << " ";
							cCoeffStream << coefficients[sb].getCoefficientC() << " ";
							const double albedo = apf.getAlbedoForBand(sb) * apf.getAlbedoMultiplicativeFactorForBand(sb);
							const double sigma = apf.getSigmaForBand(sb) * apf.getSigmaMultiplicativeFactorForBand(sb);
							absorptionStream << (1.0 - albedo) * sigma << " ";
							scatteringStream << albedo * sigma << " ";
						}
					}
				}

				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "A", aCoeffStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "C", cCoeffStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "multiscattering", "1");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "absorption", absorptionStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "scattering", scatteringStream.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "ior", "1");
			}

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForFluidOpticalFactors(apf.getIndex(), opticalFactorParameters)) {
				std::string textureName = "f" + boost::lexical_cast<std::string>(apf.getIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootVol.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", textureName + ".bin");

				if (opticalFactorParameters.sameOpticalFactorForAllBands) {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", opticalFactorParameters.fileNames[0]);
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", opticalFactorParameters.duplicateFirstLayer[0] ? "1" : "0");
				} else {
					std::ostringstream inputsOSS, duplicationOSS;
					inputsOSS << opticalFactorParameters.fileNames[0];
					duplicationOSS << opticalFactorParameters.duplicateFirstLayer[0];
					for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
						duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
					}

					if (PhaseProperty::getPolarization()) {
						// 3 * numberOfSpectralBands are added to simulate Q, U, V
						// The optical properties are repeated in these bands
						for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
							for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
								inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
								duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
							}
						}
					}

					if (PhaseProperty::getFluorescence()) {
						// 2 * numberOfSpectralBands are added to simulate PSI and PSII
						// The optical properties are repeated in these bands
						for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
							for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
								inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
								duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
							}
						}
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
				}
			}
		}
		cout << " - End of Processing of Air Phase Functions" << endl;
	}
}

void treatWater(OpticalPropertyParameters & waterPropertyParameters,
				unsigned int opIndex,
				const vector<bool> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO) {
	pkmg.setTypeFonctionDePhase(globalReferences::WATER_INTERFACE);
	pkmg.setNumeroFonctionDePhase(waterPropertyParameters.getModelIndex());
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl; // Oui

	// On check le type de la première DB pour en deduire la definition des WC
	switch (waterPropertyParameters.getDatabaseForProperty(0)->getDatabaseType()) {
		case WATER_DATABASE_UP:
		case WATER_DATABASE_DOWN: {
			outCoeffSpecProperties << pkmg.isDatabaseInWater() << "true" << endl;
			shared_ptr<WaterDatabaseParameters> waterDatabaseParametersUp = ::static_pointer_cast<WaterDatabaseParameters>(
				waterPropertyParameters.getDatabaseParameters(WATER_DATABASE_UP));

			shared_ptr<WaterDatabaseParameters> waterDatabaseParametersDown = ::static_pointer_cast<WaterDatabaseParameters>(
				waterPropertyParameters.getDatabaseParameters(WATER_DATABASE_DOWN));

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; ++spectralBandIndex) {
				pkmg.setBandeSpectrale(spectralBandIndex);
				outCoeffSpecProperties << pkmg.upwardRefractionIndexInWater() << waterDatabaseParametersUp->getrefractionForBand(spectralBandIndex)
						<< endl;
				outCoeffSpecProperties << pkmg.downwardRefractionIndexInWater()
						<< waterDatabaseParametersDown->getrefractionForBand(spectralBandIndex) << endl;
			}

			break;
		}
		case WATER_NO_DATABASE_FILE: {
			outCoeffSpecProperties << pkmg.isDatabaseInWater() << "false" << endl;

			shared_ptr<WaterNoDatabaseFileParameters> waterDatabaseParameters = ::static_pointer_cast<WaterNoDatabaseFileParameters>(
				waterPropertyParameters.getDatabaseParameters(WATER_NO_DATABASE_FILE));
			outCoeffSpecProperties << pkmg.databaseNameInWater() << waterDatabaseParameters->getWaterFileModelName() << endl;

			break;
		}
		case WATER_NO_DATABASE_USER_DEFINED_UP:
		case WATER_NO_DATABASE_USER_DEFINED_DOWN: {
			outCoeffSpecProperties << pkmg.isDatabaseInWater() << "false" << endl;
			shared_ptr<WaterDatabaseParameters> waterDatabaseParametersUp = ::static_pointer_cast<WaterDatabaseParameters>(
				waterPropertyParameters.getDatabaseParameters(WATER_NO_DATABASE_USER_DEFINED_UP));

			shared_ptr<WaterDatabaseParameters> waterDatabaseParametersDown = ::static_pointer_cast<WaterDatabaseParameters>(
				waterPropertyParameters.getDatabaseParameters(WATER_NO_DATABASE_USER_DEFINED_DOWN));

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; ++spectralBandIndex) {
				pkmg.setBandeSpectrale(spectralBandIndex);
				outCoeffSpecProperties << pkmg.upwardRefractionIndexInWater() << waterDatabaseParametersUp->getrefractionForBand(spectralBandIndex)
						<< endl;
				outCoeffSpecProperties << pkmg.downwardRefractionIndexInWater()
						<< waterDatabaseParametersDown->getrefractionForBand(spectralBandIndex) << endl;
			}
			break;
		}
		default:
			break;
	}

}

double numberOfAngularSectors(	const vector<DirectionCartesienne> & listDirections,
								double & solidAnglesSum) {
	double countNumberOfAngularSectors = 0;
	for(const DirectionCartesienne & direction : listDirections)
	{
		if (direction.getProperty() != DirectionRoot::VIRTUAL) {
			solidAnglesSum += direction.getSolidAngle();
		}
		if ((int) direction.getAngularSector() > countNumberOfAngularSectors) {
			countNumberOfAngularSectors = direction.getAngularSector();
		}
	}
	++countNumberOfAngularSectors;
	return countNumberOfAngularSectors;
}

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture/ecriture)
/**
 * Traitement des fonctions de phase de type "Lambertien + Speculaire" (mode multispectral)
 *
 * @param FILE *outCoeffspec 			: pointeur de fichier a ecrire
 * @param FILE *inCoeffspec 				: pointeur de fichier a lire
 * @param int int reflectanceModelsParameters.numberOfLambertianSpecularModels	: nombre de fonctions de phase de type "Lambertien + Speculaire"
 * @param string *Comm1					: string permettant de gerer l'acces aux bases de donnees
 * @param string Comm					: string permettant de gerer l'acces aux bases de donnees
 * @param int k							: index de l'intervalle spectral
 * @param int var_lamb_spec[]			: tableau d'index pour chaque fonction de phase pour indiquer si elle a ete calculee ou pas
 * @param OpticalProperties *optique				: structure permettant de gerer les parametres des bases de donnees
 * @param double *lowerBandWavelength					: tableau des bornes inferieures de chaque intervalle
 * @param double *upperBandWavelength					: tableau des bornes superieures de chaque intervalle
 * @param string *nom_lamb_spec			: nom du fichier de la base de donnees
 * @param int nb_dir 					: nombre de directions discretes
 * @param vecteur *tab_dir_vect 			: tableau des vecteurs directeurs des directions discretes
 * @param vector<int> & lambdaDomain :    tableau du mode (R,R+T ou T) de chaque intervalle
 */

//----------------------------------------------------------------------------------------
//**! Conversion des directions theta, phi en vecteur
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
/**!
 TRAITEMENT DE L'ATMOSPHERE: LECTURE DE LA BASE DE DONNEES
 subroutine rajoutee par Thierry Dallest, Juin 2001
 */

void setDefaultAtmosphereParameters(AtmosphereParameters& atmosphereParameters) {
	atmosphereParameters.computationMethodTOAtoBOA = 0;
	atmosphereParameters.numberOfAerosol = 1;
	atmosphereParameters.mulFactorH2O = 0.0;
	atmosphereParameters.aerosolScaleFactor = 2000.0;
	atmosphereParameters.gasOpticalDepthForScattering = 0.0;
	atmosphereParameters.aerosolOpticalDepthForScattering = 0.0;
	atmosphereParameters.aerosolSingleScatteringAlbedo = 0.0;
	atmosphereParameters.henleyGreensteinParameters.resize(1);
	atmosphereParameters.henleyGreensteinParameters[0].g1 = 0.0;
	atmosphereParameters.henleyGreensteinParameters[0].g2 = 0.0;
	atmosphereParameters.henleyGreensteinParameters[0].a = 0.0;
	atmosphereParameters.gasScaleFactor = 9000.0;
	atmosphereParameters.gasTotalTransmittanceForAbsoption = 0.0;
	atmosphereParameters.sensorHeight = 0.0;
	atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer = 0;
	atmosphereParameters.isAtmosphereDatabase = 0;
	atmosphereParameters.aerosolProfil = 1;
	atmosphereParameters.minimumNumberOfDivisions = 2;
}

void processingAtmosphere(	PhaseGeneralParametersDTO & genDTO,
							PhaseKeyMappingGenerator & pkmg,
							ofstream & outCoeffSpecProperties,
							vector<AirPhaseFunction> & airPhaseFunctions,
							int& dayOfTheYear) {
	/* Traitement Eclairement  */
	vector<double> irradianceVector;
	vector<double> skylVector;
	EXIT_IF(!(PhaseDAO::readIrradianceParameters(genDTO, irradianceVector, skylVector)), "");
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < skylVector.size(); spectralBandIndex++) {
		PhaseProperty::setMeanSKYL(spectralBandIndex, skylVector[spectralBandIndex]);
	}
	AtmosphereParameters atmosphereParameters;
	setDefaultAtmosphereParameters(atmosphereParameters);
	cout << "   -> Reading atmosphere.xml" << endl;
	EXIT_IF(!PhaseDAO::readAtmosphereParameters(atmosphereParameters), "");

	const vector<DirectionCartesienne> & listDirections = genDTO._directionSet[atmosphereParameters._directionSetIndex]._directions;

	genDTO.computationMethodTOAtoBOA = atmosphereParameters.computationMethodTOAtoBOA; // transfer this parameter for dart lux atmos

	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && atmosphereParameters.fullyPrecomputed) {
		cerr << "Error: LiDAR can not be simulated with precomputed atmospheric radiative transfer files." << endl;
		cerr << "       Atmosphere simulation has been disabled" << endl;
		atmosphereParameters.computationMethodTOAtoBOA = 0;
	}

	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.computationMethodTOAtoBOA == 2) {
		// LiDAR atmosphere's simulation is only performed with Monte-Carlo mode.
		genDTO.atmosphereRTmode = 1;
	}
	PhaseProperty::setIsComputedIrradianceBottomOfAtmosphereCheck(false);
	bool isAtmosphereDatabaseCheck = false;

	string irradianceDatabasePath = "";
	string atmosphereDatabaseName = "";

	if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0 || atmosphereParameters.isAtmosphereDatabase == 1) {

		if (!atmosphereParameters.databaseName.empty()) {
			atmosphereDatabaseName = atmosphereParameters.databaseName;
			string atmosphereDatabasePath = DartEnv::getDatabasePath(atmosphereDatabaseName) + atmosphereDatabaseName;
			PhaseProperty::setAtmosphericDatabasePath(atmosphereDatabasePath);
			isAtmosphereDatabaseCheck = true;
//			if (PhaseProperty::getCalculatorMethod() == 2 || PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 1
//					|| PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 2) {
//				PhaseProperty::getIdsInAtmosphericTables0FromGasTable(atmosphereParameters);
//			} else {
//				PhaseProperty::getIdsInAtmosphericTablesMultiThread();
//			}
		} else {
			cout << "No atmosphere database " << endl;
		}

		if (!PhaseProperty::getSolarConstantDatabaseName().empty()) {
			irradianceDatabasePath = DartEnv::getDatabasePath(PhaseProperty::getSolarConstantDatabaseName())
					+ PhaseProperty::getSolarConstantDatabaseName();
			PhaseProperty::setSolarConstantDatabaseName(irradianceDatabasePath);
			PhaseProperty::getIdsInAtmosphericTablesMultiThread();
		} else {
			if (isAtmosphereDatabaseCheck) {
				PhaseProperty::getIdsInAtmosphericTablesFromGasTable(atmosphereParameters);
			}
		}
	}
	if (PhaseProperty::getCalculatorMethod() != 2) { // NOT LIDAR
		if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 1) {
			cout << "	- Band irradiance (user defined) - TOA or BOA (+ SKYL)" << endl;
			for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
				PhaseProperty::setMeanIrradianceTopOfAtmosphere(k, irradianceVector[k]);
				if (PhaseProperty::getComputationMethodeOfToaToBoa() == 0) {
					PhaseProperty::setMeanIrradianceBottomOfAtmosphere(k, irradianceVector[k]);
				}
			}
			if (PhaseProperty::getComputationMethodeOfToaToBoa() == 0) {
				PhaseProperty::setIsComputedIrradianceBottomOfAtmosphereCheck(true);
			}
		} else if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0) {
			cout << "	- Spectral irradiance (database) - TOA or BOA (+ SKYL)";
			PhaseProperty::readDatabaseIrradiance(irradianceDatabasePath);
			// Sun-Earth distance solar day correction
			double gamma = 2 * M_PI * (dayOfTheYear - 1) / 365;
			double earthSunCorrectionFactor = 1.000110 + 0.034221 * cos(gamma) + 0.001280 * sin(gamma) + 0.000719 * cos(2 * gamma)
					+ 0.000077 * sin(2 * gamma);
			double modifiedTOA;
			if (dayOfTheYear > 0) {
				for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
					//correction de la distance terre-soleil
					modifiedTOA = PhaseProperty::getMeanIrradianceTopOfAtmosphere(k) * earthSunCorrectionFactor;
					PhaseProperty::setMeanIrradianceTopOfAtmosphere(k, modifiedTOA);
					for (unsigned int i = 0; i < PhaseProperty::getNumberOfIrradianceSpectralValues(k); i++) {
						modifiedTOA = PhaseProperty::getIrradianceTopOfAtmosphereValue(k, i) * earthSunCorrectionFactor;
						PhaseProperty::setIrradianceTopOfAtmosphereVector(k, i, modifiedTOA);
					}
				}
			}
			if (PhaseProperty::getComputationMethodeOfToaToBoa() == 0) { // No atmosphere
				for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
					PhaseProperty::setMeanIrradianceBottomOfAtmosphere(k, PhaseProperty::getMeanIrradianceTopOfAtmosphere(k));
					PhaseProperty::setIrradianceBottomOfAtmosphereVector(k, PhaseProperty::getIrradianceTopOfAtmosphereVector(k));
					PhaseProperty::updateIrradianceForPonderationVector(k, PhaseProperty::getIrradianceTopOfAtmosphereVector(k),
						genDTO.spectralDomain[k], genDTO.meanTemperature);
				}
				PhaseProperty::setIsComputedIrradianceBottomOfAtmosphereCheck(true);
				cout << ", empty atmosphere" << endl;
			} else {
				cout << endl;
			}
		}
	}
	/* Traitement Atmosphere  */
	if (atmosphereParameters.isAtmosphereDefined == 1 || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer) {
		AirPhaseFunction boaMoleculePhaseFunction(PhaseProperty::getNumberOfSpectralBands());
		vector<AirPhaseFunction> boaAerosolPhaseFunction;
		cout << "   -> Loading atmosphere parameters.." << endl;
		if (atmosphereParameters.isAtmosphereDatabase == 1) {
			if (isAtmosphereDatabaseCheck) {
				cout << "   -> with database" << endl;
				cout << "   -> number of aerosol  " << atmosphereParameters.numberOfAerosol << endl;
				executeAtmosphereWithDatabase(genDTO, atmosphereParameters, boaMoleculePhaseFunction, boaAerosolPhaseFunction, listDirections);

				if (!atmosphereParameters.fullyPrecomputed
						&& ((PhaseProperty::getComputationMethodeOfToaToBoa() == 2) || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer)) {
					if (genDTO.atmosphereRTmode == 1)
					{
						// Disable atmosphere phase function computation
					} else {
						PhaseProperty::atmospherePhaseFunction(listDirections, genDTO.solarDirection, atmosphereParameters.minimumNumberOfDivisions);
						PhaseProperty::writeAtmospherePhaseFunction(listDirections);
						if (PhaseProperty::getPolarization()) {
							for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
								/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
								/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
								PhasePolarization::computeAndWritePolarizationMatrixAtmosphere(listDirections, genDTO.solarDirection, spectralBandIndex);
							}
						}
					}
				}
			}
		} else {
			boaAerosolPhaseFunction.resize(1);
			AirPhaseFunction temp(PhaseProperty::getNumberOfSpectralBands());
			boaAerosolPhaseFunction[0] = temp;
			cout << "   -> without database" << endl;
#ifdef DART_WITH_NETCDF
			executeAtmosphereNcWithoutDatabase(atmosphereParameters, boaMoleculePhaseFunction, boaAerosolPhaseFunction[0], listDirections, genDTO.solarDirection);
#else
			cerr << "Atmosphere without database requires a build with DART_WITH_NETCDF enabled" << endl;
#endif

			if (!atmosphereParameters.fullyPrecomputed
					&& ((PhaseProperty::getComputationMethodeOfToaToBoa() == 2) || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer)) {
				if (genDTO.atmosphereRTmode == 1)
				{
					// Disable atmosphere phase function computation
				} else {
					PhaseProperty::atmospherePhaseFunction(listDirections, genDTO.solarDirection, atmosphereParameters.minimumNumberOfDivisions);
					PhaseProperty::writeAtmospherePhaseFunction(listDirections);
					if (PhaseProperty::getPolarization()) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
							/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
							/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
							PhasePolarization::computeAndWritePolarizationMatrixAtmosphere(listDirections, genDTO.solarDirection, spectralBandIndex);
						}
					}
				}
			}
		}
		// If 'Atmosphere in the scene', 2 additionnal air phase functions are added
		if (atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer) {
			boaMoleculePhaseFunction.setIndex(airPhaseFunctions.size());
			// Ajouter cles vers index de fonction phase d'air dans BOA
			outCoeffSpecProperties << pkmg.cleFonctionPhaseAirMoleculesIndice() << airPhaseFunctions.size() << endl;
			airPhaseFunctions.push_back(boaMoleculePhaseFunction);
			//atmosphereParameters.numberOfAerosol
			for (unsigned int typeindex = 0; typeindex < boaAerosolPhaseFunction.size(); typeindex++) {
				boaAerosolPhaseFunction[typeindex].setIndex(airPhaseFunctions.size());
				// Ajouter cles vers index de fonction phase d'air dans BOA
				outCoeffSpecProperties << pkmg.cleFonctionPhaseAirAerosolsIndice() << airPhaseFunctions.size() << endl;
				airPhaseFunctions.push_back(boaAerosolPhaseFunction[typeindex]);
				// cout<<"boa pf: "<<boaAerosolPhaseFunction[typeindex].getalbedomf().at(0)<<endl;
			}
		}

	} else {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
			PhaseProperty::setMeanIrradianceBottomOfAtmosphere(spectralBandIndex, PhaseProperty::getMeanIrradianceTopOfAtmosphere(spectralBandIndex));
		}
		PhaseProperty::setIsComputedIrradianceBottomOfAtmosphereCheck(true);
	}
}

void execute_batchPhase(PhaseGeneralParametersDTO & genDTO,
						string const& fileNameProperty) {

	/**
	 * VSAC 20100706 : rajout de la classe de generation des cles de recherche pour le mapping des donnees de phase
	 */
	PhaseKeyMappingGenerator pkmg(false);
	time_t tInit;
	time_t tEnd;
	time_t tMiddle;
	time(&tInit);
	PhaseProperty::setTimeInit(tInit);
	vector<AirPhaseFunction> airPhaseFunctions;
	ofstream outCoeffSpecProperties; //Ce flux de sortie va ecrire toutes les donnees relatives aux fonctions de phases pour permettre une recherche par mots-cles

	createOutputDirectory(DartEnv::getCurrentSimulationOutputPath());
	PhaseDAO::readAirPhaseFunctions(airPhaseFunctions, genDTO.numberOfSpectralBands);

// Lecture des valeurs solaires
	PhaseDAO::readAndStoreDirections(genDTO);
	PhaseProperty::setSunZenithAngleInRadians(genDTO.sourceDirection.getCenterTheta());
//	vector<DirectionCartesienne> listDirections;
	PhaseProperty::initiateSpectralBands(genDTO.numberOfSpectralBands, genDTO.lowerBandWavelength, genDTO.upperBandWavelength, genDTO.numberOfThreads);

	unsigned int modeFactor = 1;
	if (PhaseProperty::getFluorescence()) {
		modeFactor *= 3;
		genDTO.luxCoreRenderConfigProperties.ajouterCle("mode.fluorescence", "1");
		if (PhaseProperty::getCombinedPS()) {
			genDTO.luxCoreRenderConfigProperties.ajouterCle("mode.fluorescence.combinedPS", "1");
		}
	}
	if (PhaseProperty::getPolarization()) {
		modeFactor *= 4;
		genDTO.luxCoreRenderConfigProperties.ajouterCle("mode.polarization", "1");
	}
	genDTO.luxCoreRenderConfigProperties.ajouterCle("nbSpectralBands", std::to_string(genDTO.numberOfSpectralBands * modeFactor));

	string fileName = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
	FichierProperties phasePropert(fileName.c_str());
	outCoeffSpecProperties.open(fileName.c_str(), ofstream::app);

	if (!outCoeffSpecProperties) {
		cerr << "Impossible de creer le fichier phase.properties.txt" << endl;
		exit(99);
	}
	outCoeffSpecProperties.precision(15);

	// Temperature/Emission
	processEmissionMaterials(genDTO);

	// Light Sources
	LightsContainer secondarySourcesOfLight;
	PhaseDAO::readSecondarySourcesOfLight(secondarySourcesOfLight);
	writeSecondarySourcesOfLight(genDTO, outCoeffSpecProperties, secondarySourcesOfLight); // TODO: Make it work with the atmosphere

	/* ATMOSPHERE */
	cout << endl << " - Atmosphere processing :" << endl;
	processingAtmosphere(genDTO, pkmg, outCoeffSpecProperties, airPhaseFunctions, genDTO.dayOfTheYear);
	printTimeAtmosphere(tMiddle, tInit);

	/* READ DATA */
	outCoeffSpecProperties << pkmg.cleNombreBandesSpectrales() << genDTO.numberOfSpectralBands << endl;
	EXIT_IF(!PhaseDAO::readNumberOfOpticalPropertiesModels(), "");
	cleanAndCreateDirectory(airPhaseFunctions.empty());

	prepareProspect(genDTO);
	processSurfaceMaterials(genDTO, pkmg, outCoeffSpecProperties);
	phase::FluorescenceService vegetationFluorescenceService;
	phase::OpticalFactorsTextureWriter vegetationOpticalFactorsWriter;
	phase::VegetationMaterialWriter vegetationWriter(vegetationFluorescenceService, vegetationOpticalFactorsWriter);
	vegetationWriter.processAll(genDTO, pkmg, outCoeffSpecProperties);
	writePhasePropertiesSpectralIrradiance(outCoeffSpecProperties, pkmg, genDTO);

//	// Lecture des bases de donnees
//	vector<OpticalPropertyParameters> opticalPropertiesDatabases = createDatabasesReadingProperties(genDTO);
//	vector<OpticalPropertyParameters>::iterator endIterator = opticalPropertiesDatabases.end();
//
//	unsigned int currentOPIndex = 0;
//	for (vector<OpticalPropertyParameters>::iterator iterator = opticalPropertiesDatabases.begin(); iterator < endIterator; ++iterator) {
//		for (unsigned int numPropertyDatabase = 0; numPropertyDatabase < iterator->getNbDatabaseForProperty(); ++numPropertyDatabase) {
//
//			if (iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_FILE
//					&& iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_UP
//					&& iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_DOWN) { // TU LE SENS LE WATER BIEN CHIANT ???
//
//				shared_ptr<PhaseDatabaseParameters> currentDatabase = iterator->getDatabaseForProperty(numPropertyDatabase);
//				PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, currentDatabase);
	// Databases self destruct procedure
	vector<OpticalPropertyParameters> opticalPropertiesDatabases = createDatabasesReadingProperties(genDTO);
	vector<OpticalPropertyParameters>::iterator selfDestructEndIterator = opticalPropertiesDatabases.end();
	for (vector<OpticalPropertyParameters>::iterator selfDestructIterator = opticalPropertiesDatabases.begin(); selfDestructIterator < selfDestructEndIterator; ++selfDestructIterator) {
		for (unsigned int numPropertyDatabase = 0; numPropertyDatabase < selfDestructIterator->getNbDatabaseForProperty(); ++numPropertyDatabase) {
			if (selfDestructIterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_FILE
					&& selfDestructIterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_UP
					&& selfDestructIterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_DOWN) {
				shared_ptr<PhaseDatabaseParameters> currentDatabase = selfDestructIterator->getDatabaseForProperty(numPropertyDatabase);

				// Check & destruct bottom first
				if (currentDatabase.get()->isSelfDestructBottomDatabase()) {
					std::string completeNameDatabase = currentDatabase.get()->getBottomDatabasePath() + currentDatabase.get()->getBottomDatabaseName();
					if (boost::filesystem::exists(completeNameDatabase)) {
						boost::filesystem::remove(completeNameDatabase);
					}
				}

				if (currentDatabase.get()->isSelfDestructDatabase()) {
					std::string completeNameDatabase = currentDatabase.get()->getDatabasePath() + currentDatabase.get()->getDatabaseName();
					if (boost::filesystem::exists(completeNameDatabase)) {
						boost::filesystem::remove(completeNameDatabase);
					}
				}
			}
		}
	}
	time(&tEnd);
	int processingTime = tEnd - tInit;
	PhaseDAO::closePhase();
	cout << " - End of phase function generation." << endl;
	cout << "  -> Processing Time : " << processingTime / 3600 << " hours " << (processingTime % 3600) / 60 << " minutes " << processingTime % 60
			<< "  seconds" << endl;

}

void processSurfaceMaterials(	PhaseGeneralParametersDTO & genDTO,
								PhaseKeyMappingGenerator & pkmg,
								ofstream & outCoeffSpecProperties) {
    phase::SurfaceMaterialProcessor processor;

    phase::FluorescenceService fs;
    phase::OpticalFactorsTextureWriter oftw;
    processor.registerWriter(phsdata::SurfaceFunction::Type::Lambertian, std::make_shared<phase::LambertianMaterialWriter>(fs, oftw));
    processor.registerWriter(phsdata::SurfaceFunction::Type::Hapke1981, std::make_shared<phase::Hapke1981MaterialWriter>(fs, oftw));
    processor.registerWriter(phsdata::SurfaceFunction::Type::Hapke2012, std::make_shared<phase::Hapke2012MaterialWriter>(fs, oftw));
    processor.registerWriter(phsdata::SurfaceFunction::Type::RPV, std::make_shared<phase::RPVMaterialWriter>(fs, oftw));
    processor.registerWriter(phsdata::SurfaceFunction::Type::Specular, std::make_shared<phase::SpecularMaterialWriter>(fs, oftw));

    processor.processAll(genDTO, pkmg, outCoeffSpecProperties);
}

void prepareProspect(PhaseGeneralParametersDTO & genDTO) {
	ostringstream prospectPreparationFile;
	boost::uuids::uuid filePreparationUuid = boost::uuids::random_generator()();
	prospectPreparationFile << DartEnv::getOutputTempDatabasePath(true) << boost::lexical_cast<std::string>(filePreparationUuid) + ".csv";

	unsigned int prospectCount = 0;

	std::string databaseName;
	ProspectComputation prospect(prospectPreparationFile.str());
	for (std::shared_ptr<phsdata::SurfaceFunction> & sm : genDTO._surfaceMaterials.surfaceOpticalFunctions) {
		if (sm->getType() == phsdata::SurfaceFunction::Type::Lambertian) {
			phsdata::LambertianMulti & lambertianMat = static_cast<phsdata::LambertianMulti &>(*sm);
			if (lambertianMat.lambertian.is_initialized()
					&& (lambertianMat.lambertian->prospectExternalModule.useProspectExternalModule
							|| lambertianMat.lambertian->prospectExternalModule.isFluorescent)
					&& lambertianMat.lambertian->prospectExternalModule.externParameters.is_initialized()) {

				// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
				boost::uuids::uuid databaseUuid = boost::uuids::random_generator()();
				databaseName = boost::lexical_cast<std::string>(databaseUuid) + ".db";

				lambertianMat.lambertian->databaseName = databaseName;
				lambertianMat.lambertian->modelName = "model";

				// Prepare prospect for later run
				prospect.prepareProspect(databaseName, lambertianMat.lambertian->prospectExternalModule.externParameters.value());
				++prospectCount;
			}
		}
	}
	// Prospect detection & preparation for foliar phase functions (top and bottom faces)
	for (std::shared_ptr<phsdata::UnderstoryMulti> & understory : genDTO._volumesMaterials.understoryMultiFunctions) {
		phsdata::UnderstoryModel * foliarModels[2] = { understory->getTopModel(), understory->getBottomModel() };
		for (phsdata::UnderstoryModel * foliarModel : foliarModels) {
			if (foliarModel == nullptr)
				continue;
			if ((foliarModel->prospectExternalModule.useProspectExternalModule
						|| foliarModel->prospectExternalModule.isFluorescent)
					&& foliarModel->prospectExternalModule.externParameters.is_initialized()) {

				// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
				boost::uuids::uuid databaseUuid = boost::uuids::random_generator()();
				databaseName = boost::lexical_cast<std::string>(databaseUuid) + ".db";

				// Save the path to the future generated database in the model itself
				foliarModel->databaseName = databaseName;

				// Prepare prospect for later run
				prospect.prepareProspect(databaseName, foliarModel->prospectExternalModule.externParameters.value());
				++prospectCount;
			}
		}
	}
	// RUN ALL PROSPECTS
	if (prospectCount > 0) {
		cout << "    -> Compute "<< prospectCount << " prospect(s) functions ..."<< endl;
		// There is a prerared prospect
		prospect.runAllProspects();
	}
}

void executeAtmosphereWithDatabase(	PhaseGeneralParametersDTO & genDTO,
									AtmosphereParameters& atmosphereParameters,
									AirPhaseFunction & moleculeAFP,
									vector<AirPhaseFunction> & aerosolAFP,
									const vector<DirectionCartesienne> & listDirections) {

	PhaseProperty::readAndProcessGroundH2Oparameters(atmosphereParameters);
	atmosphereParameters.numberOfAbsorbingGas = CrossSectionDatabaseDAO::_molecule.size(); //co2 + h2o + o3 + ch4 + n2o

	vector<AtmosphereComposition> atmospherePerBandMeanValues(PhaseProperty::getNumberOfSpectralBands());
	std::vector<std::vector<HenyeyGreensteinParameters> > henleyGreensteinMeanParameters(atmosphereParameters.numberOfAerosol,
		std::vector<HenyeyGreensteinParameters>(PhaseProperty::getNumberOfSpectralBands()));
	if (atmosphereParameters.h2oAmountCkeckbox) {
		PhaseProperty::findWaterColumn(atmosphereParameters);
	}
	PhaseProperty::readHenyeyGreensteinParameters(atmosphereParameters, henleyGreensteinMeanParameters);
	PhaseProperty::saveHenyeyGreensteinParameters(henleyGreensteinMeanParameters);
//	PhaseProperty::readGasesParameters(atmosphereParameters);
	PhaseProperty::computeSpectralTransmittance(genDTO, atmosphereParameters, atmospherePerBandMeanValues, listDirections);
	/* Atmosphere in BOA level */
	AirRayleigh gasAFPLaw;
	AirHenleyGreenstein aerosolAFPLaw;

	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		/* Numbers are currently written in hard code. Possibility of reading them from the database ? */
		gasAFPLaw.addCoefficient(CoefficientRayleigh(0.719044332, 0.041274225));
	}
	moleculeAFP.setAirLaw(gasAFPLaw);

	/* Molecules albedos are only known after maket.exe has run. */
	vector<double> moleculeAlbedos(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setAlbedoPerBand(moleculeAlbedos);
	/* In the current code structuration, the sigmas are only known when maket.exe runs. We're going transmittance put place holders sigmas in the phase function. */
	vector<double> sigmas(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setSigmaPerBand(sigmas);

	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		AirPhaseFunction singleAerosolAFP(PhaseProperty::getNumberOfSpectralBands());
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			/* Numbers are currently written in hard code. Possibility of reading them from the database ? */
			aerosolAFPLaw.addCoefficient(
				CoefficientHenleyGreenstein(henleyGreensteinMeanParameters[i][k].a, henleyGreensteinMeanParameters[i][k].g1,
					henleyGreensteinMeanParameters[i][k].g2));
		}

		singleAerosolAFP.setAirLaw(aerosolAFPLaw);
		vector<double> albedoPerBand(PhaseProperty::getNumberOfSpectralBands());
		vector<double> multiplicativePerBand(PhaseProperty::getNumberOfSpectralBands(), 1.);
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			albedoPerBand[k] = atmospherePerBandMeanValues[k].getAerosolSingleScatteringAlbedo()[i];
		}
		singleAerosolAFP.setAlbedoPerBand(albedoPerBand);

		/* In the current code structuration, the sigmas are only known when maket.exe runs. We're going transmittance put place holders sigmas in the phase function. */
		vector<double> sigmas(PhaseProperty::getNumberOfSpectralBands(), 1.);
		singleAerosolAFP.setSigmaPerBand(sigmas);
		aerosolAFP.push_back(singleAerosolAFP);
	}
	atmosphereParameters.numberOfAbsorbingGas = CrossSectionDatabaseDAO::_molecule.size(); //co2 + h2o + o3 + ch4 + n2o
#ifdef DART_WITH_NETCDF
	writeAtmosphereNcWithDatabase(atmosphereParameters, atmospherePerBandMeanValues, henleyGreensteinMeanParameters);
#endif
}

#ifdef DART_WITH_NETCDF
void executeAtmosphereNcWithoutDatabase(	AtmosphereParameters& atmosphereParameters,
											AirPhaseFunction & moleculeAFP,
											AirPhaseFunction & aerosolAFP,
											const vector<DirectionCartesienne> & listDirections,
											const DirectionCartesienne & sunDirection) {
	AirRayleigh gasAFPLaw;
	AirHenleyGreenstein aerosolAFPLaw;

	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		gasAFPLaw.addCoefficient(CoefficientRayleigh(0.719044332, 0.041274225));
		aerosolAFPLaw.addCoefficient(
			CoefficientHenleyGreenstein(atmosphereParameters.henleyGreensteinParameters[0].a, atmosphereParameters.henleyGreensteinParameters[0].g1,
				atmosphereParameters.henleyGreensteinParameters[0].g2));
	}
	PhaseProperty::saveHenyeyGreensteinParameters(atmosphereParameters);
	moleculeAFP.setAirLaw(gasAFPLaw);
	aerosolAFP.setAirLaw(aerosolAFPLaw);
	vector<double> aerosolAlbedos(PhaseProperty::getNumberOfSpectralBands(), atmosphereParameters.aerosolSingleScatteringAlbedo);
	aerosolAFP.setAlbedoPerBand(aerosolAlbedos);
	vector<double> moleculeAlbedos(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setAlbedoPerBand(moleculeAlbedos);
	vector<double> sigmas(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setSigmaPerBand(sigmas);
	aerosolAFP.setSigmaPerBand(sigmas);
	vector<vector<double> > temperaturesPressureVector;
	vector<double> multiplicativePerBand(PhaseProperty::getNumberOfSpectralBands(), 1.);

	string fileName = DartEnv::getCurrentSimulationOutputPath() + ATMOSPHEREFILE_NC;
	cout << " Write netcdf file: "<< fileName << endl;
	NcFile atmosphereNcFile(fileName, NcFile::replace); // Erase the previous file, if exists

	atmosphereNcFile.putAtt(ATM_ATT_PROCESSED, ncInt, 1);
	atmosphereNcFile.putAtt(ATM_ATT_WITHDATABASE, ncInt, 0);
	atmosphereNcFile.putAtt(ATM_ATT_HEIGHTOFSENSOR, ncInt, atmosphereParameters.sensorHeight);

	atmosphereNcFile.putAtt(ATM_ATT_AerosolScaleFactor, ncDouble, atmosphereParameters.aerosolScaleFactor);
	atmosphereNcFile.putAtt(ATM_ATT_TotalAerosolOpticalDepth, ncDouble, atmosphereParameters.aerosolOpticalDepthForScattering);
	atmosphereNcFile.putAtt(ATM_ATT_AerosolAlbedo, ncDouble, atmosphereParameters.aerosolSingleScatteringAlbedo);

	atmosphereNcFile.putAtt(ATM_ATT_HgG1, ncDouble, atmosphereParameters.henleyGreensteinParameters[0].g1);
	atmosphereNcFile.putAtt(ATM_ATT_HgG2, ncDouble, atmosphereParameters.henleyGreensteinParameters[0].g2);
	atmosphereNcFile.putAtt(ATM_ATT_HgA, ncDouble, atmosphereParameters.henleyGreensteinParameters[0].a);

	atmosphereNcFile.putAtt(ATM_ATT_GasOpticalDepth, ncDouble, atmosphereParameters.gasOpticalDepthForScattering);
	atmosphereNcFile.putAtt(ATM_ATT_GasScaleFactor, ncDouble, atmosphereParameters.gasScaleFactor);
	atmosphereNcFile.putAtt(ATM_ATT_TransmittanceOfGasses, ncDouble, atmosphereParameters.gasTotalTransmittanceForAbsoption);

	string inputTemperaturesFiles = testFilePath(atmosphereParameters.atmosphereTemperatureFileName, DartEnv::getDatabasePath(atmosphereParameters.atmosphereTemperatureFileName));
	ifstream inputDataTemperature;
	inputDataTemperature.open(inputTemperaturesFiles.c_str());
	echappeLectureCommentaireFichier(inputDataTemperature, TWO_CHAR);
	unsigned int numberOfTemperatureLinesInFile = (unsigned int) lireDansLigneFlux(inputDataTemperature, 1);
	temperaturesPressureVector.resize(numberOfTemperatureLinesInFile + 1, vector<double>(3));
	for (unsigned int i = 0; i < numberOfTemperatureLinesInFile; i++) {
		temperaturesPressureVector[i][0] = lireDansLigneFlux(inputDataTemperature, 1);
		temperaturesPressureVector[i][1] = lireDansLigneFlux(inputDataTemperature, 1);
		temperaturesPressureVector[i][2] = lireDansLigneFlux(inputDataTemperature, 1);
		echappeLectureCommentaireFichier(inputDataTemperature, ONE_CHAR, '\n');
	}

	NcGroup vertDistribGroup = atmosphereNcFile.addGroup(ATM_GRP_VERTICALDISTRIBUTION);
	NcDim temperatureAndPressureDimension = atmosphereNcFile.addDim(ATM_DIM_ALTITUDEFORTEMP, numberOfTemperatureLinesInFile);
	NcDim temperatureAndPressureDimension2 = atmosphereNcFile.addDim(ATM_DIM_ROWALTTEMPPRESS, 3);
	vector<NcDim> vector2d_tempDistribDimension;
	vector2d_tempDistribDimension.push_back(temperatureAndPressureDimension);
	vector2d_tempDistribDimension.push_back(temperatureAndPressureDimension2);

	NcVar tempVertDistribData = vertDistribGroup.addVar(ATM_VAR_TEMPERATUREANDPRESSUREVERTDISTRIB, ncDouble, vector2d_tempDistribDimension);
	tempVertDistribData.putAtt(ATM_ATT_Header, "height(km) | temperature(K) | pressure(mb)");
	std::vector<std::vector<double> > outDataTemperatyre(temperatureAndPressureDimension.getSize(), std::vector<double>(temperatureAndPressureDimension2.getSize()));
	for (unsigned int i = 0; i < numberOfTemperatureLinesInFile; i++) {
		outDataTemperatyre[i][0] = temperaturesPressureVector[i][0];
		outDataTemperatyre[i][1] = temperaturesPressureVector[i][1];
		outDataTemperatyre[i][2] = temperaturesPressureVector[i][2];
	}
	DartNetcdf::putNetcdfDataFrom2DVector(tempVertDistribData, outDataTemperatyre);

	inputDataTemperature.close();
	atmosphereParameters.mus = fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
	atmosphereParameters.muPrimeGaz = PhaseProperty::computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.gasScaleFactor,
		atmosphereParameters.mus);
	atmosphereParameters.muPrimeAerosols = PhaseProperty::computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.aerosolScaleFactor,
		atmosphereParameters.mus);
	atmosphereParameters.mPrimeGaz = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.gasScaleFactor) / atmosphereParameters.muPrimeGaz;
	atmosphereParameters.mPrimeAerosols = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.aerosolScaleFactor)
			/ atmosphereParameters.muPrimeAerosols;
	double aerosolTransmittanceForAbsoption = exp(
		-(1 - atmosphereParameters.aerosolSingleScatteringAlbedo) * atmosphereParameters.aerosolOpticalDepthForScattering);
	double gasFactorForAnalyticalTransmittance;

	PhaseProperty::computeMPrimeGasAndAerosolsForAnalyticalTemperature(atmosphereParameters, listDirections);

	gasFactorForAnalyticalTransmittance = atmosphereParameters.gas_u * atmosphereParameters.gasOpticalDepthForScattering / atmosphereParameters.muPrimeGaz;

	double aerosolFactorForAnalyticalTransmittance = atmosphereParameters.aerosolOpticalDepthForScattering
			* atmosphereParameters.aerosolSingleScatteringAlbedo / atmosphereParameters.muPrimeAerosols * atmosphereParameters.aerosols_u;
	double computedBOA;
	double computedSKYL = 0.;
	double temperatureAtmosphereAbsorption = 0.;

	// Create temp atmosphere composition object to emulate the database for computation
	AtmosphereComposition atmosphereComposition;
	atmosphereComposition.setNbOfAerosol(1);
	atmosphereComposition.initializeAerosolPropertyVector(1);
	std::vector<double> vectorAerosolFactorForAnalyticalTransmittance = {aerosolFactorForAnalyticalTransmittance};
	atmosphereComposition.setAerosolFactorForAnalyticalTransmittance(vectorAerosolFactorForAnalyticalTransmittance);
	std::vector<double> vectorAerosolTransmittanceForAbsoption = {aerosolTransmittanceForAbsoption};
	atmosphereComposition.setAerosolTransmittanceForAbsoption(vectorAerosolTransmittanceForAbsoption);
	atmosphereComposition.setAerosolOpticalDepthPerIndex(0, atmosphereParameters.aerosolOpticalDepthForScattering);
	atmosphereComposition.setAerosolSingleScatteringAlbedoPerIndex(0, atmosphereParameters.aerosolSingleScatteringAlbedo);
	atmosphereComposition.setGasFactorForAnalyticalTransmittance(gasFactorForAnalyticalTransmittance);
	atmosphereComposition.setGasTransmittanceForAbsoption(atmosphereParameters.gasTotalTransmittanceForAbsoption);
	atmosphereComposition.setGasOpticalDepthForScattering(atmosphereParameters.gasOpticalDepthForScattering);
	atmosphereComposition.setGasTransmittanceForScattering(exp(-atmosphereParameters.gasOpticalDepthForScattering));

	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		computedBOA = PhaseProperty::computeBOA(computedSKYL, temperatureAtmosphereAbsorption, PhaseProperty::getMeanIrradianceTopOfAtmosphere(k), atmosphereComposition, atmosphereParameters);
		PhaseProperty::setMeanIrradianceBottomOfAtmosphere(k, computedBOA);
		PhaseProperty::setMeanSKYL(k, computedSKYL);
	}
}
#endif // DART_WITH_NETCDF


/**
 * Need to delete this function as soon as netcdf file is ok to force user to use it !
 */
void writeAtmosphereWithDatabase(	PhaseGeneralParametersDTO & genDTO,
									AtmosphereParameters& atmosphereParameters,
									AirPhaseFunction & moleculeAFP,
									vector<AirPhaseFunction> & aerosolAFP,
									const vector<DirectionCartesienne> & listDirections) {

	PhaseProperty::readAndProcessGroundH2Oparameters(atmosphereParameters);

	ofstream writingOuput;
// on reecrit atmosphere.txt avec un flag mis a 1 -> il est calcule
	string fileName = DartEnv::getCurrentSimulationOutputPath() + "atmosphere.txt";
	writingOuput.open(fileName.c_str(), ifstream::trunc);
	writingOuput.precision(4);
	writingOuput << "* Already processed by Phase " << "*" << _space_ << 1 << endl;
	writingOuput << "* Without/with database " << "*" << _space_ << 1 << endl;
	writingOuput << "* Database file name "<< "* " << _space_ << atmosphereParameters.databaseName << endl;
	writingOuput << "* Type of model gas " << "* " << _space_ << atmosphereParameters.gasModelName << endl;
	writingOuput << "* Type of model gas cumulative " << "* " << _space_ << atmosphereParameters.gasCumulativeModelName << endl;
//	writingOuput << "* Type of model gas parameters" << _tab_ << _tab_ << "* " << _space_ << atmosphereParameters.gasParametersModelName << endl;
	writingOuput << "* Type of model gas temperature " << "* " << _space_ << atmosphereParameters.temperatureModelName << endl;
	writingOuput << "* Number of aerosols " << "* " << _space_ << atmosphereParameters.numberOfAerosol << endl;
	atmosphereParameters.numberOfAbsorbingGas = CrossSectionDatabaseDAO::_molecule.size(); //co2 + h2o + o3 + ch4 + n2o
	writingOuput << "* Type of model aerosols " << "* ";

	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		writingOuput << _space_ << atmosphereParameters.aerosolsModelName[i];
	}
	writingOuput << endl;

	writingOuput << "* Type of model aerosols cumulative " << "* ";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		writingOuput << _space_ << atmosphereParameters.aerosolCumulativeModelName[i];
	}
	writingOuput << endl;

	writingOuput << "* Type of model hg parameters " << "* ";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		writingOuput << _space_ << atmosphereParameters.hgParametersModelName[i];
	}
	writingOuput << endl;

	//writingOuput << "* Type of model aerosols" << _tab_ << _tab_ << "* " << _space_ << atmosphereParameters.aerosolsModelName << endl;
	//writingOuput << "* Type of model aerosols cumulative" << _tab_ << _tab_ << "* " << _space_ << atmosphereParameters.aerosolCumulativeModelName<< endl;
	//writingOuput << "* Type of model hg parameters" << _tab_ << _tab_ << "* " << _space_ << atmosphereParameters.hgParametersModelName << endl;
	writingOuput << "* Precipitable water amount checkbox " << "* " << _space_ << atmosphereParameters.h2oAmountCkeckbox << endl;
	if (atmosphereParameters.h2oAmountCkeckbox) {
		writingOuput << "* Water vapour scale factor " << "* " << _space_ << atmosphereParameters.mulFactorH2O << endl;
//		writingOuput << "* Precipitable water amount(cm)  " << _tab_ << _tab_ << "* " << _space_ << atmosphereParameters.precipitableWaterAmount
//				<< endl;
	}

	/* TODO atmosphereParameters.aerosolProfil*/
	writingOuput << "* Aerosol Distribution (0:exp; 1:profile) " "*" << _space_ << atmosphereParameters.aerosolProfil << endl;
	writingOuput << "* Height of sensor (m) " << "*" << _space_ << atmosphereParameters.sensorHeight << endl;
	writingOuput.close();

	vector<AtmosphereComposition> atmospherePerBandMeanValues(PhaseProperty::getNumberOfSpectralBands());
	std::vector<std::vector<HenyeyGreensteinParameters> > henleyGreensteinMeanParameters(atmosphereParameters.numberOfAerosol,
		std::vector<HenyeyGreensteinParameters>(PhaseProperty::getNumberOfSpectralBands()));
	if (atmosphereParameters.h2oAmountCkeckbox) {
		PhaseProperty::findWaterColumn(atmosphereParameters);
	}
	PhaseProperty::readHenyeyGreensteinParameters(atmosphereParameters, henleyGreensteinMeanParameters);
	PhaseProperty::saveHenyeyGreensteinParameters(henleyGreensteinMeanParameters);
//	PhaseProperty::readGasesParameters(atmosphereParameters);
	PhaseProperty::computeSpectralTransmittance(genDTO, atmosphereParameters, atmospherePerBandMeanValues, listDirections);
	/* Atmosphere in BOA level */
	AirRayleigh gasAFPLaw;
	AirHenleyGreenstein aerosolAFPLaw;

	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		/* Numbers are currently written in hard code. Possibility of reading them from the database ? */
		gasAFPLaw.addCoefficient(CoefficientRayleigh(0.719044332, 0.041274225));
	}
	moleculeAFP.setAirLaw(gasAFPLaw);

	/* Molecules albedos are only known after maket.exe has run. */
	vector<double> moleculeAlbedos(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setAlbedoPerBand(moleculeAlbedos);
	/* In the current code structuration, the sigmas are only known when maket.exe runs. We're going transmittance put place holders sigmas in the phase function. */
	vector<double> sigmas(PhaseProperty::getNumberOfSpectralBands(), 1.);
	moleculeAFP.setSigmaPerBand(sigmas);

	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		AirPhaseFunction singleAerosolAFP(PhaseProperty::getNumberOfSpectralBands());
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			/* Numbers are currently written in hard code. Possibility of reading them from the database ? */
			aerosolAFPLaw.addCoefficient(
				CoefficientHenleyGreenstein(henleyGreensteinMeanParameters[i][k].a, henleyGreensteinMeanParameters[i][k].g1,
					henleyGreensteinMeanParameters[i][k].g2));
		}

		singleAerosolAFP.setAirLaw(aerosolAFPLaw);
		vector<double> albedoPerBand(PhaseProperty::getNumberOfSpectralBands());
		vector<double> multiplicativePerBand(PhaseProperty::getNumberOfSpectralBands(), 1.);
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			albedoPerBand[k] = atmospherePerBandMeanValues[k].getAerosolSingleScatteringAlbedo()[i];
		}
		singleAerosolAFP.setAlbedoPerBand(albedoPerBand);

		/* In the current code structuration, the sigmas are only known when maket.exe runs. We're going transmittance put place holders sigmas in the phase function. */
		vector<double> sigmas(PhaseProperty::getNumberOfSpectralBands(), 1.);
		singleAerosolAFP.setSigmaPerBand(sigmas);
		aerosolAFP.push_back(singleAerosolAFP);
	}
	cout << "   -> Writing on atmosphere.txt" << endl;
	writingOuput.open(fileName.c_str(), ofstream::app);
	writingOuput.precision(10);
	writingOuput << "* g1 [0 1] " << "*";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			writingOuput << _space_ << henleyGreensteinMeanParameters[i][k].g1;
		}
	}
	writingOuput << endl << "* g2 [0 1] " << "*";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			writingOuput << _space_ << henleyGreensteinMeanParameters[i][k].g2;
		}
	}
	writingOuput << endl << "* a [0 1] " << "*";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			writingOuput << _space_ << henleyGreensteinMeanParameters[i][k].a;
		}
	}
	writingOuput << endl << "* Spectral sampling (µm)" << "*";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << (PhaseProperty::getLowerBandWavelength(k) + PhaseProperty::getUpperBandWavelength(k)) / 2;
	}
	writingOuput << endl << "* Total aerosols optical_depth " << "*";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			writingOuput << _space_ << atmospherePerBandMeanValues[k].getAerosolOpticalDepth()[i];
		}
	}
	writingOuput << endl << "* Aerosol single_scattering albedo " << "*";
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
			writingOuput << _space_ << atmospherePerBandMeanValues[k].getAerosolSingleScatteringAlbedo()[i];
		}
	}
	writingOuput << endl << "* Gas transmittance (scattering) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getGasTransmittanceForScattering();
	}
	writingOuput << endl << "* H2O Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getH2oTransmittanceAbs();
	}
	writingOuput << endl << "* O3  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getO3TransmittanceAbs();
	}
	writingOuput << endl << "* N2  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getN2TransmittanceAbs();
	}
	writingOuput << endl << "* CO2 Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getCo2TransmittanceAbs();
	}
	writingOuput << endl << "* CO  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getCoTransmittanceAbs();
	}
	writingOuput << endl << "* CH4 Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getCh4TransmittanceAbs();
	}
	writingOuput << endl << "* N2O Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getN2oTransmittanceAbs();
	}
	writingOuput << endl << "* O2  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getO2TransmittanceAbs();
	}
	writingOuput << endl << "* NH3  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getNH3TransmittanceAbs();
	}
	writingOuput << endl << "* NO  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getNOTransmittanceAbs();
	}
	writingOuput << endl << "* NO2  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getNO2TransmittanceAbs();
	}
	writingOuput << endl << "* SO2  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getSO2TransmittanceAbs();
	}
	writingOuput << endl << "* HNO3  Transmittance (absorption) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getHNO3TransmittanceAbs();
	}
	writingOuput << endl << "* Other_gas Transmittance (abs.) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getOtherGasTransmittanceAbs();
	}
	writingOuput << endl << "* Eq._gas Transmittance (absorption_UpandDown) *";
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		writingOuput << _space_ << atmospherePerBandMeanValues[k].getGasTransmittanceAbsUpDown();
	}
	writingOuput << endl;
	PhaseProperty::readAndWriteOzone(atmosphereParameters, writingOuput);
	PhaseProperty::readAndWriteCrossSection(atmospherePerBandMeanValues, writingOuput);
	PhaseProperty::readAndWriteTemperature(atmosphereParameters, writingOuput);
	writingOuput.close();
}

#ifdef DART_WITH_NETCDF
void writeAtmosphereNcWithDatabase(	const AtmosphereParameters& atmosphereParameters,
						vector<AtmosphereComposition>& atmospherePerBandMeanValues,
						const std::vector<std::vector<HenyeyGreensteinParameters> > &henleyGreensteinMeanParameters) {

	unsigned int numberOfGases = 16;

	string fileName = DartEnv::getCurrentSimulationOutputPath() + ATMOSPHEREFILE_NC;
	cout << "   -> Writing netcdf file: " << fileName << endl;
	NcFile atmosphereNcFile(fileName, NcFile::replace); // Erase the previous file, if exists
	// 1.A Dimensions declaration
	NcDim bandDimension = atmosphereNcFile.addDim(ATM_DIM_BAND, PhaseProperty::getNumberOfSpectralBands());
	NcDim aerosolDimension = atmosphereNcFile.addDim(ATM_DIM_AEROSOL, atmosphereParameters.numberOfAerosol);
	NcDim gasDimension = atmosphereNcFile.addDim(ATM_DIM_Gas, numberOfGases);

	// 1.B Dimensions vectors (Matrixs dimensions)
	vector<NcDim> vector1d_BandDimension;
	vector1d_BandDimension.push_back(bandDimension);

	vector<NcDim> vector2d_AerosolBandDimension;
	vector2d_AerosolBandDimension.push_back(bandDimension);
	vector2d_AerosolBandDimension.push_back(aerosolDimension);

	// Global attributes
	atmosphereNcFile.putAtt(ATM_ATT_PROCESSED, ncInt, 1);
	atmosphereNcFile.putAtt(ATM_ATT_WITHDATABASE, ncInt, 1);
	atmosphereNcFile.putAtt(ATM_ATT_DATABASENAME, atmosphereParameters.databaseName);
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFMODELGAS, atmosphereParameters.gasModelName);
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFMODELGASCUM, atmosphereParameters.gasCumulativeModelName);
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFMODELGASTEMP, atmosphereParameters.temperatureModelName);
	atmosphereNcFile.putAtt(ATM_ATT_NBAEROSOLS, ncInt, atmosphereParameters.numberOfAerosol);

	ostringstream typeOfModelAerosols;
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		typeOfModelAerosols << _space_ << atmosphereParameters.aerosolsModelName[i];
	}
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFAEROSOLS, typeOfModelAerosols.str());

	ostringstream typeOfModelAerosolsCumulative;
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		typeOfModelAerosolsCumulative << _space_ << atmosphereParameters.aerosolCumulativeModelName[i];
	}
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFAEROSOLSCUM, typeOfModelAerosolsCumulative.str());

	ostringstream typeOfModelHgParam;
	for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
		typeOfModelHgParam << _space_ << atmosphereParameters.hgParametersModelName[i];
	}
	atmosphereNcFile.putAtt(ATM_ATT_TYPEOFHGPARAM, typeOfModelHgParam.str());

	atmosphereNcFile.putAtt(ATM_ATT_PRECIPITABLEWATERCHECK, ncInt, atmosphereParameters.h2oAmountCkeckbox);
	if (atmosphereParameters.h2oAmountCkeckbox) {
		atmosphereNcFile.putAtt(ATM_ATT_WaterVapourScaleFactor, ncInt, atmosphereParameters.mulFactorH2O);
	}
	atmosphereNcFile.putAtt(ATM_ATT_AEROSOLDISTRIBUTION, ncInt, atmosphereParameters.aerosolProfil);
	atmosphereNcFile.putAtt(ATM_ATT_HEIGHTOFSENSOR, ncInt, atmosphereParameters.sensorHeight);

	// Spectral sampling
	NcVar spectralSamplingVar = atmosphereNcFile.addVar(ATM_VAR_SPECTRALSAMPLING, ncDouble, vector1d_BandDimension);
	spectralSamplingVar.putAtt(ATM_ATT_UNIT, "µm");
	std::vector<double> spectralSamplingOut(bandDimension.getSize());
	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		spectralSamplingOut[k] = (PhaseProperty::getLowerBandWavelength(k) + PhaseProperty::getUpperBandWavelength(k)) / 2;
	}
	spectralSamplingVar.putVar(&spectralSamplingOut[0]);

	// Henyey Greenstein data
	NcGroup hgGroup = atmosphereNcFile.addGroup(ATM_GRP_HG);

	NcVar g1Data = hgGroup.addVar("g1", ncDouble, vector2d_AerosolBandDimension);
	g1Data.putAtt(ATM_ATT_UNIT, "g1 is in [0 1]");
	std::vector<std::vector<double> > g1Out(bandDimension.getSize(), std::vector<double>(aerosolDimension.getSize()));
	g1Data.putAtt(ATM_ATT_Header, "Aerosol(s)");

	NcVar g2Data = hgGroup.addVar("g2", ncDouble, vector2d_AerosolBandDimension);
	g2Data.putAtt(ATM_ATT_UNIT, "g2 is in [0 1]");
	std::vector<std::vector<double> > g2Out(bandDimension.getSize(), std::vector<double>(aerosolDimension.getSize()));
	g2Data.putAtt(ATM_ATT_Header, "Aerosol(s)");

	NcVar aData = hgGroup.addVar("a", ncDouble, vector2d_AerosolBandDimension);
	aData.putAtt(ATM_ATT_UNIT, "a is in [0 1]");
	std::vector<std::vector<double> > aOut(bandDimension.getSize(), std::vector<double>(aerosolDimension.getSize()));
	aData.putAtt(ATM_ATT_Header, "Aerosol(s)");

	for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
		for (int k = 0; k < atmosphereParameters.numberOfAerosol; k++) {
			g1Out[i][k] = henleyGreensteinMeanParameters[k][i].g1;
			g2Out[i][k] = henleyGreensteinMeanParameters[k][i].g2;
			aOut[i][k] = henleyGreensteinMeanParameters[k][i].a;
		}
	}
	DartNetcdf::putNetcdfDataFrom2DVector(g1Data, g1Out);
	DartNetcdf::putNetcdfDataFrom2DVector(g2Data, g2Out);
	DartNetcdf::putNetcdfDataFrom2DVector(aData, aOut);

	// Transmittance matrix data
	NcGroup transmittanceGroup = atmosphereNcFile.addGroup(ATM_GRP_TRANSMITTANCE);

	//  -- Aerosols
	NcVar totalAODvar = transmittanceGroup.addVar(ATM_VAR_TOTALAOD, ncDouble, vector2d_AerosolBandDimension);
	totalAODvar.putAtt(ATM_ATT_UNIT, "Optical depth is in [0;+inf[");
	std::vector<std::vector<double> > totalAODOut(bandDimension.getSize(), std::vector<double>(aerosolDimension.getSize()));
	totalAODvar.putAtt(ATM_ATT_Header, "Aerosols_optical_depth");

	NcVar aerosolSingleScatVar = transmittanceGroup.addVar(ATM_VAR_AEROSOLSINGLESCAT, ncDouble, vector2d_AerosolBandDimension);
	aerosolSingleScatVar.putAtt(ATM_ATT_UNIT, "Single scattering albedo is in [0 1]");
	std::vector<std::vector<double> > aerosolSingleScatOut(bandDimension.getSize(), std::vector<double>(aerosolDimension.getSize()));
	aerosolSingleScatVar.putAtt(ATM_ATT_Header, "Aerosols_single_scattering_albedo");
	for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
		for (int k = 0; k < atmosphereParameters.numberOfAerosol; k++) {
			totalAODOut[i][k] = atmospherePerBandMeanValues[i].getAerosolOpticalDepth()[k];
			aerosolSingleScatOut[i][k] = atmospherePerBandMeanValues[i].getAerosolSingleScatteringAlbedo()[k];
		}
	}
	DartNetcdf::putNetcdfDataFrom2DVector(totalAODvar, totalAODOut);
	DartNetcdf::putNetcdfDataFrom2DVector(aerosolSingleScatVar, aerosolSingleScatOut);

	//   -- Gas
	const std::string gasHeader = "Gas transmittance (scattering) | H2O Transmittance (absorption) | O3  Transmittance (absorption) | N2  Transmittance (absorption) | CO2 Transmittance (absorption) | CO  Transmittance (absorption) | CH4 Transmittance (absorption) | N2O Transmittance (absorption) | O2  Transmittance (absorption) | NH3  Transmittance (absorption) | NO  Transmittance (absorption) | NO2  Transmittance (absorption) | SO2  Transmittance (absorption) | HNO3  Transmittance (absorption) | Other_gas Transmittance (abs.) | Eq._gas Transmittance (absorption_UpandDown)";

	vector<NcDim> vector2d_gasBandDimension;
	vector2d_gasBandDimension.push_back(bandDimension);
	vector2d_gasBandDimension.push_back(gasDimension);
	NcVar totalGasvar = transmittanceGroup.addVar(ATM_VAR_AllGas, ncDouble, vector2d_gasBandDimension);
	totalGasvar.putAtt(ATM_ATT_UNIT, "Transmittance is defined between 0 and 1");
	totalGasvar.putAtt(ATM_ATT_Header, gasHeader);
	std::vector<std::vector<double> > gasOut(bandDimension.getSize(), std::vector<double>(gasDimension.getSize()));

	for (unsigned int k = 0; k < PhaseProperty::getNumberOfSpectralBands(); k++) {
		gasOut[k][0] = atmospherePerBandMeanValues[k].getGasTransmittanceForScattering();
		gasOut[k][1] = atmospherePerBandMeanValues[k].getH2oTransmittanceAbs();
		gasOut[k][2] = atmospherePerBandMeanValues[k].getO3TransmittanceAbs();
		gasOut[k][3] = atmospherePerBandMeanValues[k].getN2TransmittanceAbs();
		gasOut[k][4] = atmospherePerBandMeanValues[k].getCo2TransmittanceAbs();
		gasOut[k][5] = atmospherePerBandMeanValues[k].getCoTransmittanceAbs();
		gasOut[k][6] = atmospherePerBandMeanValues[k].getCh4TransmittanceAbs();
		gasOut[k][7] = atmospherePerBandMeanValues[k].getN2oTransmittanceAbs();
		gasOut[k][8] = atmospherePerBandMeanValues[k].getO2TransmittanceAbs();
		gasOut[k][9] = atmospherePerBandMeanValues[k].getNH3TransmittanceAbs();
		gasOut[k][10] = atmospherePerBandMeanValues[k].getNOTransmittanceAbs();
		gasOut[k][11] = atmospherePerBandMeanValues[k].getNO2TransmittanceAbs();
		gasOut[k][12] = atmospherePerBandMeanValues[k].getSO2TransmittanceAbs();
		gasOut[k][13] = atmospherePerBandMeanValues[k].getHNO3TransmittanceAbs();
		gasOut[k][14] = atmospherePerBandMeanValues[k].getOtherGasTransmittanceAbs();
		gasOut[k][15] = atmospherePerBandMeanValues[k].getGasTransmittanceAbsUpDown();
	}
	DartNetcdf::putNetcdfDataFrom2DVector(totalGasvar, gasOut);

	PhaseProperty::readAndWriteAtmosphereComponentAsNcdfGroup(atmosphereNcFile, atmospherePerBandMeanValues, atmosphereParameters);
	atmosphereNcFile.close();
}
#endif // DART_WITH_NETCDF


vector<OpticalPropertyParameters> createDatabasesReadingProperties(const PhaseGeneralParametersDTO & genDTO) {
	vector<OpticalPropertyParameters> phaseOpticalProperties;

	double deviation = 0, smc = 0, delta = 0;
	string phaseName = "", modelName = "", niModelName = "", ntModelName = "", databaseName = "",databasePath="", temp = "";
	string transmittanceModelName = "", transmittanceDatabaseName = "";
	bool useRefraction = false, keepOutputFile = false;
	int specularRadianceDistribution = 0, lambertianDefinition = 0, indexOrder=-1;

	/* PHASE EXTERN */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfPhaseExternModels(); modelIndex++) {
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForPhaseExtern(modelIndex, phaseName, modelName, databaseName), "");
		OpticalPropertyParameters opticalProperty(PHASE_EXTERN_PROPERTY, modelIndex, phaseName, indexOrder);
		shared_ptr<PhaseExternDatabaseParameters> phaseExternDatabaseParameters(
			new PhaseExternDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, genDTO.numberOfSpectralBands));
		opticalProperty.addDatabaseToProperty(phaseExternDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Specular */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfSpecularModels(); modelIndex++) {

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForSpecular(modelIndex, phaseName, databaseName, niModelName, ntModelName), "");
		OpticalPropertyParameters opticalProperty(SPECULAR_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForSpecular(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		shared_ptr<RefractiveIndexDatabaseParameters> refractiveIndexDatabaseParameters(
			new RefractiveIndexDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), niModelName, genDTO.numberOfSpectralBands, true,
				ntModelName));

		opticalProperty.addDatabaseToProperty(refractiveIndexDatabaseParameters);

		// I manually set the transmittanceDatabase
		string transmittanceDatabaseName = "Lambertian_mineral.db";
		string transmittanceModelName = "reflect_equal_1_trans_equal_0_0";
		shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
			new PhaseTemperatureDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, 1));
		opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Foliar*/
	for (unsigned int modelIndex = 0; modelIndex < genDTO._volumesMaterials.understoryMultiFunctions.size(); ++modelIndex) {
		const phsdata::UnderstoryMulti & understory = *genDTO._volumesMaterials.understoryMultiFunctions[modelIndex];
		EXIT_IF(understory.getTopModel() == nullptr, "");
		phaseOpticalProperties.push_back(buildFoliarOpticalPropertyParameters(understory, genDTO.numberOfSpectralBands));
	}
	return phaseOpticalProperties;
}

void writeSecondarySourcesOfLight(PhaseGeneralParametersDTO & genDTO, ofstream & outCoeffSpecProperties, const LightsContainer & lightsContainer) {
	for (unsigned int lightIndex = 0; lightIndex < lightsContainer.getNbLights(); ++lightIndex) {
		const Light & currentLight = lightsContainer.getLight(lightIndex);
		std::string currentLightKeyRoot = "scene.lights.sl" + boost::lexical_cast<std::string>(lightIndex) + ".";
//			if(genDTO.isSimulatedSolarNoiseCheck){
		genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "id", std::to_string(getLightIDFromType(genDTO, globalReferences::OTHER_LIGHT, "OtherLight")));
//			}
		double irradianceFactor = 1.;
		if (currentLight.getClass() == Light::UNIDIRECTIONAL) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "type", "sharpdistant");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "direction", boost::lexical_cast<std::string>(currentLight.getDirection().getDirectionCartesienne().getX()) + " " + boost::lexical_cast<std::string>(currentLight.getDirection().getDirectionCartesienne().getY()) + " " + boost::lexical_cast<std::string>(currentLight.getDirection().getDirectionCartesienne().getZ()));
		} else if (currentLight.getClass() == Light::SPHERE) {
			const LightSphere & currentLightSphere = static_cast<const LightSphere &>(currentLight);
			double groundLightDistance = currentLightSphere.getPosition().getDistanceToOrigin();
			if (groundLightDistance < 81000) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "type", "sphere");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "generator", "phase");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "position", boost::lexical_cast<std::string>(currentLightSphere.getPosition().getX()) + " " + boost::lexical_cast<std::string>(currentLightSphere.getPosition().getY()) + " " + boost::lexical_cast<std::string>(currentLightSphere.getPosition().getZ()));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "radius", boost::lexical_cast<std::string>(currentLightSphere.getRadius()));
				irradianceFactor = groundLightDistance * groundLightDistance * 4 * M_PI;
			} else {
				// Accuracy decreases as the light source is far away from the source, so we put it proportionally closer
				Vecteur<double> direction(currentLightSphere.getPosition());
				direction.normaliserEgal();
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "type", "sun");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "generator", "phase");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "dir", boost::lexical_cast<std::string>(direction.getX()) + " " + boost::lexical_cast<std::string>(direction.getY()) + " " + boost::lexical_cast<std::string>(direction.getZ()));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "radius", boost::lexical_cast<std::string>(currentLightSphere.getRadius() * 81000 / groundLightDistance));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "meanDistance", boost::lexical_cast<std::string>(81000));
				groundLightDistance = 81000;
			}
		} else if (currentLight.getClass() == Light::SPOT) {
			const SpotLight & currentSpotLight = static_cast<const SpotLight &>(currentLight);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "type", "spot");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "coneangle", boost::lexical_cast<std::string>(currentSpotLight.getOverture()));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "conedeltaangle", "0");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "position", boost::lexical_cast<std::string>(currentSpotLight.getPosition().getX()) + " " + boost::lexical_cast<std::string>(currentSpotLight.getPosition().getY()) + " " + boost::lexical_cast<std::string>(currentSpotLight.getPosition().getZ()));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "target", boost::lexical_cast<std::string>(currentSpotLight.getPosition().getX() + currentLight.getDirection().getDirectionCartesienne().getX() * 64.) + " " + boost::lexical_cast<std::string>(currentSpotLight.getPosition().getY() + currentLight.getDirection().getDirectionCartesienne().getY() * 64.) + " " + boost::lexical_cast<std::string>(currentSpotLight.getPosition().getZ() + currentLight.getDirection().getDirectionCartesienne().getZ() * 64.));
		}
		std::ostringstream irradianceValuesStr;
		irradianceValuesStr << setprecision(15);
		irradianceValuesStr << currentLight.getIrradiance(0) * irradianceFactor;
		for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
			irradianceValuesStr << " " << currentLight.getIrradiance(spectralBandIndex) * irradianceFactor;
		}
		genDTO.luxCoreRenderSceneProperties.ajouterCle(currentLightKeyRoot + "color", irradianceValuesStr.str());
	}
}
