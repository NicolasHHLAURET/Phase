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

#ifndef POINT3_H_
#define POINT3_H_

#include "Triplet.hpp"
#include "Doublet.hpp"
#include <stdio.h>
#include <math.h>

template<typename T>
class Point3: public Triplet<T> {
public:
	// Constructeurs
	Point3<T>(	const T & x = 0,
				const T & y = 0,
				const T & z = 0) :
			Triplet<T>(x, y, z) {}
	// Constructeur par copie
	Point3<T>(const Triplet<T>& p1) :
			Triplet<T>(p1.getX(), p1.getY(), p1.getZ()) {}
	
	// Constructeur depuis un doublet
	Point3<T>(	const Doublet<T>& p1,
				const T& z) :
			Triplet<T>(p1.getX(), p1.getY(), z) {}
	
	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Point3<T>(const Triplet<U>& p1) :
			Triplet<T>(p1.getX(), p1.getY(), p1.getZ()) {}
	
	//	// Destructeurs
	//	virtual ~Point3<T> () {
	//	}
	
	// Accesseurs et Modifieurs herites de Tripletf
	
	// Accesseurs
	const T getW() const;

	const T getDistanceToOrigin() const;

	template<typename U>
	const T getDistance(const Point3<U> & point) const;

	template<typename U>
	const T getDistanceSquared(const Point3<U> & point) const;
	
	// Methodes Constantes
//	const string toString() const;
//
//	friend ostream &operator<<(ostream &stream, const Point3<T> & point);
};

template<typename T>
inline const T Point3<T>::getW() const {
	return 1.;
}

template<typename T>
inline const T Point3<T>::getDistanceToOrigin() const {
	return sqrt(Triplet<T>::getX() * Triplet<T>::getX() + Triplet<T>::getY() * Triplet<T>::getY() + Triplet<T>::getZ() * Triplet<T>::getZ());
}

template<typename T>
template<typename U>
inline const T Point3<T>::getDistance(const Point3<U> & point) const {
	return sqrt(getDistanceSquared(point));
}

template<typename T>
template<typename U>
inline const T Point3<T>::getDistanceSquared(const Point3<U> & point) const {
	T x = Triplet<T>::getX() - point.Triplet < U > ::getX();
	T y = Triplet<T>::getY() - point.Triplet < U > ::getY();
	T z = Triplet<T>::getZ() - point.Triplet < U > ::getZ();
	return x * x + y * y + z * z;
}

//template<typename T>
//inline const string Point3<T>::toString() const {
//	string res = "(";
//	res.append(DartUtils::whatever2string(Triplet<T>::getX()));
//	res.append(",");
//	res.append(DartUtils::whatever2string(Triplet<T>::getY()));
//	res.append(",");
//	res.append(DartUtils::whatever2string(Triplet<T>::getZ()));
//	res.append(")");
//	return res;
//}
//
//
//template<typename T>
//ostream & operator<<(ostream &stream, const Point3<T> & point)
//{
//  stream << "(" << point.Triplet<T>::getX() << ", " << point.Triplet<T>::getY() << ", " << point.Triplet<T>::getZ() << ")";
//
//  return stream;
//}

#endif /* POINT3_H_ */
