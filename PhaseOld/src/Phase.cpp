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
#include "../../../../libDartGeometry/include/Plan.hpp"
#include "../../libDartBase/include/Lib_physic.h"

#include <stdlib.h>
#include <sstream>

#include "../../libDependencies/memory_tools.hpp"
#if defined(USE_GPERFTOOLS) && defined(ENABLE_TCMALLOC_PROFILER)
	#include "../../gperftools/src/gperftools/heap-profiler.h"
	#include "../../gperftools/src/gperftools/profiler.h"
#endif
#include <new>

#include "../libSQLite/sqlite3.h"

extern "C" SQLITE_API int sqlite3_initialize(void);
#define TYPE_MODELISATION_LAMBERTIEN 	4
#define TYPE_MODELISATION_SPEC 			7
#define TYPE_MODELISATION_RPV_SPEC 	    10
#define TYPE_MODELISATION_TURBIDE 		10 // Fonction de phase d'1 milieu turbide
#define NBSPECTRALPARAMTOREAD	10 // Nombre de parametres spectraux a lire dans la database
// Logger specifique a l'outil Phase.
//static Logger phaseLogger = Logger::getInstance(LOG4CPLUS_TEXT("Phase"));

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

	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
			|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO) {
		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
		PhaseUtils::setOutputFolder("lib_phase");
		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties phasePropert(fileNameProperty.c_str());
		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
		if (genDTO.accelerationEngine == 2) { // LuxCoreRender
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
		} else {
			// Clean up the folder of LuxCore's Files
			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
			}
			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderConfigFilePath));
			}
			std::string luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreLightPropertiesFilePath));
			}
		}
		genDTO.useSolarDirection();
		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
		execute_batchPhase(genDTO, fileNameProperty);
	} else if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == false) {
		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
		PhaseUtils::setOutputFolder("lib_phase");
		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties phasePropert(fileNameProperty.c_str());
		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
		if(genDTO.accelerationEngine == 2){ //luxcorerender
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

			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
				std::ofstream tmp(luxCoreRenderConfigFilePath.c_str(), ios::out);
				if (tmp) {
					tmp.close();
				}
			}
			genDTO.luxCoreRenderConfigProperties.changerFichierProperties(luxCoreRenderConfigFilePath, "=");
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
			readLiDAR();
		} else {
			// Clean up the folder of LuxCore's Files
			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
			}
			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderConfigFilePath));
			}
			std::string luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreLightPropertiesFilePath));
			}
		}
		genDTO.useLidarDirection();
		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
		execute_batchPhase(genDTO, fileNameProperty);
	} else if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == true) {
		//Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
		string fileNameProperty = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties phasePropert(fileNameProperty.c_str());
		phasePropert.effacerLignesCommencantParClefEtReecrire("phase");
		if(genDTO.accelerationEngine == 2){ //luxcorerender
			genDTO.lightCounter = 0;
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

			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
			if (!boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
				std::ofstream tmp(luxCoreRenderConfigFilePath.c_str(), ios::out);
				if (tmp) {
					tmp.close();
				}
			}
			genDTO.luxCoreRenderConfigProperties.changerFichierProperties(luxCoreRenderConfigFilePath, "=");
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
			readLiDAR();
		} else {
			// Clean up the folder of LuxCore's Files
			std::string luxCoreRenderSceneFilePath = DartEnv::getCurrentSimulationOutputPath() + "phase.scn";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderSceneFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderSceneFilePath));
			}
			std::string luxCoreRenderConfigFilePath = DartEnv::getCurrentSimulationOutputPath() + "scene.cfg";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreRenderConfigFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreRenderConfigFilePath));
			}
			std::string luxCoreLightPropertiesFilePath = DartEnv::getCurrentSimulationOutputPath() + "lights.properties";
			if (boost::filesystem::exists(boost::filesystem::path(luxCoreLightPropertiesFilePath))) {
				boost::filesystem::remove(boost::filesystem::path(luxCoreLightPropertiesFilePath));
			}
		}
		cout << "- Phase of LIDAR with Solar Noise: " << endl;
		cout << "- SolarNoise Phase Calculation: " << endl;
		genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::FLUXTRACKING;
		genDTO.useSolarDirection();
		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
		PhaseUtils::setOutputFolder("lib_phase_SolarNoise");
		outputPhaseDir = outputDir + PhaseUtils::getOutputFolder();
		PhaseKeyMappingGenerator::useNodeSolarNoiseSimuSN();
		execute_batchPhase(genDTO, fileNameProperty);
		PhaseProperty::reinit();
		PhaseDAO::initPhase(fromStream); //????????????????????????????????????
		cout << "- LIDAR Phase Calculation: " << endl;
		genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::LIDAR;
		genDTO.useLidarDirection();
		PhaseProperty::setCalculatorMethod(genDTO.radiativeTransferMethod);
		PhaseUtils::setOutputFolder("lib_phase_LIDAR");
		outputPhaseDir = outputDir + PhaseUtils::getOutputFolder();
		PhaseKeyMappingGenerator::useNodeSolarNoiseSimuLIDAR();
		execute_batchPhase(genDTO, fileNameProperty);
	}
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
	gest.ajouterType("yaml", false,
		"The presence of this parameter indicate that input files are stored as YaMLs.\nOtherwise, it means they are stored as XMLs.", 0,
		vector<string>(0));
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

	PhaseDAO::setYaMlUsage(gest.estPresent("yaml"));
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
 * Swig ne peut pas créer un wrapper du main donc une méthode run est définie.
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

	if (genDTO.accelerationEngine == 2) {
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
	}

	if (genDTO.accelerationEngine == 2 && (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
			|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO) && genDTO.SNrunningmode != 2) {
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
				bool thermalEmission = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeT || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeRPlusT;
				value << (thermalEmission ? M_PI * temperatureToLuminanceMeanOnSpectralBand(genDTO.atmosphericApparentTemperature, PLANCK, genDTO.lowerBandWavelength[0] * 1E-6, genDTO.upperBandWavelength[0] * 1E-6, precision, maxNumberOfIterations) : 0.f);
				for (unsigned int sbIndex = 1; sbIndex < genDTO.numberOfSpectralBands; sbIndex++) {
					thermalEmission = genDTO.spectralDomain[sbIndex] == PhaseGeneralParametersDTO::ModeT || genDTO.spectralDomain[sbIndex] == PhaseGeneralParametersDTO::ModeRPlusT;
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
				bool sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeRPlusT;
				value << (sunIllumination ? ((genDTO.atmosphereRTmode == 1 && genDTO.computationMethodTOAtoBOA == 2) ? PhaseProperty::getMeanIrradianceTopOfAtmosphere(0) : Es_BOA[0]) : 0.0);
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeRPlusT;
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

				if (genDTO.isSimulatedSolarNoiseCheck) {
					value.str("");
					value << genDTO.lightCounter;
					genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.sunlight.id", value.str());
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
				bool sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeRPlusT;
				bool skyThermalEmission = false;
				for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					valueToAdd = 0.;
					sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeRPlusT;
					if (sunIllumination) {
						valueToAdd += Es_BOA[spectralBandIndex] * PhaseProperty::getMeanSKYL(spectralBandIndex) * costheta;
					}
					skyThermalEmission = genDTO.atmosphericApparentTemperature > 0. && (genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeT || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeRPlusT);
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
					sunIllumination = genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[0] == PhaseGeneralParametersDTO::ModeRPlusT;
					value << (sunIllumination ? Es_BOA[0] * (1.0 - PhaseProperty::getMeanSKYL(0)) : 0.);
					for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						sunIllumination = genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeR || genDTO.spectralDomain[spectralBandIndex] == PhaseGeneralParametersDTO::ModeRPlusT;
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
	}
	if(genDTO.accelerationEngine == 2 && genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.SNrunningmode != 1){
		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.lidarlight.type", "lidar");
		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.lidarlight.generator", "phase");
		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.lidarlight.id", std::to_string(getLightIDFromType(genDTO, globalReferences::LIDAR, "LiDAR")));
		std::ostringstream value;
		//color
		value << PhaseProperty::getLiDARpulseEnergy();

		for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
			value << " " << PhaseProperty::getLiDARpulseEnergy();
		}
		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.lidarlight.color", value.str());
//		if(genDTO.isSimulatedSolarNoiseCheck){
//			value.str("");
//			value<<(genDTO.lightCounter++);
//			genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.lights.lidarlight.id", value.str());
//		}
	}
}
/*
 * Depending of the mode optical properties are weighted according to the solar spectrum:
 * In mode R, in mode R+T if the ratio of energy from R/T is greater than factorRatioModesRTuseSun
 */
void testIfUseSun(	const PhaseGeneralParametersDTO & genDTO,
					vector<int> & useSpectralIrradianceForReflectance) {
//	double factorRatioModesRTuseSun = 5;
//	double meanEnergyModeT = 0;
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
//		meanEnergyModeT = temperatureToLuminanceForOneWavelength(300, 1, 1e-6 * (PhaseProperty::getLowerBandWavelength(spectralBandIndex) + PhaseProperty::getUpperBandWavelength(spectralBandIndex)) * .5) * 3.14;
		useSpectralIrradianceForReflectance[spectralBandIndex] = 0;
		if (PhaseProperty::getWeightingOfReflectanceParameters() && PhaseProperty::getCalculatorMethod() != 2
				&& PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0) {
			useSpectralIrradianceForReflectance[spectralBandIndex] = 1;
		}
	}
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
	if (PhaseDAO::useYaMl()) {
		pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getLambertianProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getLambertianProperties()[i]->getItemId() << endl;
		}
		pkmg.setTypeFonctionDePhase(globalReferences::RPV_PHASE);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getRPVProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getRPVProperties()[i]->getItemId() << endl;
		}
		pkmg.setTypeFonctionDePhase(globalReferences::HAPKE_SPEC);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getHapkeProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getHapkeProperties()[i]->getItemId() << endl;
		}
		pkmg.setTypeFonctionDePhase(globalReferences::HAPKE2_SPEC);
				for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getHapke2Properties().size();++i){
					pkmg.setNumeroFonctionDePhase(i);
					outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getHapke2Properties()[i]->getItemId() << endl;
				}
		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getVegetationProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getVegetationProperties()[i]->getItemId() << endl;
		}
		pkmg.setTypeFonctionDePhase(globalReferences::AIR_FP);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getFluidProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getFluidProperties()[i]->getItemId() << endl;
		}
		pkmg.setTypeFonctionDePhase(globalReferences::PHASE_EXTERN);
		for (unsigned int i = 0; i < PhaseDAO::getInputsDAO().getDartOpticalProperties().getPhaseExternProperties().size();++i){
			pkmg.setNumeroFonctionDePhase(i);
			outCoeffSpecProperties << pkmg.cleEquivalentYAMLIndice() << PhaseDAO::getInputsDAO().getDartOpticalProperties().getPhaseExternProperties()[i]->getItemId() << endl;
			cout<<"setTypefunction:ok"<<endl;
		}
	}
}

void readMultiplicativeFactor(	int & iterationStepPhi,
								int & iterationStepTheta,
								FoliarModel & foliarModel,
								const PhaseGeneralParametersDTO & genDTO) {
	PhaseProperty::resizeForMultiplicativeFactorforLUT();
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorLambertien(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorSpecular(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorHapke(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorHapke2(genDTO), "");
	EXIT_IF(!PhaseDAO::readMultiplicativeFactorFoliar(iterationStepPhi, iterationStepTheta, foliarModel, genDTO), "");
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

void treatLambertian(	OpticalPropertyParameters & lambertianPropertyParameters,
						unsigned int opIndex,
						vector<int> & useSpectralIrradianceForReflectance,
						PhaseKeyMappingGenerator & pkmg,
						ofstream & outCoeffSpecProperties,
						PhaseGeneralParametersDTO & genDTO,
						const vector<DirectionCartesienne> & listDirections) {
	double albedo = 0;
	double dTauMoy = 0;
	vector<double> reflectance(genDTO.numberOfSpectralBands), diffuseTransmittance(genDTO.numberOfSpectralBands), directTransmittance(genDTO.numberOfSpectralBands);
	bool hasDirectTransmittance = false;
	PropertyLambertian lambertianProperty;
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		computeOpticalPropertiesLambertian(spectralBandIndex, lambertianPropertyParameters, listDirections, lambertianProperty, dTauMoy, albedo);
		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile("MultispectralOpticalFunctions.LambertianMultiFunctions.LambertianMulti[",
			lambertianPropertyParameters.getModelIndex(), -1.0, -1.0, -1.0);
		testLambertianAlbedo(lambertianPropertyParameters.getModelIndex(), lambertianProperty);
		if (genDTO.accelerationEngine == 2) {
			reflectance[spectralBandIndex] = lambertianProperty.reflectance;
			diffuseTransmittance[spectralBandIndex] = lambertianProperty.diffuseTransmittance;
			directTransmittance[spectralBandIndex] = lambertianProperty.directTransmittance;
			if (lambertianProperty.directTransmittance > 0.) {
				hasDirectTransmittance = true;
			}
		}
		writeLambertian(lambertianPropertyParameters.getModelIndex(), lambertianPropertyParameters.getModelName(), spectralBandIndex, pkmg,
			outCoeffSpecProperties, lambertianProperty, dTauMoy, albedo, listDirections);
	}

	shared_ptr<SpecularDatabaseParameters> lambertianSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
		lambertianPropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));

	/* The precomputation of the Fresnel coefficients for the specular reflection on facets could not be used */
	//bool isSpecular = false;
	//if (isSpecular) { // <=> if (lambertianSpecularParameters->get() != null)
	if (lambertianPropertyParameters.isSpecular()) {
		PhasePolarization::computeAndWriteFresnel(lambertianPropertyParameters.getModelIndex(), globalReferences::LAMBERTIEN, lambertianSpecularParameters);
	}
	treatLambertianFluorescence(lambertianPropertyParameters.getModelIndex(), pkmg, outCoeffSpecProperties, genDTO);

	if (genDTO.accelerationEngine == 2) {
//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "lambertian" <<  lambertianPropertyParameters.getModelIndex() /*<< "_" << tmpIndex*/ << ".";

			FluorescenceParameters fluorescenceParameters;
			PhaseDAO::readCoeffDiffParametersForLambertianFluspect(lambertianPropertyParameters.getModelIndex(), fluorescenceParameters);

			// Translucent materials (i.e. where transmittance != 0) are slower. We don't use them unless it's necessary.
			bool translucent = false;
			unsigned int sbi = 0;
			while (sbi < genDTO.numberOfSpectralBands and !translucent) {
				translucent = (diffuseTransmittance[sbi] > 0.);
				++sbi;
			}

			std::string materialType = "mattetranslucent";
			if (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent) {
				materialType = "fluorescent";
	//		} else if (lambertianProperty.specularData.isSpecular) {
	//			if (translucent) {
	//				materialType = "glossytranslucent";
	//			} else {
	//				materialType = "glossy2";
	//			}
			} else {
				if (translucent) {
					materialType = "mattetranslucent";
				} else {
					materialType = "matte";
				}
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", materialType);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(lambertianPropertyParameters.getModelIndex()));

			if (lambertianPropertyParameters.useDirectionalDirectTransmittance()) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(lambertianPropertyParameters._angularStep));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", lambertianPropertyParameters._ddtFilePath);
			} else if (hasDirectTransmittance) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", "90");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", writeLuxCoreDirectTransmittance(lambertianPropertyParameters, directTransmittance));
			}

			// Reflectance
			std::ostringstream valueKr, valueKt, emissivity;
			valueKr << setprecision(15);
			emissivity << setprecision(15);
			valueKr << reflectance[0];
			if(abs(1.0 - reflectance[0] - diffuseTransmittance[0]) < 1e-15) // avoid small value of emissivity, such as -2.22044604925031e-16
				emissivity << 0.0;
			else
				emissivity << 1 - reflectance[0] - diffuseTransmittance[0];
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueKr << " " << reflectance[spectralBandIndex];
				if(abs(1.0 - reflectance[spectralBandIndex] - diffuseTransmittance[spectralBandIndex]) < 1e-15)
					emissivity << " " << 0.0;
				else
					emissivity << " " << 1 - reflectance[spectralBandIndex] - diffuseTransmittance[spectralBandIndex];
			}
			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueKr << " " << reflectance[spectralBandIndex];
						emissivity << " "<< 0;
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueKr << " " << reflectance[spectralBandIndex];
						emissivity << " "<< 0;
					}
				}
			}

			if (materialType == "mattetranslucent" or materialType == "fluorescent") {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kr", valueKr.str());
			} else {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kd", valueKr.str());
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());
			if (translucent or (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent)) {
				// Transmittance
				valueKt << setprecision(15);
				if (reflectance[0] == 1) {
					valueKt << "0";
				} else {
					valueKt << (diffuseTransmittance[0] / (1. - reflectance[0]));
				}
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					if (reflectance[spectralBandIndex] == 1) {
						valueKt << " 0";
					} else {
						valueKt << " " << (diffuseTransmittance[spectralBandIndex] / (1. - reflectance[spectralBandIndex]));
					}
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							if (reflectance[spectralBandIndex] == 1) {
								valueKt << " 0";
							} else {
								valueKt << " " << (diffuseTransmittance[spectralBandIndex] / (1. - reflectance[spectralBandIndex]));
							}
						}
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							if (reflectance[spectralBandIndex] == 1) {
								valueKt << " 0";
							} else {
								valueKt << " " << (diffuseTransmittance[spectralBandIndex] / (1. - reflectance[spectralBandIndex]));
							}
						}
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());
			}

			if (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "fluorescent");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "fluorescencefiles", "fluorescence_0_" + std::to_string(lambertianPropertyParameters.getModelIndex())+ "_0") ;
				if (fluorescenceParameters.useBioCLimaticProfileEta) {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "1");
//					string bioClimaticFileRelativePath = DartEnv::getFilePath(INPUT_DIR, fluorescenceParameters.bioClimaticFileRelativePath);
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "bioClimaticFilePath", fluorescenceParameters.bioClimaticFileRelativePath);
				}
				else {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "0");
				}
			}

			if (lambertianProperty.specularData.isSpecular) {
				// Specular
				std::ostringstream valueKs;
				valueKs << setprecision(15);
				valueKs << lambertianProperty.specularData.refraction_index_down;
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					valueKs << " " << lambertianProperty.specularData.refraction_index_down;
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							valueKs << " " << lambertianProperty.specularData.refraction_index_down;
						}
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "index", valueKs.str());

				std::ostringstream valueRoughness;
				valueRoughness << setprecision(15);
				valueRoughness << 0.1;
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					valueRoughness << " " << 0.1;
				}
				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							valueRoughness << " " << 0.1;
						}
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "uroughness", valueRoughness.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "vroughness", valueRoughness.str());
			}

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(lambertianPropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "l" + boost::lexical_cast<std::string>(lambertianPropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
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
//		}
	}
}

std::string writeLuxCoreDirectTransmittance(const OpticalPropertyParameters & opp, const vector<double> & directTransmittance) {
	std::string fileName = "transmittance_" + std::to_string(opp.getOpticalPropertyType()) + "_" + std::to_string(opp.getModelIndex()) + ".txt";
	createOutputDirectory(DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase());
	std::string absoluteFilePath = DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase() + pathSeparator + fileName;

	std::ofstream stream(absoluteFilePath.c_str(), ios::out);
	for (auto value : directTransmittance) {
		stream << (1. - value) << std::endl;
	}
	stream.close();

	return LIB_PHASE + pathSeparator + fileName;
}

