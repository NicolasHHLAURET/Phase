#ifndef WRITEILWIS_HPP_
#define WRITEILWIS_HPP_

// bibliothèques utilisees
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>

#include "IliwisConstante.hpp"

using namespace std;

class WriteIlwis {
public:
// prototypage des fonctions
	static void ecrireFichierGRdiese(	const string & repertoireImage,
								const string & nomImage,
								vector<vector<vector<double> > > coordonneesGeoreferentiel,
								const unsigned int nombreLignes,
								const unsigned int nombreColonnes);
	static void ecrireFichierGRF(	const string & repertoireImage,
							const string & nomImage,
							const unsigned int nombreLignes,
							const unsigned int nombreColonnes);
	static void ecrireFichierMPR(	const string & repertoireImage,
							const string & nomImage,
							const unsigned int nombreLignes,
							const unsigned int nombreColonnes,
							const float deltaX,
							const float deltaY,
							const double valMinImage,
							const double valMaxImage,
							bool createGeo = false);
};

#endif /* WRITEILWIS_HPP_ */
