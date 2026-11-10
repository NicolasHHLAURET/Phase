/*
 * SurfaceFunction.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACEFUNCTION_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACEFUNCTION_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "DartSAParameters.h"

namespace phsdata {
class SurfaceFunction {
public:
	virtual ~SurfaceFunction() = default;

	enum class Type {
		Lambertian = 0, Hapke1981, Hapke2012, RPV, DiscreteBSDF, Specular, Mixed
	};

	// Shared attributes (common to all function types)
	std::string functionName = "undefined";
	unsigned int indexFunction = 0;
	boost::optional<std::string> databaseName;  // Not all functions have this
	boost::optional<std::string> modelName;     // Not all functions have this
	bool useMultiplicativeFactorForLUT = false;

	virtual Type getType() = 0;

	// Virtual method to load from DartSAParameters
	virtual void loadFromParameters(const std::string& prefix) {
		functionName = DartSAParameters::get<std::string>(prefix + ".functionName");
		indexFunction = DartSAParameters::get<unsigned int>(prefix + ".indexFunction");

		if (DartSAParameters::exists(prefix + ".databaseName"))
			databaseName = DartSAParameters::get<std::string>(prefix + ".databaseName");
		if (DartSAParameters::exists(prefix + ".modelName"))
			modelName = DartSAParameters::get<std::string>(prefix + ".modelName");

		if (DartSAParameters::exists(prefix + ".useMultiplicativeFactorForLUT"))
			useMultiplicativeFactorForLUT = DartSAParameters::get<bool>(prefix + ".useMultiplicativeFactorForLUT");
	}

	// Virtual method to validate (optional)
	virtual bool validate() const {
		return true;
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACEFUNCTION_HPP_ */