void treatSpecular(	OpticalPropertyParameters & specularPropertyParameters,
					unsigned int opIndex,
						vector<int> & useSpectralIrradianceForReflectance,
						PhaseKeyMappingGenerator & pkmg,
						ofstream & outCoeffSpecProperties,
						PhaseGeneralParametersDTO & genDTO,
						const vector<DirectionCartesienne> & listDirections) {

	double albedo = 0;
	double dTauMoy = 0;

	std::string baseMaterial;
	double roughness = 0.05;
	int materialType = 2;
	int baseBSDFType = 0;
	int baseBSDFIndex = 0;

	vector<double>ni(genDTO.numberOfSpectralBands),nt(genDTO.numberOfSpectralBands),
			tau(genDTO.numberOfSpectralBands), refl(genDTO.numberOfSpectralBands), trans(genDTO.numberOfSpectralBands);

	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		PropertySpecular specularProperty;

		computeOpticalPropertiesSpecular(spectralBandIndex, specularPropertyParameters, listDirections, specularProperty, albedo, dTauMoy);
		PhaseDAO::readCoeffDiffParametersForSpecularRoughness(specularPropertyParameters.getModelIndex(), specularProperty.roughness);
		PhaseDAO::readCoeffDiffParametersForSpecularMaterial(specularPropertyParameters.getModelIndex(), specularProperty.type, specularProperty.baseBSDF, specularProperty.baseBSDFIndex);

		//TODO test albeo
		// testSpecularAlbedo(specularPropertyParameters.getModelIndex(), specularProperty);

		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile("MultispectralOpticalFunctions.SpecularMultiFunctions.SpecularMulti[",
				specularPropertyParameters.getModelIndex(), -1.0, -1.0, -1.0);

		if (genDTO.accelerationEngine == 2) {
			ni[spectralBandIndex] = specularProperty.ni;
			nt[spectralBandIndex] = specularProperty.nt;
			tau[spectralBandIndex] = specularProperty.tau;
			refl[spectralBandIndex] = specularProperty.intensityR;
			trans[spectralBandIndex] = specularProperty.intensityT;
			roughness = specularProperty.roughness;
			materialType = specularProperty.type;

			if(specularProperty.baseBSDF == 0){// lambertian
				baseMaterial = "lambertian";
			}else if(specularProperty.baseBSDF == 4){ //RPV
				baseMaterial = "RPV";
			}else if(specularProperty.baseBSDF == 2){ // Hapke
				baseMaterial = "notImpementedYet";
				specularProperty.baseBSDFIndex = 0;
			}else if(specularProperty.baseBSDF == 22){ // mixed
				baseMaterial = "notImpementedYet";
				specularProperty.baseBSDFIndex = 0;
			}else{
				baseMaterial = "lambertian";
				specularProperty.baseBSDFIndex = 0;
			}
			baseBSDFType = specularProperty.baseBSDF;
			baseBSDFIndex = specularProperty.baseBSDFIndex;
		}
		//TODO write down specular information
//		writeRPV(pkmg, outCoeffSpecProperties, rpvProperty, rpvPropertyParameters.getModelName(), dTauMoy, rpvPropertyParameters.getModelIndex(),
//			spectralBandIndex);
	}

	if (genDTO.accelerationEngine == 2) {
//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "specular" <<  specularPropertyParameters.getModelIndex() /*<< "_" << tmpIndex*/ << ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));

//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(specularPropertyParameters.getModelIndex()));

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");

			std::ostringstream valueNi,valueNt,valueR,valueT, valueTau, emissivity;
			valueNi << setprecision(15);
			valueNt << setprecision(15);
			valueR << setprecision(15);
			valueT << setprecision(15); // named H in DART-FT while it name Rc in RAMI definition
			valueTau << setprecision(15);
			emissivity << setprecision(15);

			valueNi << ni[0]; valueNt<<nt[0]; valueR<<refl[0]; valueT<<trans[0]; valueTau<<tau[0];
			double R0 = (nt[0] - ni[0])/(nt[0] + ni[0]);
			R0 *= R0; // nadir incidence

			emissivity << 1.; // Force emissivity to 1, the real emissivity is computed in dart

			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueNi << " " << ni[spectralBandIndex]; valueNt << " " << nt[spectralBandIndex];
				valueR << " " << refl[spectralBandIndex]; valueT << " " << trans[spectralBandIndex];
				valueTau<< " " << tau[spectralBandIndex];
				R0 = (nt[spectralBandIndex] - ni[spectralBandIndex])/(nt[spectralBandIndex] + ni[spectralBandIndex]);
				R0 *= R0; // nadir incidence
				emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			}

			if (specularPropertyParameters.useDirectionalDirectTransmittance()) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(specularPropertyParameters._angularStep));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", specularPropertyParameters._ddtFilePath);
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueNi << " " << ni[spectralBandIndex]; valueNt << " " << nt[spectralBandIndex];
						valueR << " " << refl[spectralBandIndex]; valueT << " " << trans[spectralBandIndex];
						valueTau<< " " << tau[spectralBandIndex];
						R0 = (1 - nt[spectralBandIndex]/ni[spectralBandIndex])/(1 + nt[spectralBandIndex]/ni[spectralBandIndex]); // nadir incidence
						emissivity << " " << 1.;
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueNi << " " << ni[spectralBandIndex]; valueNt << " " << nt[spectralBandIndex];
						valueR << " " << refl[spectralBandIndex]; valueT << " " << trans[spectralBandIndex];
						valueTau<< " " << tau[spectralBandIndex];
						R0 = (1 - nt[spectralBandIndex]/ni[spectralBandIndex])/(1 + nt[spectralBandIndex]/ni[spectralBandIndex]); // nadir incidence
						emissivity << " " << 1.;
					}
				}
			}

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "ni", valueNi.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "nt", valueNt.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kr", valueR.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueT.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "tau", valueTau.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());

			if(materialType == 0){ // interface
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "inbounce", "0");
				if(roughness < 1e-3){
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "specular");
				}else{
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "glossy");
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "roughness", std::to_string(roughness));
				}
			}else if(materialType == 1){ // thin layer
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "inbounce", "1");
				if(roughness < 1e-3){
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "specular");
				}else{
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "glossy");
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "roughness", std::to_string(roughness));
				}
			}else if(materialType == 2){ // glossy coating + diffuse base
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "glossydiffuse");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "roughness", std::to_string(roughness));
				std::ostringstream baseMat;
				baseMat << baseMaterial << baseBSDFIndex /*<< "_" << tmpIndex*/;

				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "base", baseMat.str());

				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "baseType", std::to_string(baseBSDFType));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "baseIndex", std::to_string(baseBSDFIndex));
			}

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(specularPropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "l" + boost::lexical_cast<std::string>(specularPropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", textureName + ".bin");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");

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
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
				}
			}
//		}
	}
}


void treatLambertianFluorescence(	unsigned int modelIndex,
									PhaseKeyMappingGenerator & pkmg,
									ofstream & outCoeffSpecProperties,
									PhaseGeneralParametersDTO & genDTO) {
	FluorescenceResults fluorescenceResults;
	fluorescenceResults.isFluorescent = false;
	if (PhaseProperty::getFluorescence()) {
		FluorescenceParameters fluorescenceParameters;
		PhaseDAO::readCoeffDiffParametersForLambertianFluspect(modelIndex, fluorescenceParameters);
		if (fluorescenceParameters.isFluorescent) {
			vector<vector<vector<double> > > interpolatedMatrices(genDTO.numberOfSpectralBands,
				vector<vector<double> >(genDTO.numberOfSpectralBands, vector<double>(4, 0.)));
			computeFluorescenceMatrices(fluorescenceResults, interpolatedMatrices, globalReferences::LAMBERTIEN, modelIndex, fluorescenceParameters, genDTO, 0);
		}
	}
	writeFluorescenceToSimulationProperties(fluorescenceResults, pkmg, outCoeffSpecProperties);
}

void writeFluorescenceToSimulationProperties(	const FluorescenceResults & fluorescenceResults,
												PhaseKeyMappingGenerator & pkmg,
												ofstream & outCoeffSpecProperties) {
	if (fluorescenceResults.isFluorescent) {
		outCoeffSpecProperties << pkmg.cleUseFluorescence() << "1" << endl;
		outCoeffSpecProperties << pkmg.cleFluorescenceFileName() << fluorescenceResults.fileName << endl;
		outCoeffSpecProperties << pkmg.cleForceYields() << fluorescenceResults.forceYields << endl;
		outCoeffSpecProperties << pkmg.cleUseCombinedYields() << fluorescenceResults.useCombinedYields << endl;
		outCoeffSpecProperties << pkmg.cleFluorescenceNbCases() << fluorescenceResults.yieldPSI.size() << endl;
		for (unsigned int i = 0; i < fluorescenceResults.yieldPSI.size(); ++i) {
			outCoeffSpecProperties << pkmg.cleFluorescenceYieldPSI(i) << fluorescenceResults.yieldPSI[i] << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceYieldPSII(i) << fluorescenceResults.yieldPSII[i] << endl;
		}

		outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticUsage() << fluorescenceResults.useBioCLimaticProfileEta << endl;
		if (fluorescenceResults.useBioCLimaticProfileEta) {
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticFile() << fluorescenceResults.bioClimaticFileRelativePath << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticLinkedPropertyType() << fluorescenceResults.linkedOpticalPropertyType << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticLinkedPropertyIndex() << fluorescenceResults.linkedOpticalPropertyIndex << endl;
		}
	} else {
		outCoeffSpecProperties << pkmg.cleUseFluorescence() << "0" << endl;
	}
}

void computeFluorescenceMatrices(	FluorescenceResults & fluorescenceResults,
									vector<vector<vector<double> > > & interpolatedMatrices,
									const int & typeOfReflectance,
									unsigned int modelIndex,
									const FluorescenceParameters & fluorescence,
									PhaseGeneralParametersDTO & genDTO,
									unsigned int face) {
	cout << " - Computation of fluorescence conversion matrices" << endl;
	ostringstream variableConvertie;
	variableConvertie << "fluorescence_" << typeOfReflectance << "_" << modelIndex << "_" << face <<".txt";
	string fileName = variableConvertie.str();

	createOutputDirectory(DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase());
	string absoluteMatricePath = DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase() + pathSeparator
			+ fileName;

	fstream ofstreamFluorescenceMatrices;
	ofstreamFluorescenceMatrices.open(absoluteMatricePath.c_str(), std::fstream::out);
	if (ofstreamFluorescenceMatrices) {
		/* Compute the Cm used by the script, if it's the new one, need to be set to 0 if Cbc+cp is used, for the old one it's the sum of Cbc+Cp if the new cm definition is used */
		double scriptCm = fluorescence.useCombinedYields ? (fluorescence.useNewCm ? 0 : fluorescence.leafMassPerArea) : (fluorescence.useNewCm ? fluorescence.Cbc + fluorescence.Cp : fluorescence.leafMassPerArea);
		string fluspectTmpDirectory = DartEnv::getCurrentSimulationOutputPath() + "fluspectTMP" + pathSeparator;
		createOutputDirectory(fluspectTmpDirectory);
		ostringstream commandLineRoot;
		commandLineRoot << DartEnv::getPythonExecutable() << " ";
		commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "Fluspect" << pathSeparator
				<< "src" << pathSeparator << fluorescence.pythonScript << " ";
		commandLineRoot << DartEnv::getFilePath(INPUT_DIR, fluorescence.databaseName) << " ";
		commandLineRoot << fluspectTmpDirectory << " ";
		commandLineRoot << "-fluspect" << " ";
		commandLineRoot << fluorescence.chlorophyll << " "
				<< fluorescence.carotenoid << " "
				<< fluorescence.brownPigment << " "
				<< fluorescence.waterThickness << " "
				<< scriptCm << " "
				<< fluorescence.structureCoeff << " ";
		unsigned int wlEMin = 400, wlEMax = 750, wlFMin = 640, wlFMax = 850;
		vector<vector<vector<double> > > fluspectMatrices(wlEMax - wlEMin + 1, vector<vector<double> >(wlFMax - wlFMin + 1, vector<double>(5, 0.)));

		if (!fluorescence.forceYield && !PhaseProperty::getFluorescenceYields().empty()) {
			unsigned int yieldCase = 0;
			cout << "   -> Nb of yields combinations: " << PhaseProperty::getFluorescenceYields().size() << endl;
			for (const auto & pairPSI_PSII : PhaseProperty::getFluorescenceYields()) {
				ostringstream commandLine;
				commandLine << commandLineRoot.str();
				if (fluorescence.useCombinedYields) {
					// Add cbc & cp in all case, if Cm != 0 the values are not used
					commandLine << pairPSI_PSII.first << " " << fluorescence.v2z << " " << fluorescence.anthocyanin << " " << fluorescence.Cbc << " " << fluorescence.Cp;
				} else {
					commandLine << pairPSI_PSII.first << " " << pairPSI_PSII.second << " " << fluorescence.v2z << " " << fluorescence.anthocyanin;
				}
				std::cout << "    Run python cmd: " << commandLine.str() << endl;
				std::system(commandLine.str().c_str());

				string fluspectOutputFile = fluspectTmpDirectory + "Leaf_fluspect.txt";

				boost::filesystem::path pathToFluspectOutputFile(fluspectOutputFile);
				if (boost::filesystem::exists(pathToFluspectOutputFile)) {
					interpolateAndWriteInFluorescenceMatrixFile(ofstreamFluorescenceMatrices, fluspectOutputFile, fluspectMatrices,
						interpolatedMatrices, genDTO, wlEMin, wlEMax, wlFMin, wlFMax, fluorescence.useCombinedYields);
				} else {
					cerr << "Couldn't retrieve the fluspect output file." << endl;
				}
				fluorescenceResults.yieldPSI.push_back(pairPSI_PSII.first);
				fluorescenceResults.yieldPSII.push_back(pairPSI_PSII.second);
				++yieldCase;
			}
			fluorescenceResults.forceYields = false;
		} else {
			ostringstream commandLine;
			commandLine << commandLineRoot.str();
			if (fluorescence.useCombinedYields) {
				// Add cbc & cp in all case, if Cm != 0 the values are not used
				commandLine << fluorescence.yieldPSI << " -999 " << fluorescence.anthocyanin << " " << fluorescence.Cbc << " " << fluorescence.Cp;
			} else {
				commandLine << fluorescence.yieldPSI << " " << fluorescence.yieldPSII << " -999 " << fluorescence.anthocyanin;
			}

			std::cout << "    Run python cmd: " << commandLine.str() << endl;
			std::system(commandLine.str().c_str());

			string fluspectOutputFile = fluspectTmpDirectory + "Leaf_fluspect.txt";

			boost::filesystem::path pathToFluspectOutputFile(fluspectOutputFile);
			if (boost::filesystem::exists(pathToFluspectOutputFile)) {
				interpolateAndWriteInFluorescenceMatrixFile(ofstreamFluorescenceMatrices, fluspectOutputFile, fluspectMatrices, interpolatedMatrices,
					genDTO, wlEMin, wlEMax, wlFMin, wlFMax, fluorescence.useCombinedYields);
			}
			fluorescenceResults.yieldPSI.push_back(fluorescence.yieldPSI);
			fluorescenceResults.yieldPSII.push_back(fluorescence.yieldPSII);
			fluorescenceResults.forceYields = true;
		}
		fluorescenceResults.fileName = fileName;
		fluorescenceResults.isFluorescent = true;
		fluorescenceResults.useCombinedYields = fluorescence.useCombinedYields;

		// Transfer of bio-climatic parameters
		fluorescenceResults.useBioCLimaticProfileEta = fluorescence.useBioCLimaticProfileEta;
		if (fluorescenceResults.useBioCLimaticProfileEta) {
			fluorescenceResults.bioClimaticFileRelativePath = fluorescence.bioClimaticFileRelativePath;
			fluorescenceResults.linkedOpticalPropertyIndex = fluorescence.linkedOpticalPropertyIndex;
			fluorescenceResults.linkedOpticalPropertyType = fluorescence.linkedOpticalPropertyType;
		}

		boost::filesystem::path pathToFluspectTmpDirectory(fluspectTmpDirectory);
		if (boost::filesystem::exists(pathToFluspectTmpDirectory)) {
			boost::filesystem::remove_all(pathToFluspectTmpDirectory);
		}
		ofstreamFluorescenceMatrices.close();
	} else {
		fluorescenceResults.isFluorescent = false;
	}
}

void interpolateAndWriteInFluorescenceMatrixFile(	fstream & ofstreamFluorescenceMatrices,
													const string & fluspectOutputFile,
													vector<vector<vector<double> > > & fluspectMatrices,
													vector<vector<vector<double> > > & interpolatedMatrices,
													PhaseGeneralParametersDTO & genDTO,
													unsigned int wlEMin,
													unsigned int wlEMax,
													unsigned int wlFMin,
													unsigned int wlFMax,
													const bool & useCombinedYields) {
	ifstream stream(fluspectOutputFile.c_str(), ifstream::in);
	if (stream) {
		// MBI
		for (unsigned int j = 0; j <= wlFMax - wlFMin; ++j) {
			for (unsigned int i = 0; i <= wlEMax - wlEMin; ++i) {
				fluspectMatrices[i][j][0] = wlFMin + j;
				stream >> fluspectMatrices[i][j][1];
				if (useCombinedYields) {
					double unitConversionFactor = (double)(wlEMin + i) / (double)(wlFMin + j);
					fluspectMatrices[i][j][1] *= unitConversionFactor;
				}
			}
		}
		// MBII
		if (!useCombinedYields) {
			for (unsigned int j = 0; j <= wlFMax - wlFMin; ++j) {
				for (unsigned int i = 0; i <= wlEMax - wlEMin; ++i) {
					stream >> fluspectMatrices[i][j][2];
				}
			}
		}
		// MFI
		for (unsigned int j = 0; j <= wlFMax - wlFMin; ++j) {
			for (unsigned int i = 0; i <= wlEMax - wlEMin; ++i) {
				stream >> fluspectMatrices[i][j][3];
				if (useCombinedYields) {
					double unitConversionFactor = (double)(wlEMin + i) / (double)(wlFMin + j);
					fluspectMatrices[i][j][3] *= unitConversionFactor;
				}
			}
		}
		// MFII
		if (!useCombinedYields) {
			for (unsigned int j = 0; j <= wlFMax - wlFMin; ++j) {
				for (unsigned int i = 0; i <= wlEMax - wlEMin; ++i) {
					stream >> fluspectMatrices[i][j][4];
				}
			}
		}
		stream.close();

		std::vector<vector<double> > interpolationInput, interpolationOutput, interpolationResults;

		// Interpolation on the emission bands
		interpolationOutput.resize(fluspectMatrices.size(), vector<double>(1 + genDTO.numberOfSpectralBands * 4));
		interpolationInput.resize(fluspectMatrices[0].size(), vector<double>(5));
		for (unsigned int i = 0; i < fluspectMatrices.size(); ++i) {
			for (unsigned int j = 0; j < fluspectMatrices[0].size(); ++j) {
				interpolationInput[j][0] = (wlFMin + j) / 1000.;
				interpolationInput[j][1] = fluspectMatrices[i][j][1];
				interpolationInput[j][2] = fluspectMatrices[i][j][2];
				interpolationInput[j][3] = fluspectMatrices[i][j][3];
				interpolationInput[j][4] = fluspectMatrices[i][j][4];
			}

			interpolationOutput[i][0] = (wlEMin + i) / 1000.;
			for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
				interpolationResults.clear();
				interpolationResults.resize(fluspectMatrices[0].size() + 2, vector<double>(5, 0.));
				Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(spectralBandIndex2),
									  PhaseProperty::getUpperBandWavelength(spectralBandIndex2),
									  fluspectMatrices[0].size(),
									  5,
									  interpolationInput,
									  interpolationResults);

				interpolationOutput[i][spectralBandIndex2 * 4 + 1] = interpolationResults[fluspectMatrices[0].size() + 1][1];
				interpolationOutput[i][spectralBandIndex2 * 4 + 2] = interpolationResults[fluspectMatrices[0].size() + 1][2];
				interpolationOutput[i][spectralBandIndex2 * 4 + 3] = interpolationResults[fluspectMatrices[0].size() + 1][3];
				interpolationOutput[i][spectralBandIndex2 * 4 + 4] = interpolationResults[fluspectMatrices[0].size() + 1][4];
			}
		}
		for (unsigned int spectralBandIndex1 = 0; spectralBandIndex1 < genDTO.numberOfSpectralBands; spectralBandIndex1++) {
			if (PhaseProperty::getLowerBandWavelength(spectralBandIndex1) * 1000. > wlEMax || PhaseProperty::getUpperBandWavelength(spectralBandIndex1) * 1000. < wlEMin) {
				// Energy intercepted in this band doesn't generate fluorescence in the other bands
				for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
					interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][0] = 0;
					interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][1] = 0;
					interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][2] = 0;
					interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][3] = 0;
				}
			} else {
				interpolationResults.clear();
				interpolationResults.resize(fluspectMatrices.size() + 2, vector<double>(1 + genDTO.numberOfSpectralBands * 4, 0.));
				Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(spectralBandIndex1),
									  PhaseProperty::getUpperBandWavelength(spectralBandIndex1),
									  fluspectMatrices.size(),
									  1 + genDTO.numberOfSpectralBands * 4,
									  interpolationOutput,
									  interpolationResults);

				for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
					if (PhaseProperty::getLowerBandWavelength(spectralBandIndex2) * 1000. > wlFMax || PhaseProperty::getUpperBandWavelength(spectralBandIndex2) * 1000. < wlFMin) {
						// This band is outside the fluorescence band, it doesn't emit anything.
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][0] = 0;
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][1] = 0;
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][2] = 0;
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][3] = 0;
					} else {
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][0] = interpolationResults[fluspectMatrices.size() + 1][spectralBandIndex2 * 4 + 1] * (PhaseProperty::getUpperBandWavelength(spectralBandIndex1) - PhaseProperty::getLowerBandWavelength(spectralBandIndex1)) * 1000;
						interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][2] = interpolationResults[fluspectMatrices.size() + 1][spectralBandIndex2 * 4 + 3] * (PhaseProperty::getUpperBandWavelength(spectralBandIndex1) - PhaseProperty::getLowerBandWavelength(spectralBandIndex1)) * 1000;

						if (!useCombinedYields) {
							interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][1] = interpolationResults[fluspectMatrices.size() + 1][spectralBandIndex2 * 4 + 2] * (PhaseProperty::getUpperBandWavelength(spectralBandIndex1) - PhaseProperty::getLowerBandWavelength(spectralBandIndex1)) * 1000;
							interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][3] = interpolationResults[fluspectMatrices.size() + 1][spectralBandIndex2 * 4 + 4] * (PhaseProperty::getUpperBandWavelength(spectralBandIndex1) - PhaseProperty::getLowerBandWavelength(spectralBandIndex1)) * 1000;
						}
					}
				}
			}
		}

		// Write the interpolated matrices
		ofstreamFluorescenceMatrices << setprecision(15);
		for (unsigned int spectralBandIndex1 = 0; spectralBandIndex1 < genDTO.numberOfSpectralBands; spectralBandIndex1++) {
			for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
				ofstreamFluorescenceMatrices << interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][0] << " ";
			}
			ofstreamFluorescenceMatrices << endl;
		}
		ofstreamFluorescenceMatrices << endl;
		//if (!useCombinedYields) {
			for (unsigned int spectralBandIndex1 = 0; spectralBandIndex1 < genDTO.numberOfSpectralBands; spectralBandIndex1++) {
				for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
					ofstreamFluorescenceMatrices << interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][1] << " ";
				}
				ofstreamFluorescenceMatrices << endl;
			}
			ofstreamFluorescenceMatrices << endl;
		//}
		for (unsigned int spectralBandIndex1 = 0; spectralBandIndex1 < genDTO.numberOfSpectralBands; spectralBandIndex1++) {
			for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
				ofstreamFluorescenceMatrices << interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][2] << " ";
			}
			ofstreamFluorescenceMatrices << endl;
		}
		ofstreamFluorescenceMatrices << endl;
		//if (!useCombinedYields) {
			for (unsigned int spectralBandIndex1 = 0; spectralBandIndex1 < genDTO.numberOfSpectralBands; spectralBandIndex1++) {
				for (unsigned int spectralBandIndex2 = 0; spectralBandIndex2 < genDTO.numberOfSpectralBands; spectralBandIndex2++) {
					ofstreamFluorescenceMatrices << interpolatedMatrices[spectralBandIndex1][spectralBandIndex2][3] << " ";
				}
				ofstreamFluorescenceMatrices << endl;
			}
			ofstreamFluorescenceMatrices << endl;
		//}
	} else {
		cerr << "Error reading the fluspect output file from " << fluspectOutputFile << endl;
	}
}

