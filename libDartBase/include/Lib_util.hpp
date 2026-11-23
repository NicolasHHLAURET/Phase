/*! Time-stamp: <@(#) Lib_util.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : Lib_util.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 30/03/2005
 *
 *  Description : Contient diverses fonctions communes aux modules DART.
 *
 *********************************************************************
 * Historique :
 * 
 * V 4.4.1 12/09/2008  Pierre Chalier : ajout de la fonction execute
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.1 29/04/2008  Pierre Chalier : Suppression des warning "deprecated"
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 10/03/2006  CLF : Livraison : ajout fonctions vecteurs
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 */

/*****************************************************/
/*                                                   */
/*     Bibliotheque des librairies "utilitaires"     */
/*     Date : 14/05/93                               */
/*     Derniere Modification : 19/09/08              */
/*****************************************************/
#ifndef _LIB_UTIL
#define _LIB_UTIL

/**
 *  @ingroup DartBase
 * 
 *  Contient diverses fonctions communes aux modules DART.
 */
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <sys/types.h>
//modif JCR
#if defined (_WIN32)

#else
#include <sys/wait.h>
#endif

#include "../include/Lib_fileManager.h"
#include "../include/GeneralDef.hpp"
#include "../../librairieDart/include/Conversions.hpp"

//CESB-TT-206-CLF 
//typedef char Fichier[nb_car];
typedef std::string Fichier;

float max(	float,
			float);

// Reverse bytes. (big endian and little endian)
/*float pctostation (float);

 double pctostationdouble (double);*/

// Cherche a ouvrir le fichier grace au chemin en parametre.
std::string getRealPath(std::string filePath);

// Cherche le bon chemin vers le fichier en parametre.
std::string testFilePath(std::string file,
					std::string currentSimulationInputPath);

// Creer le repertoire output contenant toutes les sorties de DART. 
void createOutputDirectory(std::string currentPath);

//Opens a file with the a specific mode. Otherwise, the soft exits.
// Pierre Chalier : Suppression des warning "deprecated"
extern void Open_file(	FILE **fp,
						std::string filePath,
						const char mode[]);

// Will be replaced by "read_n_char".
void lit_remarque(FILE *); // DELETE.

/**
 * Vincent Sac : 04/02/2010
 * Meme fonction que lit_remarque(FILE *in_l) mais prenant en charge un flux
 * ifstream de lecture de fichier
 * Objectif de la fonction : il s'agit de positionner le curseur de flux apres
 * les commentaires encadres par des '*' pour pouvoir ensuite lire les valeur
 * numeriques correspondantes
 */
void echappeLectureCommentaireFichier(	std::istream &fileIn,
										int nbCaractereAEvite = 0,
										int caractereRecherche = 0);

// Copies a file.
void Copy_File(	FILE *,
				FILE *);

// Exits a program with an error message that indicates the file and the line of the error.
// Pierre Chalier : Suppression des warning "deprecated"
void usage(	const char *,
			int,
			const char *);

// Reads the file until it finds a specific number of a specific char.
void read_n_char(	FILE *,
					int,
					char);

/**
 * Vincent Sac : 18/02/2010
 * Cette fonction lit n chaine de caracteres
 * (description) d'une valeur d'un fichier de base de donnees et renvoie la valeur
 * correspondante
 */
float lireDansLigneFlux(std::ifstream & inData,
						int nbCar);


/**
 * Executes a command.
 * @param p_cmd the command to be executed
 */
void execute(char ** p_cmd);

std::string convertiePourLectureImageBandeLarge(const double & d);

/*
 * Converti quoi que ce soit en string...
 */
template<typename T>
std::string whatever2string(const T & t) {
	std::ostringstream os;
	os.precision(10);
	os.width(10);
	os.setf(std::ios::fixed, std::ios::floatfield);
	os << t;
	return os.str();
}

#endif //_LIB_UTIL
