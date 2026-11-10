/*
 * typeOption.cpp
 *
 *  Created on: 10 nov. 2009
 *      Author: goulierv
 */

#include "TypeOption.hpp"

using namespace std;

// constructeur
TypeOption::TypeOption() {}

// constructeur
TypeOption::TypeOption(	const string & nom,
						bool obligatoire,
						const string & description,
						int nb,
						const vector<string> & argumentsNames) :
		_nomType(nom), _estObligatoire(obligatoire), _description(description), _nbArg(nb), _argNames(argumentsNames) {}