void computeOpticalPropertiesLambertian(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & lambertianPropertyParameters,
										const vector<DirectionCartesienne> & listDirections,
										PropertyLambertian & lambertianProperty,
										double & dTauMoy,
										double & albedo) {
	shared_ptr<LambertianDatabaseParameters> lambertianDatabaseParameters = ::static_pointer_cast<LambertianDatabaseParameters>(lambertianPropertyParameters.getDatabaseParameters(LAMBERTIAN_DATABASE));
	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(lambertianPropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<SpecularDatabaseParameters> lambertianSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(lambertianPropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
	unsigned int modelIndex = lambertianPropertyParameters.getModelIndex();

	lambertianProperty.diffuseTransmittanceAccelerationFactor = PhaseProperty::getDiffuseTransmittanceAccelerationForLambertian(modelIndex);

	lambertianProperty.reflectance = 0.01 * lambertianDatabaseParameters.get()->getReflectanceForBand(spectralBandIndex) * PhaseProperty::getMultiplicativeFactorReflectanceForLambertian(modelIndex, spectralBandIndex);
	lambertianProperty.directTransmittance = 0.01 * lambertianDatabaseParameters.get()->getDirectTransmittanceForBand(spectralBandIndex) * PhaseProperty::getMultiplicativeFactorDirectTransmittanceFactoForLambertian(modelIndex, spectralBandIndex);
	lambertianProperty.diffuseTransmittance = 0.01 * lambertianDatabaseParameters.get()->getDiffuseTransmittanceForBand(spectralBandIndex) * PhaseProperty::getMultiplicativeFactorDiffuseTransmittanceFactoForLambertian(modelIndex, spectralBandIndex);
	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(lambertianProperty.directTransmittance, listDirections) + lambertianProperty.diffuseTransmittance) * 0.5;
	lambertianProperty.standardDeviation = lambertianDatabaseParameters->getDeviation();
	lambertianProperty.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature() * PhaseProperty::getMultiplicativeFactorDirectTransmittanceFactoForLambertian(modelIndex, spectralBandIndex);

	if (lambertianSpecularParameters.get() != NULL) {
		lambertianProperty.specularData.isSpecular = true;
		computeSpecular(lambertianProperty.specularData, PhaseProperty::getMultiplicativeFactorSpecularIntensityForLambertian(modelIndex, spectralBandIndex), spectralBandIndex, lambertianSpecularParameters);
	} else {
		lambertianProperty.specularData.isSpecular = false;
	}
}

void computeOpticalPropertiesSpecular(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & specularPropertyParameters,
										const vector<DirectionCartesienne> & listDirections,
										PropertySpecular & specularProperty,
										double & dTauMoy,
										double & albedo) {

	shared_ptr<RefractiveIndexDatabaseParameters> refractiveIndexDatabaseParameters = ::static_pointer_cast<RefractiveIndexDatabaseParameters>(
			specularPropertyParameters.getDatabaseParameters(REFRACTIVEINDEX_DATABASE));

	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
			specularPropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<TransmittanceDatabaseParameters> transmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
			specularPropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));

	unsigned int modelIndex = specularPropertyParameters.getModelIndex();

	specularProperty.ni = refractiveIndexDatabaseParameters.get()->getRefractionIndex(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorNiForSpecular(modelIndex, spectralBandIndex);

	specularProperty.nt = refractiveIndexDatabaseParameters.get()->getRefractionIndex2(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorNtForSpecular(modelIndex, spectralBandIndex);

	PhaseDAO::readCoeffDiffParametersForSpecularTau(specularPropertyParameters.getModelIndex(), specularProperty.tau);
	specularProperty.tau = specularProperty.tau * PhaseProperty::getMultiplicativeFactorTauForSpecular(modelIndex, spectralBandIndex);

	specularProperty.intensityR = PhaseProperty::getMultiplicativeFactorReflectionForSpecular(modelIndex, spectralBandIndex);
	specularProperty.intensityT = PhaseProperty::getMultiplicativeFactorTransmissionForSpecular(modelIndex, spectralBandIndex);

	// TODO I need to compute the specularProperty.specularData, specularProperty.temperatureData, dTauMoy, albedo here

	specularProperty.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature();

	double directTransmittance = 1.0;
	double diffuseTransmittance = 0.5;
	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(directTransmittance, listDirections) + diffuseTransmittance) * 0.5;
	albedo = 0.5;
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

void writeLambertian(	const unsigned int& modelIndex,
						const string & modelName,
						const unsigned int& spectralBandIndex,
						PhaseKeyMappingGenerator & pkmg,
						ofstream & outCoeffSpecProperties,
						PropertyLambertian & lambertianProperty,
						const double & dTauMoy,
						const double & albedo,
						const vector<DirectionCartesienne> & listDirections) {
	pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	pkmg.setBandeSpectrale(spectralBandIndex);
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
	outCoeffSpecProperties << pkmg.cleRO() << lambertianProperty.reflectance << endl;
	outCoeffSpecProperties << pkmg.cleAlbedo() << albedo << endl;

	writeSpecular(lambertianProperty.specularData, outCoeffSpecProperties, pkmg);

	outCoeffSpecProperties << pkmg.cleEcartType() << lambertianProperty.standardDeviation << endl;
	outCoeffSpecProperties << pkmg.cleTauDirect() << lambertianProperty.directTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauDiffus() << lambertianProperty.diffuseTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauMoy() << dTauMoy << endl;
	if (spectralBandIndex == 0) {
		outCoeffSpecProperties << pkmg.cleTauDirectPourTemperature() << lambertianProperty.temperatureData.directTransmittanceTemperature << endl;
		outCoeffSpecProperties << pkmg.cleDiffuseTransmittanceAccelerationFactor() << lambertianProperty.diffuseTransmittanceAccelerationFactor
				<< endl;
	}
}

void testLambertianAlbedo(	const unsigned int& modelIndex,
							PropertyLambertian & opticalProperties) {
	if (!opticalProperties.specularData.isSpecular) {
		if (opticalProperties.reflectance + opticalProperties.diffuseTransmittance > 1.000001) {
			cerr << " LAMBERTIEN Warning : Reflectance + DiffuseTransmittance = "
					<< opticalProperties.reflectance + opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : "
					<< modelIndex << " " << endl;
		}
	} else {
		if (!opticalProperties.specularData._useRefraction) {
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " LAMBERTIEN Warning : Reflectance + albedo_down_to_up + DiffuseTransmittance = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " LAMBERTIEN Warning : Reflectance + albedo_up_to_down + DiffuseTransmittance = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		} else {
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_down_to_up > 1.000001) {
				cerr << " LAMBERTIEN Warning : Reflectance + albedo_down_to_up + DiffuseTransmittance + albedo_refraction_down_to_up = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
								+ opticalProperties.specularData.albedo_refraction_down_to_up << " > 1 !! - lambertian phase function : "
						<< modelIndex << " " << endl;
			}
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_up_to_down > 1.000001) {
				cerr << " LAMBERTIEN Warning : Reflectance + albedo_up_to_down + DiffuseTransmittance + albedo_refraction_up_to_down = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
								+ opticalProperties.specularData.albedo_refraction_up_to_down << " > 1 !! - lambertian phase function : "
						<< modelIndex << " " << endl;
			}
		}
	}
	if (opticalProperties.reflectance < 0) {
		cerr << " Warning : Reflectance < 0 !!! (lambertian phase function : " << modelIndex << " ) -> fixed to 0" << endl;
		opticalProperties.reflectance = 0;
	}
}

/* ----------------------------------------------------------------------------------------------------------
 * 									 	Phase Extern
 */
void treatPhaseExtern(OpticalPropertyParameters & PhaseExternPropertyParameters,
						vector<int> & useSpectralIrradianceForReflectance,
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

	if (genDTO.accelerationEngine == 2) {
			std::ostringstream keyRoot;

			keyRoot << "scene.materials." << "PhaseExtern" <<  PhaseExternPropertyParameters.getModelIndex()<< ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "PhaseExtern");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "dataBase",
					std::to_string(PhaseExternPropertyParameters.getModelIndex()) + ".phaseExternData");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
	}
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



/* -----------------------------------------------------------------------------------------------------------
 * 											Hapke
 */
void treatHapke(OpticalPropertyParameters & hapkePropertyParameters,
				unsigned int opIndex,
				vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const vector<DirectionCartesienne> & listDirections) {
	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE_SPEC);
	double albedo = 0;
	double dTauMoy = 0;
	vector<double>w(genDTO.numberOfSpectralBands),c1(genDTO.numberOfSpectralBands),c2(genDTO.numberOfSpectralBands),
			c3(genDTO.numberOfSpectralBands), c4(genDTO.numberOfSpectralBands), h1(genDTO.numberOfSpectralBands),
			h2(genDTO.numberOfSpectralBands), diffuseTransmittance(genDTO.numberOfSpectralBands);

	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		PropertyHapke hapkeProperty;

		computeOpticalPropertiesHapke(spectralBandIndex, hapkePropertyParameters, listDirections, hapkeProperty);
		computeAlbedodTauMoyHapke(hapkeProperty, albedo, dTauMoy, listDirections);
		testHapkeAlbedo(hapkePropertyParameters.getModelIndex(), hapkeProperty);

		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile(
			"MultispectralOpticalFunctions.HapkeSpecularMultiFunctions.HapkeSpecularMulti[", hapkePropertyParameters.getModelIndex(), -1.0, -1.0,
			-1.0);

		if (genDTO.accelerationEngine == 2) {
			w[spectralBandIndex] = hapkeProperty.hapkeParameterW;
			c1[spectralBandIndex] = hapkeProperty.hapkeParameterC1;
			c2[spectralBandIndex] = hapkeProperty.hapkeParameterC2;
			c3[spectralBandIndex] = hapkeProperty.hapkeParameterC3;
			c4[spectralBandIndex] = hapkeProperty.hapkeParameterC4;
			h1[spectralBandIndex] = hapkeProperty.hapkeParameterH1;
			h2[spectralBandIndex] = hapkeProperty.hapkeParameterH2;
			diffuseTransmittance[spectralBandIndex] = hapkeProperty.diffuseTransmittance;
		}

		writeTreatHapke(outCoeffSpecProperties, pkmg, hapkeProperty, hapkePropertyParameters.getModelName(), spectralBandIndex,
			hapkePropertyParameters.getModelIndex(), albedo, dTauMoy);
	}

	if (genDTO.accelerationEngine == 2) {
//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "Hapke" <<  hapkePropertyParameters.getIndexOrder() /*<< "_" << tmpIndex*/ << ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "Hapke1981");

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(hapkePropertyParameters.getModelIndex()));

			// Reflectance
			std::ostringstream valueW,valueC1,valueC2,valueC3, valueC4, valueH1, valueH2, valueKt, valueThetaMax, emissivity;
			valueW << setprecision(15);
			valueC1 << setprecision(15);
			valueC2 << setprecision(15);
			valueC3 << setprecision(15);
			valueC4 << setprecision(15);
			valueH1 << setprecision(15);
			valueH2 << setprecision(15);
			valueKt << setprecision(15);
			valueThetaMax << setprecision(15);
			emissivity << setprecision(15);
			valueW << w[0]; valueC1<<c1[0]; valueC2<<c2[0]; valueC3<<c3[0]; valueC4<<c4[0]; valueH1<<h1[0]; valueH2<<h2[0]; valueKt<<((diffuseTransmittance[0]<=1e-8) ? 0.0 : diffuseTransmittance[0]/(1 - w[0]));
			emissivity << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
				valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
				valueH2<< " " <<h2[spectralBandIndex];
				valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
				emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			}

			if (hapkePropertyParameters.useDirectionalDirectTransmittance()) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(hapkePropertyParameters._angularStep));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", hapkePropertyParameters._ddtFilePath);
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
						valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
						valueH2<< " " <<h2[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
						emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
						valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
						valueH2<< " " <<h2[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
						emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
					}
				}
			}

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "w", valueW.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c1", valueC1.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c2", valueC2.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c3", valueC3.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c4", valueC4.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "h1", valueH1.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "h2", valueH2.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(hapkePropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "l" + boost::lexical_cast<std::string>(hapkePropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
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
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
				}
			}
//		}
	}
}

void computeAlbedodTauMoyHapke(	const PropertyHapke & opticalProperties,
								double & albedo,
								double & dTauMoy,
								const vector<DirectionCartesienne> & listDirections) {
	double hapkeGamma = 1.;
	if (opticalProperties.hapkeParameterW > 1.) {
		hapkeGamma = 0.;
	} else if (opticalProperties.hapkeParameterW < 0.) {
		hapkeGamma = 1.;
	} else {
		hapkeGamma = sqrt(1 - opticalProperties.hapkeParameterW);
	}
	albedo = (1. - hapkeGamma) / (1. + hapkeGamma); //Suppression du spéculaire + opticalProperties.albedoSpecular;
	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(opticalProperties.directTransmittance, listDirections)
			+ opticalProperties.diffuseTransmittance) * 0.5;
}

void testHapkeAlbedo(	const unsigned int& modelIndex,
						PropertyHapke & opticalProperties) {
	if (!opticalProperties.specularData.isSpecular) {
		if (opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance > 1.000001) {
			cerr << " HAPKE Warning : hapke W + DiffuseTransmittance = " << opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance
					<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
		}
	} else {
		if (!opticalProperties.specularData._useRefraction) {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		} else {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_down_to_up > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance + albedo_refraction_down_to_up = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_down_to_up
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_up_to_down > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance + albedo_refraction_up_to_down = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_up_to_down
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		}
	}
	if (opticalProperties.hapkeParameterW < 0) {
		cerr << " HAPKE Warning : hapke W < 0 !!! (hapke phase function : " << modelIndex << " ) -> fixed to 0" << endl;
		opticalProperties.hapkeParameterW = 0;
	}
}

void computeOpticalPropertiesHapke(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & hapkePropertyParameters,
									const vector<DirectionCartesienne> & listDirections,
									PropertyHapke & hapkeProperty) {
	shared_ptr<HapkeDatabaseParameters> hapkeDatabaseParameters = ::static_pointer_cast<HapkeDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(HAPKE_DATABASE));
	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<TransmittanceDatabaseParameters> hapkeTransmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));
	shared_ptr<SpecularDatabaseParameters> kapkeSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
	unsigned int modelIndex = hapkePropertyParameters.getModelIndex();

	if (kapkeSpecularParameters.get() != NULL) {
		hapkeProperty.specularData.isSpecular = true;
		computeSpecular(hapkeProperty.specularData, PhaseProperty::getMultiplicativeFactorSpecularIntensityForHapke(modelIndex, spectralBandIndex),
			spectralBandIndex, kapkeSpecularParameters);
	} else {
		hapkeProperty.specularData.isSpecular = false;
	}

	hapkeProperty.directTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDirectTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDirectTransmittanceFactorForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.diffuseTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDiffuseTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDiffuseTransmittanceFactorForHapke(modelIndex, spectralBandIndex);

	hapkeProperty.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature();

	hapkeProperty.hapkeParameterW = hapkeDatabaseParameters.get()->getHapkeWForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorWForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC1 = hapkeDatabaseParameters.get()->getHapkeC1ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC1ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC2 = hapkeDatabaseParameters.get()->getHapkeC2ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC2ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC3 = hapkeDatabaseParameters.get()->getHapkeC3ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC3ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC4 = hapkeDatabaseParameters.get()->getHapkeC4ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC4ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterH1 = hapkeDatabaseParameters.get()->getHapkeH1ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorH1ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterH2 = hapkeDatabaseParameters.get()->getHapkeH2ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorH2ForHapke(modelIndex, spectralBandIndex);
}

void writeTreatHapke(	ofstream & outCoeffSpecProperties,
						PhaseKeyMappingGenerator & pkmg,
						PropertyHapke & hapkeProperty,
						const string & modelName,
						const unsigned int & spectralBandIndex,
						const unsigned int & modelIndex,
						const double & albedo,
						const double & dTauMoy) {
	pkmg.setNumeroFonctionDePhase(modelIndex);
	pkmg.setBandeSpectrale(spectralBandIndex);
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
	outCoeffSpecProperties << pkmg.cleW() << hapkeProperty.hapkeParameterW << endl;
	outCoeffSpecProperties << pkmg.cleCoeffH1() << hapkeProperty.hapkeParameterH1 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffC1() << hapkeProperty.hapkeParameterC1 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffC3() << hapkeProperty.hapkeParameterC3 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffC2() << hapkeProperty.hapkeParameterC2 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffC4() << hapkeProperty.hapkeParameterC4 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffH2() << hapkeProperty.hapkeParameterH2 << endl;

	writeSpecular(hapkeProperty.specularData, outCoeffSpecProperties, pkmg);

	outCoeffSpecProperties << pkmg.cleAlbedo() << albedo << endl;
	outCoeffSpecProperties << pkmg.cleTauDirect() << hapkeProperty.directTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauDiffus() << hapkeProperty.diffuseTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauMoy() << dTauMoy << endl;
	if (spectralBandIndex == 0) {
		outCoeffSpecProperties << pkmg.cleTauDirectPourTemperature() << hapkeProperty.temperatureData.directTransmittanceTemperature << endl;
	}
}


/* ----------------------------------------------------------------------------------------------------------------------
 *                                     Hapke 2
 *           (johan) added of hapke 2012
 */

