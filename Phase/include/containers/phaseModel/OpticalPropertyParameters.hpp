/*
 * OpticalPropertyParameters.hpp
 *
 *  Created on: 6 oct. 2015
 *      Author: guilleuxj
 */

#ifndef OPTICALPROPERTYPARAMETERS_HPP_
#define OPTICALPROPERTYPARAMETERS_HPP_

#include "../../database/model/PhaseDatabaseParameters.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

/**
 * 1 vector<PhaseDatabaseParameters> == 1 phase function
 * for each (Property : vector<OpticalPropertyParameters>) {
 * 	for each (Database : vector<boostshared_ptr<PhaseDatabaseParameters> >) {
 * 	 // read
 * 	}
 * 	// compute phase property
 * 	// write phase property
 * }
 */

enum OpticalPropertyType {
	LAMBERTIAN_PROPERTY = 41, HAPKE_PROPERTY = 42, HAPKE2_PROPERTY = 48, RPV_PROPERTY = 43, FOLIAR_PROPERTY = 44, WATER_PROPERTY = 45, PHASE_EXTERN_PROPERTY = 46, SPECULAR_PROPERTY = 47
};

class OpticalPropertyParameters {
private:
	std::vector<shared_ptr<PhaseDatabaseParameters> > _opticalPropertyParameters;
	OpticalPropertyType _opticalPropertyType;
	unsigned int _modelIndex;
	std::string _modelName;
	bool _isSpecular = false;
	int _indexOrder = 0;

public:
	OpticalPropertyParameters(OpticalPropertyType opticalPropertyType,
								unsigned int modelIndex,
								const std::string & modelName,
								int indexOrder = 0)
	:_opticalPropertyType(opticalPropertyType), _modelIndex(modelIndex), _modelName(modelName), _isSpecular(false), _indexOrder(indexOrder), _useDirectionalDirectTransmittance(false), _angularStep(1.)

	{
		_opticalPropertyParameters.clear();
		_opticalPropertyParameters.reserve(4); // Au pire, on en crèveras pas si il y pas de spéculaire ~~
	}

	void addDatabaseToProperty(const shared_ptr<PhaseDatabaseParameters>& databaseModelProperty) {
		_opticalPropertyParameters.push_back(databaseModelProperty);
	}

	const unsigned int getNbDatabaseForProperty() {
		return _opticalPropertyParameters.size();
	}

	shared_ptr<PhaseDatabaseParameters> getDatabaseForProperty(const unsigned int modelIndex) const {
		return _opticalPropertyParameters.at(modelIndex);
	}

	const OpticalPropertyType & getOpticalPropertyType() const {
		return _opticalPropertyType;
	}

	shared_ptr<PhaseDatabaseParameters> getDatabaseParameters(PropertyDatabase database) {
		std::vector<shared_ptr<PhaseDatabaseParameters> >::const_iterator endIterator = _opticalPropertyParameters.end();
		for (std::vector<shared_ptr<PhaseDatabaseParameters> >::const_iterator iterator = _opticalPropertyParameters.begin();
				iterator != endIterator; ++iterator) {
			if (iterator->get()->getDatabaseType() == database) {
				return *iterator;
			}
		}
		return shared_ptr<PhaseDatabaseParameters>();
	}

	const unsigned int & getModelIndex() const {
		return _modelIndex;
	}
	
	const int & getIndexOrder() const {
		return _indexOrder;
	}

	const std::string& getModelName() const {
		return _modelName;
	}
	
	void setModelName(const std::string& modelName) {
		_modelName = modelName;
	}

	bool isSpecular() const {
		return _isSpecular;
	}

	void setSpecular(bool isSpecular) {
		this->_isSpecular = isSpecular;
	}

	void setDirectionalDirectTransmittance(bool state, const std::string & filePath) {
		_useDirectionalDirectTransmittance = state;
		_ddtFilePath = filePath;
	}

	bool useDirectionalDirectTransmittance() const {
		return _useDirectionalDirectTransmittance;
	}
	const std::string & getDirectionalDirectTransmittanceFilePath() const {
		return _ddtFilePath;
	}

	bool _useDirectionalDirectTransmittance;
	std::string _ddtFilePath;
	float _angularStep; // degrees
};

#endif /* OPTICALPROPERTYPARAMETERS_HPP_ */
