/*
 * TransmittanceDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef TRANSMITTANCEDATABASEPARAMETERS_HPP_
#define TRANSMITTANCEDATABASEPARAMETERS_HPP_


using namespace std;

class TransmittanceDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	TransmittanceDatabaseParameters(const string & databaseName,
							const string & databasePath,
							const string & modelName,
							const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("direct_transmittance");
		_databaseColumChoices.push_back("diffuse_transmittance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return TRANSMITTANCE_DATABASE;
	}

	const double & getDirectTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getDiffuseTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

};


#endif /* TRANSMITTANCEDATABASEPARAMETERS_HPP_ */
