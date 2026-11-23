/*
 * LambertianDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef LAMBERTIANCOLUMNSCHOICE_HPP_
#define LAMBERTIANCOLUMNSCHOICE_HPP_

using namespace std;

class LambertianDatabaseParameters: public PhaseDatabaseParameters {
private:
	double _deviation; // Et ouais j'ai enfin reussi à te caser ! :p

public:
	LambertianDatabaseParameters(	const string & databaseName,
									const string & databasePath,
									const string & modelName,
									const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands), _deviation(0) {
		_databaseColumChoices.push_back("reflectance");
		_databaseColumChoices.push_back("direct_transmittance");
		_databaseColumChoices.push_back("diffuse_transmittance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return LAMBERTIAN_DATABASE;
	}

	double getDeviation() const {
		return _deviation;
	}

	void setDeviation(const double & deviation) {
		_deviation = deviation;
	}

	const double & getReflectanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getDirectTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getDiffuseTransmittanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}
};

#endif /* LAMBERTIANCOLUMNSCHOICE_HPP_ */
