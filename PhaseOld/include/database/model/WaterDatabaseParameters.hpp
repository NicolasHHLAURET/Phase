/*
 * WaterDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef WATERDATABASEPARAMETERS_HPP_
#define WATERDATABASEPARAMETERS_HPP_

using namespace std;

enum RefractionOrientation {
	UP=42,
	DOWN=69
};

class WaterDatabaseParameters: public PhaseDatabaseParameters {
protected:
	RefractionOrientation _refractionOrientation;
public:
	WaterDatabaseParameters(const string & databaseName,
							const string & databasePath,
							const string & modelName,
							const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands), _refractionOrientation(UP) {
		_databaseColumChoices.push_back("refraction");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		if (_refractionOrientation == UP){
			return WATER_DATABASE_UP;
		}else{
			return WATER_DATABASE_DOWN;
		}
	}

	RefractionOrientation getRefractionOrientation() const {
		return _refractionOrientation;
	}

	void setRefractionOrientation(RefractionOrientation refractionOrientation) {
		_refractionOrientation = refractionOrientation;
	}

	const double & getrefractionForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}
};


#endif /* WATERDATABASEPARAMETERS_HPP_ */
