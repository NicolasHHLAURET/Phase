/*! Time-stamp: <@(#) MakInversionParser.h   05 nov. 2007   vbt MAGELLIUM
 *********************************************************************
 *  @file    : MakInversionParser.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 05/11/2007
 *
 *  Description : Classe de gestion du fichier de sortie de MAKET
 *
 *********************************************************************
 * Historique :
 *
 * V 1.0 05 nov. 2007  vbt : First Revision
 *
 *********************************************************************
 */

#ifndef MAK_INVPARSER_H_
#define MAK_INVPARSER_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "../../libTinyXPath/include/xpath_static.h"

#include "../../libDartEnv/include/DartEnv.h"
#include "../../librairieDart/include/Conversions.hpp"

using namespace TinyXPath;

static const std::string sPlotLAI = "PlotLAI";
static const std::string sForestLAI = "ForestLAI";
static const std::string sCrownCoverage = "CrownCoverage";

/**
 *  @ingroup DartFileParser
 * 
 *  Classe de gestion du fichier de sortie de MAKET
 */
class MakInversionParser {
public:
	/**
	 * Constructeur en ecriture par defaut 
	 */
	MakInversionParser();

	/**
	 * Constructeur en lecture par defaut
	 */
	MakInversionParser(std::string absoluteFileNameToMakOutputForInversion);

	/**
	 * Destructeur par defaut
	 */
	virtual ~MakInversionParser();

	/**
	 * Fonction permettant de recuperer tous les attributs 'isInversable' du fichier
	 */
	std::vector<std::string> getInversibles();

	/**
	 * Fonction permettant de recuperer tous les attributs 'name' du fichier
	 */
	std::vector<std::string> getNames();

	/**
	 * Fonction permettant de recuperer tous les attributs 'name' du fichier
	 */
	std::vector<std::string> getValues();

	/**
	 * Fonction d'ecriture du fichier mak_inv.xml
	 */
	void writeMak_invXML(	std::string absoluteFilePath,
							std::vector<std::string> commentsToWrite,
							std::vector<float> valuesToWrite,
							std::vector<bool> vsIsInversible);

private:
	
	/**
	 * Vecteur contenant le nom des parametres
	 */
	std::vector<std::string> _vsNames;

	/**
	 * Vecteur contenant la valeur des parametres
	 */
	std::vector<std::string> _vsValues;

	/**
	 * Vecteur contenant une information permettant de savoir si on inverse le parametre
	 */
	std::vector<std::string> _vsInversible;

	/**
	 * Lecture et stockage des informations relative au fichier makOutputForInversion.xml
	 */
	bool readMakOutputForInversion(std::string& path);
};

#endif /*MAK_INVPARSER_H_*/
