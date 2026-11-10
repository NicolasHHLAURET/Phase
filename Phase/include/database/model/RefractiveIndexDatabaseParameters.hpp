/*
 * RefractiveIndexDatabaseParameters.hpp
 *
 *  Created on: 14 mars. 2022
 *      Author: yingjie wang
 */

#ifndef REFRACTIVEINDEXCOLUMNCHOICE_HPP_
#define REFRACTIVEINDEXCOLUMNCHOICE_HPP_

using namespace std;

class RefractiveIndexDatabaseParameters: public PhaseDatabaseParameters {
private:

public:
	RefractiveIndexDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands,
								const bool & hasDifferentModel,
								const string & modelName2) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_databaseColumChoices.push_back("n");
		_databaseColumChoices.push_back("n");
		_databaseColumChoices.push_back("k");
		_databaseColumChoices.push_back("k");

		// second model - override 2nd reflectance & diffuse transmittance
		_hasBottomModel = hasDifferentModel;
		_bottomDatabaseName = databaseName;
		_bottomDatabasePath = databasePath;
		_bottomModel = modelName2;

		_bottomDatabaseColumnChoices.push_back("wavelength");
		_bottomDatabaseColumnChoices.push_back("n");
		_bottomDatabaseColumnChoices.push_back("k");
		_bottomOverrideResultPosition.push_back(0);
		_bottomOverrideResultPosition.push_back(1);
		_bottomOverrideResultPosition.push_back(4);

		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return REFRACTIVEINDEX_DATABASE;
	}

	const double & getRefractionIndex2(const unsigned int & bandNumber){
		return _parametersValues[bandNumber][0];
	}

	const double & getRefractionIndex(const unsigned int & bandNumber){
		return _parametersValues[bandNumber][1];
	}

	const double & getAbsorptionCoeff(const unsigned int & bandNumber){
		return _parametersValues[bandNumber][2];
	}

	const double & getAbsorptionCoeff2(const unsigned int & bandNumber){
		return _parametersValues[bandNumber][3];
	}

};

#endif /* REFRACTIVEINDEXCOLUMNCHOICE_HPP_ */
