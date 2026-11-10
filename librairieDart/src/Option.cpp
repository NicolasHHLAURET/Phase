/*
 * option.cpp
 *
 *  Created on: 10 nov. 2009
 *      Author: goulierv
 */

#include "Option.hpp"

using namespace std;

// constructeur
Option::Option() {}
;

// fonction qui enregistre une valeur pour l'argument
bool Option::ajouterValeur(	int nbArg,
							string val) {
	valeurs.push_back(val);
	if (valeurs.size() > (unsigned int) nbArg)
		return false;
	return true;
}
;

// fonction qui indique le nombre de valeurs d'une option
int Option::getNbOpt(string id) {
	return valeurs.size();
}

// fonction qui renvoit la valeur (string) de l'option demandee
string Option::getValeurS(int i) {
	if ((unsigned int) i >= valeurs.size()) {
		throw IndexErrorException("indice trop grand");
	}
	return valeurs[i];
}
;

// fonction qui renvoit la valeur (int) de l'option demandee
int Option::getValeurI(int i) {
	if ((unsigned int) i >= valeurs.size()) {
		throw IndexErrorException("indice trop grand");
	}
	return Conversions::convertitStringVersAutre<int>(valeurs[i]);
}
;

// fonction qui renvoit la valeur (float) de l'option demandee
float Option::getValeurF(int i) {
	if ((unsigned int) i >= valeurs.size()) {
		throw IndexErrorException("indice trop grand");
	}
	return Conversions::convertitStringVersAutre<float>(valeurs[i]);
}
;

// fonction qui renvoit la valeur (double) de l'option demandee
double Option::getValeurD(int i) {
	if ((unsigned int) i >= valeurs.size()) {
		throw IndexErrorException("indice trop grand");
	}
	return Conversions::convertitStringVersAutre<double>(valeurs[i]);
}
;

// fonction qui renvoit la valeur (bool) de l'option demandee
bool Option::getValeurB(int i) {
	if ((unsigned int) i >= valeurs.size()) {
		throw IndexErrorException("indice trop grand");
	}
	if (valeurs[i] == "true")
		return true;
	else if (valeurs[i] == "false")
		return false;
	// Tous les cas normaux sont geres a ce point. Laissons la forme suivante pour les cas d'erreur
	return Conversions::convertitStringVersAutre<bool>(valeurs[i]);
}
;