void treatHapke2(OpticalPropertyParameters & hapke2PropertyParameters,
					unsigned int opIndex,
				vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const vector<DirectionCartesienne> & listDirections) {
	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE2_SPEC);
	double albedo = 0;
	double dTauMoy = 0;
	vector<double>w(genDTO.numberOfSpectralBands),c(genDTO.numberOfSpectralBands),b(genDTO.numberOfSpectralBands),
			h(genDTO.numberOfSpectralBands), b0(genDTO.numberOfSpectralBands), theta(genDTO.numberOfSpectralBands),
			diffuseTransmittance(genDTO.numberOfSpectralBands);
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		PropertyHapke2 hapke2Property;

		computeOpticalPropertiesHapke2(spectralBandIndex, hapke2PropertyParameters, listDirections, hapke2Property);
		computeAlbedodTauMoyHapke2(hapke2Property, albedo, dTauMoy, listDirections);
		testHapke2Albedo(hapke2PropertyParameters.getModelIndex(), hapke2Property);

		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile(
			"MultispectralOpticalFunctions.HapkeSpecularMultiFunctions.HapkeSpecularMulti2[", hapke2PropertyParameters.getModelIndex(), -1.0, -1.0,
			-1.0);
		if (genDTO.accelerationEngine == 2) {
			w[spectralBandIndex] = hapke2Property.hapkeParameterW;
			c[spectralBandIndex] = hapke2Property.hapkeParameterC;
			b[spectralBandIndex] = hapke2Property.hapkeParameterB;
			h[spectralBandIndex] = hapke2Property.hapkeParameterH;
			b0[spectralBandIndex] = hapke2Property.hapkeParameterB0;
			theta[spectralBandIndex] = M_PI/180*hapke2Property.hapkeParameterTh; //in deg to rad
			diffuseTransmittance[spectralBandIndex] = hapke2Property.diffuseTransmittance;
		}

		writeTreatHapke2(outCoeffSpecProperties, pkmg, hapke2Property, hapke2PropertyParameters.getModelName(), spectralBandIndex,
			hapke2PropertyParameters.getModelIndex(), albedo, dTauMoy);

	}

	if (genDTO.accelerationEngine == 2) {
//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "Hapke" <<  hapke2PropertyParameters.getIndexOrder()/*<< "_" << tmpIndex*/ << ".";    //TO CHANNNNGGGEEE
//			if (hapkeProperty.ident=="Hapke_Phase_Function_1"){
//				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "Hapke1981");}
//			else{
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "Hapke2012");
//	}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(hapke2PropertyParameters.getModelIndex()));

			// Reflectance
			std::ostringstream valueW,valueC,valueB,valueH, valueB0, valueTh, valueKt, valueThetaMax, emissivity;
			valueW << setprecision(15);
			valueC << setprecision(15);
			valueB << setprecision(15);
			valueH << setprecision(15);
			valueB0 << setprecision(15);
			valueTh << setprecision(15);
			valueKt << setprecision(15);
			valueThetaMax << setprecision(15);
			emissivity << setprecision(15);
			valueW << w[0]; valueC<<c[0]; valueB<<b[0]; valueH<<h[0]; valueB0<<b0[0]; valueTh<<theta[0]; valueKt<<((diffuseTransmittance[0]<=1e-8) ? 0.0 : diffuseTransmittance[0]/(1 - w[0]));
			emissivity << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueW << " " << w[spectralBandIndex]; valueC<< " " <<c[spectralBandIndex]; valueB<< " " <<b[spectralBandIndex];
				valueH<< " " <<h[spectralBandIndex]; valueB0<< " " <<b0[spectralBandIndex]; valueTh<< " " <<theta[spectralBandIndex];
				valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
				emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			}

			if (hapke2PropertyParameters.useDirectionalDirectTransmittance()) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(hapke2PropertyParameters._angularStep));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", hapke2PropertyParameters._ddtFilePath);
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueW << " " << w[spectralBandIndex]; valueC<< " " <<c[spectralBandIndex]; valueB<< " " <<b[spectralBandIndex];
						valueH<< " " <<h[spectralBandIndex]; valueB0<< " " <<b0[spectralBandIndex]; valueTh<< " " <<theta[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
						emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueW << " " << w[spectralBandIndex]; valueC<< " " <<c[spectralBandIndex]; valueB<< " " <<b[spectralBandIndex];
						valueH<< " " <<h[spectralBandIndex]; valueB0<< " " <<b0[spectralBandIndex]; valueTh<< " " <<theta[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
						emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
					}
				}
			}

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "w", valueW.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c", valueC.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "b", valueB.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "h", valueH.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "b0", valueB0.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "theta", valueTh.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(hapke2PropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "l" + boost::lexical_cast<std::string>(hapke2PropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
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
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
				}
			}
//		}
	}
}

// coul be same as Hapke
void computeAlbedodTauMoyHapke2(	const PropertyHapke2 & opticalProperties,
								double & albedo,
								double & dTauMoy,
								const vector<DirectionCartesienne> & listDirections) {
	double hapkeGamma = 1.;
	if (opticalProperties.hapkeParameterW > 1.) {
		hapkeGamma = 0.;
	} else if (opticalProperties.hapkeParameterW < 0.) {
		hapkeGamma = 1.;
	} else {
		hapkeGamma = sqrt(1 - opticalProperties.hapkeParameterW);
	}
	albedo = (1. - hapkeGamma) / (1. + hapkeGamma); //Suppression du spéculaire + opticalProperties.albedoSpecular;
	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(opticalProperties.directTransmittance, listDirections)
			+ opticalProperties.diffuseTransmittance) * 0.5;
}

void readLiDAR(){
	PhaseDAO::readLiDARXML();
}

void testHapke2Albedo(	const unsigned int& modelIndex,
						PropertyHapke2 & opticalProperties) {
	if (!opticalProperties.specularData.isSpecular) {
		if (opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance > 1.000001) {
			cerr << " HAPKE Warning : hapke W + DiffuseTransmittance = " << opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance
					<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
		}
	} else {
		if (!opticalProperties.specularData._useRefraction) {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		} else {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_down_to_up > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance + albedo_refraction_down_to_up = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_down_to_up
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_up_to_down > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance + albedo_refraction_up_to_down = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_up_to_down
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		}
	}
	if (opticalProperties.hapkeParameterW < 0) {
		cerr << " HAPKE Warning : hapke W < 0 !!! (hapke phase function : " << modelIndex << " ) -> fixed to 0" << endl;
		opticalProperties.hapkeParameterW = 0;
	}
}

void computeOpticalPropertiesHapke2(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & hapke2PropertyParameters,
									const vector<DirectionCartesienne> & listDirections,
									PropertyHapke2 & hapke2Property) {
	shared_ptr<Hapke2DatabaseParameters> hapke2DatabaseParameters = ::static_pointer_cast<Hapke2DatabaseParameters>(
		hapke2PropertyParameters.getDatabaseParameters(HAPKE2_DATABASE));
	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
		hapke2PropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<TransmittanceDatabaseParameters> hapkeTransmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
		hapke2PropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));
	shared_ptr<SpecularDatabaseParameters> kapkeSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
		hapke2PropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
	unsigned int modelIndex = hapke2PropertyParameters.getModelIndex();


	if (kapkeSpecularParameters.get() != NULL) {
		hapke2Property.specularData.isSpecular = true;
		computeSpecular(hapke2Property.specularData, PhaseProperty::getMultiplicativeFactorSpecularIntensityForHapke2(modelIndex, spectralBandIndex),
			spectralBandIndex, kapkeSpecularParameters);
	} else {
		hapke2Property.specularData.isSpecular = false;
	}

	hapke2Property.directTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDirectTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDirectTransmittanceFactorForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.diffuseTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDiffuseTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDiffuseTransmittanceFactorForHapke2(modelIndex, spectralBandIndex);

	hapke2Property.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature();

	hapke2Property.hapkeParameterC = hapke2DatabaseParameters.get()->getHapke2CForBand(spectralBandIndex)
				* PhaseProperty::getMultiplicativeFactorCForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.hapkeParameterW = hapke2DatabaseParameters.get()->getHapke2WForBand(spectralBandIndex)
				* PhaseProperty::getMultiplicativeFactorWForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.hapkeParameterB = hapke2DatabaseParameters.get()->getHapke2BForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorBForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.hapkeParameterH = hapke2DatabaseParameters.get()->getHapke2HForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorHForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.hapkeParameterB0 = hapke2DatabaseParameters.get()->getHapke2B0ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorB0ForHapke2(modelIndex, spectralBandIndex);
	hapke2Property.hapkeParameterTh = hapke2DatabaseParameters.get()->getHapke2ThForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorThForHapke2(modelIndex, spectralBandIndex);
}

void writeTreatHapke2(	ofstream & outCoeffSpecProperties,
						PhaseKeyMappingGenerator & pkmg,
						PropertyHapke2 & hapke2Property,
						const string & modelName,
						const unsigned int & spectralBandIndex,
						const unsigned int & modelIndex,
						const double & albedo,
						const double & dTauMoy) {
	pkmg.setNumeroFonctionDePhase(modelIndex);
	pkmg.setBandeSpectrale(spectralBandIndex);
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
	outCoeffSpecProperties << pkmg.cleW() << hapke2Property.hapkeParameterW << endl;
	outCoeffSpecProperties << pkmg.cleCoeffTh() << hapke2Property.hapkeParameterTh << endl;
	outCoeffSpecProperties << pkmg.cleCoeffC() << hapke2Property.hapkeParameterC << endl;
	outCoeffSpecProperties << pkmg.cleCoeffB0() << hapke2Property.hapkeParameterB0 << endl;
	outCoeffSpecProperties << pkmg.cleCoeffB() << hapke2Property.hapkeParameterB << endl;
	outCoeffSpecProperties << pkmg.cleCoeffH() << hapke2Property.hapkeParameterH << endl;


	writeSpecular(hapke2Property.specularData, outCoeffSpecProperties, pkmg);

	outCoeffSpecProperties << pkmg.cleAlbedo() << albedo << endl;
	outCoeffSpecProperties << pkmg.cleTauDirect() << hapke2Property.directTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauDiffus() << hapke2Property.diffuseTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauMoy() << dTauMoy << endl;
	if (spectralBandIndex == 0) {
		outCoeffSpecProperties << pkmg.cleTauDirectPourTemperature() << hapke2Property.temperatureData.directTransmittanceTemperature << endl;
	}
}


/* -------------------------------------------------------------------------------------
 *                                          RPV
 */
void treatRPV(	OpticalPropertyParameters & rpvPropertyParameters,
				unsigned int opIndex,
				vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const vector<DirectionCartesienne> & listDirections) {
	double dTauMoy = 0;
	double theta_max = 90;
	vector<double>r0(genDTO.numberOfSpectralBands),k(genDTO.numberOfSpectralBands),g(genDTO.numberOfSpectralBands),
			rc(genDTO.numberOfSpectralBands),diffuseTransmittance(genDTO.numberOfSpectralBands);


	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		PropertyRpv rpvProperty;

		computeOpticalPropertiesRPV(spectralBandIndex, rpvPropertyParameters, listDirections, rpvProperty, dTauMoy);
		testRPVAlbedo(rpvPropertyParameters.getModelIndex(), rpvProperty);

		PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile("MultispectralOpticalFunctions.RPVMultiFunctions.RPVMulti[",
			rpvPropertyParameters.getModelIndex(), -1.0, -1.0, -1.0);

		if (genDTO.accelerationEngine == 2) {
			r0[spectralBandIndex] = rpvProperty.reflectance;
			k[spectralBandIndex] = rpvProperty.rpvParameterK;
			g[spectralBandIndex] = rpvProperty.rpvParameterG;
			rc[spectralBandIndex] = rpvProperty.rpvParameterH;
			diffuseTransmittance[spectralBandIndex] = rpvProperty.diffuseTransmittance;
			theta_max = rpvProperty.theta_max;
		}
		writeRPV(pkmg, outCoeffSpecProperties, rpvProperty, rpvPropertyParameters.getModelName(), dTauMoy, rpvPropertyParameters.getModelIndex(),
			spectralBandIndex);
	}

	if (genDTO.accelerationEngine == 2) {
//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "RPV" <<  rpvPropertyParameters.getModelIndex() /*<< "_" << tmpIndex*/ << ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "RPV");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(rpvPropertyParameters.getModelIndex()));

			// Reflectance
			std::ostringstream valueR0,valueK,valueG,valueRc, valueKt, valueThetaMax, emissivity;
			valueR0 << setprecision(15);
			valueK << setprecision(15);
			valueG << setprecision(15);
			valueRc << setprecision(15); // named H in DART-FT while it name Rc in RAMI definition
			valueKt << setprecision(15);
			valueThetaMax << setprecision(15);
			emissivity << setprecision(15);
			valueR0 << r0[0]; valueK<<k[0]; valueG<<g[0]; valueRc<<rc[0]; valueKt<<((diffuseTransmittance[0]<=1e-8) ? 0.0 : diffuseTransmittance[0]/(1 - r0[0])); valueThetaMax << theta_max;
			emissivity << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueR0 << " " << r0[spectralBandIndex]; valueK << " " << k[spectralBandIndex];
				valueG << " " << g[spectralBandIndex]; valueRc << " " << rc[spectralBandIndex];
				valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - r0[spectralBandIndex]));
				valueThetaMax << " " << theta_max;
				emissivity << " " << 1.; // Force emissivity to 1, the real emissivity is computed in dart
			}

			if (rpvPropertyParameters.useDirectionalDirectTransmittance()) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(rpvPropertyParameters._angularStep));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", rpvPropertyParameters._ddtFilePath);
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueR0 << " " << r0[spectralBandIndex]; valueK << " " << k[spectralBandIndex];
						valueG << " " << g[spectralBandIndex]; valueRc << " " << rc[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - r0[spectralBandIndex]));
						valueThetaMax << " " << theta_max;
						emissivity << " " << 1.;
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueR0 << " " << r0[spectralBandIndex]; valueK << " " << k[spectralBandIndex];
						valueG << " " << g[spectralBandIndex]; valueRc << " " << rc[spectralBandIndex];
						valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - r0[spectralBandIndex]));
						valueThetaMax << " " << theta_max;
						emissivity << " " << 1.;
					}
				}
			}

			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "r0", valueR0.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "k", valueK.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "g", valueG.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "rc", valueRc.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "theta_max", valueThetaMax.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(rpvPropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "l" + boost::lexical_cast<std::string>(rpvPropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

				std::string textureKeyRoot = "scene.textures." + textureName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
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
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
				}
			}
//		}
	}
}

/*
 * 															RPV
 */

void computeOpticalPropertiesRPV(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & rpvPropertyParameters,
									const vector<DirectionCartesienne> & listDirections,
									PropertyRpv & rpvProperty,
									double & dTauMoy) {

	shared_ptr<RpvDatabaseParameters> rpvDatabaseParameters = ::static_pointer_cast<RpvDatabaseParameters>(
		rpvPropertyParameters.getDatabaseParameters(RPV_DATABASE));
	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
		rpvPropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<TransmittanceDatabaseParameters> rpvTransmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
		rpvPropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));
	shared_ptr<SpecularDatabaseParameters> rpvSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
		rpvPropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
	unsigned int modelIndex = rpvPropertyParameters.getModelIndex();

	rpvProperty.reflectance = 0.01 * rpvDatabaseParameters.get()->getReflectanceRho0ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorRho0ForRPV(modelIndex, spectralBandIndex);

	if (rpvSpecularParameters.get() != NULL) {
		rpvProperty.specularData.isSpecular = true;
		computeSpecular(rpvProperty.specularData, PhaseProperty::getMultiplicativeFactorSpecularIntensityForRPV(modelIndex, spectralBandIndex),
			spectralBandIndex, rpvSpecularParameters);
	} else {
		rpvProperty.specularData.isSpecular = false;
	}

	rpvProperty.rpvParameterK = 0.01 * rpvDatabaseParameters.get()->getRpvKForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorKForRPV(modelIndex, spectralBandIndex);
	rpvProperty.rpvParameterG = 0.01 * rpvDatabaseParameters.get()->getRpvGForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorGForRPV(modelIndex, spectralBandIndex);
	rpvProperty.rpvParameterH = 0.01 * rpvDatabaseParameters.get()->getRpvHForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorHForRPV(modelIndex, spectralBandIndex);

	rpvProperty.directTransmittance = 0.01 * rpvTransmittanceParameters.get()->getDirectTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDirectTransmittanceForRPV(modelIndex, spectralBandIndex);
	rpvProperty.diffuseTransmittance = 0.01 * rpvTransmittanceParameters.get()->getDiffuseTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDiffuseTransmittanceForRPV(modelIndex, spectralBandIndex);

	rpvProperty.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature();

	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(rpvProperty.directTransmittance, listDirections) + rpvProperty.diffuseTransmittance)
			* 0.5;
	PhaseDAO::readCoeffDiffParametersForRPVThetaMax(rpvPropertyParameters.getModelIndex(), rpvProperty.theta_max);
}

void testRPVAlbedo(	const unsigned int& modelIndex,
					PropertyRpv & opticalProperties) {

	if (!opticalProperties.specularData.isSpecular) {
		if (opticalProperties.reflectance + opticalProperties.diffuseTransmittance > 1.000001) {
			cerr << " RPV Warning : Reflectance + DiffuseTransmittance > 1 !!! (RPV phase function : " << modelIndex << " )" << endl;
		}
	} else {
		if (!opticalProperties.specularData._useRefraction) {
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " RPV Warning : Rho0 + albedo_down_to_up + DiffuseTransmittance = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " RPV Warning : Rho0 + albedo_up_to_down + DiffuseTransmittance = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		} else {
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_down_to_up > 1.000001) {
				cerr << " RPV Warning : Rho0 + albedo_down_to_up + DiffuseTransmittance + albedo_refraction_down_to_up = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_down_to_up
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_up_to_down > 1.000001) {
				cerr << " RPV Warning : Rho0 + albedo_up_to_down + DiffuseTransmittance + albedo_refraction_up_to_down = "
						<< opticalProperties.reflectance + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_up_to_down
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		}
	}
	if (opticalProperties.reflectance < 0) {
		cerr << " RPV Warning : Rho0 < 0 !!! (RPV phase function : " << modelIndex << " ) -> fixed to 0" << endl;
		opticalProperties.reflectance = 0;
	}
}

void writeRPV(	PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PropertyRpv & rpvProperty,
				const string & modelName,
				const double & dTauMoy,
				const unsigned int& modelIndex,
				const unsigned int& spectralBandIndex) {
	pkmg.setTypeFonctionDePhase(globalReferences::RPV_PHASE);
	pkmg.setBandeSpectrale(spectralBandIndex);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
	outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
	outCoeffSpecProperties << pkmg.cleRho0_RPV() << rpvProperty.reflectance << endl;
	outCoeffSpecProperties << pkmg.cleK_RPV() << rpvProperty.rpvParameterK << endl;
	outCoeffSpecProperties << pkmg.cleG_RPV() << rpvProperty.rpvParameterG << endl;
	outCoeffSpecProperties << pkmg.cleH_RPV() << rpvProperty.rpvParameterH << endl;
	outCoeffSpecProperties << pkmg.cleThetaMax_RPV() << rpvProperty.theta_max << endl;
	outCoeffSpecProperties << pkmg.cleTauDirect() << rpvProperty.directTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauDiffus() << rpvProperty.diffuseTransmittance << endl;
	outCoeffSpecProperties << pkmg.cleTauMoy() << dTauMoy << endl;

	writeSpecular(rpvProperty.specularData, outCoeffSpecProperties, pkmg);

	if (spectralBandIndex == 0) {
		outCoeffSpecProperties << pkmg.cleTauDirectPourTemperature() << rpvProperty.temperatureData.directTransmittanceTemperature << endl;
	}
}

void treatMixedMaterial(PhaseGeneralParametersDTO & genDTO,	unsigned int opIndex, unsigned int pfIndex) {
	if (genDTO.accelerationEngine == 2) {
		// LuxCore
		unsigned int model1Type, model1Index, model2Type, model2Index;
		std::string phaseName, model1Name, model2Name;
		double percentOfModel1;
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForMixedMaterial(pfIndex, phaseName, model1Type, model1Index, model1Name, model2Type, model2Index, model2Name, percentOfModel1), "");

		std::string material1Name = getMaterialName(model1Type, model1Index);
		std::string material2Name = getMaterialName(model2Type, model2Index);

//		std::vector<TemperatureFunction> temperatureFunctions;
//		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
		std::string mixedMaterialRootKey = "scene.materials.mixedMat" + boost::lexical_cast<std::string>(pfIndex) /*+ "_" + boost::lexical_cast<std::string>(tmpIndex)*/ + ".";

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
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(mixedMaterialRootKey + "temperatureIndex", std::to_string(tmpIndex));
		}
//	}
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

void treatFoliar(	OpticalPropertyParameters & foliarPropertyParameters,
					vector<int> & useSpectralIrradianceForReflectance,
					PhaseKeyMappingGenerator & pkmg,
					ofstream & outCoeffSpecProperties,
					PhaseGeneralParametersDTO & genDTO,
					const vector<DirectionCartesienne> & listDirections,
					FTFoliar & fTFoliar,
					PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
					PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
					const SphereDiscretizationMap & sphereMap,
					FoliarComputation & foliarComputer,
					LADComputation & ladComputation,
					FoliarModel & foliarModel) {

	foliarComputer.reInitFoliarComputation();

	vector<FoliarProperty> foliarProperty;
	foliarProperty.resize(genDTO.numberOfSpectralBands);

	shared_ptr<SpecularFoliarDatabaseParameters> foliarSpecularDatabaseParameters = ::static_pointer_cast<SpecularFoliarDatabaseParameters>(
		foliarPropertyParameters.getDatabaseParameters(SPECULAR_FOLIAR_DATABASE));
	if (foliarSpecularDatabaseParameters.get() != NULL) {
		for (unsigned int numBand = 0; numBand < genDTO.numberOfSpectralBands; ++numBand) {
			foliarProperty[numBand].specularFoliarData._isSpecular = true;
			computeFoliarSpecular(foliarProperty[numBand].specularFoliarData, numBand, foliarSpecularDatabaseParameters);
		}
	} else {
		for (unsigned int numBand = 0; numBand < genDTO.numberOfSpectralBands; ++numBand) {
			foliarProperty[numBand].specularFoliarData._isSpecular = false;
		}
	}

	unsigned int modelIndex = foliarPropertyParameters.getModelIndex();
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		foliarProperty[spectralBandIndex].typeOfLeafAngleDistribution = foliarModel.getFoliarTypeOfLeafAngleDistribution(modelIndex);
		foliarProperty[spectralBandIndex].foliarDimension = foliarModel.getFoliarDimension(modelIndex);
	}
	fTFoliar.casPDF = ladComputation.initialisationLadProbabilityDensityFunctions(modelIndex);
	ostringstream val;
	val << modelIndex; //on ajoute le numero de la fx de phase dans le nom du fichier : sprintf (val, "%u.",j)
	string name = val.str() + "." + foliarPropertyParameters.getModelName();

