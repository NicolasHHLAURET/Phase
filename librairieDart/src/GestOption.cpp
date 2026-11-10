/*
 * options.cpp
 *
 *  Created on: 9 nov. 2009
 *      Author: goulierv
 */

#include "../include/GestOption.hpp"

using namespace std;

// constructeur de la classe
// c'est ici que l'on précise les options possibles
GestOption::GestOption() :
		nbOptionsObligatoires(0), nbOpt(0) {
	//logger = Logger::getInstance(LOG4CPLUS_TEXT("GestOption"));

//	man << endl << "La syntaxe est la suivante:" << endl <<
//	"-> les options commencent par '-'" << endl <<
//	"-> elles sont suivies de leurs eventuelles valeurs separees par des espaces" << endl <<
//	"-> les valeurs ne doivent pas commencer par '-'" << endl << endl <<
//	"Voici les differentes options possibles:" << endl;
}
;

// fonction qui enregistre un type d'option
GestOption& GestOption::ajouterType(const string & nom,
									bool obligatoire,
									const string & description,
									int nb,
									const vector<string> & argumentsName) {
	options.insert(pair<string, TypeOption>(nom, TypeOption(nom, obligatoire, description, nb, argumentsName)));

	if (obligatoire) {
		nbOptionsObligatoires++;
	}

	//	manSynopsisLine << " ";
	//	if (!t.estObligatoire) {
	//		manSynopsisLine << "[";
	//	}
	//	manSynopsisLine << "-" << nom;
	//	if (nb == INCONNU) {
	//		manSynopsisLine << " ...";
	//	} else {
	//		manSynopsisLine << " <" << nom << "Arg0>";
	//		for (int i = 1; i < nb; ++i) {
	//			manSynopsisLine << ", <" << nom << "Arg" << i << ">";
	//		}
	//	}
	//	if (!t.estObligatoire) {
	//		manSynopsisLine << "]";
	//	}
	//	man << "l'option " << t.nomType << " prend " << s << "argument(s)" << endl;
	return *this;
}

// fonction qui parse chaque option
bool GestOption::parser(int argc,
						char* argv[]) {
	programName = argv[0];
	// on parse chaque option
	int i = 1;
	while (i < argc) {
		// si une option est invalide on arrete
		if (!parseOption(i, argc, argv)) {
			return false;
		}
	}
	// on regarde qu'on a bien toutes les options obligatoires
	if (nbOpt < nbOptionsObligatoires) {
		ostringstream s;
		s << "All mandatories options have not been provided." << endl;
		erreur << s.str();
		//LOG4CPLUS_ERROR(logger, s.str());
		return false;
	}
	return true;
}
;

// fonction qui indique le nombre de valeurs d'une option
int GestOption::getNbOpt(string id) throw (IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("option " + id + " inexistante");
	}
	return valeurs[id].getNbOpt(id);
}

// fonction qui renvoit la valeur (string) de l'option demandee
string GestOption::getValeurS(	string id,
								int num) throw (IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("option " + id + " inexistante");
	}
	string val = "";
	try {
		val = valeurs[id].getValeurS(num);
	} catch (IndexErrorException except) {
		//LOG4CPLUS_ERROR(logger, "indice trop grand");
		throw IndexErrorException("indice trop grand");
	}
	return val;
}
;

// fonction qui renvoit la valeur (int) de l'option demandee
int GestOption::getValeurI(	string id,
							int num) throw (BadFormatException,
									IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("option " + id + " inexistante");
	}
	int val = 0;
	try {
		val = valeurs[id].getValeurI(num);
	} catch (BadFormatException except) {
		//LOG4CPLUS_ERROR(logger, "echec conversion vers int");
		throw BadFormatException("echec conversion vers int");
	} catch (IndexErrorException except2) {
		//LOG4CPLUS_ERROR(logger, "indice trop grand");
		throw IndexErrorException("indice trop grand");
	}
	return val;
}
;

