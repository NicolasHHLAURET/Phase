/*
 * Specular.hpp
 *
 *  Created on: 29 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULAR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULAR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/specular/SpecularMultiplicativeFactor.hpp"
#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"
#include "inputs/storage/OpticalLink.hpp"

namespace phsdata {
class Specular : public SurfaceFunction {
public:
	int materialCharacter;
	std::string ni;  // Refractive index (e.g., "air")
	std::string nt;  // Refractive index (e.g., "glass")
	double opticalDepth;
	double roughnessFactor;
	boost::optional<OpticalLink> opticalLink;
	boost::optional<SpecularMultiplicativeFactorList> nodeMultiplicativeFactor;

    virtual Type getType() override {
    	return SurfaceFunction::Type::Specular;
    }

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);
		materialCharacter = DartSAParameters::get<int>(prefix + ".materialCharacter");
		ni = DartSAParameters::get<std::string>(prefix + ".ni");
		nt = DartSAParameters::get<std::string>(prefix + ".nt");
		opticalDepth = DartSAParameters::get<double>(prefix + ".opticalDepth");
		roughnessFactor = DartSAParameters::get<double>(prefix + ".roughnessFactor");

		// Load repeated OpticalLink nodes
		if (DartSAParameters::exists(prefix + ".OpticalLink")) {
			opticalLink = OpticalLink();
			opticalLink->loadFromParameters(prefix + ".OpticalLink");
		}

		if (DartSAParameters::exists(prefix + ".SpecularNodeMultiplicativeFactorForLUT")) {
			nodeMultiplicativeFactor = SpecularMultiplicativeFactorList();
			nodeMultiplicativeFactor->loadFromParameters(prefix + ".SpecularNodeMultiplicativeFactorForLUT");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULAR_HPP_ */