//	PhaseUtils::setOutputFolder("lib_phase"); // Je suis un porc.
	createDirectory(DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder());

	pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	outCoeffSpecProperties << pkmg.cleThermalHotSpotFactor() << foliarModel.getHotSpotFactor(modelIndex) << endl;
	FluorescenceResults topFluorescenceResults;
	topFluorescenceResults.useBioCLimaticProfileEta = false;
	topFluorescenceResults.isFluorescent = false;
	FluorescenceResults bottomFluorescenceResults;
	bottomFluorescenceResults.useBioCLimaticProfileEta = false;
	bottomFluorescenceResults.isFluorescent = false;
	foliarComputer.setTopFluorescenceComputation(false);
	foliarComputer.setBottomFluorescenceComputation(false);
	FluorescenceParameters fluorescenceParameters;
	bool hasDifferentBottom = false;
	PhaseDAO::readCoeffDiffParametersForFoliarFluspect(modelIndex, fluorescenceParameters, hasDifferentBottom, false);
	if (PhaseProperty::getFluorescence()) {
		foliarComputer.resetFrontFaceFluorescenceMatrix(genDTO.numberOfSpectralBands);
		if (fluorescenceParameters.isFluorescent) {
			computeFluorescenceMatrices(topFluorescenceResults, foliarComputer.getFrontFaceFluorescenceMatrix(), globalReferences::FOLIAIRE, modelIndex,
				fluorescenceParameters, genDTO, 0);
			foliarComputer.setTopFluorescenceComputation(true);
			if (topFluorescenceResults.useBioCLimaticProfileEta) {
				outCoeffSpecProperties << pkmg.cleTopFluorescenceBioClimaticUsage() << "1" << endl;
			} else {
				outCoeffSpecProperties << pkmg.cleTopFluorescenceBioClimaticUsage() << "0" << endl;
			}
		}
		foliarComputer.resetBackFaceFluorescenceMatrix(genDTO.numberOfSpectralBands);
		if (hasDifferentBottom) {
			PhaseDAO::readCoeffDiffParametersForFoliarFluspect(modelIndex, fluorescenceParameters, hasDifferentBottom, true);
			if (fluorescenceParameters.isFluorescent) {
				computeFluorescenceMatrices(bottomFluorescenceResults, foliarComputer.getBackFaceFluorescenceMatrix(), globalReferences::FOLIAIRE, modelIndex,
					fluorescenceParameters, genDTO, 1);
				foliarComputer.setBottomFluorescenceComputation(true);
				if (bottomFluorescenceResults.useBioCLimaticProfileEta) {
					outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << "1" << endl;
				} else {
					outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << "0" << endl;
				}
			}
		} else {
			if (foliarComputer.isTopFluorescenceComputation()) {
				bottomFluorescenceResults = topFluorescenceResults;
				foliarComputer.getBackFaceFluorescenceMatrix() = foliarComputer.getFrontFaceFluorescenceMatrix();
				foliarComputer.setBottomFluorescenceComputation(true);
				if (topFluorescenceResults.useBioCLimaticProfileEta) {
					outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << "1" << endl;
				} else {
					outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << "0" << endl;
				}
			}
		}
		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		if (foliarComputer.isTopFluorescenceComputation() || foliarComputer.isBottomFluorescenceComputation()) {
			outCoeffSpecProperties << pkmg.cleUseFluorescence() << "1" << endl;
		} else {
			outCoeffSpecProperties << pkmg.cleUseFluorescence() << "0" << endl;
		}
		if (foliarComputer.isTopFluorescenceComputation())
		{
			if (topFluorescenceResults.useBioCLimaticProfileEta) {
				string topBCFilePath = DartEnv::getFilePathForRead(INPUT_DIR, topFluorescenceResults.bioClimaticFileRelativePath);
				vector<double> topBioClimaticThetaValues, topBioClimaticPhiValues;
				vector<vector<vector<double> > > topBioClimaticProfileEta;
				readBioClimaticFile(topBCFilePath, topBioClimaticThetaValues, topBioClimaticPhiValues, topBioClimaticProfileEta);
				string topAverageBioClimaticProfileFileName = "top_average_eta_" + std::to_string(modelIndex) + ".txt";
				string topAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + topAverageBioClimaticProfileFileName;
				ofstream ofstreamTopAverageEta;
				ofstreamTopAverageEta.open(topAverageBioClimaticProfileFilePath.c_str(), ofstream::app);
				computeAndWriteAverageBioClimaticProfile(modelIndex, foliarComputer, topBioClimaticThetaValues, topBioClimaticPhiValues, topBioClimaticProfileEta, ofstreamTopAverageEta);
				outCoeffSpecProperties << pkmg.cleTopFluorescenceBioClimaticFile() << topAverageBioClimaticProfileFileName << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceTopBioClimaticLinkedPropertyType() << topFluorescenceResults.linkedOpticalPropertyType << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceTopBioClimaticLinkedPropertyIndex() << topFluorescenceResults.linkedOpticalPropertyIndex << endl;
			}
			if (bottomFluorescenceResults.useBioCLimaticProfileEta) {
				string bottomBCFilePath = DartEnv::getFilePathForRead(INPUT_DIR, bottomFluorescenceResults.bioClimaticFileRelativePath);
				vector<double> bottomBioClimaticThetaValues, bottomBioClimaticPhiValues;
				vector<vector<vector<double> > > bottomBioClimaticProfileEta;
				readBioClimaticFile(bottomBCFilePath, bottomBioClimaticThetaValues, bottomBioClimaticPhiValues, bottomBioClimaticProfileEta);
				string bottomAverageBioClimaticProfileFileName = "bottom_average_eta_" + std::to_string(modelIndex) + ".txt";
				string bottomAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + bottomAverageBioClimaticProfileFileName;
				ofstream ofstreamBottomAverageEta;
				ofstreamBottomAverageEta.open(bottomAverageBioClimaticProfileFilePath.c_str(), ofstream::app);
				computeAndWriteAverageBioClimaticProfile(modelIndex, foliarComputer, bottomBioClimaticThetaValues, bottomBioClimaticPhiValues, bottomBioClimaticProfileEta, ofstreamBottomAverageEta);
				outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticFile() << bottomAverageBioClimaticProfileFileName << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyType() << bottomFluorescenceResults.linkedOpticalPropertyType << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyIndex() << bottomFluorescenceResults.linkedOpticalPropertyIndex << endl;
			}
		}
	}

	if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		foliarModel.setEquivalentFrontLambertianIndex(modelIndex, PhaseProperty::getNewEquivalentLambertianModelIndex());
		if (!hasDifferentBottom) {
			foliarModel.setEquivalentBackLambertianIndex(modelIndex, foliarModel.getEquivalentFrontLambertianIndex(modelIndex));
		} else {
			foliarModel.setEquivalentBackLambertianIndex(modelIndex, PhaseProperty::getNewEquivalentLambertianModelIndex());
		}

		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		outCoeffSpecProperties << pkmg.cleEquivalentFrontLambertianIndex() << (foliarModel.getEquivalentFrontLambertianIndex(modelIndex)) << endl;
		outCoeffSpecProperties << pkmg.cleEquivalentBackLambertianIndex() << (foliarModel.getEquivalentBackLambertianIndex(modelIndex)) << endl;
		pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
		pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentFrontLambertianIndex(modelIndex));
		writeFluorescenceToSimulationProperties(topFluorescenceResults, pkmg, outCoeffSpecProperties);
		if (foliarModel.getEquivalentBackLambertianIndex(modelIndex) != foliarModel.getEquivalentFrontLambertianIndex(modelIndex)) {
			pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentBackLambertianIndex(modelIndex));
			writeFluorescenceToSimulationProperties(bottomFluorescenceResults, pkmg, outCoeffSpecProperties);
		}
	}

	string pathPhaseSpec = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_SpecularFunction";
	string pathPhaseDiff = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_ScatteringFunction";
	string pathPhaseTransm = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
			+ "_TransmittanceFunction";
	string pathPhaseHotSpotFactor = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
			+ "_ThermalHotSpot";
	string pathPhaseFluorescence = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_fluorescence";

	foliarComputer.computeFRFoliarSpecularForModelIndex(pointsListSpec, pointsListDiff, listDirections, fTFoliar, modelIndex, foliarProperty[0]);
	foliarComputer.computeClumpingIndex(listDirections, modelIndex);
	foliarComputer.runComputeFoliarPerThread(foliarProperty, modelIndex, fTFoliar, foliarPropertyParameters, listDirections, sphereMap,
		pointsListSpec, pointsListDiff);
	foliarComputer.computeFoliarHotSpotDirectionalFactor(modelIndex, listDirections, fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta);

	/*
	 * Nous avons deux cas : soit on ecrase le fichier car on est au premier passage de boucle,
	 * soit on continue le remplissage du fichier
	 */

	if (genDTO.accelerationEngine == 2) {
		std::ostringstream keyRootScatObj;
		keyRootScatObj << "scene.vegscatter." << "vegetation_" <<  modelIndex << ".";
		if (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "type", "homogeneous_vegetation_fluo");
		} else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "type", "homogeneous_vegetation");
		}
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "opIndex", std::to_string(modelIndex));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "leafdimension", std::to_string(foliarModel.getFoliarDimension(modelIndex)));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "LAD", std::to_string(foliarModel.getFoliarTypeOfLeafAngleDistribution(modelIndex)));
		if (foliarModel.getFoliarTypeOfLeafAngleDistribution(modelIndex) == globalReferences::ELLIPS)
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "ala", std::to_string(foliarModel.getFoliarAverageLeafAngle(modelIndex)));

		foliarComputer.writeTransmittance(fTFoliar, listDirections, genDTO.radiativeTransferMethod, genDTO.accelerationEngine, foliarProperty[0].specularFoliarData._isSpecular,
			pathPhaseTransm, pathPhaseDiff, pathPhaseSpec, pathPhaseHotSpotFactor, pathPhaseFluorescence);
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "transferFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_ScatteringFunction" + "_mc");
		if (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "fluorescenceTransferFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_fluorescence" + "_mc");
		}
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "transmFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_TransmittanceFunction");
//			string simulationPropertiesPath = DartEnv::getCurrentSimulationOutputPath() + "simulation.properties.txt";
		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.simPropertiesPath", "simulation.properties.txt");
		if (PhaseProperty::getFluorescence() && fluorescenceParameters.isFluorescent) {
			if (topFluorescenceResults.useBioCLimaticProfileEta) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topUseBioCLimaticProfileEta", "1");
				const string topAverageBioClimaticProfileFileName = "top_average_eta_" + std::to_string(modelIndex) + ".txt";
//					const string topAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + topAverageBioClimaticProfileFileName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topBioClimaticFilePath", PhaseUtils::getOutputFolder() + pathSeparator + topAverageBioClimaticProfileFileName);
			} else {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topUseBioCLimaticProfileEta", "0");
			}
			if (bottomFluorescenceResults.useBioCLimaticProfileEta) {
				const string bottomAverageBioClimaticProfileFileName = "bottom_average_eta_" + std::to_string(modelIndex) + ".txt";
//					const string bottomAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + bottomAverageBioClimaticProfileFileName;
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomUseBioCLimaticProfileEta", "1");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomBioClimaticFilePath", PhaseUtils::getOutputFolder() + pathSeparator + bottomAverageBioClimaticProfileFileName);
			} else {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomUseBioCLimaticProfileEta", "0");
			}
		}

		OpticalFactorParameters opticalFactorParameters;
		if (PhaseDAO::readCoeffDiffParametersForFoliarOpticalFactors(foliarPropertyParameters.getModelIndex(), opticalFactorParameters)) {
			std::string textureName = "v" + boost::lexical_cast<std::string>(foliarPropertyParameters.getModelIndex()) + "OF";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "opticalFactors", textureName);

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
	foliarComputer.writeTransmittance(fTFoliar, listDirections, genDTO.radiativeTransferMethod, genDTO.accelerationEngine, foliarProperty[0].specularFoliarData._isSpecular,
		pathPhaseTransm, pathPhaseDiff, pathPhaseSpec, pathPhaseHotSpotFactor, pathPhaseFluorescence);

	PropertyLambertian tempLP;
	unsigned int numberOfFresnelCoefficient = 20;
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		foliarComputer.testAlbedo(spectralBandIndex, modelIndex);
		foliarComputer.writeFoliar(modelIndex, spectralBandIndex, foliarPropertyParameters.getModelName(), pkmg, outCoeffSpecProperties,
			foliarProperty);
		if (PhaseProperty::getPolarization()) {
			PhasePolarization::computeAndWriteFoliarPolarizationMatrix(genDTO, foliarPropertyParameters, spectralBandIndex, listDirections,
				numberOfFresnelCoefficient, foliarModel, ladComputation);
		}
		if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
			tempLP.reflectance = foliarProperty[spectralBandIndex].reflectanceTop;
			tempLP.directTransmittance = 0.;
			tempLP.diffuseTransmittance = foliarProperty[spectralBandIndex].transmittance;
			tempLP.standardDeviation = 0.;
			tempLP.temperatureData.directTransmittanceTemperature = foliarProperty[spectralBandIndex].temperatureData.transmittanceTemperature;
			tempLP.specularData.isSpecular = false;
			tempLP.specularData._useRefraction = false;
			tempLP.diffuseTransmittanceAccelerationFactor = foliarProperty[spectralBandIndex].diffuseTransmittanceAccelerationFactor;
			writeLambertian(foliarModel.getEquivalentFrontLambertianIndex(modelIndex), foliarPropertyParameters.getModelName(), spectralBandIndex,
				pkmg, outCoeffSpecProperties, tempLP, foliarProperty[spectralBandIndex].transmittance, foliarComputer.getAlbedo(spectralBandIndex),
				listDirections);

			if (foliarModel.getEquivalentBackLambertianIndex(modelIndex) != foliarModel.getEquivalentFrontLambertianIndex(modelIndex)) {
				tempLP.reflectance = foliarProperty[spectralBandIndex].reflectanceBottom;
				tempLP.diffuseTransmittance = foliarProperty[spectralBandIndex].bottomTransmittance;
				tempLP.diffuseTransmittanceAccelerationFactor = foliarProperty[spectralBandIndex].bottomDiffuseTransmittanceAccelerationFactor;
				writeLambertian(foliarModel.getEquivalentBackLambertianIndex(modelIndex), foliarPropertyParameters.getModelName(),
					spectralBandIndex, pkmg, outCoeffSpecProperties, tempLP, foliarProperty[spectralBandIndex].transmittance,
					foliarComputer.getAlbedo(spectralBandIndex), listDirections);
			}
		}
	}

	if (genDTO.accelerationEngine == 2 && genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		std::vector<TemperatureFunction> temperatureFunctions;
		PhaseDAO::readTemperatureProperties(temperatureFunctions);
//		for (unsigned int tmpIndex = 0; tmpIndex < temperatureFunctions.size(); tmpIndex++) {
			// Top leave property
			std::ostringstream keyRoot;
			keyRoot << "scene.materials." << "lambertian" << foliarModel.getEquivalentFrontLambertianIndex(modelIndex) /*<< "_" << tmpIndex*/ << ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "mattetranslucent");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(foliarModel.getEquivalentFrontLambertianIndex(modelIndex)));
			std::ostringstream valueKr, valueKt, emissivity;
			valueKr << setprecision(15);
			valueKt << setprecision(15);
			emissivity << setprecision(15);
			valueKr << foliarProperty[0].reflectanceTop;
			if(abs(1 - foliarProperty[0].reflectanceTop - foliarProperty[0].transmittance)<1e-15)
				emissivity << 0.0;
			else
				emissivity << 1 - foliarProperty[0].reflectanceTop - foliarProperty[0].transmittance;
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueKr << " " << foliarProperty[spectralBandIndex].reflectanceTop;
				if(abs(1 - foliarProperty[spectralBandIndex].reflectanceTop - foliarProperty[spectralBandIndex].transmittance)<1e-15)
					emissivity <<" "<< 0.0;
				else
					emissivity <<" "<< 1 - foliarProperty[spectralBandIndex].reflectanceTop - foliarProperty[spectralBandIndex].transmittance;
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueKr << " " << foliarProperty[spectralBandIndex].reflectanceTop;
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						valueKr << " " << foliarProperty[spectralBandIndex].reflectanceTop;
					}
				}
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kr", valueKr.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());
			if (foliarProperty[0].reflectanceTop == 1) {
				valueKt << "0" << endl;
			} else {
				valueKt << (foliarProperty[0].transmittance / (1. - foliarProperty[0].reflectanceTop));
			}
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				if (foliarProperty[spectralBandIndex].reflectanceTop == 1) {
					valueKt << " 0" << endl;
				} else {
					valueKt << " " << (foliarProperty[spectralBandIndex].transmittance / (1. - foliarProperty[spectralBandIndex].reflectanceTop));
				}
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						if (foliarProperty[spectralBandIndex].reflectanceTop == 1) {
							valueKt << " 0" << endl;
						} else {
							valueKt << " " << (foliarProperty[spectralBandIndex].transmittance / (1. - foliarProperty[spectralBandIndex].reflectanceTop));
						}
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						if (foliarProperty[spectralBandIndex].reflectanceTop == 1) {
							valueKt << " 0" << endl;
						} else {
							valueKt << " " << (foliarProperty[spectralBandIndex].transmittance / (1. - foliarProperty[spectralBandIndex].reflectanceTop));
						}
					}
				}
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());

			if (PhaseProperty::getFluorescence() && topFluorescenceResults.isFluorescent) {
				if (topFluorescenceResults.useBioCLimaticProfileEta) {
					string bcFilePath = DartEnv::getFilePathForRead(INPUT_DIR, topFluorescenceResults.bioClimaticFileRelativePath);
					vector<double> bioClimaticThetaValues, bioClimaticPhiValues;
					vector<vector<vector<double> > > bioClimaticProfileEta;
					readBioClimaticFile(bcFilePath, bioClimaticThetaValues, bioClimaticPhiValues, bioClimaticProfileEta);
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "fluorescent");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "fluorescencefiles", "fluorescence_5_" + std::to_string(modelIndex) + "_0");
				if (topFluorescenceResults.useBioCLimaticProfileEta) {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "1");
//					string bioClimaticFileRelativePath = DartEnv::getFilePath(INPUT_DIR, topFluorescenceResults.bioClimaticFileRelativePath);
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "bioClimaticFilePath", topFluorescenceResults.bioClimaticFileRelativePath);
				} else {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "0");
				}
			}

			OpticalFactorParameters opticalFactorParameters;
			if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(foliarPropertyParameters.getModelIndex(), opticalFactorParameters)) {
				std::string textureName = "v" + boost::lexical_cast<std::string>(foliarPropertyParameters.getModelIndex()) + "OF";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);
			}

			if (foliarModel.getEquivalentBackLambertianIndex(modelIndex) != foliarModel.getEquivalentFrontLambertianIndex(modelIndex)) {
				// Bottom leave property
				keyRoot.str("");
				keyRoot << "scene.materials." << "lambertian" << foliarModel.getEquivalentBackLambertianIndex(modelIndex) /*<< "_" << tmpIndex*/ << ".";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "mattetranslucent");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
