/*! Time-stamp: <@(#) Vecteur.hpp   22/09/2009   LAURET Nicolas CESBIO>
 *********************************************************************
 *  @file    : Vecteur.hpp
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : LAURET Nicolas                        Date: 22/09/2009
 *
 *  Description : Classe representant un vecteur
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

#ifndef VECTEUR_H_
#define VECTEUR_H_

#include "Triplet.hpp"
#include <string>
#include <iostream>
#include <cmath>

template<typename T>
class Vecteur: public Triplet<T> {
public:
	// Constructeurs
	Vecteur<T>(	const T & x = 0,
				const T & y = 0,
				const T & z = 0) :
			Triplet<T>(x, y, z) {}
	// ELG : Constructeurs a partir d'un vector a trois dimensions
	Vecteur<T>(const std::vector<T> & vector) :
			Triplet<T>(vector[0], vector[1], vector[2]) {
	}
	// Constructeur par copie
	Vecteur<T>(const Triplet<T>& v2) :
			Triplet<T>(v2.getX(), v2.getY(), v2.getZ()) {}
	
	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Vecteur<T>(const Triplet<U>& v2) :
			Triplet<T>(v2.getX(), v2.getY(), v2.getZ()) {}
	
	//	// Destructeurs
	//	virtual ~Vecteur<T> () {
	//	}
	
	// Accesseurs et Modifieurs herites de Triplet
	
	// Accesseurs
	const T getW() const;
	const T getLongueur() const;
	const T getLongueurCarre() const;

	// Methodes Constantes
	template<typename U>
	const T scalaire(const Vecteur<U> & v2) const;

	template<typename U>
	const T scalaire(const Triplet<U> & v2) const;

	template<typename U>
	const Vecteur<T> vectoriel(const Vecteur<U> & v2) const;

	const Vecteur<T> normaliser() const;

	// Methodes Non Constantes
	template<typename U>
	Vecteur<T> & vectorielEgal(const Vecteur<U> & v2) {
		this->setXYZ(
				Triplet<T>::getY() * v2.Triplet<U>::getZ() - Triplet<T>::getZ() * v2.Triplet<U>::getY(),
				Triplet<T>::getZ() * v2.Triplet<U>::getX() - Triplet<T>::getX() * v2.Triplet<U>::getZ(),
				Triplet<T>::getX() * v2.Triplet<U>::getY() - Triplet<T>::getY() * v2.Triplet<U>::getX());
		return *this;

	}

	Vecteur<T> & normaliserEgal();
	
	//const string toString() const;
};

template<typename T>
inline const T Vecteur<T>::getW() const {
	return 0.;
}

// Methodes Constantes
template<typename T>
inline const T Vecteur<T>::getLongueur() const {
	return sqrt(getLongueurCarre());
}

template<typename T>
inline const T Vecteur<T>::getLongueurCarre() const {
	// Le passage par template impose ici que l'appel des methodes vides heritees
	// (type_ret fun(void)) soit explicite comme utilisee a partir de la classe mere
	return (Triplet<T>::getX() * Triplet<T>::getX() + Triplet<T>::getY() * Triplet<T>::getY() + Triplet<T>::getZ() * Triplet<T>::getZ());
}

template<typename T>
template<typename U>
inline const T Vecteur<T>::scalaire(const Vecteur<U> & v2) const {
	// Le passage par template impose ici que l'appel des methodes vides heritees
	// (type_ret fun(void)) soit explicite comme utilisee a partir de la classe mere
	return (Triplet<T>::getX() * v2.Triplet < U > ::getX() + Triplet<T>::getY() * v2.Triplet < U > ::getY() + Triplet<T>::getZ() * v2.Triplet < U
			> ::getZ());
}

template<typename T>
template<typename U>
inline const T Vecteur<T>::scalaire(const Triplet<U> & v2) const {
	// Le passage par template impose ici que l'appel des methodes vides heritees
	// (type_ret fun(void)) soit explicite comme utilisee a partir de la classe mere
	return (Triplet<T>::getX() * v2.Triplet < U > ::getX() + Triplet<T>::getY() * v2.Triplet < U > ::getY() + Triplet<T>::getZ() * v2.Triplet < U
			> ::getZ());
}

template<typename T>
template<typename U>
inline const Vecteur<T> Vecteur<T>::vectoriel(const Vecteur<U> & v2) const {
	// Le passage par template impose ici que l'appel des methodes vides heritees
	// (type_ret fun(void)) soit explicite comme utilisee a partir de la classe mere
	return Vecteur<T>(Triplet<T>::getY() * v2.Triplet < U > ::getZ() - Triplet<T>::getZ() * v2.Triplet < U > ::getY(),
		Triplet<T>::getZ() * v2.Triplet < U > ::getX() - Triplet<T>::getX() * v2.Triplet < U > ::getZ(),
		Triplet<T>::getX() * v2.Triplet < U > ::getY() - Triplet<T>::getY() * v2.Triplet < U > ::getX());
}

template<typename T>
inline const Vecteur<T> Vecteur<T>::normaliser() const {
	if (this->getLongueur() > 0.0) {
		return Vecteur<T>(*this / this->getLongueur());
	} else {
		return Vecteur<T>(*this);
	}
}

//template<typename T>
//inline const string Vecteur<T>::toString() const {
//	// Le passage par template impose ici que l'appel des methodes vides heritees
//	// (type_ret fun(void)) soit explicite comme utilisee a partir de la classe mere
//	string res = "(";
//	res.append(DartUtils::whatever2string(Triplet<T>::getX()));
//	res.append(",");
//	res.append(DartUtils::whatever2string(Triplet<T>::getY()));
//	res.append(",");
//	res.append(DartUtils::whatever2string(Triplet<T>::getZ()));
//	res.append(")");
//	return res;
//}

// Methodes Modifiantes

template<typename T>
inline Vecteur<T> & Vecteur<T>::normaliserEgal() {
	this->div(this->getLongueur());
	return *this;
}

#endif /* VECTEUR_H_ */
