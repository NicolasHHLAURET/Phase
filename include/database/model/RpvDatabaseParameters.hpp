/*
 * RpvDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef RPVDATABASEPARAMETERS_HPP_
#define RPVDATABASEPARAMETERS_HPP_

using namespace std;

class RpvDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	RpvDatabaseParameters(const string & databaseName,
							const string & databasePath,
							const string & modelName,
							const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("reflectance");
		_databaseColumChoices.push_back("k");
		_databaseColumChoices.push_back("g");
		_databaseColumChoices.push_back("h");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return RPV_DATABASE;
	}

	const double & getReflectanceRho0ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getRpvKForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getRpvGForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getRpvHForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}
};



#endif /* RPVDATABASEPARAMETERS_HPP_ */
