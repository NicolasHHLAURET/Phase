/*! Time-stamp: <@(#) DartEnv.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : DartEnv.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 30/03/2005
 *
 *  Description : Contient les fonctions utiles a l'environnement d'execution
 * 				 de DART.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 */

#ifndef _DARTENV_H_
#define _DARTENV_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#if __cplusplus < 201103L && !defined(__GXX_EXPERIMENTAL_CXX0X__)
    #define BOOST_NO_CXX11_SCOPED_ENUMS
#endif

#define lcs(VAL) 	boost::lexical_cast<std::string>(VAL)

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "../../libDartBase/include/GeneralDef.hpp"

const std::string _vsDatabasePathName = "database";
const std::string _vsInputPathName = "input";
const std::string _vsSimulations = "simulations";
const std::string _vsOutputPathName = "output";
const std::string _vsLibPhase = "lib_phase";
const std::string _sInversionDirectoryName = "Inversion";
const std::string _vsLUTDirectoryName = "sequence";
const std::string _vsBinPathName = "bin";
const std::string _vsLogPathName = "log_parameters";
const std::string _sPerTypeProducts = "PerSceneElements";
const std::string _sPerLightProducts = "PerLightSources";
const std::string _sInversionFileName = "inversion.xml";
const std::string _sMakOutputForInversionFileName = "makOutputForInversion.xml";
const std::string _sPhaseFileName = "phase.xml";
const std::string _sReferenceSimulationFile = ".DART.referenceSimulations";

//Definition d'un caractere equivalent a la symbolique endl (=> "\n") pour les espaces
const static std::string _space_ = " ";

//Definition d'un caractere equivalent a la symbolique endl (=> "\n") pour les tabulations
const static std::string _tab_ = "\t";

/*
 #define _usPathSeparator  "/"
 #define _usPathSeparatorInputOutput  "../" */

/*!
 *  @ingroup DartEnv
 * 
 *  @version 1.2
 * 
 *  Classe statique d'utilitaire d'acces a l'environnement d'execution DART.
 * 
 */
class DartEnv {

public:
	static std::string getFileDirectory(const std::string &relativePath,
                                        const std::string &fileName);
	static std::string getFilePath(	const std::string &relativePath,
                                    const std::string &fileName);

	static std::string getFilePathForEdit(	const std::string &relativePath,
                                            const std::string &fileName);

	static std::string getFilePathForRead(  const std::string &relativePath,
                                            const std::string &fileName);

	static std::string cleanPath(const std::string &path);

	static std::vector<std::string> _orderSimulationPathToCheck;
	static std::vector<std::string> _orderDatabasePathToCheck;
	static void initOrderPathToCheck();
	static void loadUserSimulations();
	static void loadMotherSimulations();

	static void useSN_nodePhaseSnDir() {
		_nodePhaseSnDir = "_SolarNoise";
	}

	static void useLIDAR_nodePhaseSnDir() {
		_nodePhaseSnDir = "_LIDAR";
	}

	static void resetNodePhaseSnDir() {
		_nodePhaseSnDir = "";
	}

	/**
	 * Initialise les chemins de la simulation courante a partir du nom de la simulation passee en parametre.
	 */
	static void initCurrentSimulationPathFromSimulationName(std::string simulationName);

	/**
	 * Retourne le chemin vers le repertoire racine des simulations sur cette machine.
	 */
	static std::string getSimulationsRootDirectory();

	/**
	 * 
	 * @return le path Home de DART 
	 */
	static std::string getHomePath(void);

	/**
	 * 
	 * @return le path local de DART.
	 */
	static std::string getLocalHomePath(void);

	/**
	 * 
	 * @return le path local de DART.
	 */
	static std::string getOutputTempDatabasePath(bool createIfNotExists = false);
	static std::string getLocalTempDatabasePath(void);


	/**
	 *
	 * @param databaseFileName : nom du fichier de base de donnee.
	 *
	 * @return le chemin vers le DOSSIER contenant la base de donnees.
	 * 
	 * !!! BE CAREFULL IT'S DOESN'T RETURN THE PATH BUT PARENT DIRECTORY !!!!
	 * Don't trust the method name, need to be changed but used in a lot of code ....
	 */
	static std::string getDatabasePath(std::string databaseFileName);

	/**
	 * Retourne le chemin vers le dossier racine de la log
	 */
	static std::string getRootLogPath();

	/**
	 * @return le nom de la simulation courante.
	 */
	static std::string getCurrentSimulationName(void);

	/**
	 * @param referenceSimulationName chemin relatif au repertoire de simulations, tel que retourne par getCurrentSimulationName()
	 * @return le chemin relatif (au repertoire contenant les simulations) de la simulation donnee debarasse de caracteres annexes, base sur la forme des resultats de getCurrentSimulationName().
	 */
	static std::string cleanSimulationPath(const std::string & currentSimulationName);