// fonction qui renvoit la valeur (float) de l'option demandee
float GestOption::getValeurF(	string id,
								int num) throw (BadFormatException,
										IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("option " + id + " inexistante");
	}
	float val = 0;
	try {
		val = valeurs[id].getValeurF(num);
	} catch (BadFormatException except) {
		//LOG4CPLUS_ERROR(logger, "echec conversion vers float");
		throw BadFormatException("echec conversion vers float");
	} catch (IndexErrorException except2) {
		//LOG4CPLUS_ERROR(logger, "indice trop grand");
		throw IndexErrorException("indice trop grand");
	}
	return val;
}
;

// fonction qui renvoit la valeur (double) de l'option demandee
double GestOption::getValeurD(	string id,
								int num) throw (BadFormatException,
										IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("option " + id + " inexistante");
	}
	double val = 0;
	try {
		val = valeurs[id].getValeurD(num);
	} catch (BadFormatException except) {
		//LOG4CPLUS_ERROR(logger, "echec conversion vers double");
		throw BadFormatException("echec conversion vers double");
	} catch (IndexErrorException except2) {
		//LOG4CPLUS_ERROR(logger, "indice trop grand");
		throw IndexErrorException("indice trop grand");
	}
	return val;
}
;

// fonction qui renvoit la valeur (bool) de l'option demandee
bool GestOption::getValeurB(string id,
							int num) throw (BadFormatException,
									IndexErrorException) {
	if (!checkPresence(id)) {
		//LOG4CPLUS_ERROR(logger, "option "+id+" inexistante");
		throw IndexErrorException("Expected option [" + id + "] has not been provided.");
	}
	bool val = false;
	try {
		val = valeurs[id].getValeurB(num);
	} catch (BadFormatException except) {
		//LOG4CPLUS_ERROR(logger, "echec conversion vers bool");
		throw BadFormatException("Can not convert <" + valeurs[id].getValeurS(num) + "> to a boolean value.");
	} catch (IndexErrorException except2) {
		//LOG4CPLUS_ERROR(logger, "indice trop grand");
		throw IndexErrorException("Expected argument of option [" + id + "] not found.");
	}
	return val;
}

// fonction qui indique si une option est utilisee
bool GestOption::estPresent(string id) {
	// on regarde si l'option a ete enregistree
	map<string, Option>::iterator it = valeurs.find(id);
	if (it == valeurs.end()) {
		return false;
	}
	return true;
}

string GestOption::getMan() {
	return getSynopsis() + "\n" + getDescription();
}
string GestOption::getSynopsis() {
	ostringstream os;
	os << "Synopsys:" << endl;
	os << "      " << programName;

	map<string, TypeOption>::iterator it = options.begin(), itEnd = options.end();

	while (it != itEnd) {
		const TypeOption & currentOption = it->second;
		os << " ";
		if (!currentOption._estObligatoire) {
			os << "[";
		}
		os << "-" << currentOption._nomType;
		if (currentOption._nbArg == INCONNU) {
			os << " ...";
		} else {
			if (!currentOption._argNames.empty() && currentOption._argNames[0].size() > 0) {
				os << " <" << currentOption._argNames[0] << ">";
				for (unsigned int i = 1; (int) i < currentOption._nbArg; ++i) {
					os << " <";
					if (currentOption._argNames.size() > i || currentOption._argNames[i].size() == 0) {
						os << currentOption._nomType << "Arg" << i;
					} else {
						os << currentOption._argNames[i];
					}
					os << ">";
				}
			}
		}
		if (!currentOption._estObligatoire) {
			os << "]";
		}
		++it;
	}
	os << endl;

	return os.str();
}

