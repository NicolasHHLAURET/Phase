#ifndef READILWIS_HPP_
#define READILWIS_HPP_

// bibliothèques utilisées
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

#include "IliwisConstante.hpp"
#include "../Conversions.hpp"

using namespace std;

// prototypage des fonctions
void lireFichierGRdiese(const string repertoireImage,
						const string nomImage,
						vector<vector<vector<double> > >& coordonneesGeoreferentiel,
						unsigned int& nombreLignes,
						unsigned int& nombreColonnes);
void lireFichierGRF(const string repertoireImage,
					const string nomImage,
					unsigned int& nombreLignes,
					unsigned int& nombreColonnes);
void lireFichierMPR(const string repertoireImage,
					const string nomImage,
					unsigned int& nombreLignes,
					unsigned int& nombreColonnes,
					float& taillePixelX,
					float& taillePixelY,
					string& dataType
   				);
void allerASection(	std::ifstream& flux,
					string nomSection);

/**
 * Cette fonction permet de chercher dans un fichier (representer par son flux)
 * la valeur d'un attribut represente par son nom.
 * Un attribut est délimite reconu comme partie gauche du signe '='
 * Sa valeur est la partie droite du signe '='
 * Un message d'erreur est afficher dans le cas ou l'attribut n'a pas ete trouve.
 *
 * !! ATTENTION: le flux lut est fourni ouvert et sera rendu apres la lecture de l'attribut !!
 * !!      la position dans le flux sera changee    !!!
 *
 * @flux		flux ouvert dans lequel on vas rechercher des informations.
 * @nomAttribut	nom de l'attribut dont on veux recuperer la valeur.
 * @valeurAttribut valeur trouver pour l'attribut recherche.
 */
bool recupValeurAttribut(	std::ifstream& flux,
							string& nomAttribut,
							string& valeurAttribut);

/**
 * Cette fonction permet de chercher dans un fichier (representer par son flux)
 * la valeur d'un attribut represente par son nom.
 * Un attribut est délimite reconu comme partie gauche du signe '='
 * Sa valeur est la partie droite du signe '='
 * Un message d'erreur est afficher dans le cas ou l'attribut n'a pas ete trouve.
 *
 * !! ATTENTION: le flux lut est fourni ouvert et sera rendu dans le meme etat que lors de l'appel !!
 * !!      la position dans le flux sera inchangee    !!!
 *
 * @flux		flux ouvert dans lequel on vas rechercher des informations.
 * @nomAttribut	nom de l'attribut dont on veux recuperer la valeur.
 * @valeurAttribut valeur trouver pour l'attribut recherche.
 */
bool recupValeurAttribut_unSafe(std::ifstream& flux,
								string& nomAttribut,
								string& valeurAttribut);

#endif /* READILWIS_HPP_ */
