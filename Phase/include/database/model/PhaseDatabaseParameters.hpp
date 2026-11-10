/*
 * PhaseDatabaseProperties.hpp
 *
 *  Created on: 5 oct. 2015
 *      Author: guilleuxj
 */

#ifndef PHASEDATABASEPROPERTIES_HPP_
#define PHASEDATABASEPROPERTIES_HPP_

#include "../../PhaseStructure.hpp"

// A cause des 42 cas particuliers alakon qui ne rentrent pas dans le moule, dsl :(
enum PropertyDatabase {
	LAMBERTIAN_DATABASE = 0,
	HAPKE_DATABASE = 1,
	HAPKE2_DATABASE = 13,
	RPV_DATABASE = 2,
	FOLIAR_DATABASE = 3,
	PHASE_EXTERN_DATABASE = 17,
	PHASE_EXTERN_DIRECTIONNAL_DATABASE = 18,
	WATER_DATABASE_UP = 23,
	WATER_DATABASE_DOWN = 24,
	WATER_NO_DATABASE_FILE = 33,
	WATER_NO_DATABASE_USER_DEFINED_UP = 42,
	WATER_NO_DATABASE_USER_DEFINED_DOWN = 43,
	SPECULAR_DATABASE = 69,
	REFRACTIVEINDEX_DATABASE = 70,
	TEMPERATURE_DATABASE = 123,
	TEMPERATURE_FOLIAR_DATABASE = 666,
	TRANSMITTANCE_DATABASE = 2042,
	SPECULAR_FOLIAR_DATABASE = 5816
};

// Classe virtuelle
class PhaseDatabaseParameters {
protected:
	std::string _databaseName;
	std::string _databasePath;
	std::string _databaseModel;
	std::vector<std::vector<double> > _parametersValues;

	bool _hasBottomModel;
	std::string _bottomDatabaseName;
	std::string _bottomDatabasePath;
	std::string _bottomModel;

	// Resize à postériori lorsque la classe concrète à été définie et qu'on connait le nombre de colonnes.
	// Meilleure solution existante ?
	std::vector<std::string> _databaseColumChoices;

	std::vector<std::string> _bottomDatabaseColumnChoices;
	// Position des résultats réécrit lorsque le modèle à une face inférieure.
	// Même taille que le _bottomDatabaseColumnChoices.
	std::vector<int> _bottomOverrideResultPosition;

	// True if the database need to be deleted at the end of the phase computation
	bool _selfDestructDatabase;
	bool _selfDestructBottomDatabase;

//	/**
//	 * Vecteur contenant les positions des résultats à ne pas interpolés par rapport aux bandes spectrales
//	 * ex: PhaseExtern où on ne doit pas interpoler les valeurs des angles de visée/incident par rapport aux bandes
//	 */
//	std::vector<int> _doNotInterpolateResult;

	void resizeParametersVector() {
		for (unsigned int bandNumber = 0; bandNumber < _parametersValues.size(); ++bandNumber) {
			_parametersValues[bandNumber].resize(getColumnChoices().size() - 1);
		}
	}

public:
	virtual ~PhaseDatabaseParameters() {}
	;

	const std::vector<std::string> & getColumnChoices() const {
		return _databaseColumChoices;
	}

	virtual const PropertyDatabase getDatabaseType() = 0;

	PhaseDatabaseParameters(const std::string & databaseName,
							const std::string & databasePath,
							const std::string & modelName,
							const unsigned int numberOfBands) :
			_databaseName(databaseName), _databasePath(databasePath), _databaseModel(modelName) {
		_parametersValues.resize(numberOfBands);
		_databaseColumChoices.push_back("wavelength");

		_hasBottomModel = false;
		_selfDestructDatabase = false;
		_selfDestructBottomDatabase = false;
		_bottomDatabaseName = "";
		_bottomDatabasePath = "";
		_bottomModel = "";
	}

	const bool & hasBottomModel() {
		return _hasBottomModel;
	}

	const std::string & getBottomDatabasePath() const {
		return _bottomDatabasePath;
	}

	const std::string & getBottomDatabaseName() const {
		return _bottomDatabaseName;
	}

	const std::string & getBottomDatabaseModel() const {
		return _bottomModel;
	}

	const std::vector<std::string> & getBottomColumnChoices() const {
		return _bottomDatabaseColumnChoices;
	}

	const std::vector<int> & getBottomOverrideResultPosition() const {
		return _bottomOverrideResultPosition;
	}

	PhaseDatabaseParameters(const std::string & databaseName,
							const std::string & databasePath,
							const std::string & modelName,
							const unsigned int numberOfBands,
							const bool & hasBottomModel,
							const std::string & bottomDatabaseName,
							const std::string & bottomDatabasePath,
							const std::string & bottomModel) :
			_databaseName(databaseName), _databasePath(databasePath), _databaseModel(modelName) {
		_parametersValues.resize(numberOfBands);
		_databaseColumChoices.push_back("wavelength");

		_hasBottomModel = hasBottomModel;
		_bottomDatabaseName = bottomDatabaseName;
		_bottomDatabasePath = bottomDatabasePath;
		_bottomModel = bottomModel;
		_bottomDatabaseColumnChoices.push_back("wavelength"); // ?
		_selfDestructDatabase = false;
		_selfDestructBottomDatabase = false;
	}

	const std::string & getDatabaseName() const {
		return _databaseName;
	}

	const std::string & getDatabasePath() const {
		return _databasePath;
	}

	const std::string & getDatabaseModel() const {
		return _databaseModel;
	}

	void setSelfDestructDatabase(const bool & selfDestructDatabase) {
		_selfDestructDatabase = selfDestructDatabase;
	}

	void setSelfDestructBottomDatabase(const bool & selfDestructBottomDatabase) {
		_selfDestructBottomDatabase = selfDestructBottomDatabase;
	}

	const bool & isSelfDestructDatabase() {
		return _selfDestructDatabase;
	}

	const bool & isSelfDestructBottomDatabase() {
		return _selfDestructBottomDatabase;
	}

	const std::vector<double> & getParametersValuesForBand(const unsigned int bandNumber) const {
		return _parametersValues.at(bandNumber);
	}

	void setParametersValuesForBand(const unsigned int bandNumber,
									const std::vector<double> & values) {
		_parametersValues[bandNumber] = values;
	}

//	/**
//	 * true si la valeur doit être interpolée selon les bandes, false sinon.
//	 */
//	bool isInterpolateValueIndex(const int index) const {
//		for (unsigned int i = 0; i < _doNotInterpolateResult.size(); ++i) {
//			if (_doNotInterpolateResult.at(i) == index) {
//				return true;
//			}
//		}
//		return false;
//	}
};

#endif /* PHASEDATABASEPROPERTIES_HPP_ */
