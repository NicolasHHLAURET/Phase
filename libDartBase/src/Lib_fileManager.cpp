/*!
 *********************************************************************
 *  @file      Lib_fileManager.cpp
 *  @ingroup   DartBase
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : Pierre CHALIER                       Date: 17/07/2008
 *             Thomas GALLINARI
 *
 *  Description : Fonctions de gestion de fichiers et repertoires. 
 *
 *********************************************************************
 * Historique :
 *
 * V 2.0 08/09/2008 Pierre CHALIER : Suppression des appels systemes
 * V 1.0 17/07/2008 Pierre CHALIER, Thomas GALLINARI : First Revision
 *
 *********************************************************************
 */

#include "../include/Lib_fileManager.h"

using namespace std;

//------------------------------------------
// Path separator considering the running OS
//------------------------------------------

bool isFile(const string& p_path) {
	ifstream file;
	// Try to open the file
	file.open(p_path.c_str());
	// If the file has been open successfully
	if (!file.fail()) {
		// Close the file
		file.close();
		return true;
	}
	// If the file opening failed
	else {
		return false;
	}
}

bool isDir(const string& p_path) {
	// Try to open the directory
	DIR* dir = opendir(p_path.c_str());
	// If the directory has been open successfully
	if (dir != NULL) {
		// Close the directory
		closedir(dir);
		return true;
	}
	// If the directory opening failed
	else {
		return false;
	}
}

vector<string> ls(const string& p_path) {
	DIR* dir; // The directory to list
	struct dirent* elem; // Any element of the directory
	vector<string> list; // The elements of the directory
	
	// Open the given directory
	dir = opendir(p_path.c_str());
	if (dir == NULL) {
		cerr << "Could not list '" << p_path << "' directory" << endl;
		exit(EXIT_FAILURE);
	} else {
		// List the directory
		elem = readdir(dir);
		while (elem != NULL) {
			// Do not add "." and ".." directories
			if (strcmp(elem->d_name, ".") != 0 && strcmp(elem->d_name, "..") != 0) {
				list.push_back(elem->d_name);
			}
			elem = readdir(dir);
		}
		// Close the directory
		closedir(dir);
	}
	
	return list;
}

void createDirectory(const string& p_path) {

	if (p_path.empty())
	{ // if path empty this function do an infinite loop
		return;
	}
	size_t lastSeparator;
	//JCR compilation conditionelle
#if defined (_WIN32)
	int res = mkdir(p_path.c_str());
#else
	int res = mkdir(p_path.c_str(), 0777);
#endif
	if (res != 0) {
		switch (errno) {
			case EEXIST:
				// Le repertoire existe deja
				break;
				
			case ENOENT:
				// No such file or directory : le chemin contient des repertoires non crees
				// On cree d'abord le repertoire precedent du path
				lastSeparator = p_path.find_last_of(pathSeparator, p_path.length() - 2);
				createDirectory(p_path.substr(0, lastSeparator));
				createDirectory(p_path);
				break;
				
			default:
				string errorMessage = "\nStop condition : Error in function Lib_fileManager/createDirectory\n" + p_path;
				perror(errorMessage.c_str());
				exit(EXIT_FAILURE);
		}
	}
}

void removeDirectory(const string& p_path) {
	DIR* dir; // The directory to remove
	struct dirent* elem; // Any element of the directory
	string path;
	
	// Add the path separator
	if (!p_path.empty() && p_path.find_last_of(pathSeparator) != (p_path.length() - 1)) {
		path = p_path + pathSeparator;
	} else {
		path = p_path;
	}
	
	if (remove(path.c_str()) == -1) {
		if ((dir = opendir(path.c_str())) != NULL) {
			// The directory is not empty, we remove its content
			while ((elem = readdir(dir))) {
				// Do not process "." and ".." directories
				if (strcmp(elem->d_name, ".") != 0 && strcmp(elem->d_name, "..") != 0) {
					string pathElem = path + string(elem->d_name);
					if (isDir(pathElem))
						removeDirectory(pathElem);
					else
						remove(pathElem.c_str());
				}
			}
			
			// Close the directory
			closedir(dir);
			
			// Remove the emptied directory
			// Ancien Code, la fonction remove ne remove pas les repertoires sous windows
			//remove(path.c_str());
			if (!rmdir(p_path.c_str())) { // delete the directory
				//cerr << "le processus ne peut pas detruire le repertoire :" << p_path << endl;
			}
		}
	}
	
}

void removeFile(const string& p_path) {
	int res = remove(p_path.c_str());
	
	// ENOENT = No such file or directory
	if (res != 0 && errno != ENOENT) {
		string errorMessage = "\nStop condition : Error in function Lib_fileManager/removeFile\n" + p_path;
		perror(errorMessage.c_str());
		exit(EXIT_FAILURE);
	}
}

void copyFile(	const string& p_source,
				const string& p_dest) {
	FILE* fileSrc;
	FILE* fileDest;
	char buffer[512];
	int nbRead;
	
	if ((fileSrc = fopen(p_source.c_str(), "rb")) == NULL) {
		string errorMessage = "\nStop condition : Error in function Lib_fileManager/copyFile, \nsource=" + p_source;
		perror(errorMessage.c_str());
		exit(EXIT_FAILURE);
	}
	
	if ((fileDest = fopen(p_dest.c_str(), "wb")) == NULL) {
		string errorMessage = "\nStop condition : Error in function Lib_fileManager/copyFile, \ndestination=" + p_dest;
		perror(errorMessage.c_str());
		exit(EXIT_FAILURE);
	}
	
	while ((nbRead = fread(buffer, 1, 512, fileSrc)) != 0)
		fwrite(buffer, 1, nbRead, fileDest);
	
	fclose(fileSrc);
	fclose(fileDest);
}

void renameOrMoveFile(	const string& p_source,
						const string& p_dest) {
	//int res = rename(p_source.c_str(), p_dest.c_str());
	// sous windows le rename n'efface pas le fichier destination
#if defined (_WIN32)
	if (_access(p_dest.c_str(), 0) == 0) {
		if (remove(p_dest.c_str()) != 0) {
			string errorMessage = "\nStop condition : Error in function Lib_fileManager/renameOrMoveFile\n remove " + p_source + " to " + p_dest;
			perror(errorMessage.c_str());
			exit (EXIT_FAILURE);
		}
	}
#endif
	if (rename(p_source.c_str(), p_dest.c_str()) != 0) {
		string errorMessage = "\nStop condition : Error in function Lib_fileManager/renameOrMoveFile\n rename " + p_source + " to " + p_dest;
		perror(errorMessage.c_str());
		exit(EXIT_FAILURE);
	}
	
//	if(res != 0)
//	{
//		string errorMessage = "\nStop condition : Error in function Lib_fileManager/renameOrMoveFile\nmove " + p_source + " to " + p_dest;
//		perror(errorMessage.c_str());
//		exit (EXIT_FAILURE) ;
//	}
}
