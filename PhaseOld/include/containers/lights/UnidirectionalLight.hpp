/*
 * UnidirectionalLight.hpp
 *
 *  Created on: Jul 2, 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_LIGHTS_UNIDIRECTIONALLIGHT_HPP_
#define INCLUDE_CONTAINERS_LIGHTS_UNIDIRECTIONALLIGHT_HPP_

#include "Light.hpp"

class UnidirectionalLight: public Light {
public:
	UnidirectionalLight(const Vecteur<double> & cartesianDirection) {
		_direction.setDirectionCartesienne(cartesianDirection.normaliser());
	}

	UnidirectionalLight(double zenith, double azimuth) {
		_direction.setCenterTheta(zenith * M_PI / 180);
		_direction.setCenterPhi(azimuth * M_PI / 180);
		_direction.calculerCoordonneesCartesiennes();
	}

	UnidirectionalLight(DirectionCartesienne::ModeTaitBryan rotationMode,
							double angle1,
							double angle2,
							double angle3,
							bool invertDirection) {
		_direction.setDirectionCartesienne(DirectionCartesienne::convertTaitBryanToCartesian(rotationMode, angle1, angle2, angle3));
		if (invertDirection) {
			_direction.setDirectionCartesienne(_direction.getDirectionCartesienne().negate());
		}
	}

	virtual shared_ptr<Light> clone() const {
		return shared_ptr<Light>(new UnidirectionalLight(*this));
	}

	virtual LightType getClass() const {
		return LightType::UNIDIRECTIONAL;
	}
};

#endif /* INCLUDE_CONTAINERS_LIGHTS_UNIDIRECTIONALLIGHT_HPP_ */
