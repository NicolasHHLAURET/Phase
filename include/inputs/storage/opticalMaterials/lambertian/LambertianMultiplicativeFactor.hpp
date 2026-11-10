/*
 * LambertianNodeMultiplicativeFactorForLUT.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/MultiplicativeFactor.hpp"

namespace phsdata {
class LambertianMultiplicativeFactor: public MultiplicativeFactor {
public:
	double reflectanceFactor = 1.;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		reflectanceFactor = DartSAParameters::get<double>(prefix + ".reflectanceFactor");
	}
};

class LambertianMultiplicativeFactorList: public MultiplicativeFactorList {
public:
	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

		// Load repeated nodes (e.g., LambertianMultiplicativeFactorForLUT[0], [1], ...)
		int count = DartSAParameters::getNbOf(prefix + ".LambertianMultiplicativeFactorForLUT");
		for (int i = 0; i < count; ++i) {
			auto factor = std::make_shared<LambertianMultiplicativeFactor>();
			factor->loadFromParameters(prefix + ".LambertianMultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(factor);
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_ */