//				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(foliarModel.getEquivalentBackLambertianIndex(modelIndex)));
				valueKr.str("");
				emissivity.str("");
				valueKr << foliarProperty[0].reflectanceBottom;
				if(abs(1 - foliarProperty[0].reflectanceBottom - foliarProperty[0].bottomTransmittance)<1e-15)
					emissivity << 0.0;
				else
					emissivity << 1 - foliarProperty[0].reflectanceBottom - foliarProperty[0].bottomTransmittance;
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					valueKr << " " << foliarProperty[spectralBandIndex].reflectanceBottom;
					if(abs(1 - foliarProperty[spectralBandIndex].reflectanceBottom - foliarProperty[spectralBandIndex].bottomTransmittance)<1e-15)
						emissivity <<" "<< 0.0;
					else
						emissivity <<" "<< 1 - foliarProperty[spectralBandIndex].reflectanceBottom - foliarProperty[spectralBandIndex].bottomTransmittance;
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							valueKr << " " << foliarProperty[spectralBandIndex].reflectanceBottom;
						}
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							valueKr << " " << foliarProperty[spectralBandIndex].reflectanceBottom;
						}
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kr", valueKr.str());
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());
				valueKt.str("");
				if (foliarProperty[0].reflectanceBottom == 1) {
					valueKt << "0" << endl;
				} else {
					valueKt << (foliarProperty[0].bottomTransmittance / (1. - foliarProperty[0].reflectanceBottom));
				}
				for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
					if (foliarProperty[spectralBandIndex].reflectanceBottom == 1) {
						valueKt << " 0" << endl;
					} else {
						valueKt << " " << (foliarProperty[spectralBandIndex].bottomTransmittance / (1. - foliarProperty[spectralBandIndex].reflectanceBottom));
					}
				}

				if (PhaseProperty::getPolarization()) {
					// 3 * numberOfSpectralBands are added to simulate Q, U, V
					// The optical properties are repeated in these bands
					for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							if (foliarProperty[spectralBandIndex].reflectanceBottom == 1) {
								valueKt << " 0" << endl;
							} else {
								valueKt << " " << (foliarProperty[spectralBandIndex].bottomTransmittance / (1. - foliarProperty[spectralBandIndex].reflectanceBottom));
							}
						}
					}
				}

				if (PhaseProperty::getFluorescence()) {
					// 2 * numberOfSpectralBands are added to simulate PSI and PSII
					// The optical properties are repeated in these bands
					for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
							if (foliarProperty[spectralBandIndex].reflectanceBottom == 1) {
								valueKt << " 0" << endl;
							} else {
								valueKt << " " << (foliarProperty[spectralBandIndex].bottomTransmittance / (1. - foliarProperty[spectralBandIndex].reflectanceBottom));
							}
						}
					}
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());

				if (PhaseProperty::getFluorescence() && bottomFluorescenceResults.isFluorescent) {
					if (hasDifferentBottom) {
						if (bottomFluorescenceResults.useBioCLimaticProfileEta) {
							string bcFilePath = DartEnv::getFilePathForRead(INPUT_DIR, bottomFluorescenceResults.bioClimaticFileRelativePath);
							vector<double> bioClimaticThetaValues, bioClimaticPhiValues;
							vector<vector<vector<double> > > bioClimaticProfileEta;
							readBioClimaticFile(bcFilePath, bioClimaticThetaValues, bioClimaticPhiValues, bioClimaticProfileEta);
						}
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "fluorescent");
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "fluorescencefiles", "fluorescence_5_" + std::to_string(modelIndex) + "_1");
					} else {
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "fluorescent");
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "fluorescencefiles", "fluorescence_5_" + std::to_string(modelIndex) + "_0");
					}
					if (bottomFluorescenceResults.useBioCLimaticProfileEta) {
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "1");
	//					string bioClimaticFileRelativePath = DartEnv::getFilePath(INPUT_DIR, bottomFluorescenceResults.bioClimaticFileRelativePath);
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "bioClimaticFilePath", bottomFluorescenceResults.bioClimaticFileRelativePath);
					} else {
						genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "useBioCLimaticProfileEta", "0");
					}
				}

				OpticalFactorParameters opticalFactorParameters;
				if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(foliarPropertyParameters.getModelIndex(), opticalFactorParameters)) {
					std::string textureName = "v" + boost::lexical_cast<std::string>(foliarPropertyParameters.getModelIndex()) + "OF";
					genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);
				}
			}
		}
//	}
}

void readBioClimaticFile(	string bcFilePath, vector<double> & bioClimaticThetaValues,
							vector<double> & bioClimaticPhiValues,
							vector<vector<vector<double> > > & bioClimaticProfileEta) {
	bioClimaticThetaValues.clear();
	bioClimaticPhiValues.clear();
	bool binary = boost::algorithm::ends_with(bcFilePath, ".bin");
	ifstream bcStream;
	if (binary) {
		bcStream.open(bcFilePath.c_str(), ifstream::in | ifstream::binary);
	} else {
		bcStream.open(bcFilePath.c_str(), ifstream::in);
	}
	if (bcStream) {
		unsigned int nbLayer, nbTheta, nbPhi;
		if (binary) {
			bcStream.read((char*) &nbLayer, sizeof(unsigned int));
			bcStream.read((char*) &nbTheta, sizeof(unsigned int));
			bcStream.read((char*) &nbPhi, sizeof(unsigned int));
		} else {
			bcStream >> nbLayer >> nbTheta >> nbPhi;
		}
		if (nbTheta != 1) {
			// Recover the median value of each range
			bioClimaticThetaValues.resize(nbTheta);
			for (unsigned int i = 0; i < nbTheta; ++i) {
				if (binary) {
					bcStream.read((char*) &bioClimaticThetaValues[i], sizeof(double));
				} else {
					bcStream >> bioClimaticThetaValues[i];
				}
			}
		}
		if (nbPhi != 1) {
			// Recover the median value of each range
			bioClimaticPhiValues.resize(nbPhi);
			for (unsigned int i = 0; i < nbPhi; ++i) {
				if (binary) {
					bcStream.read((char*) &bioClimaticPhiValues[i], sizeof(double));
				} else {
					bcStream >> bioClimaticPhiValues[i];
				}
			}
		}
		bioClimaticProfileEta.clear();
		bioClimaticProfileEta.resize(nbLayer, std::vector<std::vector<double> >(nbTheta, std::vector<double>(nbPhi)));
		for (unsigned int l = 0; l < nbLayer; ++l) {
			for (unsigned int t = 0; t < nbTheta; ++t) {
				for (unsigned int p = 0; p < nbPhi; ++p) {
					if (binary) {
						bcStream.read((char*) &bioClimaticProfileEta[l][t][p], sizeof(double));
					} else {
						bcStream >> bioClimaticProfileEta[l][t][p];
					}
				}
			}
		}
		bcStream.close();
	}
}

void computeAndWriteAverageBioClimaticProfile(	const unsigned int & modelIndex,
												const FoliarComputation & foliarComputer,
												const vector<double> & centralBioClimaticThetaValues,
												const vector<double> & centralBioClimaticPhiValues,
												const vector<vector<vector<double> > > & bioClimaticProfileEta,
												ofstream & ofstreamAverageEta) {
	vector<double> averageBioClimaticProfileEta;
	unsigned int nbLayers = bioClimaticProfileEta.size(), nbTheta = centralBioClimaticThetaValues.size(), nbPhi = centralBioClimaticPhiValues.size();
	nbTheta = max(1, nbTheta);
	nbPhi = max(1, nbPhi);
	if (nbTheta == 1 && nbPhi == 1) {
		// nbTheta == 1 && nbPhi == 1 => The biochemical profile eta is aleady an average
		averageBioClimaticProfileEta.resize(nbLayers);
		for (unsigned int i = 0; i < nbLayers; i++) {
			averageBioClimaticProfileEta[i] = bioClimaticProfileEta[i][0][0];
		}
	}
	else {
		// We compute an average profile of bioclimatic eta per layer
		averageBioClimaticProfileEta.resize(nbLayers);
		vector<double> thetaValues, phiValues;
		thetaValues.resize(nbTheta + 1);
		phiValues.resize(nbPhi + 1);
		thetaValues[0] = 0;
		thetaValues[1] = 2 * centralBioClimaticThetaValues[0];
		phiValues[0] = 0;
		phiValues[1] = 2 * centralBioClimaticPhiValues[0];
		for (unsigned int i = 2; i < thetaValues.size(); i++) {
			thetaValues[i] = 2 * centralBioClimaticThetaValues[i - 1] - thetaValues[i - 1];
		}
		for (unsigned int i = 2; i < phiValues.size(); i++) {
			phiValues[i] = 2 * centralBioClimaticPhiValues[i - 1] - phiValues[i - 1];
		}
		vector<double> dThetaValues, dPhiValues;
		dThetaValues.resize(nbTheta);
		dPhiValues.resize(nbPhi);
		for (unsigned int i = 0; i < dThetaValues.size(); i++) {
			dThetaValues[i] = thetaValues[i+1] - thetaValues[i];
		}
		for (unsigned int i = 0; i < dPhiValues.size(); i++) {
			dPhiValues[i] = phiValues[i+1] - phiValues[i];
		}
		for (unsigned int layer = 0; layer < nbLayers; layer++) {
			double average = 0.0;
			for (unsigned int phiIndex = 0; phiIndex < nbPhi; phiIndex++) {
				double thetaIntegral = 0.0;
				for (unsigned int thetaIndex = 0; thetaIndex < nbTheta; thetaIndex++) {
					thetaIntegral += bioClimaticProfileEta[layer][thetaIndex][phiIndex] *
							foliarComputer.getLeafAngleDistributionForAngle(modelIndex, centralBioClimaticThetaValues[thetaIndex]) *
							dThetaValues[thetaIndex];
				}
				average += thetaIntegral * dPhiValues[phiIndex];
			}
			averageBioClimaticProfileEta[layer] = average / 360;
		}
	}
	if (ofstreamAverageEta) {
		ofstreamAverageEta << nbLayers << " 1 1" << std::endl;
		for (unsigned int layer = 0; layer < nbLayers; layer++){
			ofstreamAverageEta << averageBioClimaticProfileEta[layer] << std::endl;
		}
	}
	ofstreamAverageEta.close();

}

void computeFoliarSpecular(	SpecularFoliarData & specularProperty,
							const unsigned int & spectralBandIndex,
							shared_ptr<SpecularFoliarDatabaseParameters> & foliarSpecularDatabaseParameters) {

	if (specularProperty._isSpecular) {
		specularProperty.refractionIndexBottom = foliarSpecularDatabaseParameters->getBottomRefractionIndexForBand(spectralBandIndex);
		specularProperty.refractionIndexTop = foliarSpecularDatabaseParameters->getTopRefractionIndexForBand(spectralBandIndex);
		specularProperty.roughnessBottom = foliarSpecularDatabaseParameters->getBottomForBand(spectralBandIndex);
		specularProperty.roughnessTop = foliarSpecularDatabaseParameters->getTopForBand(spectralBandIndex);
		specularProperty.roughnessCoefficientBottom = foliarSpecularDatabaseParameters->getBottomIndexForBand(spectralBandIndex);
		specularProperty.roughnessCoefficientTop = foliarSpecularDatabaseParameters->getTopIndexForBand(spectralBandIndex);
	}
}

/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
void treatFluid(PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				vector<AirPhaseFunction> & airPhaseFunctions,
				PhaseGeneralParametersDTO & genDTO,
				const vector<DirectionCartesienne> & listDirections) {
	pkmg.setTypeFonctionDePhase(globalReferences::AIR_FP);
	outCoeffSpecProperties << pkmg.cleNombreFonctionDePhase() << airPhaseFunctions.size() << endl;
	if (!airPhaseFunctions.empty()) {
		cout << " - Processing of Air Phase Functions" << endl;
		if (genDTO.accelerationEngine == 2) {
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

		} else {
			for(AirPhaseFunction & apf : airPhaseFunctions) {
				apf.computeScatteringFunction(genDTO.lowerBandWavelength, genDTO.upperBandWavelength, listDirections,
					PhaseProperty::getNumberOfAngularSectors() + 1);
				apf.writeData(pkmg, outCoeffSpecProperties);
				if (PhaseProperty::getPolarization()) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
						PhasePolarization::computeAndWritePolarizationMatrixFluid(apf, listDirections, spectralBandIndex);
					}
				}
			}
		}
		cout << " - End of Processing of Air Phase Functions" << endl;
	}
}