	/**
	 * @param referenceSimulationName chemin relatif au repertoire de simulations, tel que retourne par getCurrentSimulationName()
	 * @return le nom de la simulation donnee debarasse de caracteres annexes, base sur la forme des resultats de getCurrentSimulationName().
	 */
	static std::string cleanSimulationName(const std::string & currentSimulationName);

	/**
	 * @param referenceSimulationName chemin relatif au repertoire de simulations, tel que retourne par getCurrentSimulationName()
	 * @return le nom de la sequence, base sur la forme des resultats de getCurrentSimulationName() quand on l'apelle durant une sequence.
	 */
	static std::string cleanSequenceName(const std::string & currentSimulationName);

	/**
	 * 
	 * @return le path complet vers le repertoire d'inversion de la simulation
	 * de reference.
	 */
	static std::string getInversionDirectoryName(std::string referenceSimulationName);

	/**
	 *
	 * @return le path complet vers le repertoire d'inversion de la simulation
	 * de reference.
	 */
	static std::string getInversionXMLName();

	/**
	 * @return le chemin vers la simulation courante.
	 */
	static std::string getCurrentSimulationPath(void);

	/**
	 * Permet d'obtenir le chemin complet vers le repertoire de simulations.
	 */
	static std::string getSimulationDirectory(std::string referenceSimulationName);

	/**
	 * @return le chemin vers le repertoire de LUT.
	 */
	static std::string getLUTPath(std::string referenceSimulationName);
	/**
	 * @return le chemin vers le repertoire de LUT.
	 */
	static std::string getLUTPathFromCurrentSimulation();

	/**
	 * @return le path vers le repertoire input de la simulation courante.
	 */
	static std::string getCurrentSimulationInputPath(void);

	/**
	 * @return le path output de la simulation courante.
	 */
	static std::string getCurrentSimulationOutputPath(void);

	/**
	 * @return le nom du repertoire output entoure du separateur.
	 */
	static std::string getOutputDirectoryName(void);

	/**
	 * @return le separateur
	 */
	static std::string getSeparator(void);

	/**
	 * @return le chemin complet vers le fichier inversion.xml de la simulation de
	 * reference.
	 */
	static std::string getInversionAbsoluteFilePath(std::string referenceSimulation);

	static std::string getInversionAbsoluteFilePathFromCurrentSimulation();
	/**
	 * Fonction permettant de recuperer le chemin complet vers le fichier makOutputForInversion.xml
	 */
	static std::string getMakOutputForInversionAbsoluteFilePath(std::string referenceSimulation);

	/**
	 * @return le chemin complet vers le fichier inversion.xml de la simulation de
	 * reference.
	 */
	static std::string getPhaseAbsoluteFilePath(std::string referenceSimulation);

	/**
	 * @return le chemin absolu vers le repertoire input
	 */
	static std::string getInputAbsolutePath(std::string referenceSimulationName);

	/**
	 * @return le chemin absolu vers le repertoire bin
	 */
	static std::string getbinPath(void);

	/**
	 * @return le chemin lib_phase de la simulation courante
	 */
	static std::string getCurrentSimulationLibPhase();

	static std::string getRelativePathToLibPhase() {
		return OUTPUT_DIR + pathSeparator + LIB_PHASE + _nodePhaseSnDir;
	}

	/**
	 * @return le bouleen indiquant si l'on doit utiliser la base de donnees pour l'inversion
	 */
	static bool getUsingDatabase();

	/**
	 * Fonction permettant de changer la valeur du bouleen indiquant si l'on doit utiliser la base de donnees pour l'inversion
	 */
	static void setUsingDatabase(bool withDatabase);

	static std::string getPathToFileReferenceSimulation() {
		return getLocalHomePath() + pathSeparator + _sReferenceSimulationFile;
	}

	/**
	 * Function that read the version file and set DART version
	 */
	static void initDARTVersion();

	/**
	 * Return the DART version file path
	 */
	static std::string getFileVersionPath() {
		return getHomePath() + _vsBinPathName + getSeparator() + _versionFileName;
	}

	/**
	 * Return the DART version
	 */
	static const std::string & getDARTversion() {
		return _sDARTversion;
	}

	/**
	 * Return the DART date
	 */
	static const std::string & getDARTdate() {
		return _sDARTdate;
	}

	/**
	 * Return the DART build number
	 */
	static const std::string & getDARTbuild() {
		return _sDARTbuild;
	}

	/**
	 * Return the DART suffix
	 */
	static const std::string & getDARTsuffix() {
		return _sDARTsuffix;
	}

	/**
	 * Return the DART suffix
	 */
	static const std::string & getPythonExecutable() {
		if (strcmp(_sPythonExecutable.c_str(), "") == 0) {
			std::string pythonPathLinux = DartEnv::getHomePath() + "bin"  + pathSeparator + "python" + pathSeparator + "python";
			boost::filesystem::path pathToPythonLinux(pythonPathLinux);
			if (boost::filesystem::exists(pathToPythonLinux)) {
				_sPythonExecutable = pythonPathLinux;
			} else {
				std::string pythonPathWindows = DartEnv::getHomePath() + "bin"  + pathSeparator + "python" + pathSeparator + "python.bat";
				boost::filesystem::path pathToPythonWindows(pythonPathWindows);
				if (boost::filesystem::exists(pathToPythonWindows)) {
					_sPythonExecutable = pythonPathWindows;
				} else {
					_sPythonExecutable = "python";
				}
			}
		}
		return _sPythonExecutable;
	}

