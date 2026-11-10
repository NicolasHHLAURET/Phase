/*
 * Doublet.hpp
 *
 *  Created on: 9 avr. 2010
 *      Author: lauretn
 */

#ifndef DOUBLET_HPP_
#define DOUBLET_HPP_

#include "Triplet.hpp"
#include <vector>
#include <string>
#include <sstream>

template<typename T>
class Doublet {
public:
	// Attributs
	T _x;
	T _y;

public:
	static const int X = 0;
	static const int Y = 1;

	// Constructeur
	Doublet<T>(	const T & x = 0,
				const T & y = 0) :
			_x(x), _y(y) {}
	
	// Constructeur par copie
	Doublet<T>(const Doublet<T> & doublet) :
			_x(doublet._x), _y(doublet._y) {}

	// Operateur d'affectation
	Doublet<T> & operator=(const Doublet<T> & doublet) {
		if (this != &doublet) {
			this->_x = doublet._x;
			this->_y = doublet._y;
		}
		return *this;
	}
	
	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Doublet<T>(const Doublet<U> & doublet) :
			_x(doublet.getX()), _y(doublet.getY()) {}
	
	//	// Destructeurs
	virtual ~Doublet<T>() {}
	
	// Accesseurs
	const T& getX() const;
	const T& getY() const;

	const T & operator[](const int & indice) const;

	// Accesseurs non const
	T & operator[](const int & indice);

	// Modifieurs
	virtual Doublet<T> & setX(const T & x);
	virtual Doublet<T> & setY(const T & y);
	virtual Doublet<T> & setXY(	const T & x,
								const T & y);
	virtual Doublet<T> & setXY(const Doublet<T> & XY);

	// Methodes Constantes
	/**
	 * Methode de comparaison entre Triplets
	 */
	template<typename U>
	const bool equal(const Doublet<U> & doublet2) const;
	template<typename U>
	const bool operator==(const Doublet<U> & doublet2) const;
	template<typename U>
	const bool operator!=(const Doublet<U> & doublet2) const;

	template<typename U>
	const bool operator<(const Doublet<U> & doublet2) const;

	/**
	 * Retourne ce triplet sous forme d'un std::vector
	 */
	const std::vector<T> toSTDvector() const;

	/**
	 * Retourne une chaine de caractere decrivant ce Doublet
	 */
	virtual const std::string toString() const;

	/**
	 * Retourne la taille de cet objet en memoire
	 */
	virtual const unsigned int memorySize() const;

