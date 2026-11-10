/* Time-stamp: <@(#) DartEnv.cpp  20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  File    : DartEnv.cpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 30/03/2005
 *
 *  Description :
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.1 21/04/2006  CLF : Correction du bug sur la mise a jour du coeff_diff.xml
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.1 08/07/2005  VBT : Correction du bug sur la mise a jour du coeff_diff.xml
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 */

#include "../include/DartEnv.h"

#if defined(WIN64) or defined(_WIN64) or defined(_WIN32)
	#include <boost/algorithm/string/replace.hpp>
#endif

using namespace std;

// -----------------------------------------------------------------------------

string DartEnv::_sCurrentSimulationName = "";
string DartEnv::_sCurrentSimulationUserInputPath = "";
string DartEnv::_sCurrentSimulationUserOutputPath = "";
string DartEnv::_sCurrentSimulationPath = "";
string DartEnv::_sDART_HOME = "";
string DartEnv::_sDART_LOCAL = "";
string DartEnv::_nodePhaseSnDir = "";
string DartEnv::_sDARTversion = "5.7.6";
string DartEnv::_sDARTdate = "";
string DartEnv::_sDARTbuild = "";
string DartEnv::_sDARTsuffix = "";
string DartEnv::_sPythonExecutable = "";
string DartEnv::_sLuxCoreExecutable = "";
bool DartEnv::_bUseYaml = false;
const string DartEnv::_versionFileName = "version";
vector<string> DartEnv::_orderDatabasePathToCheck;
vector<string> DartEnv::_orderSimulationPathToCheck;

bool DartEnv::_bUsingDatabase = false;

/**
 * Init the DART version from version file if exist
 */
void DartEnv::initDARTVersion() {
	// Init variables
	string DARTversion = "";
	string DARTdate = "";
	string DARTbuild = "";
	string DARTsuffix = "";
	string line = "";

	// Get file version path
	string versionFname = DartEnv::getFileVersionPath();

	// Open file
	ifstream input;
	input.open(versionFname.c_str());

	// Read file
	if (input) {
		// Read only the first line
		getline(input, line);

		// Parse line
		vector<size_t> found;
		size_t pos = 0;
		// Search all '_' in line
		while (pos != line.size()) {
			// store read value
			if (line[pos] == '_') {
				found.push_back(pos);
			}
			++pos;
		}

		// Split line
		size_t start = 0;
		size_t end = 0;
		string value = "";
		int i = 0;
		for (i = 0; i <= int(found.size()); ++i) {
			// Look for next '_' position or end of line
			if (i == int(found.size())) {
				end = line.size();
			} else {
				end = found[i];
			}

			// Get value of line between 'start' and 'end'
			value = line.substr(start, end-start);

			// Assign result
			if (i == 0 and value.compare("") != 0) {
				DartEnv::replaceCharInString('-', '.', value);
				DARTversion = value;
			} else if (i == 1 and value.compare("") != 0) {
				DartEnv::replaceCharInString('-', '/', value);
				DARTdate = value;
			} else if (i == 2 and value.compare("") != 0) {
				DARTbuild = value;
			} else if (i == 3 and value.compare("") != 0) {
				DARTsuffix = value;
			}

			// Move start position to end + 1
			start = end + 1;
		}
	}

	// Close file
	input.close();

	// Set new value to DartEnv
	DartEnv::setDARTversion(DARTversion);
	DartEnv::setDARTdate(DARTdate);
	DartEnv::setDARTbuild(DARTbuild);
	DartEnv::setDARTsuffix(DARTsuffix);
}

string DartEnv::cleanPath(const string &path) {
	string p = path;
	boost::replace_all(p, "\\", "/");
	boost::replace_all(p, "/:", ":");
	return p;
}

/**
 * Retourne le path d'un fichier passé en paramètres.
 * Dans le cas d'une séquence, il est possible que le fichier n'ait pas été copié dans la simulation
 * (fichiers du dossier output), dans ce cas il est recherché dans la simulation mère, puis copié dans
 * la simulation courante pour pouvoir être édité.
 * @return path vers le fichier.
 */
string DartEnv::getFilePathForEdit(	const string &relativePath,
									const string &fileName) {
	string localPath = DartEnv::getCurrentSimulationPath() + relativePath + pathSeparator + fileName;
	string searchPath = getFilePath(relativePath, fileName);
#if defined(WIN64) or defined(_WIN64) or defined(_WIN32)
	boost::replace_all(searchPath, "\\", "/"); // Note: localPath is already "\ free"
#endif

	if (searchPath != "" && !boost::filesystem::equivalent(searchPath, localPath)) {
		boost::filesystem::copy(searchPath, localPath);
	}
	return localPath;
}

