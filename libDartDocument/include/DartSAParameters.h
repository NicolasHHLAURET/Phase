/*! Time-stamp: <@(#) DartSAParameters.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : DartSAParameters.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 10/02/2005
 *
 *  Description : Classe statique permettant des acces en lecture a des fichiers xml.
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
 * CVS $Log : DartSAParameters.h,v$
 */

#ifndef _DARTSAPARAMETERS_H_
#define _DARTSAPARAMETERS_H_

#include <map>
#include <string>
#include <iostream>

#include "DartException.h"
#include "DartXML.hpp"
using namespace std;

/**
 *  @ingroup DartDocument
 * 
 *  Classe statique permettant des acces en lecture a des fichiers xml.
 * 
 */

class DartSAParameters {
public:
	
	/**
	 * Constructeur.
	 */
	DartSAParameters();

	/**
	 * Destructeur.
	 */
	virtual ~DartSAParameters();

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param double &dValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type double caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					double &dValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param bool &bValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type bool caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					bool &bValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param int &iValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type int caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					int &iValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param long int &liValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type long int caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					long int &liValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param unsigned int &uiValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type unsigned int caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					unsigned int &uiValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param float &uiValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type float caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					float &uiValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param string &sValue : variable dans laquelle on stocke le resultat obtenu.
	 * 
	 * Permet de recuperer la valeur d'un element de type string caracterise 
	 * par le chemin sParameterPath.
	 */
	static void get(string sParameterPath,
					string &sValue) throw (DartException,BadFormatException);

	/**
	 * 
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param T &Value : variable dans laquelle on stocke le resultat obtenu.
	 *
	 * Permet de recuperer la valeur d'un element caracterise par le chemin sParameterPath.
	 */
	template<typename T>
	static void get(string sParameterPath,
					T &Value) throw (DartException,BadFormatException) {
		string sValue = "";
		get(sParameterPath, sValue);
		istringstream iss(sValue);
		iss >> Value;
	}

	/**
	 *
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param string &Value : variable string dans laquelle on stocke la string obtenue
	 *
	 * Permet de recuperer la valeur d'un element caracterise par le chemin sParameterPath.
	 */
	static void getStringWithPossibleWhiteSpace(string sParameterPath,
												std::string &sValue) throw (DartException,BadFormatException);

	/**
	 *
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 * @param T &Value : variable dans laquelle on stocke le resultat obtenu.
	 *
	 * Permet de recuperer la valeur d'un element caracterise par le chemin sParameterPath.
	 */
	template<typename T>
	static void get(string fileName,
					string sParameterPath,
					T &Value) throw (DartException,BadFormatException) {
		Value = DartXML::get<T>(fileName, sParameterPath);
	}
	
	/**
	 *
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 *
	 * Permet de recuperer la valeur d'un element caracterise par le chemin sParameterPath.
	 */
	template<typename T>
	static T get(string sParameterPath) throw (DartException,BadFormatException) {
		T Value;
		string sValue = "";
		get(sParameterPath, sValue);
		istringstream iss(sValue);
		iss >> Value;
		return Value;
	}
	
	/**
	 *
	 * @param string sParameterPath : chemin vers la variable que l'on veut obtenir.
	 *
	 * Permet de recuperer la valeur d'un element caracterise par le chemin sParameterPath.
	 */
	template<typename T>
	static T get(	string fileName,
					string sParameterPath) throw (DartException,BadFormatException) {
		return DartXML::get<T>(fileName, sParameterPath);
	}
	
	/**
	 *
	 * @param string sPropertiesNodePath : chemin vers la variable que l'on veut obtenir.
	 * 
	 * @return le nombre de fois ou apparait le noeud, caracterise par la string 
	 * 		   en parametre, dans le fichier courant.
	 */
	static int getNbOf(string sPropertiesNodePath);
	static int getNbOf(	string nomXML,
						string sPropertiesNodePath);

	/**
	 * Returns the key reading index if the key is part of a list.
	 * Returns -1 if the index is not found.
	 */
	static int getKeyIndex(string sPropertiesNodePath);
	/**
	 * Returns the key reading index if the key is part of a list.
	 * Returns -1 if the index is not found.
	 */
	static int getKeyIndex(	string nomXML,
							string sPropertiesNodePath);

	static bool exists(string sPropertiesNodePath);
	static bool exists(	string nomXML,
						string sPropertiesNodePath);

	/**
	 * 
	 * @param string path : chemin de la simulation courante.
	 * 
	 * Permet d'initialiser _mapFilesLoaded avec les fichiers traites 
	 * actuellement dans DART.
	 * Permet egalement de stocker le chemin de la simulation courante passe
	 * en parametre. 
	 */
	static void initialize(string path);

	static void openFile(string fileName);

	/**
	 * Parse un flux de donnees.
	 * @param stream Le flux a parser
	 * @param name Le nom du fichier
	 */
	static void parseStream(std::istream & stream,
							const string& name);

	/**
	 * Libere les elements presents dans les map.
	 */
	static void terminate(void);

	/**
	 * Modifie la valeur associee a une cle.
	 * @param key Cle a modifier
	 * @param value Nouvelle valeur de la cle
	 */
	static void set(const string & key,
					const string & value);

	/**
	 * Retourne toutes les cles de la map keyMap.
	 * @return Un vecteur contenant toutes les cles de la map
	 */
	static vector<string> getAllKeys();

private:
	
	/**
	 * Contient le chemin de la simulation courante.
	 */
	static string currentPath;

	/**
	 * Recupere et retourne le nom du fichier a traiter en fonction du premier 
	 * element(decoupage suivant les '.') de la chaine en parametre.
	 */
	static string getFileNameFromDataPath(string sFileName);
};

#endif //_DARTSAPARAMETERS_H_
