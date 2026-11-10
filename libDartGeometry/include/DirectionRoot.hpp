/*
 * Direction.hpp
 *
 *  Created on: 27 January 2012
 *      Author: Tiangang Yin
 */

#ifndef DIRECTIONROOT_HPP_
#define DIRECTIONROOT_HPP_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sstream>
#include<algorithm>
#include<iostream>
#include<iomanip>

#include "GeneralDef.hpp"
#include "Lib_math.h"

#include "Doublet.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include<boost/shared_ptr.hpp>
    using namespace boost;
#endif

class DirectionRoot {
public:
	typedef enum Property {
		DISCRETE = 0, EMBEDDED = 1, OCCUPIED = 2, VIRTUAL = 3
	} Property;

	typedef enum Shape {
		RECTANGLE = 0, CIRCLE = 1
	} Shape;

	static std::string stringOfProperty(DirectionRoot::Property property) {
		switch (property) {
			case EMBEDDED:
				return "E";
			case OCCUPIED:
				return "O";
			case VIRTUAL:
				return "V";
			case DISCRETE:
			default:
				return "D";
		}
	}

	static DirectionRoot::Property propertyOfString(const std::string & property) {
		if (property == "E") {
			return EMBEDDED;
		}
		if (property == "O") {
			return OCCUPIED;
		}
		if (property == "V") {
			return VIRTUAL;
		}
		/* if (property == "D") */
		return DISCRETE;
	}

	static DirectionRoot::Shape shapeOfInt(unsigned int shape) {
		switch (shape) {
					case 0:
						return DirectionRoot::RECTANGLE;
					case 1:
						return DirectionRoot::CIRCLE;
					default:
						cerr<<"Error: Shape key int does not exist"<<endl;
						abort();
		}
	}

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
	static constexpr double PRECISION = 1e-14;
#else
	static const double PRECISION = 1e-14;
#endif

	typedef enum Unit {
		RADIANS = 0, DEGREES = 1,
	} Unit;

private:
	double _begin_theta;
	double _centerTheta;
	double _end_theta; //the begin and end zenith angle of the solid angle area.
	double _begin_phi;
	double _center_phi; //the center of the solid angle area.
	double _end_phi; //the begin and end azimuth angle of the solid angle area.
	double _solidAngle; // solid angle/omega
	double _cosThetaDOmega;
	unsigned int _angularSector; //The angular Sector;
	DirectionRoot::Property _property; // "D" for 'discrete', E for 'Embedded', O for 'Occupied', V for 'Virtual'
	DirectionRoot::Shape _shape;
	DirectionRoot::Unit _unit;
	bool _ifPhaseOnly;

public:
	// Constructors
	DirectionRoot(const DirectionRoot::Unit unit = RADIANS) :
			_begin_theta(0), _centerTheta(0), _end_theta(0), _begin_phi(0), _center_phi(0), _end_phi(0), _solidAngle(0), _cosThetaDOmega(0.1), _angularSector(
				0), _property(DirectionRoot::DISCRETE), _shape(DirectionRoot::RECTANGLE), _unit(unit), _ifPhaseOnly(false) {}
	DirectionRoot(	double theta,
					double phi,
					double solidAngle = 0.1,
					const DirectionRoot::Unit unit = RADIANS/*, const unsigned int angularSector = 0*/) :
			_begin_theta(theta), _centerTheta(theta), _end_theta(theta), _begin_phi(phi), _center_phi(phi), _end_phi(phi), _solidAngle(solidAngle), _cosThetaDOmega(
				0.1), _angularSector(9999), _property(DirectionRoot::VIRTUAL), _shape(DirectionRoot::RECTANGLE), _unit(unit), _ifPhaseOnly(false) {
		this->normalizeAngles();
	}
	DirectionRoot(	const double& theta_b,
					const double& theta_e,
					const double& phi_b,
					const double& phi_e,
					const DirectionRoot::Unit unit = RADIANS) :
			_begin_theta(theta_b), _centerTheta(1e8), _end_theta(theta_e), _begin_phi(phi_b), _center_phi(0.0), _end_phi(phi_e), _solidAngle(0), _cosThetaDOmega(
				0.1), _angularSector(0), _property(DirectionRoot::DISCRETE), _shape(DirectionRoot::RECTANGLE), _unit(unit), _ifPhaseOnly(false) {
		//To avoid error of _center_theta=0, which can not be defined with four corners;
		this->normalizeAngles();
		this->calculateCenter().calculateSolidAngle();
	}

