/*
 * SpotLight.hpp
 *
 *  Created on: Jul 2, 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_LIGHTS_SPOTLIGHT_HPP_
#define INCLUDE_CONTAINERS_LIGHTS_SPOTLIGHT_HPP_

#include "Light.hpp"
#include "Point3.hpp"

class SpotLight: public Light {
private:
	Point3<double> _position;
	double _overture;

public:
	SpotLight(	const Point3<double> & position,
				double fov,
				const Vecteur<double> & cartesianDirection) :
			_position(position), _overture(fov) {
		_direction.setDirectionCartesienne(cartesianDirection.normaliser());
	}

	SpotLight(	const Point3<double> & position,
				double fov,
				double zenith,
				double azimuth) :
			_position(position), _overture(fov) {
		_direction.setCenterTheta(zenith * M_PI / 180);
		_direction.setCenterPhi(azimuth * M_PI / 180);
		_direction.calculerCoordonneesCartesiennes();
	}

	SpotLight(	const Point3<double> & position,
				double fov,
				DirectionCartesienne::ModeTaitBryan rotationMode,
				double angle1,
				double angle2,
				double angle3,
				bool invertDirection) :
			_position(position), _overture(fov) {
		_direction.setDirectionCartesienne(DirectionCartesienne::convertTaitBryanToCartesian(rotationMode, angle1, angle2, angle3));
		if (invertDirection) {
			_direction.setDirectionCartesienne(_direction.getDirectionCartesienne().negate());
		}
	}

	virtual shared_ptr<Light> clone() const {
		return shared_ptr<Light>(new SpotLight(*this));
	}

	virtual LightType getClass() const {
		return LightType::SPOT;
	}

	double getOverture() const {
		return _overture;
	}

	const Point3<double> & getPosition() const {
		return _position;
	}
};

#endif /* INCLUDE_CONTAINERS_LIGHTS_SPOTLIGHT_HPP_ */
