/*
 * LightSphere.hpp
 *
 *  Created on: Jul 2, 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_LIGHTS_LIGHTSPHERE_HPP_
#define INCLUDE_CONTAINERS_LIGHTS_LIGHTSPHERE_HPP_

#include "Light.hpp"
#include "Point3.hpp"

class LightSphere: public Light {
private:
	double _radius;
	Point3<double> _center;
	unsigned int _sampling;

public:
	LightSphere(double radius, Point3<double> position, unsigned int sampling): _radius(radius), _center(position), _sampling(sampling) {
		_direction.setDirectionCartesienne(position / position.getDistanceToOrigin() * -1.);
		// TODO: Conversion to spherical coordinates
		_direction.setCenterTheta(std::acos(-_direction.getDirectionCartesienne().getZ()));
	}
	LightSphere(double radius, double zenith, double azimuth, double distance, unsigned int sampling): _radius(radius), _sampling(sampling) {
		_direction.setCenterTheta(zenith * M_PI / 180);
		_direction.setCenterPhi(azimuth * M_PI / 180);
		_direction.calculerCoordonneesCartesiennes();
		_center = _direction.getDirectionCartesienne() * distance * -1.;
	}

	virtual shared_ptr<Light> clone() const {
		return shared_ptr<Light>(new LightSphere(*this));
	}

	const Point3<double> & getPosition() const {
		return _center;
	}
	void setPosition(const Point3<double> & position) {
		_center = position;
	}

	double getRadius() const {
		return _radius;
	}
	void setRadius(double radius) {
		_radius = radius;
	}
	
	unsigned int getSampling() const {
		return _sampling;
	}
	void setSampling(unsigned int sampling) {
		_sampling = sampling;
	}

	virtual LightType getClass() const {
		return LightType::SPHERE;
	}
};

#endif /* INCLUDE_CONTAINERS_LIGHTS_LIGHTSPHERE_HPP_ */