	// Direct Setters
	DirectionRoot & setBeginTheta(double beginTheta);
	DirectionRoot & setCenterTheta(double centerTheta);
	DirectionRoot & setEndTheta(double endTheta);

	DirectionRoot & setBeginPhi(double beginPhi);
	DirectionRoot & setCenterPhi(double centerPhi);
	DirectionRoot & setEndPhi(double endPhi);

	DirectionRoot & setCosThetaDOmega(double cosThetaDOmega);

	DirectionRoot & setSolidAngle(double omega);
	DirectionRoot & setAngularSector(unsigned int angularSector);
	DirectionRoot & setProperty(DirectionRoot::Property property);
	DirectionRoot & setShape(DirectionRoot::Shape shape);

	DirectionRoot & setUnit(DirectionRoot::Unit unit);
	DirectionRoot & setIfPhaseOnly(bool ifPhaseOnly);

	// Direct Getters
	double getBeginPhi() const;
	double getCenterPhi() const;
	double getEndPhi() const;

	double getBeginTheta() const;
	double getCenterTheta() const;
	double getEndTheta() const;

	double getCosThetaDOmega() const;

	double getSolidAngle() const;
	unsigned int getAngularSector() const;
	bool isPhaseOnly() const;
	DirectionRoot::Property getProperty() const;
	DirectionRoot::Shape getShape() const;

	DirectionRoot::Unit getUnit() const;

	// Indirect Getters (Do some calculations)
	virtual double getDeltaCosineTheta() const;
	virtual double getDeltaPhi() const;
	virtual double getDeltaTheta() const;

	// Operators
	bool operator==(const DirectionRoot & d2) const;
	bool operator<(const DirectionRoot & d2) const;

	friend ostream & operator<<(ostream & stream,
								const DirectionRoot & direction);

	// Unit Management (Degrees, Radians)
	DirectionRoot toRadians() const;
	virtual void toRadians();

	DirectionRoot toDegrees() const;
	virtual void toDegrees();

	double getQuarterCircleAngle() const {
		if (this->getUnit() == DirectionRoot::RADIANS) {
			return M_PI * 0.5;
		} else {
			return 90.;
		}
	}
	double getHalfCircleAngle() const {
		if (this->getUnit() == DirectionRoot::RADIANS) {
			return M_PI;
		} else {
			return 180.;
		}
	}

	double getFullCircleAngle() const {
		if (this->getUnit() == DirectionRoot::RADIANS) {
			return M_PI2;
		} else {
			return 360.;
		}
	}
	virtual ~DirectionRoot() {}

private:
	static double constModulo(	double valeur,
								double modulo) {
		double res = valeur;
		if (res >= modulo) {
			res = fmod(res, modulo);
		} else if (res < 0) {
			do {
				res += modulo;
			} while (res < 0);
		}
		return res;
	}

	static void modulo(	double & valeur,
						double modulo) {
		if (valeur >= modulo) {
			valeur = fmod(valeur, modulo);
		} else if (valeur < 0) {
			do {
				valeur += modulo;
			} while (valeur < 0);
		}
	}

public:

	// Methods
	/**
	 * Calculate the center theta and phi from theta/phi begin and end.
	 * Overwrite the previous center theta and phi stored in the direction.
	 */
	DirectionRoot & calculateCenter();

	/**
	 * Calculate this direction solid angle from theta/phi begin and end.
	 * Overwrite the previous solid angle stored in the direction.
	 */
	DirectionRoot & calculateSolidAngle();

	DirectionRoot & normalizeAngles();

	static const int RootDirection = -1;
	static const int SimpleDirection = 0;
	static const int Airborne = 1;
	static const int Sensor = 2;
//	static const unsigned int SUN_ANGLE = 1;
//	static const unsigned int LAT_LONG = 2;

	virtual int getClass() const;

	virtual std::string toString() const;

	inline shared_ptr<DirectionRoot> clone() const {
		return shared_ptr<DirectionRoot>(new DirectionRoot(*this));
	}

