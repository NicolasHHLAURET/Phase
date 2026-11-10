/*
 * FoliarTemperatureDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef FOLIARTEMPERATUREDATABASEPARAMETERS_HPP_
#define FOLIARTEMPERATUREDATABASEPARAMETERS_HPP_

using namespace std;

class FoliarTemperatureDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	FoliarTemperatureDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("transmittance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return TEMPERATURE_FOLIAR_DATABASE;
	}

	const double & getFoliarTransmittanceTemperature() {
		return _parametersValues[0][0];
	}
};




#endif /* FOLIARTEMPERATUREDATABASEPARAMETERS_HPP_ */
