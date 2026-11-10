/*! Time-stamp: <@(#) Lib_util.c   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : Lib_util.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : GPR                               Date: 07/03/2005
 *
 *  Description : Utilitaire permettant de faire divers controle et de quitter proprement.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.1 12/09/2008  Pierre Chalier : ajout des fonctions execute et printExecuteError
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.1 07/07/2005  VBT : Suppression de la fonction "move_file"
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0   07/03/2005  GPR : First Revision
 *
 *********************************************************************
 * CVS $Log : Lib_util.c,v$
 */

#include "../include/Lib_util.hpp"

#define MAX_LINE_LEN 1000
#define SIZE_OF_FLOAT 4
#define SIZE_OF_DOUBLE 8 

using namespace std;

// ------------------UNIONS-----------------------------------------------------
// -----------------------------------------------------------------------------
union myfloat {
	char pos[SIZE_OF_FLOAT];
	float f;
};

union mydouble {
	char pos[SIZE_OF_DOUBLE];
	double f;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float max(	float a,
			float b) {
	return (a > b) ? a : b;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
/*float pctostation (float num)
 {
 union myfloat e;
 union myfloat o;
 int i;
 
 o.f = num;
 for (i = 0; i < SIZE_OF_FLOAT; i++)
 e.pos[i] = o.pos[SIZE_OF_FLOAT - 1 - i];
 
 return e.f;
 }

 // -----------------------------------------------------------------------------
 // -----------------------------------------------------------------------------
 double pctostationdouble (double num)
 {
 union mydouble e;
 union mydouble o;
 int i;

 o.f = num;
 for (i = 0; i < SIZE_OF_DOUBLE; i++)
 e.pos[i] = o.pos[SIZE_OF_DOUBLE - 1 - i];
 
 return e.f;
 }*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string getRealPath(string filePath) {
	/**
	 * ANCIEN CODE :
	 * //FILE *in ;
	 * //char *buffer = (char*)"";
	 * if ((in = fopen (filePath.c_str(), "r")) != NULL)
	 {
	 fclose (in) ;

	 size_t size = filePath.size() + 1 ;
	 buffer = new char[size] ;
	 
	 strncpy(buffer, filePath.c_str(), size) ;
	 }

	 return buffer;

	 NB : si on arrive a ouvrir le flux de fichier alors on renvoie l'url
	 du fichier a utiliser sinon on renvoie une chaine vide
	 */
	ifstream in;
	string buffer = "";
	in.open(filePath.c_str());
	
	if (in) {
		in.close();
		return (filePath);
	} else {
		return (buffer);
	}
	
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
string testFilePath(string file,
					string currentSimulationInputPath) {
	string filePath = file;
	string realPath = getRealPath(filePath);
	
	//Si l'url testee est vide alors on concatene l'url des repertoire avec le nom de fichier
	if (realPath == "") {
		filePath = currentSimulationInputPath + file;
	}
	return filePath;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void createOutputDirectory(string currentPath) {
	/*
	 * Pierre Chalier - 17/07/08
	 * Remplacement des appels systemes par appels a libDartBase/Lib_fileManager
	 * 
	 * old : string directoryPath = "mkdir -p \"" + currentPath + "\"" ;
	 *       system (directoryPath.c_str()) ;
	 */
	createDirectory(currentPath);
}
// -----------------------------------------------------------------------------

void Open_file(	FILE **fp,
				string filePath,
				const char mode[]) {
	if ((*fp = fopen(filePath.c_str(), mode)) == NULL) {
		cout << "\n\n Ouverture impossible sur le fichier '" << filePath << "' !!! \n\n\r";
		usage(__FILE__, __LINE__, "Exiting...");
	} else
		rewind(*fp);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void lit_remarque(FILE *in_l) {
	int c = getc(in_l);	//clf
	int i;
	
	for (i = 0; i < 2; i++, c = getc(in_l))
		while (c != '*' && !feof(in_l) && !ferror(in_l))
			c = getc(in_l);
}

void echappeLectureCommentaireFichier(	istream &fluxEntree,
										int nbCaractereAEvite,
										int caractereRecherche) {
	int caractereCourant, caractereParcours = '*', limiteSuperieure = 2;
	
	//Si le nombre de pattern est a 0, on met celui par defaut = 2
	if (nbCaractereAEvite != 0) {
		limiteSuperieure = nbCaractereAEvite;
	}
	
	//Si le caractere passe en parametre n'est pas vide : on enregistre le caractere sinon on garde *
	if (caractereRecherche != 0) {
		caractereParcours = caractereRecherche;
	}
	
	for (int i = 0; i < limiteSuperieure; i++) {
		caractereCourant = fluxEntree.get();
		while (caractereCourant != caractereParcours && !fluxEntree.fail() && !fluxEntree.eof()) {
			caractereCourant = fluxEntree.get();
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Copy_File(	FILE *dest,
				FILE *src) {
	
	char buf[MAX_LINE_LEN + 1];
	
	while (fgets(buf, MAX_LINE_LEN + 1, src)) {
		if (fputs(buf, dest) == EOF) // error writing data
		{
			fprintf(stderr, "Error writing to target file: ");
			exit(1);
		}
	}
	if (!feof(src)) // fgets failed _not_ due to encountering EOF 
		{
		fprintf(stderr, "Error reading from source file: ");
		exit(1);
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void usage(	const char *fichier,
			int ligne,
			const char *message) {
	fprintf(stderr, "\n\n%s:%d condition d'arret:  %s  \n\n", fichier, ligne, message);
	exit(EXIT_FAILURE);
}
// -----------------------------------------------------------------------------
/**
 * Vincent Sac
 * TODO : methode a supprimer une fois la lecture des fichiers faite en C sera
 * remplacee par une lecture faite en C++ avec les flux fstream.
 * Il exsite une methode generique equivalente : echappeLectureCommentaireFichier
 */
void read_n_char(	FILE *in_l,
					int n,
					char ch) {
	int i;
	int c;
	
	for (i = 0; i < n; i++) {
		c = fgetc(in_l);
		while (c != (int) ch && !feof(in_l) && !ferror(in_l)) {
			c = fgetc(in_l);
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
/*char read_comments_with_char (FILE *fp,
 char c)
 {
 int i = (int)c;
 while ((i !='*') && !feof(fp) && !ferror(fp)) 
 i = fgetc(fp);	//clf
 
 if (feof(fp) || ferror(fp))
 return EOF ;
 
 while (( (i = fgetc(fp)) !='*') && !feof(fp) && !ferror(fp))
 ;
 
 return (char)i ;  
 }*/

float lireDansLigneFlux(ifstream & inData,
						int nbCar) {
	string tampon;
	int k;
	float valeur = 0.0;
	
	//cerr << "NbCar : " << nbCar << " debut de la boucle" << endl;
	
	//cout << "[1] Resultat du tampon : ";
	for (k = 1; k < nbCar && !inData.fail() && !inData.eof(); k++) {
		if (!(inData >> tampon)) {
			inData.clear();
			//cerr << "Ok nbCar = [" << nbCar << "] k = "<< k << endl;
		}
		//cout << tampon << " ";
	}
	inData.clear();
	//cerr << endl;
	
	//cerr << "Fin de boucle " << endl;
	
	/*
	 pas de caracteres d'espacement ni de guillemets au niveau des lignes contenant
	 des parametres a lire!
	 */
	if (!(inData >> valeur)) {
		//cerr << "lireDansFlux : valeur non trouvee => resultat = " << valeur << endl;
	}
	//cout << "Valeur float : " << valeur << endl;
	
	return valeur;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Affiche un message d'erreur pour la fonction execute et quitte le programme 
// -----------------------------------------------------------------------------
int printExecuteError(char * p_cmd) {
	char * errorMessage = (char*) "\nStop condition : Error in function Lib_util/execute\n";
	char * completeErrorMessage = (char *) malloc((1 + strlen(errorMessage) + strlen(p_cmd)) * sizeof(char));
	
	strcpy(completeErrorMessage, errorMessage);
	strcat(completeErrorMessage, p_cmd);
	perror(completeErrorMessage);
	exit(EXIT_FAILURE);
}

// -----------------------------------------------------------------------------
// Execute une commande 
// -----------------------------------------------------------------------------
void execute(char ** p_cmd) {
	// Modif JCR pour tester compilation sous windows / mingw
//	pid_t pid;
//	int resultExecute;
//
//	switch (pid = fork())
//	{
//	case -1:
//		printExecuteError(p_cmd[0]);
//
//	case 0:
//		execv(p_cmd[0],p_cmd);
//		printExecuteError(p_cmd[0]);
//
//	default:
//		waitpid(pid, &resultExecute, NULL);
//	}
}
// -----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

string convertiePourLectureImageBandeLarge(const double &d) {
	ostringstream osAngleMorceauNomImage;
	ostringstream osMorceauNomImage;
	//on s'assure que l'option fixed ne soit pas par defaut
	osAngleMorceauNomImage.setf(ios::fixed);
	osMorceauNomImage.setf(ios::adjustfield);
	osMorceauNomImage.width(5);
	osMorceauNomImage.fill('0');
	osMorceauNomImage.setf(ios::right);
	osAngleMorceauNomImage.precision(1);
	
	osAngleMorceauNomImage << d;
	osMorceauNomImage << osAngleMorceauNomImage.str();
	if (osAngleMorceauNomImage.fail() || osMorceauNomImage.fail()) {
		throw BadFormatException("echec conversion");
	}
	return osMorceauNomImage.str();
}

// Source : http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
/**
 * Découpe un string (&s) selon un caractère (delim)
 * Retourne un vecteur de string contenant la découpe.
 */
vector<string> &split(	const string &s,
						char delim,
						vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
vector<string> split(	const string &s,
						char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}
