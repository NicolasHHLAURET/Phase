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
	std::string getValeurS(int i);
	int getValeurI(int i);
	float getValeurF(int i);
	double getValeurD(int i);
	bool getValeurB(int i);

private:
	std::vector<std::string> valeurs;
};

#endif /* OPTION_HPP_ */