void treatWater(OpticalPropertyParameters & waterPropertyParameters,
				unsigned int opIndex,
				vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO) {
	pkmg.setTypeFonctionDePhase(globalReferences::WATER_INTERFACE);
	pkmg.setNumeroFonctionDePhase(waterPropertyParameters.getModelIndex());
	outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl; // Oui

	// On check le type de la première DB pour en déduire la définition des WC
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
							int& dayOfTheYear,
							const vector<DirectionCartesienne> & listDirections) {
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
	genDTO.computationMethodTOAtoBOA = atmosphereParameters.computationMethodTOAtoBOA; // transfer this parameter for dart lux atmos

	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && atmosphereParameters.fullyPrecomputed) {
		cerr << "Error: LiDAR can not be simulated with precomputed atmospheric radiative transfer files." << endl;
		cerr << "       Atmosphere simulation has been disabled" << endl;
		atmosphereParameters.computationMethodTOAtoBOA = 0;
	}

	if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.computationMethodTOAtoBOA == 2 && genDTO.accelerationEngine == 2) {
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
					if (genDTO.accelerationEngine == 2 && genDTO.atmosphereRTmode == 1)
					{
						// Disable atmosphere phase function computation
					} else {
						PhaseProperty::atmospherePhaseFunction(listDirections, atmosphereParameters.minimumNumberOfDivisions);
						PhaseProperty::writeAtmospherePhaseFunction(listDirections);
						if (PhaseProperty::getPolarization()) {
							for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
								/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
								/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
								PhasePolarization::computeAndWritePolarizationMatrixAtmosphere(listDirections, spectralBandIndex);
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
			executeAtmosphereNcWithoutDatabase(atmosphereParameters, boaMoleculePhaseFunction, boaAerosolPhaseFunction[0], listDirections);

			if (!atmosphereParameters.fullyPrecomputed
					&& ((PhaseProperty::getComputationMethodeOfToaToBoa() == 2) || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer)) {
				if (genDTO.accelerationEngine == 2 && genDTO.atmosphereRTmode == 1)
				{
					// Disable atmosphere phase function computation
				} else {
					PhaseProperty::atmospherePhaseFunction(listDirections, atmosphereParameters.minimumNumberOfDivisions);
					PhaseProperty::writeAtmospherePhaseFunction(listDirections);
					if (PhaseProperty::getPolarization()) {
						for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
							/* At the moment, concerning the polarization, the fluids are treated like Rayleigh molecules */
							/* TODO Treat fluids "like fluids" by finding their scattering Mueller matrices (An idea: Light scattering reviews 10) */
							PhasePolarization::computeAndWritePolarizationMatrixAtmosphere(listDirections, spectralBandIndex);
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
	PhaseProperty::setSunZenithAngleInRadians(genDTO.sourceDirection.getCenterTheta());
	vector<DirectionCartesienne> listDirections;
	PhaseDAO::readAndStoreDirections(listDirections);
	PhaseProperty::initiateSpectralBands(genDTO.numberOfSpectralBands, genDTO.lowerBandWavelength, genDTO.upperBandWavelength, genDTO.numberOfThreads);
	if (genDTO.accelerationEngine == 2) {
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
	}

// Le nombre de directions (hors direction solaire)
	PhaseProperty::setNumberOfDirection(listDirections.size());

// Calcul de la somme des angle solides
// Recuperation du nombre total de secteurs angulaires
	double solidAnglesSum = 0.;
	double countNumberOfAngularSectors = numberOfAngularSectors(listDirections, solidAnglesSum);
	int iterationStepPhi = 10.0;
	int iterationStepTheta = 1.0;
	PhaseProperty::setNumberOfAngularSectors(countNumberOfAngularSectors);

// Ajout de la direction solaire
	genDTO.sourceDirection.setAngularSector(countNumberOfAngularSectors);
	listDirections.push_back(genDTO.sourceDirection);

	// Reading of secondary illumination sources
	LightsContainer secondarySourcesOfLight;
	PhaseDAO::readSecondarySourcesOfLight(secondarySourcesOfLight);

	/*
	 L'index de la direction solaire
	 nb_sector = Initialisations(nb_directions, solidAnglesSum, AS, ZS, fich_angle, Sector, nb_dir, tab_dir_vect);
	 Creation et ouverture du fichier phases.properties.txt (s'il existe deja, il est efface)
	 */
	string fileName = DartEnv::getFilePathForEdit(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
	FichierProperties phasePropert(fileName.c_str());
	outCoeffSpecProperties.open(fileName.c_str(), ofstream::app);

	/*
	 * Vector of cosine and sine used for computation of integrals
	 */

	if (outCoeffSpecProperties) {
		outCoeffSpecProperties.precision(15);

		std::vector<TemperatureFunction> temperatureFunctions;
		PhaseDAO::readTemperatureProperties(temperatureFunctions);
		writeTemperatureProperties(genDTO, outCoeffSpecProperties, temperatureFunctions);
		if (genDTO.accelerationEngine == 2) {
			writeTemperaturePropertiesLuxCore(genDTO, temperatureFunctions);
		}

		writeSecondarySourcesOfLight(genDTO, outCoeffSpecProperties, secondarySourcesOfLight); // TODO: Make it work with the atmosphere

		/* ATMOSPHERE */
		cout << endl << " - Atmosphere processing :" << endl;
		processingAtmosphere(genDTO, pkmg, outCoeffSpecProperties, airPhaseFunctions, genDTO.dayOfTheYear, listDirections);
		printTimeAtmosphere(tMiddle, tInit);

		/* READ DATA */
		outCoeffSpecProperties << pkmg.cleNombreBandesSpectrales() << genDTO.numberOfSpectralBands << endl;
		EXIT_IF(!PhaseDAO::readNumberOfOpticalPropertiesModels(), "");
		cleanAndCreateDirectory(airPhaseFunctions.empty());

		FoliarModel foliarModel;
		foliarModel.initFoliarModel(PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands,
			PhaseProperty::_numberOfDirections);

		FoliarComputation foliarComputer(foliarModel);
		foliarComputer.initFoliarComputation(PhaseProperty::_numberOfDirections, PhaseProperty::_numberOfAngularSectors,
			PhaseProperty::_numberOfSpectralBands, PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfThreads,
			PhaseProperty::_threadEnd, foliarModel);

		LADComputation ladComputation(foliarModel);
		ladComputation.initLADComputation(PhaseProperty::_reflectanceModelsParameters.numberOfFoliarModels, PhaseProperty::_numberOfSpectralBands);

		computeLadData(genDTO, ladComputation, foliarModel);


		PhaseProperty::setParametersForPrintingProgress();
		PhaseProperty::resizePhaseParametersVectors();

		readMultiplicativeFactor(iterationStepPhi, iterationStepTheta, foliarModel, genDTO);
		printNumberOfFoliarModels();

		foliarComputer.computeFoliarCosinePsi(listDirections, iterationStepPhi, iterationStepTheta);
		vector<int> useSpectralIrradianceForReflectance(genDTO.numberOfSpectralBands);
		testIfUseSun(genDTO, useSpectralIrradianceForReflectance);
		writePhasePropertiesSpectralIrradiance(outCoeffSpecProperties, pkmg, genDTO);

		/* FOLIAR precomputing */
		if (PhaseProperty::getNumberOfFoliarModels() > 0) {
			cout << "	Processing Vegetation models" << endl;
		}
		FTFoliar fTFoliar = foliarPrecomputing(genDTO, listDirections, iterationStepTheta, iterationStepPhi, solidAnglesSum);
		PointsListsNCIntegration<DirectionCartesienne> pointsListSpec;
		PointsListsNCIntegration<DirectionCartesienne> pointsListDiff;

		if (PhaseProperty::getFluorescence()) {
			PhaseProperty::initYields();
			if (PhaseProperty::getNumberOfFoliarModels() > 0) {
				foliarComputer.initFluorescenceMatrix();
			}
		}

		static SphereDiscretizationMap sphereMap(1.0, fTFoliar.iterationStepPhi);

		// Lecture des bases de données
		vector<OpticalPropertyParameters> opticalPropertiesDatabases = createDatabasesReadingProperties(genDTO.numberOfSpectralBands);
		vector<OpticalPropertyParameters>::iterator endIterator = opticalPropertiesDatabases.end();

		unsigned int currentOPIndex = 0;
		for (vector<OpticalPropertyParameters>::iterator iterator = opticalPropertiesDatabases.begin(); iterator < endIterator; ++iterator) {
			for (unsigned int numPropertyDatabase = 0; numPropertyDatabase < iterator->getNbDatabaseForProperty(); ++numPropertyDatabase) {

				if (iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_FILE
						&& iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_UP
						&& iterator->getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_DOWN) { // TU LE SENS LE WATER BIEN CHIANT ???

					shared_ptr<PhaseDatabaseParameters> currentDatabase = iterator->getDatabaseForProperty(numPropertyDatabase);
					PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, currentDatabase);
				}
			}
			PhaseExternComputation pec;
			switch (iterator->getOpticalPropertyType()) {
				case LAMBERTIAN_PROPERTY:
					treatLambertian(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					break;
				case SPECULAR_PROPERTY:
					treatSpecular(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					break;
				case HAPKE_PROPERTY:
					treatHapke(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					break;
				case HAPKE2_PROPERTY:
					treatHapke2(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					break;
				case RPV_PROPERTY:
					treatRPV(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					break;
				case WATER_PROPERTY:
					treatWater(*iterator, currentOPIndex, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO);
					break;
				case FOLIAR_PROPERTY:
					treatFoliar(*iterator, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties, genDTO, listDirections, fTFoliar,
						pointsListSpec, pointsListDiff, sphereMap, foliarComputer, ladComputation, foliarModel);
					break;
				case PHASE_EXTERN_PROPERTY:
					cout<<"phase extern property:" <<flush;
					pec.computePhaseExtern_MultiDirection(*iterator, pkmg, outCoeffSpecProperties, genDTO, listDirections);
					treatPhaseExtern(*iterator, useSpectralIrradianceForReflectance, pkmg, outCoeffSpecProperties,genDTO, listDirections);
					cout<<"...done"<<endl;
					break;
				default:
					break;
			}
			++currentOPIndex;
		}

		/* Mixed Material (LuxCore Only) */
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfMixedMaterialModels(); modelIndex++) {
			treatMixedMaterial(genDTO, currentOPIndex, modelIndex);
			++currentOPIndex;
		}

		/* FLUID */
		treatFluid(pkmg, outCoeffSpecProperties, airPhaseFunctions, genDTO, listDirections);

		cout << " -> Writing LuxCoreRender configuration files" << endl;

		if (genDTO.accelerationEngine == 2) {
			genDTO.luxCoreRenderSceneProperties.writeFile(DartEnv::getCurrentSimulationOutputPath() + "phase.scn");
			genDTO.luxCoreRenderConfigProperties.writeFile(DartEnv::getCurrentSimulationOutputPath() + "scene.cfg");
			genDTO.luxCoreLightProperties.writeFile(DartEnv::getCurrentSimulationOutputPath() + "lights.properties");
		}

		writePhasePropertiesNumberOfModels(outCoeffSpecProperties, pkmg, genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation);

		/* Fermeture du fichier phase.properties.txt */
		outCoeffSpecProperties.close();

		// Databases self destruct procedure
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

	} else {
		cerr << "Impossible de creer le fichier phase.properties.txt" << endl;
		exit(99);
	}
	time(&tEnd);
	int processingTime = tEnd - tInit;
	PhaseDAO::closePhase();
	cout << " - End of phase function generation." << endl;
	cout << "  -> Processing Time : " << processingTime / 3600 << " hours " << (processingTime % 3600) / 60 << " minutes " << processingTime % 60
			<< "  seconds" << endl;

}

/**
 * TODO: Il y a une dépendance cyclique PhaseDAO <-> PhaseProperty
 */
void computeLadData(const PhaseGeneralParametersDTO & genDTO,
					LADComputation & ladComputation,
					FoliarModel & foliarModel) {
	double ladMParameter = 0;
	double labB0Parameter = 0;
	double ladB1Parameter = 0;
	double ladB2Parameter = 0;
	ofstream writeOutputLeafAngleDistribution;
	ofstream writeOutputElliptical;
	string ladFileName, nameDatabaseLAD = "";
	for (unsigned int foliarModelIndice = 0; foliarModelIndice < PhaseProperty::getNumberOfFoliarModels(); foliarModelIndice++) {
		ladMParameter = 0;
		labB0Parameter = 0;
		ladB1Parameter = 0;
		ladB2Parameter = 0;
		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForFoliar(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter,
				nameDatabaseLAD, foliarModel), "");
		switch (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice)) {
			case globalReferences::ELLIPS:
				if (foliarModel.getFoliarAverageLeafAngle(foliarModelIndice) >= 1e-10) {
					ladComputation.computeEllipsoidalLadDistribution(foliarModelIndice, foliarModel.getFoliarAverageLeafAngle(foliarModelIndice));
				} else {
					foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelIndice, globalReferences::HORIZ);
					ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 0);
				}
				break;
			case globalReferences::BOUNDEDUNI:
				ladComputation.computeBoundedUniformLadDistribution(foliarModelIndice, foliarModel.getNumberOfAla(foliarModelIndice),
					foliarModel.getNthOfAla(foliarModelIndice));
				break;

			case globalReferences::ELLIPT:
				ladComputation.computeEllipticalLadDistribution(foliarModelIndice, foliarModel.getFoliarEllipsePrincipalAngle(foliarModelIndice),
					foliarModel.getFoliarEllipseEccentricity(foliarModelIndice));
				ladComputation.writeEllipticalLad(foliarModelIndice, writeOutputElliptical);
				break;
			case globalReferences::HORIZ:
				foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelIndice, globalReferences::HORIZ);
				ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 0);
				break;
			case globalReferences::VERTI:
				ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 90);
				break;
			case globalReferences::MANUAL: // De Wit
				ladComputation.computeClassicalLadDistribution(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter);
				break;
			case globalReferences::USERDEFINED:
				ladFileName = DartEnv::getDatabasePath(nameDatabaseLAD) + nameDatabaseLAD;
				ladComputation.readLadFile(ladFileName, foliarModelIndice);
				break;
			default:
				ladComputation.getLadFile(foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice), foliarModelIndice);
				break;
		}
		if (genDTO.isAskedForWritingLadResultsCheck == 1) {
			ladComputation.writeLad(foliarModelIndice, writeOutputLeafAngleDistribution,
				foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice));
		}
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
	writeAtmosphereNcWithDatabase(atmosphereParameters, atmospherePerBandMeanValues, henleyGreensteinMeanParameters);
}

void executeAtmosphereNcWithoutDatabase(	AtmosphereParameters& atmosphereParameters,
											AirPhaseFunction & moleculeAFP,
											AirPhaseFunction & aerosolAFP,
											const vector<DirectionCartesienne> & listDirections) {
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

vector<OpticalPropertyParameters> createDatabasesReadingProperties(const unsigned int & numberOfBands) {
	vector<OpticalPropertyParameters> phaseOpticalProperties;

	double deviation = 0, smc = 0, delta = 0;
	string phaseName = "", modelName = "", niModelName = "", ntModelName = "", databaseName = "",databasePath="", temp = "";
	string specularModelName = "", specularDatabase = "";
	string transmittanceModelName = "", transmittanceDatabaseName = "";
	bool isSpecular = false, useRefraction = false, hasDifferentModelForBottom = false, keepOutputFile = false, useProspectGen = false;
	int specularRadianceDistribution = 0, lambertianDefinition = 0, indexOrder=-1;

	/* PHASE EXTERN */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfPhaseExternModels(); modelIndex++) {
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForPhaseExtern(modelIndex, phaseName, modelName, databaseName), "");
		OpticalPropertyParameters opticalProperty(PHASE_EXTERN_PROPERTY, modelIndex, phaseName, indexOrder);
		shared_ptr<PhaseExternDatabaseParameters> phaseExternDatabaseParameters(
			new PhaseExternDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(phaseExternDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Prospect preparation for lambertian + foliar */
	map<unsigned int, std::string> mapPrepareProspect;
	map<unsigned int, std::string> mapPrepareProspectFoliarTop; // include foliar mono model
	map<unsigned int, std::string> mapPrepareProspectFoliarBottom;
	ostringstream prospectPreparationFile;
	boost::uuids::uuid filePreparationUuid = boost::uuids::random_generator()();
	prospectPreparationFile << DartEnv::getOutputTempDatabasePath(true) << boost::lexical_cast<std::string>(filePreparationUuid) + ".csv";
	ProspectComputation prospect(prospectPreparationFile.str());
	// Prospect detection & preparation for lambertian phase functions ...
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfLambertianModels(); modelIndex++) {
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForIsProspectUseLambertian(modelIndex,  useProspectGen, lambertianDefinition), "");
		if (lambertianDefinition == 0 && useProspectGen)
		{
			ProspectParameters prospectParameters;
			PhaseDAO::readCoeffDiffParametersForProspect(modelIndex, globalReferences::PROSPECT_LAMBERTIEN, prospectParameters);
			// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
			boost::uuids::uuid databaseUuid = boost::uuids::random_generator()();
			databaseName = boost::lexical_cast<std::string>(databaseUuid) + ".db";
			// Prepare prospect for later run
			prospect.prepareProspect(databaseName, prospectParameters);
			// Save the path to future generated database
			mapPrepareProspect[modelIndex] = databaseName;
		}
	}
	// Prospect detection & preparation for foliar phase functions ...
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfFoliarModels(); modelIndex++) {
		bool bottomProspectGen = false;
		bool hasDifferentModelForBottom = false;
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForIsProspectUseFoliar(modelIndex,  useProspectGen, bottomProspectGen, hasDifferentModelForBottom), "");
		if (useProspectGen)
		{
			ProspectParameters prospectParameters;
			if (hasDifferentModelForBottom) {
				PhaseDAO::readCoeffDiffParametersForProspect(modelIndex, globalReferences::PROSPECT_FOLIAIRE_TOP, prospectParameters);
			} else {
				PhaseDAO::readCoeffDiffParametersForProspect(modelIndex, globalReferences::PROSPECT_FOLIAIRE, prospectParameters);
			}
			// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
			boost::uuids::uuid databaseUuid = boost::uuids::random_generator()();
			databaseName = boost::lexical_cast<std::string>(databaseUuid) + ".db";
			// Prepare prospect for later run
			prospect.prepareProspect(databaseName, prospectParameters);
			// Save the path to future generated database
			mapPrepareProspectFoliarTop[modelIndex] = databaseName;
		}

		if (bottomProspectGen) {
			ProspectParameters prospectParameters;
			PhaseDAO::readCoeffDiffParametersForProspect(modelIndex, globalReferences::PROSPECT_FOLIAIRE_BOTTOM, prospectParameters);

			// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
			boost::uuids::uuid databaseUuid = boost::uuids::random_generator()();
			databaseName = boost::lexical_cast<std::string>(databaseUuid) + ".db";
			// Prepare prospect for later run
			prospect.prepareProspect(databaseName, prospectParameters);
			// Save the path to future generated database
			mapPrepareProspectFoliarBottom[modelIndex] = databaseName;
		}
	}
	// RUN ALL PROSPECTS
	if (mapPrepareProspect.size() > 0 || mapPrepareProspectFoliarTop.size() > 0 || mapPrepareProspectFoliarBottom.size() > 0)
	{
		cout << "    -> Compute "<< (mapPrepareProspect.size() + mapPrepareProspectFoliarTop.size() + mapPrepareProspectFoliarBottom.size()) << " prospect(s) functions ..."<< endl;
		// There is a prerared prospect
		prospect.runAllProspects();
	}

	/* Lambertian */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfLambertianModels(); modelIndex++) {
		EXIT_IF(!PhaseDAO::readCoeffDiffParametersForLambertian(modelIndex, phaseName, modelName, databaseName, deviation, isSpecular, useProspectGen, lambertianDefinition, smc, delta, keepOutputFile), "");
		OpticalPropertyParameters opticalProperty(LAMBERTIAN_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForLambertian(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		if (lambertianDefinition == 0) {
			if (isSpecular) {
				EXIT_IF(
					!PhaseDAO::readCoeffDiffParametersForSpecularLambertian(modelIndex, specularModelName, specularDatabase, useRefraction,
						specularRadianceDistribution), "");
				shared_ptr<SpecularDatabaseParameters> specularDatabaseParameters(
					new SpecularDatabaseParameters(specularDatabase, DartEnv::getDatabasePath(specularDatabase), specularModelName, numberOfBands));
				specularDatabaseParameters.get()->setUseRefraction(useRefraction);
				specularDatabaseParameters.get()->setRadianceDistribution(specularRadianceDistribution);
				opticalProperty.addDatabaseToProperty(specularDatabaseParameters);
				opticalProperty.setSpecular(true);
			}
			if (useProspectGen) {
				databaseName = mapPrepareProspect[modelIndex];
				databasePath = DartEnv::getOutputTempDatabasePath(true);
				// Maybe change the model name in the future, but this modelName are not findable by the user.
				// Spaces and special caracters are not allowed in modelName so ...
				modelName = "Model";
				// And affect them for the standard phase procedure
				shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
					new PhaseTemperatureDatabaseParameters(databaseName, databasePath, modelName, 1));
				temperatureDatabaseParameters.get()->setSelfDestructDatabase(true);
				opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);

				shared_ptr<LambertianDatabaseParameters> lambertianDatabaseParameters(
					new LambertianDatabaseParameters(databaseName, databasePath, modelName, numberOfBands));
				lambertianDatabaseParameters.get()->setDeviation(deviation);
				lambertianDatabaseParameters.get()->setSelfDestructDatabase(true);
				opticalProperty.addDatabaseToProperty(lambertianDatabaseParameters);
				phaseOpticalProperties.push_back(opticalProperty);
			} else {
				shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
					new PhaseTemperatureDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, 1));
				opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);

				shared_ptr<LambertianDatabaseParameters> lambertianDatabaseParameters(
					new LambertianDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
				lambertianDatabaseParameters.get()->setDeviation(deviation);
				opticalProperty.addDatabaseToProperty(lambertianDatabaseParameters);
				phaseOpticalProperties.push_back(opticalProperty);
			}
		} else if (lambertianDefinition == 1) {
			// Generate an unique name for the database name (Unique for multithreading / will be deleted after)
			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			databaseName = boost::lexical_cast<std::string>(uuid) + ".db";

			// run marmit, generate the database
			MarmitComputation marmit(smc, delta, modelName, databaseName);
			marmit.runMarmitDatabaseGeneration();
			if (keepOutputFile) {
				marmit.runMarmitOutputFileGeneration();
			}
			// Maybe change the model name in the future, but this modelName are not findable by the user.
			// Spaces and special caracters are not allowed in modelName so ...
			modelName = "Model";
			// And affect them for the standard phase procedure
			shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
				new PhaseTemperatureDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, 1));
			temperatureDatabaseParameters.get()->setSelfDestructDatabase(true);
			opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);

			shared_ptr<LambertianDatabaseParameters> lambertianDatabaseParameters(
				new LambertianDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
			lambertianDatabaseParameters.get()->setDeviation(deviation);
			lambertianDatabaseParameters.get()->setSelfDestructDatabase(true);
			opticalProperty.addDatabaseToProperty(lambertianDatabaseParameters);
			phaseOpticalProperties.push_back(opticalProperty);
		}
	}

	/* Specular */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfSpecularModels(); modelIndex++) {

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForSpecular(modelIndex, phaseName, databaseName, niModelName, ntModelName), "");
		OpticalPropertyParameters opticalProperty(SPECULAR_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForSpecular(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		shared_ptr<RefractiveIndexDatabaseParameters> refractiveIndexDatabaseParameters(
			new RefractiveIndexDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), niModelName, numberOfBands, true,
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

	/* Hapke */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfHapkeModels(); modelIndex++) {
		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForHapke(modelIndex, phaseName, modelName, databaseName, transmittanceModelName,
				transmittanceDatabaseName, isSpecular, indexOrder), "");
		OpticalPropertyParameters opticalProperty(HAPKE_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForHapke(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		if (isSpecular) {
			EXIT_IF(
				!PhaseDAO::readCoeffDiffParametersForHapkeSpecular(modelIndex, specularModelName, specularDatabase, useRefraction,
					specularRadianceDistribution, indexOrder), "");
			shared_ptr<SpecularDatabaseParameters> specularDatabaseParameters(
				new SpecularDatabaseParameters(specularDatabase, DartEnv::getDatabasePath(specularDatabase), specularModelName, numberOfBands));
			specularDatabaseParameters.get()->setUseRefraction(useRefraction);
			specularDatabaseParameters.get()->setRadianceDistribution(specularRadianceDistribution);
			opticalProperty.addDatabaseToProperty(specularDatabaseParameters);
			opticalProperty.setSpecular(true);
		}

		shared_ptr<HapkeDatabaseParameters> hapkeDatabaseParameters(
			new HapkeDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(hapkeDatabaseParameters);

		shared_ptr<TransmittanceDatabaseParameters> transmittanceDatabaseParameters(
			new TransmittanceDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);

		shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
			new PhaseTemperatureDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, 1));
		opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Hapke2 */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfHapke2Models(); modelIndex++) {

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForHapke2(modelIndex, phaseName, modelName, databaseName, transmittanceModelName,
				transmittanceDatabaseName, isSpecular, indexOrder), "");
		OpticalPropertyParameters opticalProperty(HAPKE2_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForHapke2(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		if (isSpecular) {
			EXIT_IF(
				!PhaseDAO::readCoeffDiffParametersForHapke2Specular(modelIndex, specularModelName, specularDatabase, useRefraction,
					specularRadianceDistribution, indexOrder), "");
			shared_ptr<SpecularDatabaseParameters> specularDatabaseParameters(
				new SpecularDatabaseParameters(specularDatabase, DartEnv::getDatabasePath(specularDatabase), specularModelName, numberOfBands));
			specularDatabaseParameters.get()->setUseRefraction(useRefraction);
			specularDatabaseParameters.get()->setRadianceDistribution(specularRadianceDistribution);
			opticalProperty.addDatabaseToProperty(specularDatabaseParameters);
			opticalProperty.setSpecular(true);
		}

		shared_ptr<Hapke2DatabaseParameters> hapke2DatabaseParameters(
			new Hapke2DatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(hapke2DatabaseParameters);

		shared_ptr<TransmittanceDatabaseParameters> transmittanceDatabaseParameters(
			new TransmittanceDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);

		shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(new PhaseTemperatureDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName), transmittanceModelName, 1));
		opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* RPV */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfRPVModels(); modelIndex++) {
		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForRPV(modelIndex, phaseName, modelName, databaseName, isSpecular, transmittanceModelName,
				transmittanceDatabaseName), "");
		OpticalPropertyParameters opticalProperty(RPV_PROPERTY, modelIndex, phaseName, indexOrder);

		EXIT_IF(!PhaseDAO::readDirectionalDirectTransmittanceForRPV(modelIndex, opticalProperty._useDirectionalDirectTransmittance, opticalProperty._ddtFilePath, opticalProperty._angularStep), "");

		if (isSpecular) {
			EXIT_IF(
				!PhaseDAO::readCoeffDiffParametersForRPVSpecular(modelIndex, specularModelName, specularDatabase, useRefraction,
					specularRadianceDistribution), "");
			shared_ptr<SpecularDatabaseParameters> specularDatabaseParameters(
				new SpecularDatabaseParameters(specularDatabase, DartEnv::getDatabasePath(specularDatabase), specularModelName, numberOfBands));
			specularDatabaseParameters.get()->setUseRefraction(useRefraction);
			specularDatabaseParameters.get()->setRadianceDistribution(specularRadianceDistribution);
			opticalProperty.addDatabaseToProperty(specularDatabaseParameters);
			opticalProperty.setSpecular(true);
		}

		shared_ptr<RpvDatabaseParameters> rpvDatabaseParameters(
			new RpvDatabaseParameters(databaseName, DartEnv::getDatabasePath(databaseName), modelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(rpvDatabaseParameters);

		shared_ptr<TransmittanceDatabaseParameters> transmittanceDatabaseParameters(
			new TransmittanceDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, numberOfBands));
		opticalProperty.addDatabaseToProperty(transmittanceDatabaseParameters);

		shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters(
			new PhaseTemperatureDatabaseParameters(transmittanceDatabaseName, DartEnv::getDatabasePath(transmittanceDatabaseName),
				transmittanceModelName, 1));
		opticalProperty.addDatabaseToProperty(temperatureDatabaseParameters);
		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Foliar*/
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfFoliarModels(); modelIndex++) {

		// Bottom foliar model
		string bottomDatabaseName;
		string bottomDatabasePath;
		string bottomModel;
		bool bottomProspectGen = false;

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForFoliarDatabases(modelIndex, phaseName, modelName, databaseName, isSpecular, useProspectGen,
				hasDifferentModelForBottom, specularDatabase, specularModelName, bottomDatabaseName, bottomModel, bottomProspectGen), "");

		if (useProspectGen) {
			databaseName = mapPrepareProspectFoliarTop[modelIndex];
			databasePath = DartEnv::getOutputTempDatabasePath(true);
			// Maybe change the model name in the future, but this modelName are not findable by the user.
			// Spaces and special caracters are not allowed in modelName so ...
			modelName = "Model";
		} else {
			databasePath = DartEnv::getDatabasePath(databaseName);
		}
		if (bottomProspectGen) {
			bottomDatabaseName = mapPrepareProspectFoliarBottom[modelIndex];
			bottomDatabasePath = DartEnv::getOutputTempDatabasePath(true);
			// Maybe change the model name in the future, but this modelName are not findable by the user.
			// Spaces and special caracters are not allowed in modelName so ...
			bottomModel = "Model";
		}else {
			bottomDatabasePath = DartEnv::getDatabasePath(bottomDatabaseName);
		}

		OpticalPropertyParameters opticalProperty(FOLIAR_PROPERTY, modelIndex, phaseName, indexOrder);
		shared_ptr<FoliarDatabaseParameters> foliarDatabaseParameters(
			new FoliarDatabaseParameters(databaseName, databasePath, modelName, numberOfBands, hasDifferentModelForBottom,
					bottomDatabaseName, bottomDatabasePath, bottomModel));
		if (useProspectGen) {
			foliarDatabaseParameters.get()->setSelfDestructDatabase(true);
		}
		if (bottomProspectGen) {
			foliarDatabaseParameters.get()->setSelfDestructBottomDatabase(true);
		}
		opticalProperty.addDatabaseToProperty(foliarDatabaseParameters);

		if (isSpecular) {
			shared_ptr<SpecularFoliarDatabaseParameters> specularFoliarDatabaseParameters(
				new SpecularFoliarDatabaseParameters(specularDatabase, DartEnv::getDatabasePath(specularDatabase), specularModelName, numberOfBands));
			opticalProperty.addDatabaseToProperty(specularFoliarDatabaseParameters);
			opticalProperty.setSpecular(true);
		}

		phaseOpticalProperties.push_back(opticalProperty);
	}

	/* Water (ça existe encore ?) */
	for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfWaterInterfaceModels(); modelIndex++) {
		unsigned int waterMode = 0;
		double refractionIndexUp, refractionIndexDown;

		EXIT_IF(!PhaseDAO::readCoeffDiffWaterMode(modelIndex, waterMode, phaseName), "");
		OpticalPropertyParameters opticalProperty(WATER_PROPERTY, modelIndex, phaseName, indexOrder);

		switch (waterMode) {
			case 0: // cas particulier casse couilles 1: user defined
			{
				shared_ptr<WaterNoDatabaseUserDefinedParameters> waterDatabaseUserDefinedUp(new WaterNoDatabaseUserDefinedParameters(numberOfBands));
				waterDatabaseUserDefinedUp->setRefractionOrientation(UP);
				shared_ptr<WaterNoDatabaseUserDefinedParameters> waterDatabaseUserDefinedDown(
					new WaterNoDatabaseUserDefinedParameters(numberOfBands));
				waterDatabaseUserDefinedDown->setRefractionOrientation(DOWN);

				for (unsigned int numBand = 0; numBand < numberOfBands; ++numBand) {
					PhaseDAO::readCoeffDiffParametersWaterUserDefinedForBand(modelIndex, numBand, refractionIndexUp, refractionIndexDown);
					waterDatabaseUserDefinedUp->setRefractionForBand(numBand, refractionIndexUp);
					waterDatabaseUserDefinedDown->setRefractionForBand(numBand, refractionIndexDown);
				}
				opticalProperty.addDatabaseToProperty(waterDatabaseUserDefinedUp);
				opticalProperty.addDatabaseToProperty(waterDatabaseUserDefinedDown);
				break;
			}
			case 1: // cas particulier casse couilles 2: file defined
			{
				string waterFile;
				PhaseDAO::readCoeffDiffParametersWaterFile(modelIndex, waterFile);
				shared_ptr<WaterNoDatabaseFileParameters> waterDatabaseParametersFile(new WaterNoDatabaseFileParameters(numberOfBands, waterFile));

				opticalProperty.addDatabaseToProperty(waterDatabaseParametersFile);
				break;
			}
			case 2: // cas normal
			{
				string databaseNameUp, databaseNameDown, databaseModelUp, databaseModelDown;
				PhaseDAO::readCoeffDiffParametersWaterDatabases(modelIndex, databaseNameUp, databaseNameDown, databaseModelUp, databaseModelDown);

				shared_ptr<WaterDatabaseParameters> waterDatabaseParametersUp(
					new WaterDatabaseParameters(databaseNameUp, DartEnv::getDatabasePath(databaseNameUp), databaseModelUp, numberOfBands));
				waterDatabaseParametersUp.get()->setRefractionOrientation(UP);

				shared_ptr<WaterDatabaseParameters> waterDatabaseParametersDown(
					new WaterDatabaseParameters(databaseNameDown, DartEnv::getDatabasePath(databaseNameDown), databaseModelDown, numberOfBands));
				waterDatabaseParametersDown.get()->setRefractionOrientation(DOWN);

				opticalProperty.addDatabaseToProperty(waterDatabaseParametersUp);
				opticalProperty.addDatabaseToProperty(waterDatabaseParametersDown);

				break;
			}
			default:
				// cerr << Si tu modifie l'XML à la main, ne viens pas pleurer que ça ne marche plus ! << endl;
				break;
		}
		phaseOpticalProperties.push_back(opticalProperty);
	}
	return phaseOpticalProperties;
}

