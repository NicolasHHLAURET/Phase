/*
 * DiscreteBSDF.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_DISCRETEBSDF_DISCRETEBSDF_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_DISCRETEBSDF_DISCRETEBSDF_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"
#include "inputs/storage/opticalMaterials/lambertian/LambertianMultiplicativeFactor.hpp"
#include "inputs/storage/DirectionSetLink.hpp"

namespace phsdata {
class DiscreteBSDF : public SurfaceFunction {
public:
	bool addPhaseExternToDatabase;
	DirectionSetLink directionSetLink;  // Child node

    virtual Type getType() override {
    	return SurfaceFunction::Type::DiscreteBSDF;
    }

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);
		addPhaseExternToDatabase = DartSAParameters::get<bool>(prefix + ".addPhaseExternToDatabase");
		useMultiplicativeFactorForLUT = false;  // PhaseExtern doesn't use this (default to false)

		if (DartSAParameters::exists(prefix + ".DirectionSetLink")) {
			directionSetLink.loadFromParameters(prefix + ".DirectionSetLink");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_DISCRETEBSDF_DISCRETEBSDF_HPP_ */