	static const std::string & getLuxCoreExecutable() {
		if (strcmp(_sLuxCoreExecutable.c_str(), "") == 0) {
			std::string luxCorePathLinux = DartEnv::getHomePath() + "bin"  + pathSeparator + "luxcoreconsole.exe";
			boost::filesystem::path pathToLuxCoreLinux(luxCorePathLinux);
			if (boost::filesystem::exists(pathToLuxCoreLinux)) {
				_sLuxCoreExecutable = luxCorePathLinux;
			} else {
				_sLuxCoreExecutable = "luxcoreconsole.exe";
			}
		}
		return _sLuxCoreExecutable;
	}

	/**
	 * Return a formatted DART version
	 */
	static std::string getFullDARTversion() {
		bool toClosed = false;
		std::string version = _sDARTversion;
		if (_sDARTdate.compare("") != 0) {
			toClosed = true;
			version += " (" + _sDARTdate;
		}
		if (_sDARTbuild.compare("") != 0) {
			version += " " + _sDARTbuild;
		}
		if (_sDARTsuffix.compare("") != 0) {
			version += " " + _sDARTsuffix;
		}
		if (toClosed) {
			version += ")";
		}
		return version;
	}

	/**
	 * Replace char by char in string
	 */
	static void replaceCharInString(const char & a,
									const char & b,
									std::string & str) {
		size_t pos = 0;
		while (pos < str.size()) {
			if (str[pos] == a) {
				str[pos] = b;
			}
			++pos;
		}
	}

	/**
	 * Set true if use yaml files instead of xmls
	 */
	static void setIsUseYamlFiles(const bool & isUseYamlFiles){
		_bUseYaml = isUseYamlFiles;
	}

	/**
	 * Return true if use yaml files instead of xmls
	 */
	static const bool & getIsUseYamlFiles() {
		return _bUseYaml;
	}

private:
	/**
	 * String that contain the DART version
	 */
	static const std::string _versionFileName;
	static std::string _sDARTversion;
	static std::string _sDARTdate;
	static std::string _sDARTbuild;
	static std::string _sDARTsuffix;

	/**
	 * Set true if use yaml files instead of xmls
	 */
	static bool _bUseYaml;

	/**
	 * Set DART version
	 */
	static void setDARTversion(const std::string & value) {
		if (value.compare("") != 0) {
			_sDARTversion = value;
		}
	}

	/**
	 * Set DART date
	 */
	static void setDARTdate(const std::string & value) {
		if (value.compare("") != 0) {
			_sDARTdate = value;
		}
	}

	/**
	 * Set DART build number
	 */
	static void setDARTbuild(const std::string & value) {
		if (value.compare("") != 0) {
			_sDARTbuild = value;
		}
	}

	/**
	 * Set DART suffix
	 */
	static void setDARTsuffix(const std::string & value) {
		if (value.compare("") != 0) {
			_sDARTsuffix = value;
		}
	}
	/**
	 * variable contenant la variable d'environnement local pour DART.
	 */
	static std::string _sDART_LOCAL;

	/**
	 * variable contenant la variable d'environnement global pour DART.
	 */
	static std::string _sDART_HOME;

	/**
	 * variable contenant le nom de la simulation courante.
	 */
	static std::string _sCurrentSimulationName;

	/**
	 * variable contenant le chemin vers la simulation courante.
	 */
	static std::string _sCurrentSimulationPath;

	/**
	 * variable contenant le chemin vers le repertoire 'input' de la simulation courante.
	 */
	static std::string _sCurrentSimulationUserInputPath;

	/**
	 * variable contenant le chemin vers le repertoire 'output' de la simulation courante.
	 */
	static std::string _sCurrentSimulationUserOutputPath;

	/**
	 * Path (if found) to the python executable
	 */
	static std::string _sPythonExecutable;
	/**
	 * Path (if found) to the LuxCore executable
	 */
	static std::string _sLuxCoreExecutable;

	/**
	 * variable contenant un bouleen indiquant si l'on doit utiliser les donnees en base ou non. (Inversion)
	 */
	static bool _bUsingDatabase;

	/**
	 * Converti les '\' d'une variable d'environnement en '/' pour permettre la compatibilite sur
	 * differents systemes.
	 * 
	 * @param string &sVarEnvValue : chaine contenant le resultat attendu.
	 * @param  const char *sVarEnvName : nom de la variable d'environnement.
	 * 
	 */
	static void InitPathFromEnv(std::string &sVarEnvValue,
								const char *sVarEnvName);

	static std::string _nodePhaseSnDir;
};

#endif //_DARTENV_H_
