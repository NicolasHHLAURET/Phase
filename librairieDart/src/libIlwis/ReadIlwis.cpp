#include "../../include/libIlwis/ReadIlwis.hpp"

using namespace std;

void lireFichierGRdiese(const string repertoireImage,
						const string nomImage,
						vector<vector<vector<double> > >& coordonneesGeoreferentiel,
						unsigned int& nombreLignes,
						unsigned int& nombreColonnes) {
	// TODO
	int O_S = LINUX_OS;
	/**
	 * L'entete du fichier ILWIS doit etre compose de 128 octets.
	 * Les 16 premiers contiennent la chaine "ILWIS 2.00 Table".
	 * Les 4 suivants sont les valeurs en octet : 13, 10, 26 et 0.
	 * Les 108 octets suivants sont remplis par des 0 (en octet)
	 */
	string entete = "ILWIS 2.00 Table";
	unsigned char zero /*= 0*/;
	char c1 /*= 13*/, c2 /*= 10*/, c3 /*= 26*/, c4 /*= 0*/;
	
	string grDiezFilePath = repertoireImage + nomImage + ".gr#";
	ifstream grDiezBinaryStream(grDiezFilePath.c_str(), ios::in | ios::binary);
	
	if (grDiezBinaryStream) {
		grDiezBinaryStream.read((char *) &(entete[0]), entete.size());
		grDiezBinaryStream.read((char *) &c1, 1);
		grDiezBinaryStream.read((char *) &c2, 1);
		grDiezBinaryStream.read((char *) &c3, 1);
		grDiezBinaryStream.read((char *) &c4, 1);
		for (unsigned int i = 0; i < 108; ++i) {
			grDiezBinaryStream.read((char *) &zero, 1);
		}
		// Les fichiers ILWIS doivent etre ecrit en little Endian
		// Si l'OS est en big Endian, il faudra donc "inverser" l'ordre d'ecriture des octets.
		
		// Une "union" est une structure dont les variables/attributs partagent le meme emplacement memoire.
		// Elle va nous permettre ici de "convertir" nos valeurs double, int etc, en tableau de char/octet,
		// que l'on pourra ensuite "inverser" le cas echeant.
		union doubleEnChar {
			double doubleValue; // Contient la meme information que octetArray
			char octetArray[sizeof(double)]; // Contient la meme information que doubleValue
			
			void inverserEndian() {
				const unsigned int size = sizeof(double);
				char temp;
				for (unsigned int i = 0; i < size / 2; ++i) {
					temp = octetArray[i];
					octetArray[i] = octetArray[size - i - 1];
					octetArray[size - i - 1] = temp;
				}
			}
		};
		
		union intEnChar {
			int intValue; // Contient la meme information que octetArray
			char octetArray[sizeof(int)]; // Contient la meme information que intValue
			
			void inverserEndian() {
				const unsigned int size = sizeof(int);
				char temp;
				for (unsigned int i = 0; i < size / 2; ++i) {
					temp = octetArray[i];
					octetArray[i] = octetArray[size - i - 1];
					octetArray[size - i - 1] = temp;
				}
			}
		};
		
		doubleEnChar dimensionLigne;
		doubleEnChar dimensionColonne;
		intEnChar indiceLigne;
		intEnChar indiceColonne;
		intEnChar actif;
		
		grDiezBinaryStream.read(dimensionLigne.octetArray, sizeof(double));
		grDiezBinaryStream.read(dimensionColonne.octetArray, sizeof(double));
		grDiezBinaryStream.read(indiceLigne.octetArray, sizeof(int));
		grDiezBinaryStream.read(indiceColonne.octetArray, sizeof(int));
		grDiezBinaryStream.read(actif.octetArray, sizeof(int));
		
		if (O_S == UNIX_OS) {
			indiceLigne.inverserEndian();
			indiceColonne.inverserEndian();
			dimensionLigne.inverserEndian();
			dimensionColonne.inverserEndian();
			actif.inverserEndian();
		}
		
		for (unsigned int i = 0; i < 4; ++i) {
			switch (i) {
				case 0: {
					//(X min,Y max ) --> (ligne min,  colonne min) :
					indiceLigne.intValue = 1;
					indiceColonne.intValue = 1;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[0][1][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[0][1][0];
					break;
				}
				case 1: {
					//(X max,Y max ) --> (ligne min, colonne max) :
					indiceLigne.intValue = 1;
					indiceColonne.intValue = nombreColonnes;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[3][1][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[3][1][0];
					break;
				}
				case 2: {
					//(X min,Y min ) --> (ligne max, colonne min) :
					indiceLigne.intValue = nombreLignes;
					indiceColonne.intValue = 1;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[1][1][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[1][1][0];
					break;
				}
				case 3:
				default: {
					//(X max,Y min ) --> (ligne max, colonne max) :
					indiceLigne.intValue = nombreLignes;
					indiceColonne.intValue = nombreColonnes;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[2][1][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[2][1][0];
					break;
				}
			} // Fin switch
		}
		grDiezBinaryStream.close();
	} else {
		cerr << "Unable to open file " << grDiezFilePath << endl;
	}
}

void lireFichierGRF(const string repertoireImage,
					const string nomImage,
					unsigned int& nombreLignes,
					unsigned int& nombreColonnes) {
	string fileName = repertoireImage + nomImage + ".grf";
	string valAttribut, nomAttribut;
	ifstream fichierGRF(fileName.c_str(), ios::in);
	
	if (fichierGRF) {
		// recuperation du nombre de lignes
		nomAttribut = "lines";
		recupValeurAttribut(fichierGRF, nomAttribut, valAttribut);
		nombreLignes = Conversions::convertitStringVersAutre<unsigned int>(valAttribut);
		// recuperation du nombre de colonnes
		nomAttribut = "columns";
		recupValeurAttribut(fichierGRF, nomAttribut, valAttribut);
		nombreColonnes = Conversions::convertitStringVersAutre<unsigned int>(valAttribut);
		fichierGRF.close();
	} else {
		cerr << "Unable to open file " << fileName << endl;
	}
}

void lireFichierMPR(const string repertoireImage,
					const string nomImage,
					unsigned int& nombreLignes,
					unsigned int& nombreColonnes,
					float& taillePixelX,
					float& taillePixelY,
					string& dataType
   				) {
	string fileName = repertoireImage + nomImage + ".mpr";
	string line, valAttribut, nomAttribut;
	ifstream fichierMPR(fileName.c_str(), ios::in);
	float pMinx, pMiny, pMaxx, pMaxy;
	
	if (fichierMPR) {
		while (!fichierMPR.fail() && !fichierMPR.eof()) {
			getline(fichierMPR, line);
			if (line.find("CoordBounds") == 0) {
				stringstream ss(line.substr(line.find("=") + 1, line.length()), istringstream::in);
				ss >> pMiny >> pMinx  >> pMaxy >> pMaxx;
			}
			else if (line.find("Size") == 0) {
				stringstream ss(line.substr(line.find("=") + 1, line.length()), istringstream::in);
				ss >> nombreLignes >> nombreColonnes;
			}
			else if (line.find("Type") == 0) {
				dataType = line.substr(line.find("=") + 1, line.length());
			}
		}
		fichierMPR.close();
		// calcul de la taille des pixels
		taillePixelX = fabs(pMaxx - pMinx) / nombreLignes;
		taillePixelY = fabs(pMaxy - pMiny) / nombreColonnes;
	} else {
		cerr << "Unable to open file " << fileName << endl;
		exit(EXIT_FAILURE);
	}
}

void allerASection(	ifstream& flux,
					string nomSection) {
	string line;		// ligne courante lue dans le flux
	bool trouve = false;	// indique si l'on a trouver l'attribut recherche
	
	// on recherche l'attribut
	while (!flux.fail() && !flux.eof() && !trouve) {
		getline(flux, line);
		if (line.find("[" + nomSection + "]") == 0) {
			trouve = true;
		}
	}
	// on previent l'utilisateur dans le cas ou on a pas trouver l'attribut
	if (!trouve) {
		cerr << "The section " << nomSection << " can not be found!" << endl;
	}
}

bool recupValeurAttribut(	ifstream& flux,
							string& nomAttribut,
							string& valeurAttribut) {
	string line;		// ligne courante lue dans le flux
	bool trouve = false;	// indique si l'on a trouver l'attribut recherche
	
	// on recherche l'attribut
	while (!flux.fail() && !flux.eof() && !trouve) {
		getline(flux, line);
		if (line.find(nomAttribut) == 0) {
			valeurAttribut = line.substr(line.find("=") + 1, line.length());
			trouve = true;
		}
	}
	// on previent l'utilisateur dans le cas ou on a pas trouver l'attribut
	if (!trouve) {
		cerr << "The attribute " << nomAttribut << " can not be found!" << endl;
	}
	return trouve;
}

bool recupValeurAttribut_unSafe(ifstream& flux,
								string& nomAttribut,
								string& valeurAttribut) {
	string line;		// ligne courante lue dans le flux
	bool trouve = false;	// indique si l'on a trouver l'attribut recherche
	long pos;			// sauvegarde de la position du curseur a l'appel de la fonction
	
	// on stocke la position courante du flux
	pos = flux.tellg();
	// on recherche l'attribut
	while (!flux.fail() && !flux.eof() && !trouve) {
		getline(flux, line);
		if (line.find(nomAttribut) == 0) {
			valeurAttribut = line.substr(line.find("=") + 1, line.length());
			trouve = true;
		}
	}
	// on previent l'utilisateur dans le cas ou on a pas trouver l'attribut
	if (!trouve) {
		cerr << "The attribute " << nomAttribut << " can not be found!" << endl;
	}
	// on replace le curseur de flux a sa position initiale
	flux.seekg(pos);
	return trouve;
}
