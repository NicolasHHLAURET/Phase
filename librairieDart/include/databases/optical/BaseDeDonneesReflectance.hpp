/*
 * BaseDeDonneesReflectance.hpp
 *
 *  Created on: 19 mai 2014
 *      Author: guilleuxj
 */

#ifndef BASEDEDONNEESREFLECTANCE_HPP_
#define BASEDEDONNEESREFLECTANCE_HPP_

#include <map>
#include <math.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <boost/algorithm/string/predicate.hpp>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include "FichierProperties.hpp"
#include "Conversions.hpp"
#include "../lut/object/ObjectDatabase.hpp"
#include "../../libDartEnv/include/DartEnv.h"

using namespace soci;

class BaseDeDonneesReflectance {
private:
	std::string _pathToDatabase;
	backend_factory const &_sqlBackend;
	session _session;

public:
	BaseDeDonneesReflectance(const std::string & cheminVersFichierBDD) :
			_sqlBackend(*soci::factory_sqlite3()) {
		_pathToDatabase = std::string("dbname=\"") + cheminVersFichierBDD + std::string("\" timeout=600");
	}

	~BaseDeDonneesReflectance() {
		this->closeConnexion();
	}

	/**
	 * Ouvre une connexion vers la base de données.
	 */
	void openConnexion();

	/**
	 * Ferme la connexion vers la base de données.
	 */
	void closeConnexion();

	/**
	 * Check l'intégrité de la base de données
	 * Throw l'exception levée et insulte l'utilisateur si la base n'est pas valide/est modifiée
	 */
	void checkIntegrity(const std::string & nomTable);

	std::string getComments(const std::string & tableName);
	int getNbLignesTable(const std::string & tableName);
	std::vector<double> getLigneTable(	const std::string & tableName,
									const int & numLigne);

	double getLambdaMin(const std::string & tableName);
	double getLambdaMax(const std::string & tableName);

	void getWavelengthForTable(const std::string &tableName, std::vector<double> &wavelength);
	void getReflectanceForTable(const std::string &tableName, std::vector<double> &reflectance);

	static std::string int2str(const int &number) ;

};

#endif /* BASEDEDONNEESREFLECTANCE_HPP_ */

