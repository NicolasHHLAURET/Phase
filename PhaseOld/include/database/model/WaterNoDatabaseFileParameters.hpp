/*
 * WaterNoDatabaseFileParameters.hpp
 *
 *  Created on: 8 oct. 2015
 *      Author: guilleuxj
 */

#ifndef WATERNODATABASEFILEPARAMETERS_HPP_
#define WATERNODATABASEFILEPARAMETERS_HPP_


using namespace std;

class WaterNoDatabaseFileParameters: public WaterDatabaseParameters {
private:
	string _waterFileModelName;

public:
	WaterNoDatabaseFileParameters(const unsigned int numberOfBands, string waterFile) :
		WaterDatabaseParameters("", "", "", numberOfBands), _waterFileModelName(waterFile) {
	}

	const PropertyDatabase getDatabaseType() {
		return WATER_NO_DATABASE_FILE;
	}

	const string& getWaterFileModelName() const {
		return _waterFileModelName;
	}

	void setWaterFileModelName(const string& waterFileModelName) {
		_waterFileModelName = waterFileModelName;
	}
};



#endif /* WATERNODATABASEFILEPARAMETERS_HPP_ */
