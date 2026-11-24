/*
 * SpecularFoliarDatabaseParameters.hpp
 *
 *  Created on: 9 oct. 2015
 *      Author: guilleuxj
 */

#ifndef SPECULARFOLIARDATABASEPARAMETERS_HPP_
#define SPECULARFOLIARDATABASEPARAMETERS_HPP_

using namespace std;

class SpecularFoliarDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	SpecularFoliarDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("bottom_refraction_index");
		_databaseColumChoices.push_back("top_refraction_index");
		_databaseColumChoices.push_back("bottom");
		_databaseColumChoices.push_back("top");
		_databaseColumChoices.push_back("bottom_index");
		_databaseColumChoices.push_back("top_index");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return SPECULAR_FOLIAR_DATABASE;
	}

	const double & getBottomRefractionIndexForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getTopRefractionIndexForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getBottomForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getTopForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}

	const double & getBottomIndexForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][4];
	}

	const double & getTopIndexForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][5];
	}

};



#endif /* SPECULARFOLIARDATABASEPARAMETERS_HPP_ */