/**
 * Retourne le path vers le fichier à lire.
 * Un fichier en lecture peut être absent de la simulation, par exemple dans le cas d'une séquence
 * où les fichiers non modifiés ne sont pas copiés.
 * Dans ce cas, le path retourné est le fichier dans la simulation mère (donc doit être en lecture seule).
 * Pour éditer un fichier, utiliser la méthode getFilePathForEdit.
 * @return path vers le fichier trouvé.
 */
string DartEnv::getFilePathForRead(	const string &relativePath,
									const string &fileName) {
	return getFilePath(relativePath, fileName);
}

/**
 * Recherche et retourne le path absolu d'un fichier passé en paramètre de la façon suivante:
 * @param relativePath : path relatif dans l'aborescence d'une simulation, exemple, pour un fichier dans
 * le dossier BAND0, le relative path à passer est "output/BAND0"
 * @param file : nom du fichier a passer avec l'extension, exemple pour le fichier atmosphère, le parametre
 * a passer est "atmosphere.xml".
 * @return getFilePath : le path absolu vers le fichier trouvé,
 * exemple "/home/user/DART/user_data/simulations/test/input/atmosphere.xml"
 * @return getFileDirectory : le path absolu vers le répertoire dans lequel est le fichier,
 * exemple "/home/user/DART/user_data/simulations/test/input/"
 * @return une chaine vide si pas trouvé (ie. fichiers optionnels).
 */
string DartEnv::getFilePath(const string &relativePath,
							const string &fileName) {
#if defined(WIN64) or defined(_WIN64) or defined(_WIN32)
	// 1st: we check is the filename is not in fact an absolute path
	string fn(fileName);
	boost::replace_all(fn, "\\", "/");
	boost::filesystem::path pathToFile(fn);
	if (boost::filesystem::exists(pathToFile)) {
		fn = boost::filesystem::canonical(pathToFile).string();
		boost::replace_all(fn, "\\", "/");
		return fn;
	}
	string rp(relativePath);
	boost::replace_all(rp, "\\", "/");
	string directoryFile = getFileDirectory(rp, fn);
	boost::replace_all(directoryFile, "\\", "/");
	return directoryFile != "" ? (directoryFile + fn) : "";
#else
	// 1st: we check is the filename is not in fact an absolute path
	boost::filesystem::path pathToFile(fileName);
	if (boost::filesystem::exists(pathToFile)) {
		return boost::filesystem::canonical(pathToFile).string();
	}

	string directoryFile = getFileDirectory(relativePath, fileName);
	return directoryFile != "" ? (directoryFile + fileName) : "";
#endif
}

string DartEnv::getFileDirectory(	const string &relativePath,
									const string &fileName) {
	// On cherche dans les simulation référencées dans l'ordre.
	vector<string>::const_iterator itrPathToCheck, itrEndPathToCheck = _orderSimulationPathToCheck.end();
	for (itrPathToCheck = _orderSimulationPathToCheck.begin(); itrPathToCheck != itrEndPathToCheck; ++itrPathToCheck) {
		boost::filesystem::path pathToFile((*itrPathToCheck) + pathSeparator + relativePath + pathSeparator + fileName);
		if (boost::filesystem::exists(pathToFile)) {
			return boost::filesystem::canonical((*itrPathToCheck)).string() + pathSeparator + relativePath + pathSeparator;
		}
	}
	//Si on trouve pas, on tente les databases locales, puis globales
	vector<string>::const_iterator itrPathDbToCheck, itrEndPathDbToCheck = _orderDatabasePathToCheck.end();
	for (itrPathDbToCheck = _orderDatabasePathToCheck.begin(); itrPathDbToCheck != itrEndPathDbToCheck; ++itrPathDbToCheck) {
		boost::filesystem::path pathToFile((*itrPathDbToCheck) + pathSeparator + fileName);
		if (boost::filesystem::exists(pathToFile)) {
			return boost::filesystem::canonical((*itrPathDbToCheck)).string() + pathSeparator;
		}
	}
	return "";
}

/**
 * Initialise l'ordre de recherche des fichiers.
 * _orderSimulationPathToCheck -> les simulations "mères" dans lesquelles on va chercher les fichiers,
 * la première simulation est la simulation qui est executée.
 * _orderDatabasePathToCheck -> les dossiers database dans lesquels on va chercher si on trouve pas dans
 * les simulations.
 */
