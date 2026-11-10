/*
 * DirectionsMetier.h
 *
 *  Created on: 25 sept. 2009
 *      Author: crosj
 */

#ifndef ACCESFICHIERS_H_
#define ACCESFICHIERS_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "LibDartException.hpp"

class AccesFichiers {
private:
	/**
	 * Attribut de la classe
	 * @param separateur : le separateur differenciant les composantes d'une cle de recherche et
	 * la valeur correspondante
	 */
	std::string separateur;

public:
	/**
	 * Constructeur par defaut
	 * @param defaultSperateur : permet de definir le separateur que l'on souhaite utiliser pour le mapping
	 * d'un fichier de donnees. (Par defaut, on prend ":")
	 */
	AccesFichiers(const std::string & defaultSeparateur = ":") :
			separateur(defaultSeparateur) {}
	
	/**
	 * Getter et setter de l'attribut de la classe
	 */
	const std::string & getSeparateur();
	void setSeparateur(const std::string & nouveauSeparateur);

	/**
	 * Methode de lecture des proprietes d'un fichier
	 */
	std::map<std::string, std::string> lireFichierProperties(const std::string & cheminAbsolu);

	/**
	 * Destructeurs
	 */
	virtual ~AccesFichiers() {}
};

inline const std::string & AccesFichiers::getSeparateur() {
	return (this->separateur);
}

inline void AccesFichiers::setSeparateur(const std::string & nouveauSeparateur) {
	this->separateur = nouveauSeparateur;
}

#endif /* ACCESFICHIERS_H_ */