void writeSecondarySourcesOfLight(PhaseGeneralParametersDTO & genDTO, ofstream & outCoeffSpecProperties, const LightsContainer & lightsContainer) {
	if(genDTO.accelerationEngine == 2){ //luxcorerender
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
	} else {
		for (unsigned int lightIndex = 0; lightIndex < lightsContainer.getNbLights(); ++lightIndex) {
			const Light & currentLight = lightsContainer.getLight(lightIndex);
			std::string currentLightKeyRoot = "phase.lights.secondaryLight" + boost::lexical_cast<std::string>(lightIndex) + ".";
			outCoeffSpecProperties << currentLightKeyRoot << "type:" << currentLight.getClass() << endl;
			if (currentLight.getClass() == Light::UNIDIRECTIONAL) {
				outCoeffSpecProperties << currentLightKeyRoot << "theta:" << (currentLight.getDirection().getCenterTheta() * 180. / M_PI) << endl;
				outCoeffSpecProperties << currentLightKeyRoot << "phi:" << (currentLight.getDirection().getCenterPhi() * 180. / M_PI) << endl;
			} else {
				const LightSphere & currentLightSphere = static_cast<const LightSphere &>(currentLight);
				outCoeffSpecProperties << currentLightKeyRoot << "sampling:" << currentLightSphere.getSampling() << endl;
				outCoeffSpecProperties << currentLightKeyRoot << "radius:" << currentLightSphere.getRadius() << endl;
				outCoeffSpecProperties << currentLightKeyRoot << "position.x:" << currentLightSphere.getPosition().getX() << endl;
				outCoeffSpecProperties << currentLightKeyRoot << "position.y:" << currentLightSphere.getPosition().getY() << endl;
				outCoeffSpecProperties << currentLightKeyRoot << "position.z:" << currentLightSphere.getPosition().getZ() << endl;
			}
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				outCoeffSpecProperties << currentLightKeyRoot << "irradiance.BAND" << spectralBandIndex << ":" << currentLight.getIrradiance(spectralBandIndex) << endl;
			}
		}
	}
}

void writeTemperatureProperties(PhaseGeneralParametersDTO & genDTO, ofstream & outCoeffSpecProperties, const std::vector<TemperatureFunction> & temperatureFunctions) {
	outCoeffSpecProperties << "phase.nbTemperatureFunction:" << temperatureFunctions.size() << endl;
	for (unsigned int tfIndex = 0; tfIndex < temperatureFunctions.size(); ++tfIndex) {
		std::string currentTemperatureKeyRoot = "phase.temperatureFunction" + boost::lexical_cast<std::string>(tfIndex) + ".";
		outCoeffSpecProperties << currentTemperatureKeyRoot << "id:" << temperatureFunctions[tfIndex].getIdTemperature() << endl;
		outCoeffSpecProperties << currentTemperatureKeyRoot << "meanTemperature:" << temperatureFunctions[tfIndex].getMeanTemperature() << endl;
		outCoeffSpecProperties << currentTemperatureKeyRoot << "deltaTemperature:" << temperatureFunctions[tfIndex].getDeltaTemperature() << endl;
		outCoeffSpecProperties << currentTemperatureKeyRoot << "overrideGlobalTemperatureFile:" << temperatureFunctions[tfIndex].isOverrideTemperatureProfile() << endl;
		outCoeffSpecProperties << currentTemperatureKeyRoot << "singleSurfaceTemperature:" << temperatureFunctions[tfIndex].isSingleTemperatureSurface() << endl;
		outCoeffSpecProperties << currentTemperatureKeyRoot << "usePrecomputedIPARs:" << temperatureFunctions[tfIndex].isUsingPrecomputedIpaRs() << endl;
		if (temperatureFunctions[tfIndex].isUsingPrecomputedIpaRs()) {
			outCoeffSpecProperties << currentTemperatureKeyRoot << "minIPAR:" << temperatureFunctions[tfIndex].getMinIpar() << endl;
			outCoeffSpecProperties << currentTemperatureKeyRoot << "maxIPAR:" << temperatureFunctions[tfIndex].getMaxIpar() << endl;
		}
		outCoeffSpecProperties << currentTemperatureKeyRoot << "use3DOpticalFactors:" << temperatureFunctions[tfIndex].isUsingOpticalFactorMatrix() << endl;
		if (temperatureFunctions[tfIndex].isUsingOpticalFactorMatrix()) {
			outCoeffSpecProperties << currentTemperatureKeyRoot << "opticalFactorFileName:" << temperatureFunctions[tfIndex].getOpticalFactorMatrixFile() << endl;
			outCoeffSpecProperties << currentTemperatureKeyRoot << "duplicateFirstOpticalMatrixLayer:" << temperatureFunctions[tfIndex].isDuplicateFirstOpticalMatrixLayer() << endl;
		}
	}
}

void writeScopeRadianceFile(PhaseGeneralParametersDTO & genDTO, unsigned int & nbLayers, std::vector<float> & thetas, std::vector<float> & phis, const std::string & temperatureInFile, const std::string & outputFolder, const std::string & outputFileName) {
	std::ifstream influx(DartEnv::getFilePathForRead(INPUT_DIR, temperatureInFile).c_str(), std::ios::in);
	if (influx) {
		unsigned int nbTheta, nbPhi;
		influx >> nbLayers >> nbTheta >> nbPhi;
		if (nbTheta != 1) {
			// Recover the median value of each range
			thetas.resize(nbTheta);
			for (unsigned int i = 0; i < nbTheta; ++i) {
				influx >> thetas[i];
			}
		} else {
			thetas.push_back(180);
		}
		if (nbPhi != 1) {
			// Recover the median value of each range
			phis.resize(nbPhi);
			for (unsigned int i = 0; i < nbPhi; ++i) {
				influx >> phis[i];
			}
		} else {
			phis.push_back(360);
		}

		createDirectory(outputFolder);
		std::ofstream outflux((outputFolder + outputFileName).c_str(), std::ios::binary | std::ios::out);
		float currentTemperature, radiance;
		const double precision = 0.001;
		const unsigned int maxIter = 1000;
		for (unsigned int l = 0; l < nbLayers; ++l) {
			for (unsigned int t = 0; t < nbTheta; ++t) {
				for (unsigned int p = 0; p < nbPhi; ++p) {
					influx >> currentTemperature;
					for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
						radiance = genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
							currentTemperature,
							genDTO.emissionLaw[bandIndex],
							genDTO.lowerBandWavelength[bandIndex] * 1E-6,
							genDTO.upperBandWavelength[bandIndex] * 1E-6,
							precision,
							maxIter);
						outflux.write((const char *)&radiance, sizeof(float));
					}
				}
			}
		}
		outflux.close();
		influx.close();
	}
}

void writeRadianceFile(PhaseGeneralParametersDTO & genDTO, const std::string & temperatureInFile, const std::string & outputFolder, const std::string & outputFileName) {
	std::ifstream influx(DartEnv::getFilePathForRead(INPUT_DIR, temperatureInFile).c_str(), std::ios::in);
	if (!influx.fail() && !influx.eof()) {
		createDirectory(outputFolder);
		std::ofstream outflux((outputFolder + outputFileName).c_str(), std::ios::binary | std::ios::out);
		const double precision = 0.001;
		const unsigned int maxIter = 1000;
		double currentTemperature;
		float radiance;
		while (influx >> currentTemperature) {
			for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
				radiance = genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
					currentTemperature,
					genDTO.emissionLaw[bandIndex],
					genDTO.lowerBandWavelength[bandIndex] * 1E-6,
					genDTO.upperBandWavelength[bandIndex] * 1E-6,
					precision,
					maxIter);
				outflux.write((const char *)&radiance, sizeof(float));
			}
		}
		outflux.close();
		influx.close();
	}
}

std::string getOrCreateScopeTexture(PhaseGeneralParametersDTO & genDTO, std::unordered_map<std::string, std::string> & scopeFileToTextureMap, unsigned int & currentTextureIndex, const std::string & scopeFileName) {
	std::unordered_map<std::string, std::string>::const_iterator it = scopeFileToTextureMap.find(scopeFileName);
	if (it == scopeFileToTextureMap.end()) {
		std::string textureName = "scopeProfileTex" + boost::lexical_cast<std::string>(currentTextureIndex++);
		std::string textureKeyRoot = "scene.textures." + textureName;

		unsigned int nbLayers;
		std::vector<float> thetas, phis;

		std::string outputTextureFile = PhaseUtils::getOutputFolder() + pathSeparator + textureName + ".bin";
		writeScopeRadianceFile(genDTO, nbLayers, thetas, phis, scopeFileName, DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator, textureName + ".bin");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", outputTextureFile);

		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".nbLayers", boost::lexical_cast<std::string>(nbLayers));

		std::ostringstream valueStream;
		for (const auto value: thetas)
			valueStream << value << " ";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".thetas", valueStream.str());

		valueStream.str("");
		for (const auto value: phis)
			valueStream << value << " ";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".phis", valueStream.str());

		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "scopetemperature");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");

		scopeFileToTextureMap[scopeFileName] = textureName;
		return textureName;
	} else {
		return it->second;
	}
}

void writeTemperaturePropertiesLuxCore(PhaseGeneralParametersDTO & genDTO, const std::vector<TemperatureFunction> & temperatureFunctions) {
	if (genDTO.isThermalEmissionEnabled) {
		bool precomputationNeeded = false;

		std::unordered_map<std::string, std::string> temperatureToRadianceFile;
		std::unordered_map<std::string, std::string> scopeFileToTextureMap;
		unsigned int textureIndex = 0;

		string key = "scene.radiations.";

		// Add default temperature property
		string defaultEmissionName = "defaultEmission";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".type", "null");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".generator", "phase");

		double precision = 0.001;
		unsigned int maxIter = 1000;
		for (unsigned int i = 0; i < temperatureFunctions.size(); i++) {
			std::string emissionKey = key + "emisFunc" + std::to_string(i) + ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");

			if ((genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_REALTIME
					|| genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT
					|| temperatureFunctions[i].isOverrideTemperatureProfile())
						&& !temperatureFunctions[i].isUsingOpticalFactorMatrix()
						&& !temperatureFunctions[i].isUsingSunlitShadedTemperatures()
						&& !temperatureFunctions[i].isUsingScopeFiles()) {
				if (temperatureFunctions[i].getDeltaTemperature() == 0.0) {
					if (temperatureFunctions[i].getMeanTemperature() == 0) {
						// No emission
						genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
					} else {
						// Constant
						genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
						std::ostringstream emission;
						emission.str("");
						emission << (genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
									 temperatureFunctions[i].getMeanTemperature(),
									 genDTO.emissionLaw[0],
									 genDTO.lowerBandWavelength[0] * 1E-6,
									 genDTO.upperBandWavelength[0] * 1E-6,
									 precision,
									 maxIter));

						for (unsigned int bandIndex = 1; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
							// genDTO.lowerBandWavelength[bandIndex]: wavelength unit: micrometer
							emission << " " << (genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
												temperatureFunctions[i].getMeanTemperature(),
												genDTO.emissionLaw[bandIndex],
												genDTO.lowerBandWavelength[bandIndex] * 1E-6,
												genDTO.upperBandWavelength[bandIndex] * 1E-6,
												precision,
												maxIter));
						}
						genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", emission.str());
					}
				} else {
					// Illumination Ratio
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");

					std::ostringstream minRadianceStream, maxRadianceStream;
					double minRadiance = genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
						temperatureFunctions[i].getMeanTemperature() - temperatureFunctions[i].getDeltaTemperature() / 2,
						genDTO.emissionLaw[0],
						genDTO.lowerBandWavelength[0] * 1E-6,
						genDTO.upperBandWavelength[0] * 1E-6,
						precision,
						maxIter);
					double maxRadiance = genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
						temperatureFunctions[i].getMeanTemperature() + temperatureFunctions[i].getDeltaTemperature() / 2,
						genDTO.emissionLaw[0],
						genDTO.lowerBandWavelength[0] * 1E-6,
						genDTO.upperBandWavelength[0] * 1E-6,
						precision,
						maxIter);
					minRadianceStream << minRadiance;
					maxRadianceStream << maxRadiance;

					for (unsigned int bandIndex = 1; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
						double minRadiance = genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
							temperatureFunctions[i].getMeanTemperature() - temperatureFunctions[i].getDeltaTemperature() / 2,
							genDTO.emissionLaw[bandIndex],
							genDTO.lowerBandWavelength[bandIndex] * 1E-6,
							genDTO.upperBandWavelength[bandIndex] * 1E-6,
							precision,
							maxIter);
						double maxRadiance = genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
							temperatureFunctions[i].getMeanTemperature() + temperatureFunctions[i].getDeltaTemperature() / 2,
							genDTO.emissionLaw[bandIndex],
							genDTO.lowerBandWavelength[bandIndex] * 1E-6,
							genDTO.upperBandWavelength[bandIndex] * 1E-6,
							precision,
							maxIter);
						minRadianceStream << " " << minRadiance;
						maxRadianceStream << " " << maxRadiance;
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", minRadianceStream.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", maxRadianceStream.str());
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(!temperatureFunctions[i].isSingleTemperatureSurface()));
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT));

					precomputationNeeded = precomputationNeeded || genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT;
				}
			}

			if (temperatureFunctions[i].isUsingOpticalFactorMatrix()
					|| (genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_MATRICE_3D
							&& !temperatureFunctions[i].isOverrideTemperatureProfile()
							&& !temperatureFunctions[i].isUsingSunlitShadedTemperatures()
							&& !temperatureFunctions[i].isUsingScopeFiles())) {
				// 3D Matrix
				std::string temperatureFile = temperatureFunctions[i].getOpticalFactorMatrixFile();
				if (!temperatureFunctions[i].isUsingOpticalFactorMatrix()) {
					temperatureFile = genDTO.temperatureFile;
				}

				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");

				std::string textureName = "temperaturematrix" + boost::lexical_cast<std::string>(temperatureToRadianceFile.size());
				std::string textureKeyRoot = "scene.textures." + textureName + ".";
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "type", "matrix3d");
				std::unordered_map<std::string, std::string>::const_iterator it = temperatureToRadianceFile.find(temperatureFile);
				if (it == temperatureToRadianceFile.end()) {
					std::string outputTextureFile = PhaseUtils::getOutputFolder() + pathSeparator + textureName + ".bin";
					writeRadianceFile(genDTO, temperatureFile, DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator, textureName + ".bin");
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", outputTextureFile);
					temperatureToRadianceFile[temperatureFile] = outputTextureFile;
				} else {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", it->second);
				}

				// In the current version, maket will add the dimensions keys. In DART 6, the user will provide directly the information
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.x", "modulo");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.y", "modulo");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.z", temperatureFunctions[i].isDuplicateFirstOpticalMatrixLayer()?"repeatlast":"zeros");

				genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "generator", "phase");

				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", textureName);
			}

			if (temperatureFunctions[i].isUsingScopeFiles()) {
				// Scope Profile
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(!temperatureFunctions[i].isSingleTemperatureSurface()));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT));

				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", getOrCreateScopeTexture(genDTO, scopeFileToTextureMap, textureIndex, temperatureFunctions[i].getShadedScopeFile()));
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", getOrCreateScopeTexture(genDTO, scopeFileToTextureMap, textureIndex, temperatureFunctions[i].getSunlitScopeFile()));

				precomputationNeeded = precomputationNeeded || genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT;
			}

			if (temperatureFunctions[i].isUsingSunlitShadedTemperatures()) {
				if (temperatureFunctions[i].getShadedTemperature() != temperatureFunctions[i].getSunlitTemperature()) {
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(!temperatureFunctions[i].isSingleTemperatureSurface()));
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT));

					std::ostringstream sEmission;
					sEmission.str("");
					sEmission << (genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(temperatureFunctions[i].getSunlitTemperature(), genDTO.emissionLaw[0],
							genDTO.lowerBandWavelength[0] * 1E-6, genDTO.upperBandWavelength[0] * 1E-6, precision, maxIter));
					for (unsigned int bandIndex = 1; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
						sEmission << " " << (genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(temperatureFunctions[i].getSunlitTemperature(), genDTO.emissionLaw[bandIndex],
								genDTO.lowerBandWavelength[bandIndex] * 1E-6, genDTO.upperBandWavelength[bandIndex] * 1E-6, precision, maxIter));
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", sEmission.str());
					sEmission.str("");
					sEmission << (genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(temperatureFunctions[i].getShadedTemperature(), genDTO.emissionLaw[0],
							genDTO.lowerBandWavelength[0] * 1E-6, genDTO.upperBandWavelength[0] * 1E-6, precision, maxIter));
					for (unsigned int bandIndex = 1; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
						sEmission << " " << (genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(temperatureFunctions[i].getShadedTemperature(), genDTO.emissionLaw[bandIndex],
								genDTO.lowerBandWavelength[bandIndex] * 1E-6, genDTO.upperBandWavelength[bandIndex] * 1E-6, precision, maxIter));
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min",  sEmission.str());

					precomputationNeeded = precomputationNeeded || genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_ECLAIREMENT;
				} else {
					// Constant
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
					std::ostringstream emission;
					emission.str("");
					emission << (genDTO.spectralDomain[0] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
								temperatureFunctions[i].getSunlitTemperature(),
								genDTO.emissionLaw[0],
								genDTO.lowerBandWavelength[0] * 1E-6,
								genDTO.upperBandWavelength[0] * 1E-6,
								precision,
								maxIter));

					for (unsigned int bandIndex = 1; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
						// genDTO.lowerBandWavelength[bandIndex]: wavelength unit: micrometer
						emission << " " << (genDTO.spectralDomain[bandIndex] == 0 ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
									temperatureFunctions[i].getSunlitTemperature(),
									genDTO.emissionLaw[bandIndex],
									genDTO.lowerBandWavelength[bandIndex] * 1E-6,
									genDTO.upperBandWavelength[bandIndex] * 1E-6,
									precision,
									maxIter));
					}
					genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", emission.str());
				}
			}
		}

		genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.illuminationNeeded", std::to_string(precomputationNeeded));
	} else {
		// Write Null Emission materials (non light-sources)
		string key = "scene.radiations.";

		// Add default temperature property
		string defaultEmissionName = "defaultEmission";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".type", "null");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".generator", "phase");

		for (unsigned int i = 0; i < temperatureFunctions.size(); i++) {
			std::string emissionKey = key + "emisFunc" + std::to_string(i) + ".";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
		}
	}
}
