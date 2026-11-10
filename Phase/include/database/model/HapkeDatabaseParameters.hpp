/*
 * HapkeDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef HAPKEDATABASEPARAMETER_HPP_
#define HAPKEDATABASEPARAMETER_HPP_

using namespace std;

class HapkeDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	HapkeDatabaseParameters(const string & databaseName,
							const string & databasePath,
							const string & modelName,
							const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("w");
		_databaseColumChoices.push_back("c1");
		_databaseColumChoices.push_back("c2");
		_databaseColumChoices.push_back("c3");
		_databaseColumChoices.push_back("c4");
		_databaseColumChoices.push_back("h1");
		_databaseColumChoices.push_back("h2");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return HAPKE_DATABASE;
	}

	const double & getHapkeWForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getHapkeC1ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getHapkeC2ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getHapkeC3ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}

	const double & getHapkeC4ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][4];
	}

	const double & getHapkeH1ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][5];
	}

	const double & getHapkeH2ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][6];
	}
};


class Hapke2DatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	Hapke2DatabaseParameters(const string & databaseName,
							const string & databasePath,
							const string & modelName,
							const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("w");
		_databaseColumChoices.push_back("c");
		_databaseColumChoices.push_back("b");
		_databaseColumChoices.push_back("h");
		_databaseColumChoices.push_back("b0");
		_databaseColumChoices.push_back("theta");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return HAPKE2_DATABASE;
	}


	const double & getHapke2WForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getHapke2CForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getHapke2BForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getHapke2HForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}

	const double & getHapke2B0ForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][4];
	}

	const double & getHapke2ThForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][5];
	}

};
#endif /* HAPKEDATABASEPARAMETER_HPP_ */