	bool isVirtual() const;

	Doublet<DirectionRoot> split() const {
		//Split one solid angle into a vector of two solid angles if the two sides of the solid angles lied between the 0/2PI line.
		//Split is useful when the two phi end of the solid angle falls between 0/2PI line.
		//The solid angle is splitted into two solid angles, one on the left side, and the other one the right side.
		double adjustedBeginPhi = constModulo(this->getBeginPhi(), this->getFullCircleAngle());
		double adjustedEndPhi = constModulo(this->getEndPhi(), this->getFullCircleAngle());

		return Doublet<DirectionRoot>(DirectionRoot(this->getBeginTheta(), this->getEndTheta(), adjustedBeginPhi, this->getFullCircleAngle()),
			DirectionRoot(this->getBeginTheta(), this->getEndTheta(), 0, adjustedEndPhi));
	}

	bool isSplit() const {
		//First check if the phi sides of the solid angle falls on the two sides of 0/2PI line.
		return ((this->getEndPhi() < this->getBeginPhi())
				|| (this->getEndPhi() > this->getFullCircleAngle() && this->getBeginPhi() < this->getFullCircleAngle())
				|| (this->getBeginPhi() < 0 && this->getEndPhi() > 0));
	}

	bool checkInclude(	double theta,
						double phi) const {
		//Check if the coordinate theta, phi is inside the direction tag
		if (theta <= this->getEndTheta() && theta >= this->getBeginTheta()) {
			if (phi <= this->getEndPhi() && phi >= this->getBeginPhi()) {
				return true;
			} else if (this->isSplit()) {
				Doublet<DirectionRoot> directionDivision = this->split();
				return (directionDivision.getX().checkInclude(theta, phi) || directionDivision.getY().checkInclude(theta, phi));
			}
		}
		return false;
	}

	bool checkInclude_OPEN(	double theta,
							double phi) const {
		//Check if the coordinate theta, phi is inside the direction tag
		if (theta < this->getEndTheta() && theta > this->getBeginTheta()) {
			if (phi < this->getEndPhi() && phi > this->getBeginPhi()) {
				return true;
			} else if (this->isSplit()) {
				Doublet<DirectionRoot> directionDivision = this->split();
				return (directionDivision.getX().checkInclude_OPEN(theta, phi) || directionDivision.getY().checkInclude_OPEN(theta, phi));
			}
		}
		return false;
	}

	bool findJoint(	const DirectionRoot & dir2,
					DirectionRoot & joint) const {
		//Find the Joint Area of the Two Directions.
		joint.setBeginTheta(max(this->getBeginTheta(), dir2.getBeginTheta()));
		joint.setEndTheta(min(this->getEndTheta(), dir2.getEndTheta()));
		double joint_delta_theta = joint.getEndTheta() - joint.getBeginTheta();
		if (joint_delta_theta > 0) {
			joint.setBeginPhi(max(this->getBeginPhi(), dir2.getBeginPhi()));
			joint.setEndPhi(min(this->getEndPhi(), this->getEndPhi()));
			double joint_delta_phi = joint.getEndPhi() - joint.getBeginPhi();

			if (joint_delta_phi > 0 || (this->getBeginPhi() > this->getEndPhi() && dir2.getBeginPhi() > dir2.getEndPhi())) {
				return true;
			} else if (this->isSplit()) {
				Doublet<DirectionRoot> direction1Division = this->split();
				if (dir2.isSplit()) {
					// La methode ci-dessous est generale,
					// Si les directions sont correctement normalisees, la jointure calculee precedemment est la solution
					Doublet<DirectionRoot> direction2Division = dir2.split();

					joint.setBeginPhi(
						max(max(direction1Division.getX().getBeginPhi(), direction1Division.getY().getBeginPhi()),
							max(direction2Division.getX().getBeginPhi(), direction2Division.getY().getBeginPhi())));
					joint.setEndPhi(
						min(min(direction1Division.getX().getEndPhi(), direction1Division.getY().getEndPhi()),
							min(direction2Division.getX().getEndPhi(), direction2Division.getY().getEndPhi())));
				} else {
					return (direction1Division.getX().findJoint(dir2, joint) || direction1Division.getY().findJoint(dir2, joint));
				}
			} else if (dir2.isSplit()) {
				Doublet<DirectionRoot> direction2Division = dir2.split();
				return (this->findJoint(direction2Division.getX(), joint) || this->findJoint(direction2Division.getY(), joint));
			}
		}
		return false;
	}
};

