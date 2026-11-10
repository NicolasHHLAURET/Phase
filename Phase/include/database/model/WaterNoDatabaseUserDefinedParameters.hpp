/*
 * WaterNoDatabaseUserDefinedParameters.hpp
 *
 *  Created on: 8 oct. 2015
 *      Author: guilleuxj
 */

#ifndef WATERNODATABASEPARAMETERS_HPP_
#define WATERNODATABASEPARAMETERS_HPP_

using namespace std;

class WaterNoDatabaseUserDefinedParameters: public WaterDatabaseParameters {
private:

public:
	WaterNoDatabaseUserDefinedParameters(const unsigned int numberOfBands) :
		WaterDatabaseParameters("", "", "", numberOfBands) {
	}

	const PropertyDatabase getDatabaseType() {
		if (_refractionOrientation == UP){
			return WATER_NO_DATABASE_USER_DEFINED_UP;
		}else{
			return WATER_NO_DATABASE_USER_DEFINED_DOWN;
		}	}

	void setRefractionForBand(const unsigned int & bandNumber, double & refractionValue) {
		_parametersValues[bandNumber][0] = refractionValue;
	}
	
};




#endif /* WATERNODATABASEPARAMETERS_HPP_ */
