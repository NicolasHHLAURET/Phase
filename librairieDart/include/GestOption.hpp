/*
 * options.hpp
 *
 *  Created on: 9 nov. 2009
 *      Author: goulierv
 */

#ifndef GESTOPTION_HPP_
#define GESTOPTION_HPP_

#define INCONNU -1

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>

#include "Option.hpp"
#include "../../librairieDart/include/IndexErrorException.hpp"
#include "../../librairieDart/include/Conversions.hpp"
#include "../../libDartEnv/include/DartEnv.h"
//#include "../../libLog/include/log4cplus/logger.h"
//#include "../../libLog/include/log4cplus/configurator.h"

//using namespace log4cplus;
class GestOption {
public:
	GestOption();
	GestOption& ajouterType(const std::string & nom,
							bool obligatoire,
							const std::string & description,
							int nb,
							const std::vector<std::string> & argumentsName);
	bool parser(int argc,
				char* argv[]);
	int getNbOpt(std::string id);
	std::string getValeurS(	std::string id,
							int num);
	int getValeurI(	std::string id,
							int num);
	float getValeurF(	std::string id,
							int num);
	double getValeurD(	std::string id,
							int num);
	bool getValeurB(std::string id,
							int num);
	bool estPresent(std::string id);
	void setProgramDescription(const std::string & description) {
		programDescription = description;
	}

	std::string getMan();
	std::string getStatus();
	std::string getErreur();
	std::string getSynopsis();
	std::string getDescription();

private:
	int nbOptionsObligatoires;
	int nbOpt;
	std::map<std::string, TypeOption> options;
	std::map<std::string, Option> valeurs;
	std::string programName;
	std::string programDescription;
	std::ostringstream description;
	std::ostringstream status;
	std::ostringstream erreur;
	//Logger logger;
	
	bool checkPresence(std::string id);
	bool isOption(std::string& tab);
	bool parseOption(	int& indice,
						int argc,
						char* argv[]);
};

#endif /* GESTOPTION_HPP_ */
