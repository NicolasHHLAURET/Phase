/*!
 *********************************************************************
 *  @file      Lib_fileManager.h
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
 * V 1.0 17/07/2008 Pierre CHALIER, Thomas GALLINARI : First Revision
 *
 *********************************************************************
 */

#ifndef _LIB_FILEMANAGER
#define _LIB_FILEMANAGER

#include <string>
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "GeneralDef.hpp"

/**
 * Checks wether the given file exists or not.
 * @param p_path the path to the file
 * @return true if the file exists, false otherwise
 */
bool isFile(const std::string& p_path);

/**
 * Checks wether the given directory exists or not.
 * @param p_path the path to the directory
 * @return true if the directory exists, false otherwise
 */
bool isDir(const std::string& p_path);

/**
 * Lists the given directory.
 * @param p_path the path to the directory to list
 * @return the name of the elements of the given directory
 */
std::vector<std::string> ls(const std::string& p_path);

/**
 * Creates a directory if it does not already exist.
 * @param p_path the path to the directory
 */
void createDirectory(const std::string& p_path);

/**
 * Removes a directory.
 * @param p_path the path to the directory
 */
void removeDirectory(const std::string& p_path);

/**
 * Removes a file.
 * @param p_path the path to the file
 */
void removeFile(const std::string& p_path);

/**
 * Copies a file or a directory to a destination.
 * @param p_source path to the source
 * @param p_dest path to the destination
 */
void copyFile(	const std::string& p_source,
				const std::string& p_dest);

/**
 * Renames or moves a file to a destination.
 * @param p_source path to the source
 * @param p_dest path to the destination
 */
void renameOrMoveFile(	const std::string& p_source,
						const std::string& p_dest);

#endif /*_LIB_FILEMANAGER*/
