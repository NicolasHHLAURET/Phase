/*
 * RPV.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPV_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPV_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"
#include "inputs/storage/opticalMaterials/rpv/RPVMultiplicativeFactor.hpp"

namespace phsdata {
class RPVMulti : public SurfaceFunction {
public:
	std::string transmittanceDatabaseName;
	std::string transmittanceModelName;
	double theta_max = 90.;  // RPV-specific attribute
	boost::optional<RPVMultiplicativeFactorList> nodeMultiplicativeFactor;

    virtual Type getType() override {
    	return SurfaceFunction::Type::RPV;
    }

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);
		transmittanceDatabaseName = DartSAParameters::get<std::string>(prefix + ".transmittanceDatabaseName");
		transmittanceModelName = DartSAParameters::get<std::string>(prefix + ".transmittanceModelName");
		theta_max = DartSAParameters::get<double>(prefix + ".theta_max");

		if (DartSAParameters::exists(prefix + ".RPVNodeMultiplicativeFactorForLUT")) {
			nodeMultiplicativeFactor = RPVMultiplicativeFactorList();
			nodeMultiplicativeFactor->loadFromParameters(prefix + ".RPVNodeMultiplicativeFactorForLUT");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPV_HPP_ */
