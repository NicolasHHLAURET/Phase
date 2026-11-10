/*
 * BaseDeDonneesReflectance.cpp
 *
 *  Created on: 19 mai 2014
 *      Author: guilleuxj
 */

#include "../include/databases/optical/BaseDeDonneesReflectance.hpp"
#include <string.h>

using namespace std;

void BaseDeDonneesReflectance::openConnexion() {
	_session.open(_sqlBackend, _pathToDatabase);
	_session << "PRAGMA journal_mode = OFF";
	_session << "PRAGMA synchronous = OFF";
	_session << "PRAGMA temp_store = MEMORY";
}

void BaseDeDonneesReflectance::closeConnexion() {
	_session.close();
}

string BaseDeDonneesReflectance::getComments(const string &tableName) {
	string comments = "";
	try {
		string requete = "SELECT " + tableName + " FROM _comments ;";
		_session << requete, into(comments);
	} catch (const soci_error & se) {
		cerr << "Error : error in read comments" << endl;
		_session.close();
		throw se;
	}
	return comments;
}

void BaseDeDonneesReflectance::getWavelengthForTable(	const string &tableName,
														vector<double> &wavelength) {
	stringstream requete;
	try {
		requete << " SELECT wavelength ";
		requete << " FROM " << tableName;
		_session << requete.str(), into(wavelength);
	} catch (const soci_error & se) {
		cerr << "Reflectance Database Error : error when try to get wavelength for table : " << tableName << endl;
		_session.close();
		throw se;
	}
}

void BaseDeDonneesReflectance::getReflectanceForTable(	const string &tableName,
														vector<double> &reflectance) {
	stringstream requete;
	try {
		requete << " SELECT reflectance ";
		requete << " FROM " << tableName;
		_session << requete.str(), into(reflectance);
	} catch (const soci_error & se) {
		cerr << "Reflectance Database Error : error when try to get reflectance for table : " << tableName << endl;
		_session.close();
		throw se;
	}
}

int BaseDeDonneesReflectance::getNbLignesTable(const string & tableName) {
	int nbLignes = -1;
	try {
		string requete = "SELECT count(*) FROM " + tableName + " ;";
		_session << requete, into(nbLignes);
	} catch (const soci_error & se) {
		cerr << "Error : error in read nblignes" << endl;
		_session.close();
		throw se;
	}
	return nbLignes;
}

string BaseDeDonneesReflectance::int2str(const int &number) {
	stringstream fluxString;
	fluxString << number;
	return fluxString.str();
}

vector<double> BaseDeDonneesReflectance::getLigneTable(	const string & tableName,
														const int &numLigne) {
	vector<double> ligne(2);
	try {
		string requete = "SELECT wavelength, reflectance FROM " + tableName + " LIMIT 1 OFFSET " + int2str(numLigne) + " ;";
		_session << requete, into(ligne[0]), into(ligne[1]);
	} catch (const soci_error & se) {
		cerr << "Error : error in read line of database" << endl;
		_session.close();
		throw se;
	}
	return ligne;
}

/**
 * Détecte changement dans la base de données réflectance et tire les oreilles concernées ;)
 * (A augmenter pour des cas différents de l'inversion)
 * @param: la table qu'on veut lire dans la base
 * @exception: throw l'exception qui à causée l'erreur (pour interception éventuelle)
 */
void BaseDeDonneesReflectance::checkIntegrity(const string &nomTable) {
	try {
		_session << "SELECT 42 FROM " << nomTable << " WHERE wavelength=42 AND reflectance=42 ;";
	} catch (std::exception &e) { // Si quelque chose est intercepté c'est que l'intégrité est violée -> on gueule
		cerr << "An unexpecting are occur when checking integrity of reflectance database." << endl;
		cerr << "Check if this path is path to your selected database: " << _pathToDatabase << endl;
		cerr << "Check if this ground are existing in database: " << nomTable << endl;
		cerr << "Else, this error are caused by modification in reflectance database !!" << endl;
		throw e; // Propagation
	}
}

double BaseDeDonneesReflectance::getLambdaMin(const string &tableName) {
	double lambdaMin = -1;
	try {
		string requete = "SELECT MIN(wavelength) FROM " + tableName + " ;";
		_session << requete, into(lambdaMin);
	} catch (const soci_error & se) {
		cerr << "Error : error in read lambdaMin" << endl;
		_session.close();
		throw se;
	}
	return lambdaMin;
}

double BaseDeDonneesReflectance::getLambdaMax(const string &tableName) {
	double lambdaMax = -1;
	try {
		string requete = "SELECT MAX(wavelength) FROM " + tableName + " ;";
		_session << requete, into(lambdaMax);
	} catch (const soci_error & se) {
		cerr << "Error : error in read lambdaMax" << endl;
		_session.close();
		throw se;
	}
	return lambdaMax;
}
