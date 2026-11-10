/*
 * DirectionLayer.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: yint
 */

#include "DirectionLayer.hpp"


	double const& DirectionLayer::getBeginTheta() const {
		return _begin_theta;
	}

	void DirectionLayer::setBeginTheta(double const& beginTheta) {
		_begin_theta = beginTheta;
	}

	double const& DirectionLayer::getDeltaTheta() const {
		return _delta_theta;
	}

	void DirectionLayer::setDeltaTheta(double const& deltaTheta) {
		_delta_theta = deltaTheta;
	}

	double const& DirectionLayer::getEndTheta() const {
		return _end_theta;
	}

	void DirectionLayer::setEndTheta(double const& endTheta) {
		_end_theta = endTheta;
	}

	double const& DirectionLayer::getCenterTheta() const {
		return _center_theta;
	}

	void DirectionLayer::setCenterTheta(double const& centerTheta) {
		_center_theta = centerTheta;
	}

	double const& DirectionLayer::getMaxSolidAngle() const {
		return _max_solid_angle;
	}

	double const& DirectionLayer::getSqrtMaxSolidAngle() const {
		return _sqrt_max_solid_angle;
	}


	void DirectionLayer::setMaxSolidAngle(double const& maxSA) {
		_max_solid_angle=maxSA;
		_sqrt_max_solid_angle=sqrt(_max_solid_angle);
	}

	vector<unsigned int> const& DirectionLayer::getDirIndices() const {
		return _dir_indices;
	}

	void DirectionLayer::addDirIndex(unsigned int const& ind) {
		_dir_indices.push_back(ind);
	}

	DirectionRoot::Property const& DirectionLayer::getProperty() const {
		return _property;
	}

	void DirectionLayer::setProperty(DirectionRoot::Property const& property) {
		_property = property;
	}

	DirectionRoot::Shape const& DirectionLayer::getShape() const {
		return _shapePerDir;
	}

	void DirectionLayer::setShape(DirectionRoot::Shape const& shape) {
		_shapePerDir = shape;
	}

