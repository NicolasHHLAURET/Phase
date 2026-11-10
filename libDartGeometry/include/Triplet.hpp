/*! Time-stamp: <@(#) Triplet.hpp   22/09/2009   LAURET Nicolas CESBIO>
 *********************************************************************
 *  @file    : Triplet.hpp
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : LAURET Nicolas                        Date: 22/09/2009
 *
 *  Description : Classe-mere a tous les objets de representations
 *  de coordonnees a l'aide de triples de 3 nombres du type
 *  defini par le developpeur. (Point et Vecteur).
 * 
 *********************************************************************
 * Historique :
 *
 * v 1.1 02/10/2009  LAURET Nicolas : Conversion en classe generique (template)
 * v 1.0 22/09/2009  LAURET Nicolas : Creation
 *
 *********************************************************************
 */

#ifndef TRIPLET_H_
#define TRIPLET_H_

#include <vector>
#include <string>
#include <sstream>

template<typename T>
class Triplet {
public:
	// Attributs
	T _x;
	T _y;
	T _z;

public:
	static const int X = 0;
	static const int Y = 1;
	static const int Z = 2;

	// Constructeur
	Triplet<T>(	const T & x = T(),
				const T & y = T(),
				const T & z = T()) :
			_x(x), _y(y), _z(z) {}
	
	// Constructeur par copie
	Triplet<T>(const Triplet<T> & tuple) :
			_x(tuple._x), _y(tuple._y), _z(tuple._z) {}
	
	// Operateur d'affectation
	Triplet<T> & operator=(const Triplet<T> & tuple) {
		if (this != &tuple) {
			this->_x = tuple._x;
			this->_y = tuple._y;
			this->_z = tuple._z;
		}
		return *this;
	}
	
	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Triplet<T>(const Triplet<U> & tuple) :
			_x(tuple.getX()), _y(tuple.getY()), _z(tuple.getZ()) {}
	
	//	// Destructeurs
	//	virtual ~Triplet<T>() {
	//	}
	
	// Accesseurs	
	const T & getX() const;
	const T & getY() const;
	const T & getZ() const;
	virtual const T getW() const {
		return T();
	}
	
	const T & operator[](const int & indice) const;

	// Accesseurs non const
	T & operator[](const int & indice);

	// Modifieurs
	virtual Triplet<T> & setX(const T & x);
	virtual Triplet<T> & setY(const T & y);
	virtual Triplet<T> & setZ(const T & z);
	virtual Triplet<T> & setXYZ(const T & x,
								const T & y,
								const T & z);
	virtual Triplet<T> & setXYZ(const Triplet<T> & XYZ);
	virtual Triplet<T> & setXY(	const T & x,
								const T & y);

	// Methodes Constantes
	/**
	 * Methode de comparaison entre Triplets
	 */
	template<typename U>
	const bool equal(const Triplet<U> & tuple2) const;
	template<typename U>
	const bool operator==(const Triplet<U> & tuple2) const;
	template<typename U>
	const bool operator!=(const Triplet<U> & tuple2) const;

	template<typename U>
	const bool operator<(const Triplet<U> & tuple2) const;

	/**
	 * Retourne ce triplet sous forme d'un std::vector
	 */
	const std::vector<T> toSTDvector() const;

	/**
	 * Retourne une chaine de caractere decrivant ce Triplet
	 */
	virtual const std::string toString() const;

	template<typename U>
	friend std::ostream &operator<<(	std::ostream &stream,
								const Triplet<U> & point);

	/**
	 * Retourne la taille de cet objet en memoire
	 */
	virtual const unsigned int memorySize() const;

