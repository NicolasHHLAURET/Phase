#include "../../include/libIlwis/WriteIlwis.hpp"

using namespace std;

void WriteIlwis::ecrireFichierGRdiese(	const string & repertoireImage,
										const string & nomImage,
										vector<vector<vector<double> > > coordonneesGeoreferentiel,
										const unsigned int nombreLignes,
										const unsigned int nombreColonnes) {
	// TODO
	int O_S = LINUX_OS;
	/**
	 * L'entete du fichier ILWIS doit etre compose de 128 octets.
	 * Les 16 premiers contiennent la chaine "ILWIS 2.00 Table".
	 * Les 4 suivants sont les valeurs en octet : 13, 10, 26 et 0.
	 * Les 108 octets suivants sont remplis par des 0 (en octet)
	 */
	string entete = "ILWIS 2.00 Table";
	unsigned char zero = 0;
	char c1 = 13, c2 = 10, c3 = 26, c4 = 0;
	
	string grDiezFilePath = repertoireImage + nomImage + ".gr#";
	ofstream grDiezBinaryStream(grDiezFilePath.c_str(), ios::out | ios::binary);
	
	if (grDiezBinaryStream) {
		grDiezBinaryStream.write((char *) &(entete[0]), entete.size());
		grDiezBinaryStream.write((char *) &c1, 1);
		grDiezBinaryStream.write((char *) &c2, 1);
		grDiezBinaryStream.write((char *) &c3, 1);
		grDiezBinaryStream.write((char *) &c4, 1);
		for (unsigned int i = 0; i < 108; ++i) {
			grDiezBinaryStream.write((char *) &zero, 1);
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
		
//		int i_actif = 2;
		
		actif.intValue = 2;
		if (O_S == UNIX_OS) {
			actif.inverserEndian();
		}
		
		for (unsigned int i = 0; i < 4; ++i) {
			switch (i) {
				case 0: {
					//(X min,Y max ) --> (ligne min,  colonne min) :
					indiceLigne.intValue = 1;
					indiceColonne.intValue = 1;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[0][0][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[0][1][0];
					break;
				}
				case 1: {
					//(X max,Y max ) --> (ligne min, colonne max) :
					indiceLigne.intValue = 1;
					indiceColonne.intValue = nombreColonnes;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[3][0][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[3][1][0];
					break;
				}
				case 2: {
					//(X min,Y min ) --> (ligne max, colonne min) :
					indiceLigne.intValue = nombreLignes;
					indiceColonne.intValue = 1;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[1][0][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[1][1][0];
					break;
				}
				case 3:
				default: {
					//(X max,Y min ) --> (ligne max, colonne max) :
					indiceLigne.intValue = nombreLignes;
					indiceColonne.intValue = nombreColonnes;
					dimensionLigne.doubleValue = coordonneesGeoreferentiel[2][0][0];
					dimensionColonne.doubleValue = coordonneesGeoreferentiel[2][1][0];
					break;
				}
			} // Fin switch
			
			if (O_S == UNIX_OS) {
				indiceLigne.inverserEndian();
				indiceColonne.inverserEndian();
				dimensionLigne.inverserEndian();
				dimensionColonne.inverserEndian();
			}
			
			grDiezBinaryStream.write(dimensionLigne.octetArray, sizeof(double));
			grDiezBinaryStream.write(dimensionColonne.octetArray, sizeof(double));
			grDiezBinaryStream.write(indiceLigne.octetArray, sizeof(int));
			grDiezBinaryStream.write(indiceColonne.octetArray, sizeof(int));
			grDiezBinaryStream.write(actif.octetArray, sizeof(int));
		}
		grDiezBinaryStream.close();
	} else {
		cerr << "Unable to open file " << grDiezFilePath << endl;
	}
}

void WriteIlwis::ecrireFichierGRF(	const string & repertoireImage,
									const string & nomImage,
									const unsigned int nombreLignes,
									const unsigned int nombreColonnes) {
	string fileName = repertoireImage + nomImage + ".grf";
	ofstream fichierGRF(fileName.c_str(), ios::out);
	
	if (fichierGRF) {
		fichierGRF << "[Ilwis]" << endl;
		fichierGRF << "Description=" << endl << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=GeoRef"
				<< endl;
		fichierGRF << endl << "[Table]" << endl;
		fichierGRF << "Domain=None" << endl << "Columns=5" << endl << "Records=4" << endl << "Type=TableStore" << endl;
		fichierGRF << endl << "[TableStore]" << endl;
		fichierGRF << "Data=" << nomImage << ".gr#" << endl;
		fichierGRF << "Col0=X" << endl << "Type=TableBinary" << endl << "Col1=Y" << endl << "Col2=Row" << endl << "Col3=Col" << endl << "Col4=Active"
				<< endl;
		
		fichierGRF << endl << "[Col:X]" << endl;
		fichierGRF << "Description=" << endl << "Domain=value.dom" << endl << "Range=-1000000000.000:1000000000.000:0.001" << endl;
		fichierGRF << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=ColumnStore" << endl << "Stored=yes"
				<< endl;
		fichierGRF << "NrDepObjects=0" << endl << "StoreType=Real" << endl << "MinMax=0:50" << endl;
		
		fichierGRF << endl << "[Col:Y]" << endl;
		fichierGRF << "Description=" << endl << "Domain=value.dom" << endl << "Range=-1000000000.000:1000000000.000:0.001" << endl;
		fichierGRF << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=ColumnStore" << endl << "Stored=yes"
				<< endl;
		fichierGRF << "NrDepObjects=0" << endl << "StoreType=Real" << endl << "MinMax=0:50" << endl;
		
		fichierGRF << endl << "[Col:Row]" << endl;
		fichierGRF << "Description=" << endl << "Domain=value.dom" << endl << "Range=-" << nombreLignes << ":" << 2 * nombreLignes << ":offset=0"
				<< endl;
		fichierGRF << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=ColumnStore" << endl << "Stored=yes"
				<< endl;
		fichierGRF << "NrDepObjects=0" << endl << "StoreType=Int" << endl;
		
		fichierGRF << endl << "[Col:Col]" << endl;
		fichierGRF << "Description=" << endl << "Domain=value.dom" << endl << "Range=-" << nombreColonnes << ":" << 2 * nombreColonnes << ":offset=0"
				<< endl;
		fichierGRF << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=ColumnStore" << endl << "Stored=yes"
				<< endl;
		fichierGRF << "NrDepObjects=0" << endl << "StoreType=Int" << endl << "MinMax=1:" << nombreColonnes << endl;
		
		fichierGRF << endl << "[Col:Active]" << endl;
		fichierGRF << "Description=" << endl << "Domain=bool.dom" << endl << "Range=0:1" << endl;
		fichierGRF << "Time=0" << endl << "DataReadOnly=no" << endl << "PropertiesReadOnly=no" << endl << "Type=ColumnStore" << endl << "Stored=yes"
				<< endl;
		fichierGRF << "NrDepObjects=0" << endl << "StoreType=Int" << endl << "MinMax=1:1" << endl;
		
		fichierGRF << endl << "[GeoRef]" << endl;
		fichierGRF << "CoordSystem=unknown.csy" << endl << "lines=" << nombreLignes << endl << "columns=" << nombreColonnes << endl
				<< "Type=GeoRefCTP" << endl;
		
		fichierGRF << endl << "[GeoRefCTP]" << endl;
		fichierGRF << "Transformation=Affine" << endl << "Reference Map=" << nomImage << ".mpr" << endl;
		
		fichierGRF.close();
	} else {
		cerr << "Unable to open file " << fileName << endl;
	}
}

void WriteIlwis::ecrireFichierMPR(	const string & repertoireImage,
									const string & nomImage,
									const unsigned int nombreLignes,
									const unsigned int nombreColonnes,
									const float deltaX,
									const float deltaY,
									const double valMinImage,
									const double valMaxImage,
									bool createGeo) {
	string fileName = repertoireImage + nomImage + ".mpr";
	ofstream fichierMPR(fileName.c_str(), ios::out);
	
	if (fichierMPR) {
		fichierMPR << "[Ilwis]" << endl;
		fichierMPR << "Description=Raster Map " << nomImage << endl;
		fichierMPR << "Time=0" << endl << "Type=BaseMap" << endl;
		fichierMPR << endl << "[BaseMap]" << endl;
		fichierMPR << "CoordSystem=unknown.csy" << endl;
		fichierMPR << "CoordBounds=0 -" << nombreColonnes * deltaY << " " << nombreLignes * deltaX << " 0" << endl;
		fichierMPR << "Domain=value.dom" << endl << "DomainInfo=value.dom;Long;value;0;-9999999.9:9999999.9:0.1:offset=0;" << endl;
		fichierMPR << "Range=-1e+300:1e+300:0.000000:offset=0" << endl;
		fichierMPR << "Type=Map" << endl;
		fichierMPR << "MinMax=" << valMinImage << ":" << valMaxImage << endl;
		fichierMPR << endl << "[Map]" << endl;
		if (createGeo) {
			fichierMPR << "GeoRef=" << nomImage << ".grf" << endl;
		} else {
			fichierMPR << "GeoRef=none" << endl;
		}
		fichierMPR << "Size=" << nombreLignes << " " << nombreColonnes << endl;
		fichierMPR << "Type=MapStore" << endl;
		fichierMPR << endl << "[MapStore]" << endl;
		fichierMPR << "Data=" << nomImage << ".mp#" << endl;
		fichierMPR << "Type=Real" << endl;
		fichierMPR << "Structure=Line" << endl << "StartOffset=0" << endl << "RowLength=" << nombreColonnes << endl;
		fichierMPR.close();
	} else {
		cerr << "Unable to open file " << fileName << endl;
	}
}
