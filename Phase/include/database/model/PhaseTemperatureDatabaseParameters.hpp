/*
 * PhaseTemperatureDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef PHASETEMPERATUREDATABASEPARAMETERS_HPP_
#define PHASETEMPERATUREDATABASEPARAMETERS_HPP_

using namespace std;

class PhaseTemperatureDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	PhaseTemperatureDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("direct_transmittance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return TEMPERATURE_DATABASE;
	}

	const double & getDirectTransmittanceTemperature() {
		return _parametersValues[0][0];
	}
};



#endif /* PHASETEMPERATUREDATABASEPARAMETERS_HPP_ */