	// Methodes modifiantes
	/*********************************************************************
	 * Methodes arithmetiques entre 2 Doublet.
	 * Les operations sont effectuees membre a membre.
	 * Modifie l'instance courante.
	 *********************************************************************/
	template<typename U>
	Doublet<T> & add(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & sub(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & mul(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & div(const Doublet<U> & t2);

	/*********************************************************************
	 * Operateurs arithmetiques entre 2 Doublet.
	 * Les operations sont effectuees membre a membre.
	 * Modifie l'instance courante.
	 *********************************************************************/
	template<typename U>
	Doublet<T> & operator+=(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & operator-=(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & operator*=(const Doublet<U> & t2);
	template<typename U>
	Doublet<T> & operator/=(const Doublet<U> & t2);

	/*********************************************************************
	 * Methodes arithmetiques entre un Doublet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Doublet.
	 * Modifie l'instance courante.
	 *********************************************************************/
	Doublet<T> & add(const T & a);
	Doublet<T> & sub(const T & m);
	Doublet<T> & mul(const T & m);
	Doublet<T> & div(const T & d);

	/*********************************************************************
	 * Operateurs arithmetiques entre un Doublet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Doublet.
	 * Modifie l'instance courante.
	 *********************************************************************/
	Doublet<T> & operator+=(const T & a);
	Doublet<T> & operator-=(const T & m);
	Doublet<T> & operator*=(const T & m);
	Doublet<T> & operator/=(const T & d);

	// Methodes constantes
	/*********************************************************************
	 * Methodes arithmetiques entre 2 Doublet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Doublet<T> add(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> sub(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> mul(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> div(const Doublet<U> & t2) const;

	/*********************************************************************
	 * Operateurs arithmetiques entre 2 Doublet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Doublet<T> operator+(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator-(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator*(const Doublet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator/(const Doublet<U> & t2) const;

	// Methodes constantes
	/*********************************************************************
	 * Methodes arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Doublet<T> add(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> sub(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> mul(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> div(const Triplet<U> & t2) const;

	/*********************************************************************
	 * Operateurs arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Doublet<T> operator+(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator-(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator*(const Triplet<U> & t2) const;
	template<typename U>
	const Doublet<T> operator/(const Triplet<U> & t2) const;

	/*********************************************************************
	 * Methodes arithmetiques entre un Doublet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Doublet.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Doublet<T> add(const T & a) const;
	const Doublet<T> sub(const T & m) const;
	const Doublet<T> mul(const T & m) const;
	const Doublet<T> div(const T & d) const;

	/*********************************************************************
	 * Operateurs arithmetiques entre un Doublet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Doublet.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Doublet<T> operator+(const T & a) const;
	const Doublet<T> operator-(const T & m) const;
	const Doublet<T> operator*(const T & m) const;
	const Doublet<T> operator/(const T & d) const;

	/*********************************************************************
	 * Retourne l'oppose du Doublet : (-Doublet).
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Doublet<T> negate() const;

	template<typename U>
	friend std::ostream &operator<<(	std::ostream &stream,
								const Doublet<U> & doublet);
};

template<typename U>
std::ostream & operator<<(	std::ostream &stream,
						const Doublet<U> & doublet) {
	stream << "(" << doublet.getX() << ", " << doublet.getY() << ")";
	
	return stream;
}

template<typename T>
inline const T& Doublet<T>::getX() const {
	return _x;
}
template<typename T>
inline const T& Doublet<T>::getY() const {
	return _y;
}

template<typename T>
const T & Doublet<T>::operator[](const int & indice) const {
	if (indice == Doublet<T>::X) {
		return _x;
	} else { // TODO exception si different Y
		return _y;
	}
}

template<typename T>
T & Doublet<T>::operator[](const int & indice) {
	if (indice == Doublet<T>::X) {
		return _x;
	} else { // TODO exception si different Y
		return _y;
	}
}

template<typename T>
inline Doublet<T> & Doublet<T>::setX(const T & x) {
	_x = x;
	return (*this);
}
template<typename T>
inline Doublet<T> & Doublet<T>::setY(const T & y) {
	_y = y;
	return (*this);
}

template<typename T>
inline Doublet<T> & Doublet<T>::setXY(	const T & x,
										const T & y) {
	_x = x;
	_y = y;
	return (*this);
}
template<typename T>
inline Doublet<T> & Doublet<T>::setXY(const Doublet<T> & XY) {
	_x = XY.getX();
	_y = XY.getY();
	return (*this);
}

/*********************************************************************
 * Methodes arithmetiques entre 2 Doublet.
 * Les operations sont effectuees membre a membre.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::add(const Doublet<U> & t2) {
	setXY(getX() + t2.getX(), getY() + t2.getY());
	return (*this);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::sub(const Doublet<U> & t2) {
	setXY(getX() - t2.getX(), getY() - t2.getY());
	return (*this);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::mul(const Doublet<U> & t2) {
	setXY(getX() * t2.getX(), getY() * t2.getY());
	return (*this);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::div(const Doublet<U> & t2) {
	setXY(getX() / t2.getX(), getY() / t2.getY());
	return (*this);
}

/*********************************************************************
 * Operateurs arithmetiques entre 2 Doublet.
 * Les operations sont effectuees membre a membre.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::operator+=(const Doublet<U> & t2) {
	return add(t2);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::operator-=(const Doublet<U> & t2) {
	return sub(t2);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::operator*=(const Doublet<U> & t2) {
	return mul(t2);
}

template<typename T>
template<typename U>
inline Doublet<T> & Doublet<T>::operator/=(const Doublet<U> & t2) {
	return div(t2);
}

/*********************************************************************
 * Methodes arithmetiques entre un Doublet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Doublet.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
inline Doublet<T> & Doublet<T>::add(const T & a) {
	setXY(getX() + a, getY() + a);
	return (*this);
}

template<typename T>
inline Doublet<T> & Doublet<T>::sub(const T & s) {
	setXY(getX() - s, getY() - s);
	return (*this);
}

template<typename T>
inline Doublet<T> & Doublet<T>::mul(const T & m) {
	setXY(getX() * m, getY() * m);
	return (*this);
}

template<typename T>
inline Doublet<T> & Doublet<T>::div(const T & d) {
	setXY(getX() / d, getY() / d);
	return (*this);
}

/*********************************************************************
 * Operateurs arithmetiques entre un Doublet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Doublet.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
inline Doublet<T> & Doublet<T>::operator+=(const T & a) {
	return add(a);
}

template<typename T>
inline Doublet<T> & Doublet<T>::operator-=(const T & s) {
	return sub(s);
}

template<typename T>
inline Doublet<T> & Doublet<T>::operator*=(const T & m) {
	return mul(m);
}

template<typename T>
inline Doublet<T> & Doublet<T>::operator/=(const T & d) {
	return div(d);
}

/*********************************************************************
 * Methodes arithmetiques entre 2 Doublet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::add(const Doublet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() + t2.getX(), getY() + t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::sub(const Doublet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() - t2.getX(), getY() - t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::mul(const Doublet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() * t2.getX(), getY() * t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::div(const Doublet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() / t2.getX(), getY() / t2.getY());
	return copie;
}

/*********************************************************************
 * Operateurs arithmetiques entre 2 Doublet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator+(const Doublet<U> & t2) const {
	return add(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator-(const Doublet<U> & t2) const {
	return sub(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator*(const Doublet<U> & t2) const {
	return mul(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator/(const Doublet<U> & t2) const {
	return div(t2);
}

/*********************************************************************
 * Methodes arithmetiques entre 1 Doublet et un Triplet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::add(const Triplet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() + t2.getX(), getY() + t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::sub(const Triplet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() - t2.getX(), getY() - t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::mul(const Triplet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() * t2.getX(), getY() * t2.getY());
	return copie;
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::div(const Triplet<U> & t2) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() / t2.getX(), getY() / t2.getY());
	return copie;
}

/*********************************************************************
 * Operateurs arithmetiques entre 1 Doublet et un Triplet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator+(const Triplet<U> & t2) const {
	return add(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator-(const Triplet<U> & t2) const {
	return sub(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator*(const Triplet<U> & t2) const {
	return mul(t2);
}

template<typename T>
template<typename U>
inline const Doublet<T> Doublet<T>::operator/(const Triplet<U> & t2) const {
	return div(t2);
}

/*********************************************************************
 * Operations arithmetiques entre un Doublet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Doublet.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
inline const Doublet<T> Doublet<T>::add(const T & a) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() + a, getY() + a);
	return copie;
}

template<typename T>
inline const Doublet<T> Doublet<T>::sub(const T & s) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() - s, getY() - s);
	return copie;
}

template<typename T>
inline const Doublet<T> Doublet<T>::mul(const T & m) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() * m, getY() * m);
	return copie;
}

template<typename T>
inline const Doublet<T> Doublet<T>::div(const T & d) const {
	Doublet<T> copie(*this);
	copie.setXY(getX() / d, getY() / d);
	return copie;
}

/*********************************************************************
 * Operateurs arithmetiques entre un Doublet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Doublet.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
inline const Doublet<T> Doublet<T>::operator+(const T & a) const {
	return add(a);
}

template<typename T>
inline const Doublet<T> Doublet<T>::operator-(const T & s) const {
	return sub(s);
}

template<typename T>
inline const Doublet<T> Doublet<T>::operator*(const T & m) const {
	return mul(m);
}

template<typename T>
inline const Doublet<T> Doublet<T>::operator/(const T & d) const {
	return div(d);
}

/**
 * Retourne l'oppose du Doublet : (-Doublet).
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 */
template<typename T>
inline const Doublet<T> Doublet<T>::negate() const {
	Doublet<T> copie(*this);
	copie.setXY(-getX(), -getY());
	return copie;
}

template<typename T>
template<typename U>
inline const bool Doublet<T>::equal(const Doublet<U> & tuple2) const {
	return (getX() == tuple2.getX() && getY() == tuple2.getY());
}

template<typename T>
template<typename U>
inline const bool Doublet<T>::operator==(const Doublet<U> & tuple2) const {
	return (equal(tuple2));
}

template<typename T>
template<typename U>
inline const bool Doublet<T>::operator!=(const Doublet<U> & tuple2) const {
	return (!equal(tuple2));
}

template<typename T>
template<typename U>
/**
 * Attention : Operateur utilise pour construire les map
 */
inline const bool Doublet<T>::operator<(const Doublet<U> & doublet2) const {
	if (this->getY() < doublet2.getY()) {
		return true;
	} else if (this->getY() == doublet2.getY() && this->getX() < doublet2.getX()) {
		return true;
	}
	return false;
}

template<typename T>
inline const std::vector<T> Doublet<T>::toSTDvector() const {
	std::vector<T> res();
	res.push_back(getX());
	res.push_back(getY());
	return res;
}

template<typename T>
inline const std::string Doublet<T>::toString() const {
	std::ostringstream os;
	os << *this;
	return os.str();
}

/**
 * Retourne la taille de cet objet en memoire
 */
template<typename T>
inline const unsigned int Doublet<T>::memorySize() const {
	return 2 * sizeof(T);
}

#endif /* DOUBLET_HPP_ */
