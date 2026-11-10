/*
 * HapkeSpecularMulti.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "Hapke2012MultiplicativeFactor.hpp"
#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"

namespace phsdata {
class Hapke2012: public SurfaceFunction {
public:
	std::string transmittanceDatabaseName;
	std::string transmittanceModelName;
	boost::optional<Hapke2012MultiplicativeFactorList> nodeMultiplicativeFactor;

	virtual Type getType() override {
		return SurfaceFunction::Type::Hapke2012;
	}

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);
		transmittanceDatabaseName = DartSAParameters::get<std::string>(prefix + ".transmittanceDatabaseName");
		transmittanceModelName = DartSAParameters::get<std::string>(prefix + ".transmittanceModelName");

		if (DartSAParameters::exists(prefix + ".Hapke2NodeMultiplicativeFactorForLUT")) {
			nodeMultiplicativeFactor = Hapke2012MultiplicativeFactorList();
			nodeMultiplicativeFactor->loadFromParameters(prefix + ".Hapke2NodeMultiplicativeFactorForLUT");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012_HPP_ */
