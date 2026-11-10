/*
 * Mixed.hpp
 *
 *  Created on: 29 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MIXED_MIXED_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MIXED_MIXED_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"

namespace phsdata {
class Property {
public:
	int indexOpticalLink;
	int percentFirst;  // Only for FirstProperty (optional)
	int typeOpticalLink;

	void loadFromParameters(const std::string& prefix) {
		indexOpticalLink = DartSAParameters::get<int>(prefix + ".indexOpticalLink");
		typeOpticalLink = DartSAParameters::get<int>(prefix + ".typeOpticalLink");

		// percentFirst is only for FirstProperty
		if (DartSAParameters::exists(prefix + ".percentFirst")) {
			percentFirst = DartSAParameters::get<int>(prefix + ".percentFirst");
		}
	}
};

class Mixed : public SurfaceFunction {
public:
	Property firstProperties;
	Property secondProperties;

    virtual Type getType() override {
    	return SurfaceFunction::Type::Mixed;
    }

	void loadFromParameters(const std::string& prefix) override {
		functionName = DartSAParameters::get<std::string>(prefix + ".functionName");
		indexFunction = DartSAParameters::get<int>(prefix + ".indexFunction");
		useMultiplicativeFactorForLUT = false;  // MixedMulti doesn't use this (default to false)

		firstProperties.loadFromParameters(prefix + ".FirstProperty");
		secondProperties.loadFromParameters(prefix + ".SecondProperty");
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MIXED_MIXED_HPP_ */
