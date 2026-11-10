/*! Time-stamp: <@(#) Point3.hpp   22/09/2009   LAURET Nicolas CESBIO>
 *********************************************************************
 *  @file    : Point3.hpp
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : LAURET Nicolas                        Date: 22/09/2009
 *
 *  Description : Classe representant un point
 *  sous sa forme carthesienne a l'aide de 3 nombres du type
 *  defini par le developpeur.
 * 
 *********************************************************************
 * Historique :
 *
 * v 1.1 02/10/2009  LAURET Nicolas : Conversion en classe generique (template)
 * v 1.0 22/09/2009  LAURET Nicolas : Creation
 *
 *********************************************************************
 */

#ifndef POINT2_H_
#define POINT2_H_

#include "Doublet.hpp"
#include <stdio.h>
#include <math.h>

using namespace std;

template<typename T>
class Point2: public Doublet<T> {
public:
	// Constructeurs
	Point2<T>(	const T & x = 0,
				const T & y = 0) :
			Doublet<T>(x, y) {}
	// Constructeur par copie
	Point2<T>(const Doublet<T>& p1) :
			Doublet<T>(p1.getX(), p1.getY()) {}
	
	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Point2<T>(const Doublet<U>& p1) :
			Doublet<T>(p1.getX(), p1.getY()) {}
	
	//	// Destructeurs
	//	virtual ~Point3<T> () {
	//	}
	
	// Accesseurs et Modifieurs herites de Tripletf
	
	// Accesseurs
	const T getW() const;

	template<typename U>
	const T getDistance(const Point2<U> & point) const;

	template<typename U>
	const T getDistanceSquared(const Point2<U> & point) const;

	/// Compute the distance from the point (x1, y1) to (x2, y2)
	static T getDistance(const T & x1, const T & y1, const T & x2, const T & y2) {
		T x = x1 - x2;
		T y = y1 - y2;
		return sqrt(x * x + y * y);
	}
};

template<typename T>
inline const T Point2<T>::getW() const {
	return 1.;
}

template<typename T>
template<typename U>
inline const T Point2<T>::getDistance(const Point2<U> & point) const {
	return sqrt(getDistanceSquared(point));
}

template<typename T>
template<typename U>
inline const T Point2<T>::getDistanceSquared(const Point2<U> & point) const {
	T x = Doublet<T>::getX() - point.Doublet < U > ::getX();
	T y = Doublet<T>::getY() - point.Doublet < U > ::getY();
	return x * x + y * y;
}

#endif /* POINT3_H_ */
