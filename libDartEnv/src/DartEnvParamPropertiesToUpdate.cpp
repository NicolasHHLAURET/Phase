/* Time-stamp: <@(#) DartEnvParamPropertiesToUpdate.cpp  20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  File    : DartEnvParamPropertiesToUpdate.cpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 15/06/2005
 *
 *  Description : Permet la mise a jour de parametre dans les fichiers xml.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.2 20/07/2005  VBT : Correction d'un autre bug sur la mise a jour du coeff_diff.xml
 * V 4.0.1 08/07/2005  VBT : Correction du bug sur la mise a jour du coeff_diff.xml
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 */

#include "../include/DartEnvParamPropertiesToUpdate.h"

using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
map<string, string> DartEnvParamPropertiesToUpdate::_mapParamToUpdate;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
DartEnvParamPropertiesToUpdate::DartEnvParamPropertiesToUpdate() {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
DartEnvParamPropertiesToUpdate::~DartEnvParamPropertiesToUpdate() {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DartEnvParamPropertiesToUpdate::addParamPropertyToUpdate(	string path,
																string value) {
	_mapParamToUpdate[path] = value;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void DartEnvParamPropertiesToUpdate::ApplyUpdate(void) {
	
	string simulationPath = DartEnv::getCurrentSimulationPath();
	
	if (strcmp(simulationPath.c_str(), "") == 0) {
		cout << "\nRemarque : Le fichier coeff_diff.xml ne sera pas mis a jour.\n";
		return;
	}
	
	string pathToUpdateProperty = simulationPath + ".paramPropertyToUpdate";
	FILE *in;
	if (!_mapParamToUpdate.empty() && (in = fopen(pathToUpdateProperty.c_str(), "w")) != NULL) {
		/*
		 * Vincent Sac - 03/04/2009
		 * Mise en place de la gestion des sous-repertoires pour simulations\
		 	 * Probleme rencontre (necessitant cette modification) : le fichier .paramPropertyToUpdate
		 * contient le nom du repertoire de la simulation mais pour gerer la notion de sous-repertoire,
		 * il faut lui ajouter le nom du sous-repertoire afin de ne pas perdre l'arborescence.
		 * 
		 * Ancien code : string simulationName = DartEnv::getCurrentSimulationName() ;
		 * 				pathToUpdateProperty = DartEnv::getLocalHomePath () + "/simulations/" ;
		 *				pathToUpdateProperty += DartEnv::getCurrentSimulationName() + "/.paramPropertyToUpdate" ;
		 */
		//Pattern de recherche pour le repertoire principal des simulations		
		string principal_dir = "simulations/";
		int occurrence_dir = simulationPath.find(principal_dir);
		
		//Si l'occurrence = -1 : le repertoire de simulation n'a pas ete trouve donc
		// on recommance la recherche avec un antislash
		if (occurrence_dir == -1) {
			principal_dir = "simulations\\";
			occurrence_dir = simulationPath.find(principal_dir);
		}
		
		int length_dir = principal_dir.length();
		
		//Le nom de la simulation se fait maintenant de la maniere suivante
		// NOM_SOUS_REPERTOIRE_SIMULATION/NOM_SIMULATION
		string simulationName = simulationPath.substr(occurrence_dir + length_dir);
		int length_path = simulationName.length() - 1;
		simulationName = simulationName.substr(0, length_path);
		
		//string separateur = ""+pathSeparator+"";
		
		//Conversion du separateur pathSeparator en chaene de caracteres
		//On cherche la premiere occurrence du '/'
		string path_separator = pathSeparator;
		string::size_type i = 0;
		i = simulationName.find('/', i);
		//Tant que non fin de chaene atteinte
		while (i != (string::npos)) {
			//Remplacement, s'ils existent, du '/' par un '\' puis passage e l'occurrence suivante
			simulationName.replace(i++, 1, path_separator);
			i = simulationName.find('/', i);
		}
		/*
		 //Affichages de DEBUG	
		 cout << "Indice premiere occurence " << occurrence_dir << endl;
		 cout << "longueur 'simulations\\' : " << length_dir << endl;
		 cout << "longueur 'PATH' : " << length_path << endl;
		 cout << "Arborescence OK ? " << simulationName << endl;
		 cout << "Separateur : " << pathSeparator_2 << endl;
		 cout << "longueur 'SIMULATION NAME' : " << simulationName.length() << endl;*/

		//Enregistrement dans le fichier .paramPropertyToUpdate
		fprintf(in, "%s\n", simulationName.c_str());
		map<string, string>::const_iterator iter = _mapParamToUpdate.begin();
		while (iter != _mapParamToUpdate.end()) {
			fprintf(in, "%s ", (iter->first).c_str());
			fprintf(in, "%s\n", (iter->second).c_str());
			++iter;
		}
		fclose(in);
		
		/*
		 * Pierre Chalier - 10/09/2008
		 * Remplacement des appels systèmes par la fonction execute
		 * old : pathToUpdateProperty = "\"" + DartEnv::getLocalHomePath () + "/simulations/" ;
		 *       pathToUpdateProperty += DartEnv::getCurrentSimulationName() + "/.paramPropertyToUpdate\"" ;
		 *       string executeUpdate = DartEnv::getHomePath () + "/bin/DARTSimulationPropertiesValues.bat " + pathToUpdateProperty ;
		 *       system (executeUpdate.c_str()) ;
		 */
		pathToUpdateProperty = DartEnv::getCurrentSimulationPath() + ".paramPropertyToUpdate";
		
		/*cout << "UPDATE PARAM property : " << pathToUpdateProperty << endl;*/

		string executeUpdate = DartEnv::getHomePath() + "bin/DARTSimulationPropertiesValues.bat";
		
		char * cmd[3] = { (char*) executeUpdate.c_str(), (char*) pathToUpdateProperty.c_str(), NULL };
		
		execute(cmd);
		
		_mapParamToUpdate.clear();
	}
}
// -----------------------------------------------------------------------------
