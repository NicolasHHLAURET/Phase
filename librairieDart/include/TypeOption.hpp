/*
 * typeOption.hpp
 *
 *  Created on: 10 nov. 2009
 *      Author: goulierv
 */

#ifndef TYPEOPTION_HPP_
#define TYPEOPTION_HPP_

#include <string>
#include <vector>

class TypeOption {
public:
	TypeOption();
	TypeOption(	const std::string & nom,
				bool obligatoire,
				const std::string & description,
				int nb,
				const std::vector<std::string> & argumentsNames);
	std::string _nomType;
	bool _estObligatoire;
	std::string _description;
	int _nbArg;
	std::vector<std::string> _argNames;
};

#endif /* TYPEOPTION_HPP_ */
