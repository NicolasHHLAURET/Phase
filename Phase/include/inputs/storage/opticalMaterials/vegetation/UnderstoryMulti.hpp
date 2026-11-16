/*
 * UnderstoryMulti.hpp
 *
 *  Created on: 10 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_

#include <string>

#include "DartSAParameters.h"
#include "../../DirectionSetLink.hpp"

namespace phsdata {
class UnderstoryMulti {
public:
	DirectionSetLink directionSetLink;

	void loadFromParameters(const std::string & prefix) {
		if (DartSAParameters::exists(prefix + ".DirectionSetLink")) {
			directionSetLink.loadFromParameters(prefix + ".DirectionSetLink");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_ */
