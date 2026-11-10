/*
 * OpticalLink.hpp
 *
 *  Created on: 29 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALLINK_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALLINK_HPP_

#include <string>
#include "DartSAParameters.h"

namespace phsdata {
class OpticalLink {
public:
    int indexOpticalLink;
    int typeOpticalLink;

    void loadFromParameters(const std::string& prefix) {
        indexOpticalLink = DartSAParameters::get<int>(prefix + ".indexOpticalLink");
        typeOpticalLink = DartSAParameters::get<int>(prefix + ".typeOpticalLink");
    }
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALLINK_HPP_ */
