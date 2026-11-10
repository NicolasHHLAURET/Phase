/*
 * Disque.hpp
 *
 *  Created on: 8 juin 2010
 *      Author: graue
 */

#ifndef ELLIPSE2D_HPP_
#define ELLIPSE2D_HPP_

#include "Point2.hpp"
#include "Point3.hpp"

class Ellipse2D {
private:
	Point2<double> _Centre;
	Point2<double> _Rayon;
	double _surface;

public:
	// Constructeur par defaut
	Ellipse2D(	const double centreX = 0,
				const double centreY = 0,
				const double rayonX = 0,
				const double rayonY = 0) :
			_Centre(Point2<double>(centreX, centreY)), _Rayon(Point2<double>(rayonX, rayonY)) {
		_surface = M_PI * rayonX * rayonY;
	}
	
	// Constructeur par copie
	Ellipse2D(const Ellipse2D& ellipse) :
			_Centre(ellipse._Centre), _Rayon(ellipse._Rayon), _surface(ellipse._surface) {}
	
	// Operateur d'affectation
	Ellipse2D & operator=(const Ellipse2D & ellipse) {
		if (this != &ellipse) {
			this->_Rayon = ellipse._Rayon;
			this->_Centre = ellipse._Centre;
			this->_surface = ellipse._surface;
		}
		return *this;
	}
	
	const Point2<double>& getCentre() const {
		return _Centre;
	}
	const Point2<double>& getRayon() const {
		return _Rayon;
	}
	const double& getSurface() const {
		return _surface;
	}
	void setCentre(Point2<double> const& _Centre) {
		this->_Centre = _Centre;
	}
	void setRayon(Point2<double> const& _Rayon) {
		this->_Rayon = _Rayon;
	}
	
	const std::string toString() const;

	bool contient(const Point2<double> &position) const;
	bool contient(const Point3<double> &position) const;
	bool contient(	const double &positionX,
					const double &positionY) const;
};

inline const std::string Ellipse2D::toString() const {
	std::string res = "Ellipse{centre: ";
	res.append(_Centre.toString());
	res.append(" ; Rayon:");
	res.append(_Rayon.toString());
	res.append("}");
	return res;
}

inline bool Ellipse2D::contient(const Point2<double> &position) const {
	return contient(position.getX(), position.getY());
}
inline bool Ellipse2D::contient(const Point3<double> &position) const {
	return contient(position.getX(), position.getY());
}
inline bool Ellipse2D::contient(const double &positionX,
								const double &positionY) const {
	if (_Rayon.getY() + _Rayon.getX() < 0.00001) {
		return false;
	}
	double equationEllipse = (positionX - _Centre.getX()) * (positionX - _Centre.getX()) / (_Rayon.getX() * _Rayon.getX())
			+ (positionY - _Centre.getY()) * (positionY - _Centre.getY()) / (_Rayon.getY() * _Rayon.getY());
	
	//cout <<"equationEllipse:"<<equationEllipse<<endl;
	if (equationEllipse <= 1) {
		return true;
	}
	return false;
}

#endif /* ELLIPSE2D_HPP_ */