	// Methodes modifiantes
	/*********************************************************************
	 * Methodes arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Modifie l'instance courante.
	 *********************************************************************/
	template<typename U>
	Triplet<T> & add(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & sub(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & mul(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & div(const Triplet<U> & t2);

	/*********************************************************************
	 * Operateurs arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Modifie l'instance courante.
	 *********************************************************************/
	template<typename U>
	Triplet<T> & operator+=(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & operator-=(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & operator*=(const Triplet<U> & t2);
	template<typename U>
	Triplet<T> & operator/=(const Triplet<U> & t2);

	/*********************************************************************
	 * Methodes arithmetiques entre un Triplet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Triplet.
	 * Modifie l'instance courante.
	 *********************************************************************/
	Triplet<T> & add(const T & a);
	Triplet<T> & sub(const T & m);
	Triplet<T> & mul(const T & m);
	Triplet<T> & div(const T & d);

	/*********************************************************************
	 * Operateurs arithmetiques entre un Triplet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Triplet.
	 * Modifie l'instance courante.
	 *********************************************************************/
	Triplet<T> & operator+=(const T & a);
	Triplet<T> & operator-=(const T & m);
	Triplet<T> & operator*=(const T & m);
	Triplet<T> & operator/=(const T & d);

	// Methodes constantes
	/*********************************************************************
	 * Methodes arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Triplet<T> add(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> sub(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> mul(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> div(const Triplet<U> & t2) const;

	/*********************************************************************
	 * Operateurs arithmetiques entre 2 Triplet.
	 * Les operations sont effectuees membre a membre.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	template<typename U>
	const Triplet<T> operator+(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> operator-(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> operator*(const Triplet<U> & t2) const;
	template<typename U>
	const Triplet<T> operator/(const Triplet<U> & t2) const;

	/*********************************************************************
	 * Methodes arithmetiques entre un Triplet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Triplet.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Triplet<T> add(const T & a) const;
	const Triplet<T> sub(const T & m) const;
	const Triplet<T> mul(const T & m) const;
	const Triplet<T> div(const T & d) const;

	/*********************************************************************
	 * Operateurs arithmetiques entre un Triplet et un facteur (scalaire).
	 * Le facteur est additionner/soustrait/multiplier/diviser
	 * a chaque membre du Triplet.
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Triplet<T> operator+(const T & a) const;
	const Triplet<T> operator-(const T & m) const;
	const Triplet<T> operator*(const T & m) const;
	const Triplet<T> operator/(const T & d) const;

	/*********************************************************************
	 * Retourne l'oppose du Triplet : (-Triplet).
	 * Ne modifie pas l'instance courante :
	 * retourne le resultat dans une nouvelle instance d'Energie.
	 *********************************************************************/
	const Triplet<T> negate() const;

	template<typename U>
	/**
	 * Calcule le determinant avec le Tuple t2 dans le plan XY
	 */
	const T determinantXY(const Triplet<U> & t2);

	template<typename U>
	/**
	 * Calcule le determinant avec le Tuple t2 dans le plan YZ
	 */
	const T determinantYZ(const Triplet<U> & t2);

	template<typename U>
	/**
	 * Calcule le determinant avec le Tuple t2 dans le plan ZX
	 */
	const T determinantZX(const Triplet<U> & t2);
};

template<typename U>
std::ostream & operator<<(	std::ostream &stream,
						const Triplet<U> & triplet) {
	stream << "(" << triplet.getX() << ", " << triplet.getY() << ", " << triplet.getZ() << ")";
	
	return stream;
}

template<class T>
const int Triplet<T>::X;
template<class T>
const int Triplet<T>::Y;
template<class T>
const int Triplet<T>::Z;

template<typename T>
inline const T & Triplet<T>::getX() const {
	return _x;
}
template<typename T>
inline const T & Triplet<T>::getY() const {
	return _y;
}
template<typename T>
inline const T & Triplet<T>::getZ() const {
	return _z;
}
template<typename T>
const T & Triplet<T>::operator[](const int & indice) const {
	if (indice == Triplet<T>::X) {
		return _x;
	} else if (indice == Triplet<T>::Y) {
		return _y;
	} else { // TODO exception si different Z
		return _z;
	}
}

template<typename T>
T & Triplet<T>::operator[](const int & indice) {
	if (indice == Triplet<T>::X) {
		return _x;
	} else if (indice == Triplet<T>::Y) {
		return _y;
	} else { // TODO exception si different Z
		return _z;
	}
}

template<typename T>
inline Triplet<T> & Triplet<T>::setX(const T & x) {
	_x = x;
	return (*this);
}
template<typename T>
inline Triplet<T> & Triplet<T>::setY(const T & y) {
	_y = y;
	return (*this);
}

template<typename T>
inline Triplet<T> & Triplet<T>::setZ(const T & z) {
	_z = z;
	return (*this);
}

template<typename T>
inline Triplet<T> & Triplet<T>::setXYZ(	const T & x,
										const T & y,
										const T & z) {
	_x = x;
	_y = y;
	_z = z;
	return (*this);
}
template<typename T>
inline Triplet<T> & Triplet<T>::setXYZ(const Triplet<T> & XYZ) {
	_x = XYZ.getX();
	_y = XYZ.getY();
	_z = XYZ.getZ();
	return (*this);
}
template<typename T>
inline Triplet<T> & Triplet<T>::setXY(	const T & x,
										const T & y) {
	_x = x;
	_y = y;
	return (*this);
}

/*********************************************************************
 * Methodes arithmetiques entre 2 Triplet.
 * Les operations sont effectuees membre a membre.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::add(const Triplet<U> & t2) {
	_x += t2.getX();
	_y += t2.getY();
	_z += t2.getZ();
	return *this;
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::sub(const Triplet<U> & t2) {
	_x -= t2.getX();
	_y -= t2.getY();
	_z -= t2.getZ();
	return *this;
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::mul(const Triplet<U> & t2) {
	_x *= t2.getX();
	_y *= t2.getY();
	_z *= t2.getZ();
	return *this;
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::div(const Triplet<U> & t2) {
	_x /= t2.getX();
	_y /= t2.getY();
	_z /= t2.getZ();
	return *this;
}

/*********************************************************************
 * Operateurs arithmetiques entre 2 Triplet.
 * Les operations sont effectuees membre a membre.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::operator+=(const Triplet<U> & t2) {
	return add(t2);
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::operator-=(const Triplet<U> & t2) {
	return sub(t2);
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::operator*=(const Triplet<U> & t2) {
	return mul(t2);
}

template<typename T>
template<typename U>
inline Triplet<T> & Triplet<T>::operator/=(const Triplet<U> & t2) {
	return div(t2);
}

/*********************************************************************
 * Methodes arithmetiques entre un Triplet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Triplet.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
inline Triplet<T> & Triplet<T>::add(const T & a) {
	_x += a;
	_y += a;
	_z += a;
	return *this;
}

template<typename T>
inline Triplet<T> & Triplet<T>::sub(const T & s) {
	_x -= s;
	_y -= s;
	_z -= s;
	return *this;
}

template<typename T>
inline Triplet<T> & Triplet<T>::mul(const T & m) {
	_x *= m;
	_y *= m;
	_z *= m;
	return *this;
}

template<typename T>
inline Triplet<T> & Triplet<T>::div(const T & d) {
	_x /= d;
	_y /= d;
	_z /= d;
	return *this;
}

/*********************************************************************
 * Operateurs arithmetiques entre un Triplet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Triplet.
 * Modifie l'instance courante.
 *********************************************************************/
template<typename T>
inline Triplet<T> & Triplet<T>::operator+=(const T & a) {
	return add(a);
}

template<typename T>
inline Triplet<T> & Triplet<T>::operator-=(const T & s) {
	return sub(s);
}

template<typename T>
inline Triplet<T> & Triplet<T>::operator*=(const T & m) {
	return mul(m);
}

template<typename T>
inline Triplet<T> & Triplet<T>::operator/=(const T & d) {
	return div(d);
}

/*********************************************************************
 * Methodes arithmetiques entre 2 Triplet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::add(const Triplet<U> & t2) const {
	return Triplet<T>(getX() + t2.getX(), getY() + t2.getY(), getZ() + t2.getZ());
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::sub(const Triplet<U> & t2) const {
	return Triplet<T>(getX() - t2.getX(), getY() - t2.getY(), getZ() - t2.getZ());
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::mul(const Triplet<U> & t2) const {
	return Triplet<T>(getX() * t2.getX(), getY() * t2.getY(), getZ() * t2.getZ());
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::div(const Triplet<U> & t2) const {
	return Triplet<T>(getX() / t2.getX(), getY() / t2.getY(), getZ() / t2.getZ());
}

/*********************************************************************
 * Operateurs arithmetiques entre 2 Triplet.
 * Les operations sont effectuees membre a membre.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::operator+(const Triplet<U> & t2) const {
	return add(t2);
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::operator-(const Triplet<U> & t2) const {
	return sub(t2);
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::operator*(const Triplet<U> & t2) const {
	return mul(t2);
}

template<typename T>
template<typename U>
inline const Triplet<T> Triplet<T>::operator/(const Triplet<U> & t2) const {
	return div(t2);
}

/*********************************************************************
 * Operations arithmetiques entre un Triplet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Triplet.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
inline const Triplet<T> Triplet<T>::add(const T & a) const {
	return Triplet<T>(getX() + a, getY() + a, getZ() + a);
}

template<typename T>
inline const Triplet<T> Triplet<T>::sub(const T & s) const {
	return Triplet<T>(getX() - s, getY() - s, getZ() - s);
}

template<typename T>
inline const Triplet<T> Triplet<T>::mul(const T & m) const {
	return Triplet<T>(getX() * m, getY() * m, getZ() * m);
}

template<typename T>
inline const Triplet<T> Triplet<T>::div(const T & d) const {
	return Triplet<T>(getX() / d, getY() / d, getZ() / d);
}

/*********************************************************************
 * Operateurs arithmetiques entre un Triplet et un facteur (scalaire).
 * Le facteur est additionner/soustrait/multiplier/diviser
 * a chaque membre du Triplet.
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 *********************************************************************/
template<typename T>
inline const Triplet<T> Triplet<T>::operator+(const T & a) const {
	return add(a);
}

template<typename T>
inline const Triplet<T> Triplet<T>::operator-(const T & s) const {
	return sub(s);
}

template<typename T>
inline const Triplet<T> Triplet<T>::operator*(const T & m) const {
	return mul(m);
}

template<typename T>
inline const Triplet<T> Triplet<T>::operator/(const T & d) const {
	return div(d);
}

/**
 * Retourne l'oppose du Triplet : (-Triplet).
 * Ne modifie pas l'instance courante :
 * retourne le resultat dans une nouvelle instance d'Energie.
 */
template<typename T>
inline const Triplet<T> Triplet<T>::negate() const {
	return Triplet<T>(-getX(), -getY(), -getZ());
}

template<typename T>
template<typename U>
inline const bool Triplet<T>::equal(const Triplet<U> & tuple2) const {
	return (getX() == tuple2.getX() && getY() == tuple2.getY() && getZ() == tuple2.getZ() && getW() == tuple2.getW());
}

template<typename T>
template<typename U>
inline const bool Triplet<T>::operator==(const Triplet<U> & tuple2) const {
	return (equal(tuple2));
}

template<typename T>
template<typename U>
inline const bool Triplet<T>::operator!=(const Triplet<U> & tuple2) const {
	return (!equal(tuple2));
}

template<typename T>
template<typename U>
inline const bool Triplet<T>::operator<(const Triplet<U> & tuple2) const {
	// On ordonne selon les Z, puis les Y, puis les X
	return ((this->getZ() < tuple2.getZ())
			|| ((this->getZ() == tuple2.getZ()) && (this->getY() < tuple2.getY() || (this->getY() == tuple2.getY() && this->getX() < tuple2.getX()))));
}

template<typename T>
template<typename U>
/**
 * Calcule le determinant avec le Tuple t2 dans le plan XY
 */
inline const T Triplet<T>::determinantXY(const Triplet<U> & t2) {
	return getX() * t2.getY() - getY() * t2.getX();
}

template<typename T>
template<typename U>
/**
 * Calcule le determinant avec le Tuple t2 dans le plan YZ
 */
inline const T Triplet<T>::determinantYZ(const Triplet<U> & t2) {
	return getY() * t2.getZ() - getZ() * t2.getY();
}

template<typename T>
template<typename U>
/**
 * Calcule le determinant avec le Tuple t2 dans le plan ZX
 */
inline const T Triplet<T>::determinantZX(const Triplet<U> & t2) {
	return getZ() * t2.getX() - getX() * t2.getZ();
}

template<typename T>
inline const std::vector<T> Triplet<T>::toSTDvector() const {
	std::vector<T> res(3);
	res[0] = getX();
	res[1] = getY();
	res[2] = getZ();
	return res;
}

template<typename T>
inline const std::string Triplet<T>::toString() const {
	std::ostringstream os;
	os << *this;
	return os.str();
}

/**
 * Retourne la taille de cet objet en memoire
 */
template<typename T>
inline const unsigned int Triplet<T>::memorySize() const {
	return 3 * sizeof(T);
}

#endif /* TRIPLET_H_ */
