/*
 * UnderstoryMultiplicativeFactor.hpp
 *
 *  Multiplicative factors of one foliar (vegetation) model face: same layout
 *  as the lambertian ones, but with leafTransmittanceFactor instead of
 *  direct/diffuseTransmittanceFactor (see XMLTemplate v6, UnderstoryMultiModel
 *  subtree).
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "../MultiplicativeFactor.hpp"

namespace phsdata {
class UnderstoryMultiplicativeFactor: public MultiplicativeFactor {
public:
	double reflectanceFactor = 1.;
	double leafTransmittanceFactor = 1.;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		reflectanceFactor = DartSAParameters::get<double>(prefix + ".reflectanceFactor");
		leafTransmittanceFactor = DartSAParameters::get<double>(prefix + ".leafTransmittanceFactor");
	}
};

class UnderstoryMultiplicativeFactorList: public MultiplicativeFactorList {
public:
	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

		// Load repeated nodes (UnderstoryMultiplicativeFactorForLUT[0], [1], ...)
		int count = DartSAParameters::getNbOf(prefix + ".UnderstoryMultiplicativeFactorForLUT");
		for (int i = 0; i < count; ++i) {
			auto factor = std::make_shared<UnderstoryMultiplicativeFactor>();
			factor->loadFromParameters(prefix + ".UnderstoryMultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(factor);
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTIPLICATIVEFACTOR_HPP_ */
