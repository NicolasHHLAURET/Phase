/*
 * RPVMultiplicativeFactor.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPVMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPVMULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/MultiplicativeFactor.hpp"

namespace phsdata {
class RPVMultiplicativeFactor : public MultiplicativeFactor {
public:
	double g_RPVMultiplicativeFactor;
	double h_RPVMultiplicativeFactor;
	double k_RPVMultiplicativeFactor;
	double rhoZero_RPVMultiplicativeFactor;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		g_RPVMultiplicativeFactor = DartSAParameters::get<double>(prefix + ".g_RPVMultiplicativeFactor");
		h_RPVMultiplicativeFactor = DartSAParameters::get<double>(prefix + ".h_RPVMultiplicativeFactor");
		k_RPVMultiplicativeFactor = DartSAParameters::get<double>(prefix + ".k_RPVMultiplicativeFactor");
		rhoZero_RPVMultiplicativeFactor = DartSAParameters::get<double>(prefix + ".rhoZero_RPVMultiplicativeFactor");
	}
};

class RPVMultiplicativeFactorList : public MultiplicativeFactorList {
public:
	void loadFromParameters(const std::string& prefix) {
    	MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

		int count = DartSAParameters::getNbOf(prefix + ".RPVMultiplicativeFactorForLUT");
		for (int i = 0; i < count; ++i) {
			auto factor = std::make_shared<RPVMultiplicativeFactor>();
			factor->loadFromParameters(prefix + ".RPVMultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(factor);
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_RPV_RPVMULTIPLICATIVEFACTOR_HPP_ */