void DartEnv::initOrderPathToCheck() {
	string localDirectory = _sCurrentSimulationPath; //_sCurrentSimulationUserInputPath + pathSeparator;
	string localWayToDatabaseFile = getLocalHomePath() + _vsDatabasePathName + pathSeparator;
	string localTempWayToDatabaseFile = getLocalTempDatabasePath();
	string globalWayToDatabaseFile = getHomePath() + _vsDatabasePathName + pathSeparator;

	_orderSimulationPathToCheck.clear();
	_orderSimulationPathToCheck.push_back(localDirectory);
	loadMotherSimulations();
	loadUserSimulations();

	_orderDatabasePathToCheck.clear();
	_orderDatabasePathToCheck.push_back(localWayToDatabaseFile);
	_orderDatabasePathToCheck.push_back(globalWayToDatabaseFile);
	_orderDatabasePathToCheck.push_back(localTempWayToDatabaseFile);
}

/**
 * Charge les simulations de référence de l'utilisateur.
 * elles sont contenues dans le fichier .DART.referenceSimulations.
 */
void DartEnv::loadUserSimulations() {
	//Todo: toudoux =)
	/*string line;
	 fstream userFile(getPathToFileReferenceSimulation().c_str(),ios_base::in);
	 while (getline(userFile,line)) {
	 boost::filesystem::path userPath(line);
	 if (boost::filesystem::exists(userPath)) {
	 _orderDatabasePathToCheck.push_back(line);
	 }
	 }
	 userFile.close();*/
}

/**
 * Charge les simulations parent (jusqu'au dossier simulations).
 * exemple pour "/home/user/DART/user_data/simulations/myRootSimulation/mySimulation/sequence/mySimu_1/"
 * -> "/home/user/DART/user_data/simulations/myRootSimulation/mySimulation/sequence/mySimu_1/"
 * -> "/home/user/DART/user_data/simulations/myRootSimulation/mySimulation/sequence/"
 * -> "/home/user/DART/user_data/simulations/myRootSimulation/mySimulation/"
 * -> "/home/user/DART/user_data/simulations/myRootSimulation/"
 */
