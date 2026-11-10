/*
 * PointIntersection.hpp
 *
 *  Created on: 30 déc. 2009
 *      Author: lauretn
 */

#ifndef POINTINTERSECTION_HPP_
#define POINTINTERSECTION_HPP_

#include "Point3.hpp"

class Figure;

template<typename T>
class PointIntersection: public Point3<T> {
private:
	/**
	 * Reference vers la figure qui a genere ce point d'intersection.
	 * Attention : reference croisee.
	 */
	const Figure * _figureReference;

	T _distancePointOrigine;

	unsigned int _figureIndex;

	bool _estPointIntersectionValide;

public:
	// Constructeurs par defaut
	PointIntersection(	const T & x = 0,
						const T & y = 0,
						const T & z = 0,
						const Figure * figureReference = NULL,
						const T & distance = -1,
						const bool & isIntersectionValid = true) :
			Point3<T>(x, y, z), _figureReference(figureReference), _distancePointOrigine(distance), _figureIndex(0), _estPointIntersectionValide(isIntersectionValid) {
	}

	PointIntersection(	const bool & isIntersectionValid,
						const Figure * figureReference = NULL) :
			Point3<T>(), _figureReference(figureReference), _distancePointOrigine(-1), _figureIndex(0), _estPointIntersectionValide(isIntersectionValid) {
	}

	template<typename U>
	PointIntersection(	const Point3<U> & coordonnees,
						const Figure * figureReference = NULL,
						const T & distance = -1,
						const bool & isIntersectionValid = true) :
			Point3<T>(coordonnees), _figureReference(figureReference), _distancePointOrigine(distance), _figureIndex(0), _estPointIntersectionValide(isIntersectionValid) {
	}

	//	// Constructeur par copie
	//	template<typename U>
	//	PointIntersection(const PointIntersection<U> & pointIntersection, Figure * const figureReference = 0) :
	//		Point3<T>(pointIntersection.getX(), pointIntersection.getY(), pointIntersection.getZ()), _distancePointOrigine(pointIntersection.getDistanceAOrigine()), _estPointIntersectionValide(pointIntersection.isValide()), _figureReference(figureReference) {
	//	}

	virtual ~PointIntersection() {
	}

	// Accesseurs
	/**
	 * Retourne la distance de ce point d'intersection a l'origine du rayon.
	 * Vaut une valeur negative si une telle distance n'a pas d'existance logique
	 * pour ce type de point d'intersection (par exemple, calcul point d'intersection-droite),
	 * ou si ce point decrit un point d'intersection vide (pas d'intersection).
	 */
	const T & getDistanceAOrigine() const;

	/**
	 * Retourne un pointeur sur la figure associee a ce point d'intersection.
	 */
	const Figure * getFigureReference() const;

	const unsigned int & getFigureIndex() const {
		return _figureIndex;
	}

	/**
	 * Retourne vrai si ce point d'intersection est valide,
	 * et faux autrement (point d'intersection non trouve).
	 */
	const bool & isValide() const;

	// Modifieurs
	void setDistanceAOrigine(const T & distance);
	void setIntersectionValide(const bool & isValid);
	void setFigureReference(const Figure * figureReference);
	void setPointIntersection(const Point3<T> & pointIntersection);
	void setFigureIndex(const unsigned int & figureIndex) {
		_figureIndex = figureIndex;
	}

	// Operation inferieure utilisee pour ordonne les points d'intersections
	const bool operator<(const PointIntersection<T> & pointIntersection2) const;

	/**
	 * Retourne la taille de cet objet en memoire
	 */
	virtual const unsigned int memorySize() const {
		return sizeof(T) + sizeof(bool) + sizeof(const Figure *);
	}
};

template<typename T>
inline const T & PointIntersection<T>::getDistanceAOrigine() const {
	return _distancePointOrigine;
}

template<typename T>
inline const bool & PointIntersection<T>::isValide() const {
	return _estPointIntersectionValide;
}

template<typename T>
const Figure * PointIntersection<T>::getFigureReference() const {
	return _figureReference;
}

template<typename T>
inline void PointIntersection<T>::setDistanceAOrigine(const T & distance) {
	_distancePointOrigine = distance;
}

template<typename T>
inline void PointIntersection<T>::setIntersectionValide(const bool & isValid) {
	_estPointIntersectionValide = isValid;
}

template<typename T>
inline void PointIntersection<T>::setFigureReference(const Figure * figureReference) {
	_figureReference = figureReference;
}

template<typename T>
inline void PointIntersection<T>::setPointIntersection(const Point3<T> & pointIntersection) {
	this->setXYZ(pointIntersection.getX(), pointIntersection.getY(), pointIntersection.getZ());
}

template<typename T>
inline const bool PointIntersection<T>::operator<(const PointIntersection<T> & pointIntersection2) const {
	return _distancePointOrigine < pointIntersection2.getDistanceAOrigine();
}

#endif /* POINTINTERSECTION_HPP_ */
