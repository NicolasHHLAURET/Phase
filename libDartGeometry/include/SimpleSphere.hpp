/*
 * Sphere.hpp
 *
 *  Created on: 9 mai 2011
 *      Author: lauretn
 */

#ifndef SIMPLESPHERE_HPP_
#define SIMPLESPHERE_HPP_

#include "Point3.hpp"

class SimpleSphere {
private:
	double _Rayon;
	Point3<double> _Centre;

public:
	SimpleSphere(	const double & rayon = 1,
			const Point3<double> & centre = Point3<double>(0, 0, 0)) :
			_Rayon(rayon), _Centre(centre) {}
	
	const double & getRayon() const {
		return _Rayon;
	}
	const Point3<double> & getCentre() const {
		return _Centre;
	}
	
	SimpleSphere & setRayon(const double & rayon) {
		_Rayon = rayon;
		return *this;
	}
	SimpleSphere & setCentre(const Point3<double> & centre) {
		_Centre = centre;
		return *this;
	}
	
	bool contient(const Point3<double> & point) const {
		return (pow(_Centre.getX() - point.getX(), 2) + pow(_Centre.getY() - point.getY(), 2) + pow(_Centre.getZ() - point.getZ(), 2))
				<= pow(_Rayon, 2);
	}
};

#endif /* SIMPLESPHERE_HPP_ */
