/*
 * Ligne.hpp
 *
 *  Created on: 6 mai 2011
 *      Author: graue
 */

#ifndef DROITE3D_HPP_
#define DROITE3D_HPP_

#include <iostream>
#include "Point3.hpp"
#include "Vecteur.hpp"
#include "PointIntersection.hpp"

class Droite3d {
	Vecteur<double> _direction; // unit normal vector
	Point3<double> _point1; // unit normal vector
	Point3<double> _point2; // unit normal vector
	
public:
//	Droite3d(const Vecteur<double> &direction, const Point3<double> &origine) :
//		_direction(direction), _point1(origine) {
//		_point2 = _point1 + _direction * 10;
//	}
	
	Droite3d(	const Point3<double> &origine,
				const Point3<double> &point) :
			_point1(origine), _point2(point) {
		_direction = _point1;
		_direction -= _point2;
		_direction.normaliser();
	}
	
	Droite3d(	const Vecteur<double> &direction = Vecteur<double>(),
				const Point3<double> &origine = Point3<double>()) :
			_direction(direction), _point1(origine) {
		_point2 = _point1 + _direction * 10;
	}
	
	const Vecteur<double>& getDirection() const;
	const Point3<double>& getPoint1() const;
	const Point3<double>& getPoint2() const;
	void setDirection(const Vecteur<double> &_direction);
	void setPoint2(const Point3<double> &_origine);
	void setPoint1(const Point3<double> &_point);

	PointIntersection<double> calculPointIntersection(const Droite3d & droite) const;

	const std::string toString() const {
		std::string res = "{dir : ";
		res.append(_direction.toString());
		res.append(", point1 ");
		res.append(_point1.toString());
		res.append(", point2 ");
		res.append(_point2.toString());
		res.append("}");
		return res;
	}
};

inline const Vecteur<double>& Droite3d::getDirection() const {
	return _direction;
}

inline const Point3<double>& Droite3d::getPoint1() const {
	return _point1;
}

inline const Point3<double>& Droite3d::getPoint2() const {
	return _point2;
}

inline void Droite3d::setDirection(const Vecteur<double> &_direction) {
	//TODO : si on change la direction els points ne sont plus alignes
	this->_direction = _direction;
}

inline void Droite3d::setPoint1(const Point3<double> &_origine) {
	//TODO : si on change l'origine verifier qu'on est toujours sur la meme droite
	this->_point1 = _origine;
}

inline void Droite3d::setPoint2(const Point3<double> &_point) {
	//TODO : si on change l'origine verifier qu'on est toujours sur la meme droite
	this->_point2 = _point;
}

inline PointIntersection<double> Droite3d::calculPointIntersection(const Droite3d & droite) const {
	Vecteur<double> dc = droite.getPoint1() - _point1;
	Vecteur<double> vectorielDroites = _direction.vectoriel(droite.getDirection());
	
	if (dc.scalaire(vectorielDroites) != 0.0) { // lines are not coplanar
		std::cerr << "No intersection point found" << std::endl;
		return PointIntersection<double>();
	}
	
	double s = (dc.vectoriel(droite.getDirection())).scalaire(vectorielDroites) / vectorielDroites.getLongueurCarre();
	
	if (s >= 0.0 && s <= 1.0) {
		return Point3<double>(_point1 + _direction * Point3<double>(s, s, s));
	}
	
	std::cerr << "No intersection point found" << std::endl;
	return PointIntersection<double>();
	
}
#endif /* DROITE3D_HPP_ */
