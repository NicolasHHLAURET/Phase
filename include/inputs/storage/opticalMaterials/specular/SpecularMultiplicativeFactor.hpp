/*
 * SpecularMultiplicativeFactor.hpp
 *
 *  Created on: 29 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULARMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULARMULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/MultiplicativeFactor.hpp"

namespace phsdata {
class SpecularMultiplicativeFactor : public MultiplicativeFactor {
public:
	double niFactor;
	double ntFactor;
	double opticalDepthFactor;
	double reflectionIntensityFactor;
	double transmissionIntensityFactor;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		niFactor = DartSAParameters::get<double>(prefix + ".niFactor");
		ntFactor = DartSAParameters::get<double>(prefix + ".ntFactor");
		opticalDepthFactor = DartSAParameters::get<double>(prefix + ".opticalDepthFactor");
		reflectionIntensityFactor = DartSAParameters::get<double>(prefix + ".reflectionIntensityFactor");
		transmissionIntensityFactor = DartSAParameters::get<double>(prefix + ".transmissionIntensityFactor");
	}
};

class SpecularMultiplicativeFactorList : public MultiplicativeFactorList {
public:
    void loadFromParameters(const std::string& prefix) {
    	MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

        int count = DartSAParameters::getNbOf(prefix + ".SpecularMultiplicativeFactorForLUT");
        for (int i = 0; i < count; ++i) {
        	SpecularMultiplicativeFactor * factor = new SpecularMultiplicativeFactor();
            factor->loadFromParameters(prefix + ".SpecularMultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(shared_ptr<MultiplicativeFactor>(factor));
        }
    }
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SPECULAR_SPECULARMULTIPLICATIVEFACTOR_HPP_ */
