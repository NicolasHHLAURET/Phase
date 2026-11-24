/*
 * FoliarDatabaseParameters.hpp
 *
 *  Created on: 9 oct. 2015
 *      Author: guilleuxj
 */

#ifndef FOLIARDATABASEPARAMETERS_HPP_
#define FOLIARDATABASEPARAMETERS_HPP_

using namespace std;

class FoliarDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	FoliarDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands,
								const bool & hasDifferentModelForBottom,
								const string & bottomDatabaseName,
								const string & bottomDatabasePath,
								const string & bottomModel) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {

		// Foliar Model
		_databaseColumChoices.push_back("reflectance");
		_databaseColumChoices.push_back("reflectance");
		_databaseColumChoices.push_back("diffuse_transmittance");
		_databaseColumChoices.push_back("diffuse_transmittance");

		// Bottom model - override 2nd reflectance & diffuse transmittance
		_hasBottomModel = hasDifferentModelForBottom;
		_bottomDatabaseName = bottomDatabaseName;
		_bottomDatabasePath = bottomDatabasePath;
		_bottomModel = bottomModel;
		_bottomDatabaseColumnChoices.push_back("wavelength");
		_bottomDatabaseColumnChoices.push_back("reflectance");
		_bottomDatabaseColumnChoices.push_back("diffuse_transmittance");
		_bottomOverrideResultPosition.push_back(0);
		_bottomOverrideResultPosition.push_back(1);
		_bottomOverrideResultPosition.push_back(4);

		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return FOLIAR_DATABASE;
	}

	const double & getBottomReflectanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getTopReflectanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getBottomTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}
};

#endif /* FOLIARDATABASEPARAMETERS_HPP_ */
