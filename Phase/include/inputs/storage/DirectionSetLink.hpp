/*
 * DirectionSetLink.hpp
 *
 *  Created on: 29 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_DIRECTIONSETLINK_HPP_
#define INCLUDE_INPUTS_STORAGE_DIRECTIONSETLINK_HPP_

#include <string>
#include "DartSAParameters.h"

namespace phsdata {
class DirectionSetLink {
public:
    int indexDirectionSetLink = 0;

    void loadFromParameters(const std::string & prefix) {
        indexDirectionSetLink = DartSAParameters::get<int>(prefix + ".indexDirectionSetLink");
    }
};
}

#endif /* INCLUDE_INPUTS_STORAGE_DIRECTIONSETLINK_HPP_ */