string GestOption::getDescription() {
	ostringstream os;
	os << "Description:" << endl;
	os << "      " << programDescription << endl;

	map<string, TypeOption>::iterator it = options.begin(), itEnd = options.end();

	while (it != itEnd) {
		const TypeOption & currentOption = it->second;
		os << endl;
		os << "      ";
		if (!currentOption._estObligatoire) {
			os << "[";
		}
		os << "-" << currentOption._nomType;
		if (currentOption._nbArg == INCONNU) {
			os << " ...";
		} else {
			if (!currentOption._argNames.empty() && currentOption._argNames[0].size() > 0) {
				os << " <" << currentOption._argNames[0] << ">";
				for (unsigned int i = 1; (int) i < currentOption._nbArg; ++i) {
					os << " <";
					if (currentOption._argNames.size() > i || currentOption._argNames[i].size() == 0) {
						os << currentOption._nomType << "Arg" << i;
					} else {
						os << currentOption._argNames[i];
					}
					os << ">";
				}
			}
		}
		if (!currentOption._estObligatoire) {
			os << "]";
		}
		os << endl;
		os << "         " << currentOption._description << endl;
		++it;
	}
	os << endl;

	return os.str();
}

string GestOption::getStatus() {
	return status.str();
}

string GestOption::getErreur() {
	return erreur.str();
}

// fonction qui vérifie la présence d'une option
bool GestOption::checkPresence(string id) {
	if (!estPresent(id)) {
		ostringstream s;
		s << "Expected option [" + id + "] has not been provided." << endl;
		erreur << s.str();
		//LOG4CPLUS_ERROR(logger, s.str());
		return false;
	}
	return true;
}
;

// fonction qui determine si une chaine est une option
bool GestOption::isOption(string& tab) {
	if (tab[0] == '-' && (tab[1] < '0' || tab[1] > '9')) {
		return true;
	}
	return false;
}

// fonction qui analyse 1 option
bool GestOption::parseOption(	int& indice,
								int argc,
								char* argv[]) {
	string sopt = argv[indice++];
	// si ce n'est pas une option, il y a erreur
	if (!isOption(sopt)) {
		ostringstream s;
		s << "Token [" << sopt << "] isn't a recognized option." << endl;
		erreur << s.str();
		//LOG4CPLUS_ERROR(logger, s.str());
		return false;
	}
	// sinon, 1'option est detectee
	else {
		status << endl << "option " << (string) &sopt[1] << " detecte" << endl;
		// on regarde si l'option existe
		map<string, TypeOption>::iterator it = options.find(&sopt[1]);
		if (it == options.end()) {
			ostringstream s;
			s << "option " << (string) &sopt[1] << " inconnue" << endl;
			erreur << s.str();
			//LOG4CPLUS_ERROR(logger, s.str());
			return false;
		}
		// si elle existe on l'ajoute
		TypeOption type = options[&sopt[1]];
		valeurs.insert(pair<string, Option>(type._nomType, Option()));
		if (type._estObligatoire) {
			nbOpt++;
		}
		// on enregistre les valeurs de l'option
		for (int i = 0; (i < type._nbArg) || (type._nbArg == INCONNU); i++) {
			// on regarde si l'indice n'est pas trop grand
			if (indice >= argc) {
				// si le nombre d'arguments était inconnu, c'est OK
				if ((type._nbArg == INCONNU)) {
					return true;
				}
				ostringstream s;
				s << "vous n'avez pas saisi tous les arguments" << endl;
				erreur << s.str();
				//LOG4CPLUS_ERROR(logger, s.str());
				return false;
			}
			// sinon on enregistre
			string val = argv[indice];
			// on regarde si c'est bien une valeur
			if (isOption(val)) {
				// si le nombre d'arguments était inconnu, c'est OK
				if ((type._nbArg == INCONNU)) {
					return true;
				}
				// sinon, il manque des arguments
				ostringstream s;
				s << "vous n'avez pas saisi tous les arguments de l'option: " << type._nomType << endl;
				erreur << s.str();
				//LOG4CPLUS_ERROR(logger, s.str());
				return false;
			}
			indice++;
			stringstream s;
			s << "valeur " << (string) Conversions::convertitEnString(i) << " de l'option: " << val << endl;
			status << s.str();
			//LOG4CPLUS_INFO(logger, s.str());
			if (!valeurs[type._nomType].ajouterValeur(type._nbArg, val)) {
				ostringstream s;
				s << "vous avez saisi trop d'arguments pour l'option " << (string) &sopt[1] << endl;
				erreur << s.str();
				//LOG4CPLUS_ERROR(logger, s.str());
				return false;
			}
		}
	}
	return true;
}
;