inline bool DirectionRoot::operator<(const DirectionRoot & d2) const {
	return (this->getCenterTheta() < d2.getCenterTheta()
			|| (this->getCenterTheta() == d2.getCenterTheta() && (this->getCenterPhi() < d2.getCenterPhi()
					|| (this->getCenterPhi() == d2.getCenterPhi() && this->getSolidAngle() < d2.getSolidAngle()))));
}

inline int DirectionRoot::getClass() const {
	return DirectionRoot::RootDirection;
}

inline std::string DirectionRoot::toString() const {
	std::ostringstream ostream;
	ostream << *this;
	return ostream.str();
}

inline bool DirectionRoot::isVirtual() const {
	return this->getProperty() == DirectionRoot::VIRTUAL;
}

inline DirectionRoot & DirectionRoot::calculateSolidAngle() {
	_solidAngle = fabs(this->getDeltaPhi()) * (fabs(this->getDeltaCosineTheta()));
	return *this;
}

inline bool DirectionRoot::operator==(const DirectionRoot & d2) const {
	return ((_begin_theta == d2._begin_theta) && (_end_theta == d2._end_theta) && (_centerTheta == d2._centerTheta) && (_center_phi == d2._center_phi)
			&& (_begin_phi == d2._begin_phi) && (_end_phi == d2._end_phi) && (_solidAngle==d2._solidAngle) && (_property == d2._property) && (_shape == d2._shape)
			&& (_cosThetaDOmega == d2._cosThetaDOmega) && (_angularSector == d2._angularSector) && (_ifPhaseOnly == d2._ifPhaseOnly) && (_unit == d2._unit));
}

inline double DirectionRoot::getCenterPhi() const {
	return _center_phi;
}

inline double DirectionRoot::getCenterTheta() const {
	return _centerTheta;
}

inline DirectionRoot & DirectionRoot::setCenterPhi(double centerPhi) {
	this->_center_phi = centerPhi;
	return *this;
}

inline DirectionRoot & DirectionRoot::setCenterTheta(double centerTheta) {
	this->_centerTheta = centerTheta;
	return *this;
}

inline double DirectionRoot::getBeginPhi() const {
	return _begin_phi;
}

inline double DirectionRoot::getBeginTheta() const {
	return _begin_theta;
}

inline double DirectionRoot::getEndPhi() const {
	return _end_phi;
}

inline double DirectionRoot::getEndTheta() const {
	return _end_theta;
}

inline DirectionRoot & DirectionRoot::setBeginPhi(double beginPhi) {
	this->_begin_phi = beginPhi;
	return *this;
}

inline DirectionRoot & DirectionRoot::setBeginTheta(double beginTheta) {
	this->_begin_theta = beginTheta;
	return *this;
}

inline DirectionRoot & DirectionRoot::setEndPhi(double endPhi) {
	this->_end_phi = endPhi;
	return *this;
}

inline DirectionRoot & DirectionRoot::setEndTheta(double endTheta) {
	this->_end_theta = endTheta;
	return *this;
}

inline double DirectionRoot::getDeltaCosineTheta() const {
	return cos(_begin_theta) - cos(_end_theta);
}

inline double DirectionRoot::getDeltaPhi() const {
	if (_end_phi >= _begin_phi) {
		return _end_phi - _begin_phi;
	} else {
		return _end_phi + this->getFullCircleAngle() - _begin_phi;
	}
}

inline double DirectionRoot::getDeltaTheta() const {
	return _end_theta - _begin_theta;
}

inline DirectionRoot & DirectionRoot::setSolidAngle(double solidAngle) {
	this->_solidAngle = solidAngle;
	return *this;
}

inline double DirectionRoot::getSolidAngle() const {
	return _solidAngle;
}

inline DirectionRoot::Property DirectionRoot::getProperty() const {
	return _property;
}

inline DirectionRoot::Shape DirectionRoot::getShape() const {
	return _shape;
}


