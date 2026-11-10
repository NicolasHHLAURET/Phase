/*
 * option.hpp
 *
 *  Created on: 10 nov. 2009
 *      Author: goulierv
 */

#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "TypeOption.hpp"
#include "../../librairieDart/include/Conversions.hpp"
#include "IndexErrorException.hpp"
#include "../../libDartEnv/include/DartEnv.h"

class Option {
public:
	Option();
	bool ajouterValeur(	int nbArg,
						std::string val);
	int getNbOpt(std::string id);
	std::string getValeurS(int i) throw (IndexErrorException);
	int getValeurI(int i)	throw (BadFormatException,
							IndexErrorException);
	float getValeurF(int i)	throw (BadFormatException,
							IndexErrorException);
	double getValeurD(int i) throw (BadFormatException,
							IndexErrorException);
	bool getValeurB(int i)	throw (BadFormatException,
							IndexErrorException);

private:
	std::vector<std::string> valeurs;
};

#endif /* OPTION_HPP_ */
