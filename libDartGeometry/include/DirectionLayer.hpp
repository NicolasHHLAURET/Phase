/*
 * DirectionLayer.hpp
 *
 *  Created on: Apr 17, 2015
 *      Author: yint
 */

#ifndef INCLUDE_DIRECTIONLAYER_HPP_
#define INCLUDE_DIRECTIONLAYER_HPP_

#include "DirectionRoot.hpp"

class DirectionLayer {
public:

	DirectionLayer(double const& bTheta, double const& eTheta, double const& cTheta, const DirectionRoot::Unit unit = DirectionRoot::RADIANS):
		_begin_theta(bTheta), _end_theta(eTheta), _center_theta(cTheta), _delta_theta(_end_theta-_begin_theta), _max_solid_angle(0), _sqrt_max_solid_angle(0), _dir_indices(vector<unsigned int>()), _unit(unit), _property(DirectionRoot::DISCRETE), _shapePerDir(DirectionRoot::RECTANGLE)
	{
	}

	double const& getBeginTheta() const;

	void setBeginTheta(double const& beginTheta);

	double const& getDeltaTheta() const;

	void setDeltaTheta(double const& deltaTheta);

	double const& getEndTheta() const;

	void setEndTheta(double const& endTheta);

	double const& getCenterTheta() const;

	void setCenterTheta(double const& centerTheta);

	double const& getMaxSolidAngle() const;

	double const& getSqrtMaxSolidAngle() const;

	void setMaxSolidAngle(double const& maxSA);

	vector<unsigned int> const& getDirIndices() const;

	void addDirIndex(unsigned int const& ind);

	DirectionRoot::Property const& getProperty() const;

	void setProperty(DirectionRoot::Property const& property);

	DirectionRoot::Shape const& getShape() const;

	void setShape(DirectionRoot::Shape const& shape);

private:
	double _begin_theta;
	double _end_theta;
	double _center_theta;
	double _delta_theta;
	double _max_solid_angle;
	double _sqrt_max_solid_angle;
	vector<unsigned int> _dir_indices;
	DirectionRoot::Unit _unit;
	DirectionRoot::Property _property;
	DirectionRoot::Shape _shapePerDir;
};


#endif /* INCLUDE_DIRECTIONLAYER_HPP_ */
