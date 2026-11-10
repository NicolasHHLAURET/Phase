/*
 * Light.hpp
 *
 *  Created on: Jul 2, 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_LIGHTS_LIGHT_HPP_
#define INCLUDE_CONTAINERS_LIGHTS_LIGHT_HPP_

#include <vector>
#include "../DirectionCartesienne.hpp"
#include "PhaseProperty.hpp"

class Light {
public:
	enum LightType {
		UNIDIRECTIONAL,
		SPHERE,
		SPOT
	};

protected:
	DirectionCartesienne _direction;
	std::vector<double> _irradiances;

public:
	Light() {}

	Light(const std::vector<double> & irradiances): _irradiances(irradiances) {

	}

	virtual ~Light() {

	}

	virtual shared_ptr<Light> clone() const = 0;

	void initIrradianceThroughDatabase(const std::string & irradianceDatabaseAbsoluteFilePath, const std::string & irradianceDatabaseTableName, const std::string & irradianceColumnName) {
		_irradiances.resize(PhaseProperty::_userSpectralBands.size(), 0.);

		DartDatabaseManager databaseManager(irradianceDatabaseAbsoluteFilePath);
		if (databaseManager.connectDatabase()) {
			std::vector<double> databaseWavelength, databaseIrradiance;
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::_userSpectralBands.size(); spectralBandIndex++) {
				PhaseDatabaseDAO::readIrradianceAtmosphericParametersPerBand(databaseManager, irradianceDatabaseTableName, irradianceColumnName,
					PhaseProperty::_userSpectralBands[spectralBandIndex], databaseWavelength, databaseIrradiance);

				int maxId = databaseIrradiance.size() - 1;

				// Cas de la borne minimum (Interpole la valeur min de la bande pour correspondre à la valeur utilisateur)
				databaseIrradiance[0] = Interpolation_lineaire(databaseWavelength[0], databaseWavelength[1], PhaseProperty::_userSpectralBands[spectralBandIndex].lowerLambdaValue,
					databaseIrradiance[0], databaseIrradiance[1]);
				databaseWavelength[0] = PhaseProperty::_userSpectralBands[spectralBandIndex].lowerLambdaValue;

				// Cas de la borne maximum (Interpole la valeur max de la bande pour correspondre à la valeur utilisateur)
				databaseIrradiance[maxId] = Interpolation_lineaire(databaseWavelength[maxId - 1], databaseWavelength[maxId],
					PhaseProperty::_userSpectralBands[spectralBandIndex].upperLambdaValue, databaseIrradiance[maxId - 1], databaseIrradiance[maxId]);
				databaseWavelength[maxId] = PhaseProperty::_userSpectralBands[spectralBandIndex].upperLambdaValue;

				_irradiances[spectralBandIndex] = PhaseProperty::computeMeanIrradianceWithTrapezoidalMethod(databaseWavelength.size(), spectralBandIndex, databaseWavelength, databaseIrradiance);
			}
			databaseManager.deconnectDatabase();
		}
	}

	const std::vector<double> & getIrradiances() const {
		return _irradiances;
	}
	const double & getIrradiance(unsigned int spectralBandIndex) const {
		return _irradiances[spectralBandIndex];
	}
	const DirectionCartesienne & getDirection() const {
		return _direction;
	}

	void setDirection(const DirectionCartesienne& direction) {
		_direction = direction;
	}
	void setIrradiances(const std::vector<double>& irradiances) {
		_irradiances = irradiances;
	}

	virtual LightType getClass() const = 0;
};

#endif /* INCLUDE_CONTAINERS_LIGHTS_LIGHT_HPP_ */