void DartEnv::loadMotherSimulations() {
	string localDirectory = _sCurrentSimulationPath;
	// Il faut que le dossier simulations soit dans l'arborescence
	if (localDirectory.find(pathSeparator + "simulations" + pathSeparator) != string::npos) {
		vector<string> tokens;
		boost::split(tokens, localDirectory, boost::is_any_of(pathSeparator));
		// On parcours l'arborescence à l'envers
		vector<string>::const_reverse_iterator itrTokens, itrEndTokens = tokens.rend();
		for (itrTokens = ++tokens.rbegin(); itrTokens != itrEndTokens; itrTokens++) {
			// Et tant qu'on est pas remonté au dossier simulations
			if ((*itrTokens) != "simulations") {
				// On remonte d'un element et on l'ajoute aux simulations à vérifier
				string element = (*itrTokens) + pathSeparator;
				boost::replace_last(localDirectory, element, "");
				_orderSimulationPathToCheck.push_back(localDirectory);
			} else {
				break;
			}
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DartEnv::initCurrentSimulationPathFromSimulationName(string simulationName) {
	// TODO : si le chemin commencer par simulationsRootDirectory, il faut faire la procédure inverse
	_sCurrentSimulationName = simulationName;
#if defined(WIN64) or defined(_WIN64) or defined(_WIN32)
	boost::replace_all(_sCurrentSimulationName, "\\", "/");
#endif
	_sCurrentSimulationPath = getSimulationsRootDirectory() + _sCurrentSimulationName + pathSeparator;
	_sCurrentSimulationUserInputPath = _sCurrentSimulationPath + _vsInputPathName + pathSeparator;
	_sCurrentSimulationUserOutputPath = _sCurrentSimulationPath + _vsOutputPathName + pathSeparator;
	initOrderPathToCheck();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getSimulationsRootDirectory() {
	return getLocalHomePath() + _vsSimulations + pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DartEnv::InitPathFromEnv(	string &sVarEnvValue,
								const char *sVarEnvName) {
	sVarEnvValue = getenv(sVarEnvName);
	size_t len = sVarEnvValue.size();
	for (size_t i = 0; i < len; i++) {
		if (sVarEnvValue[i] == '\\') {
			sVarEnvValue[i] = '/';
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getHomePath(void) {
	if (strcmp(_sDART_HOME.c_str(), "") == 0) {
		InitPathFromEnv(_sDART_HOME, "DART_HOME");
		if(_sDART_HOME.back() != pathSeparator[0]) _sDART_HOME += pathSeparator;
	}
	return _sDART_HOME;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getLocalHomePath(void) {
	if (strcmp(_sDART_LOCAL.c_str(), "") == 0) {
		InitPathFromEnv(_sDART_LOCAL, "DART_LOCAL");
		if(_sDART_LOCAL.back() != pathSeparator[0]) _sDART_LOCAL += pathSeparator;
	}
	return _sDART_LOCAL;
}
string DartEnv::getOutputTempDatabasePath(bool createIfNotExists) {
	if (strcmp(_sDART_LOCAL.c_str(), "") == 0) {
		InitPathFromEnv(_sDART_LOCAL, "DART_LOCAL");
		if(_sDART_LOCAL.back() != pathSeparator[0]) _sDART_LOCAL += pathSeparator;
	}
	string tempDbPath = DartEnv::getCurrentSimulationOutputPath() + pathSeparator + "temp" + pathSeparator;
	if ((!boost::filesystem::exists(tempDbPath)) && createIfNotExists) {
		boost::filesystem::create_directory(tempDbPath);
	}
	return tempDbPath;
}
string DartEnv::getLocalTempDatabasePath(void) {
	if (strcmp(_sDART_LOCAL.c_str(), "") == 0) {
		InitPathFromEnv(_sDART_LOCAL, "DART_LOCAL");
		if(_sDART_LOCAL.back() != pathSeparator[0]) _sDART_LOCAL += pathSeparator;
	}
	string tempDbPath = _sDART_LOCAL + pathSeparator + "database" + pathSeparator + "temp" + pathSeparator;
	if (!boost::filesystem::exists(tempDbPath)) {
		boost::filesystem::create_directory(tempDbPath);
	}
	return tempDbPath;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getInversionDirectoryName(string referenceSimulationName) {
	return getSimulationsRootDirectory() + referenceSimulationName + pathSeparator + _vsOutputPathName + pathSeparator + _sInversionDirectoryName;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getInversionXMLName() {
	return _sInversionFileName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getDatabasePath(string databaseFileName) {
	return getFileDirectory("input", databaseFileName);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getRootLogPath() {
	return getLocalHomePath() + _vsLogPathName + pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getCurrentSimulationName(void) {
	return _sCurrentSimulationName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::cleanSimulationPath(const string & currentSimulationName) {
	/*
	 * Exemple
	 * retour de getCurrentSimulationName() : ./test/LutSpectral/sequence/sol_bande_14
	 * l'on souhaite obtenir : test/LutSpectral
	 * (test etant un sous-dossier rajoute manuellement dans le repertoire de simulation)
	 */

	if (currentSimulationName.empty()) {
		return "";
	}

	string simulationName = "";

	if (boost::starts_with(currentSimulationName, string(".") + pathSeparator)) {
		simulationName = currentSimulationName.substr(2);
	} else {
		simulationName = currentSimulationName;
	}

	string motifDeFin = pathSeparator + "sequence" + pathSeparator;
	// on cherche si la chaine contient "/sequence/", si oui ce qui nous interesse est ce qui se trouve avant la premiere occurence de cette chaine
	const unsigned int sequencePos = simulationName.rfind(motifDeFin);
	if (sequencePos < simulationName.size() && sequencePos > 0) {
		simulationName = simulationName.substr(0, sequencePos);
	}

	return simulationName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::cleanSimulationName(const string & currentSimulationName) {
	/*
	 * Exemple
	 * retour de getCurrentSimulationName() : ./LutSpectral/sequence/sol_bande_14
	 * l'on souhaite obtenir : LutSpectral
	 */

	if (currentSimulationName.empty()) {
		return "";
	}
	string simulationName = cleanSimulationPath(currentSimulationName);
	if (boost::starts_with(simulationName, pathSeparator)) {
		simulationName = simulationName.substr(1);
	}
	if (boost::starts_with(simulationName, string(".") + pathSeparator)) {
		simulationName = simulationName.substr(2);
	}
	return simulationName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::cleanSequenceName(const string & currentSimulationName) {
	/*
	 * Exemple
	 * retour de getCurrentSimulationName() : ./LutSpectral/sequence/sol_bande_14
	 * l'on souhaite obtenir : sol_bande
	 */

	if (currentSimulationName.empty()) {
		return "";
	}

	string sequenceName = currentSimulationName;

	// ([^DELIMITEUR]+)(_[0-9]+)?$   <--- serait bien cool mais ... pas de regex en C++ :/ a moins de compiler le module boost adequat, mais vu la taille de la methode ce serait se compliquer la vie

	if (boost::ends_with(sequenceName, pathSeparator)) {
		sequenceName = sequenceName.substr(0, sequenceName.size() - 1);
	}

	// on cherche si le nom de la sequence est precede d'un chemin, et s'il l'est on tronque la chaine pour ne prendre que ce qui suit le dernier pathSeparator
	const unsigned int separatorPos = sequenceName.find_last_of(pathSeparator);
	if (separatorPos < sequenceName.size()) {
		sequenceName = sequenceName.substr(separatorPos + 1);

		bool terminaison = false;
		const unsigned int seqSize = sequenceName.length();
		// on cherche une terminaison de la forme _[0-9]+
		for (unsigned int i = 1; i <= seqSize; i++) {
			if (isdigit(sequenceName.at(seqSize - i))) {
				terminaison = true;
			} else if (sequenceName.at(seqSize - i) == '_') {
				break;
			} else {
				terminaison = false;
				break;
			}
		}

		// si terminaison est true alors on a bien rencontre un ou plusieurs chiffres immediatement precedes d'un underscore et on les tronque
		if (terminaison) {
			sequenceName = sequenceName.substr(0, sequenceName.find_last_of('_'));
		}
	}
	// si pas de chemin on considere que l'on n'est pas dans le cas normal et saute donc la recherche et elimination de la terminaison du cas normal

	return sequenceName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getCurrentSimulationPath(void) {
	return _sCurrentSimulationPath;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getSimulationDirectory(string referenceSimulationName) {
	return getSimulationsRootDirectory() + referenceSimulationName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getLUTPath(string referenceSimulationName) {
	return getSimulationDirectory(referenceSimulationName) + pathSeparator + _vsLUTDirectoryName + pathSeparator;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getLUTPathFromCurrentSimulation() {
	return getCurrentSimulationPath() + pathSeparator + _vsLUTDirectoryName + pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getCurrentSimulationInputPath(void) {
	if (strcmp(_sCurrentSimulationUserInputPath.c_str(), "") != 0) {
		return _sCurrentSimulationUserInputPath;
	} else {
		return getCurrentSimulationName() + _vsInputPathName + pathSeparator;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getInputAbsolutePath(string referenceSimulationName) {
	return getSimulationDirectory(referenceSimulationName) + pathSeparator + _vsInputPathName + pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getCurrentSimulationOutputPath(void) {
	if (strcmp(_sCurrentSimulationUserOutputPath.c_str(), "") != 0) {
		return _sCurrentSimulationUserOutputPath;
	} else {
		return getCurrentSimulationName() + _vsOutputPathName + pathSeparator;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getOutputDirectoryName(void) {
	string sOutput = "";

	sOutput += pathSeparator;
	sOutput += _vsOutputPathName;
	sOutput += pathSeparator;
	return sOutput;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getSeparator(void) {
	return pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getInversionAbsoluteFilePath(string referenceSimulation) {
	string inversionFilePath = "";

	inversionFilePath = getSimulationDirectory(referenceSimulation) + pathSeparator + _vsInputPathName + pathSeparator + _sInversionFileName;

	return inversionFilePath;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getInversionAbsoluteFilePathFromCurrentSimulation() {
	return getCurrentSimulationInputPath() + pathSeparator + _sInversionFileName;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getMakOutputForInversionAbsoluteFilePath(string referenceSimulation) {
	string path = "";

	path = getSimulationDirectory(referenceSimulation) + getOutputDirectoryName() + _sMakOutputForInversionFileName;

	return path;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getPhaseAbsoluteFilePath(string referenceSimulation) {
	string phaseFilePath = "";
	phaseFilePath = getSimulationDirectory(referenceSimulation) + pathSeparator + _vsInputPathName + pathSeparator + _sPhaseFileName;
	return phaseFilePath;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getbinPath(void) {
	return getHomePath() + _vsBinPathName + pathSeparator;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string DartEnv::getCurrentSimulationLibPhase() {
	return (DartEnv::getCurrentSimulationOutputPath() + _vsLibPhase + _nodePhaseSnDir + pathSeparator);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool DartEnv::getUsingDatabase(void) {
	return _bUsingDatabase;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DartEnv::setUsingDatabase(bool withDatabase) {
	_bUsingDatabase = withDatabase;
}

