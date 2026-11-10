/*
 * DirectionSet.hpp
 *
 *  Created on: 25 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_DIRECTIONSET_HPP_
#define INCLUDE_INPUTS_STORAGE_DIRECTIONSET_HPP_

#include "containers/DirectionCartesienne.hpp"
#include <vector>

namespace phsdata {
class DirectionSet {
public:
	std::vector<DirectionCartesienne> _directions;
	std::string _name;
	unsigned int _index;

};
}

#endif /* INCLUDE_INPUTS_STORAGE_DIRECTIONSET_HPP_ */