inline DirectionRoot & DirectionRoot::setProperty(DirectionRoot::Property property) {
	this->_property = property;
	return *this;
}

inline DirectionRoot & DirectionRoot::setShape(DirectionRoot::Shape shape) {
	this->_shape = shape;
	return *this;
}

inline DirectionRoot::Unit DirectionRoot::getUnit() const {
	return this->_unit;
}

inline DirectionRoot & DirectionRoot::setUnit(DirectionRoot::Unit unit) {
	this->_unit = unit;
	return *this;
}

inline DirectionRoot & DirectionRoot::setIfPhaseOnly(bool ifPhaseOnly) {
	this->_ifPhaseOnly = ifPhaseOnly;
	return *this;
}

inline unsigned int DirectionRoot::getAngularSector() const {
	return _angularSector;
}

inline bool DirectionRoot::isPhaseOnly() const {
	return _ifPhaseOnly;
}

inline DirectionRoot & DirectionRoot::setAngularSector(unsigned int angularSector) {
	this->_angularSector = angularSector;
	return *this;
}

inline DirectionRoot & DirectionRoot::setCosThetaDOmega(double cosThetaDOmega) {
	this->_cosThetaDOmega = cosThetaDOmega;
	return *this;
}

inline double DirectionRoot::getCosThetaDOmega() const {
	return _cosThetaDOmega;
}

inline DirectionRoot & DirectionRoot::calculateCenter() {
	_centerTheta = acos((cos(_begin_theta) + cos(_end_theta)) / 2.);
	if (_end_phi >= _begin_phi) {
		_center_phi = (_begin_phi + _end_phi) / 2.;
	} else {
		_center_phi = (_begin_phi + _end_phi) / 2. + this->getHalfCircleAngle();
	}
	return *this;
}

inline DirectionRoot & DirectionRoot::normalizeAngles() {
	if (_centerTheta == 0 || _centerTheta == this->getHalfCircleAngle())
		_center_phi = 0;

	if (_begin_phi != 0 && _end_phi != this->getFullCircleAngle()) {
		modulo(_begin_phi, this->getFullCircleAngle());
		modulo(_center_phi, this->getFullCircleAngle());
		modulo(_end_phi, this->getFullCircleAngle());
	}
//	modulo(_begin_theta, this->getFullCircleAngle());
//	modulo(_centerTheta, this->getFullCircleAngle());
//	modulo(_end_theta, this->getFullCircleAngle());
	return *this;
}

inline DirectionRoot DirectionRoot::toRadians() const {
	DirectionRoot res = *this;
	res.toRadians();
	return res;
}

inline void DirectionRoot::toRadians() {
	if (this->getUnit() == DirectionRoot::DEGREES) {
		double conversionFactor = M_PI / 180.;
		setBeginTheta(getBeginTheta() * conversionFactor);
		setCenterTheta(getCenterTheta() * conversionFactor);
		setEndTheta(getEndTheta() * conversionFactor);
		setBeginPhi(getBeginPhi() * conversionFactor);
		setCenterPhi(getCenterPhi() * conversionFactor);
		setEndPhi(getEndPhi() * conversionFactor);
		setUnit(DirectionRoot::RADIANS);
	} else {
		cerr << "Warning: Converting direction defined in radiances to radiances." << endl;
		abort();
	}
}

inline DirectionRoot DirectionRoot::toDegrees() const {
	DirectionRoot res = *this;
	res.toDegrees();
	return res;
}

inline void DirectionRoot::toDegrees() {
	if (this->getUnit() == DirectionRoot::RADIANS) {
		double conversionFactor = 180. / M_PI;
		setBeginTheta(getBeginTheta() * conversionFactor);
		setCenterTheta(getCenterTheta() * conversionFactor);
		setEndTheta(getEndTheta() * conversionFactor);
		setBeginPhi(getBeginPhi() * conversionFactor);
		setCenterPhi(getCenterPhi() * conversionFactor);
		setEndPhi(getEndPhi() * conversionFactor);
		setUnit(DirectionRoot::DEGREES);
	} else {
		cerr << "Warning: Converting direction defined in degrees to degrees." << endl;
		abort();
	}
}

#endif /* DIRECTIONROOT_HPP_ */
