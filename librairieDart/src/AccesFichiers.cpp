/*
 * Dao.cpp
 *
 *  Created on: 29 sept. 2009
 *      Author: crosj
 */
#include "../include/AccesFichiers.hpp"

using namespace std;
map<string, string> AccesFichiers::lireFichierProperties(const string & cheminAbsolu) {
	//std::cout << "AccesFichiers::lireFichierProperties , chemin : " << cheminAbsolu << std::endl;
	ifstream fichierProperties(cheminAbsolu.c_str(), ifstream::in);
	map<string, string> listeProperties;
	string ligne;
	string clef, valeurProperties;
	size_t positionSeparateur;
	if (fichierProperties) {
		//TODO respecter les caracteristiques des properties ? Ex: ':' et '=' separent clef/valeur, mais '\:' et '\=' non. Ou encore: '#' et '!' ne commentent que s'ils sont le premier caractere non "blanc" de la ligne | '\' en fin de ligne signale qu'on ignore le saut de ligne (et les caracteres blancs qui suivent). cf http://en.wikipedia.org/wiki/.properties et http://download.oracle.com/javase/6/docs/api/java/util/Properties.html
		while (getline(fichierProperties, ligne)) {
			//TODO commentaires # et ! a detecter moins rudimentairement (ici commentaire seulement si premier caractere)
			if (ligne.size() > 0 && ligne[0] != '#' && ligne[0] != '!') {
				//Recherche de la position du separateur (en generale ":") dans la chaine de caracteres
				positionSeparateur = ligne.find(this->getSeparateur());
				//Si on trouve le separateur dans la chaine
				if (positionSeparateur != string::npos) {
					//On cree la cle avec tous les elements de la chaine se trouvant a gauche du separateur
					clef = ligne.substr(0, positionSeparateur);
					//La valeur represente tous les elements a droite du separateur
					valeurProperties = ligne.substr(positionSeparateur + 1, ligne.npos);
					listeProperties[clef] = valeurProperties;
				} else {
					//Recherche de la position du separateur (cas "=") dans la chaine de caracteres
					positionSeparateur = ligne.find("=");
					//Si on trouve le separateur dans la chaine
					if (positionSeparateur != string::npos) {
						//On cree la cle avec tous les elements de la chaine se trouvant a gauche du separateur
						clef = ligne.substr(0, positionSeparateur);
						//La valeur represente tous les elements a droite du separateur
						valeurProperties = ligne.substr(positionSeparateur + 1, ligne.npos);
						listeProperties[clef] = valeurProperties;
					}
				}
			}
		}
		fichierProperties.close();
	} else {
		//On leve une libDartException car si le fichier n'existe pas, il vaut mieux tout arreter
		string message = "fichier " + cheminAbsolu + "inexistant : verifier le chemin de la simulation : " + cheminAbsolu;
		throw LibDartException(message);
	}
	return listeProperties;
}

