/*
 * HapkeMultiplicativeFactor.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012MULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012MULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/MultiplicativeFactor.hpp"

namespace phsdata {
class Hapke2012MultiplicativeFactor: public MultiplicativeFactor {
public:
	double hapkeParameterB;
	double hapkeParameterB0;
	double hapkeParameterC;
	double hapkeParameterH;
	double hapkeParameterTh;
	double hapkeParameterW;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		hapkeParameterB = DartSAParameters::get<double>(prefix + ".hapkeParameterB");
		hapkeParameterB0 = DartSAParameters::get<double>(prefix + ".hapkeParameterB0");
		hapkeParameterC = DartSAParameters::get<double>(prefix + ".hapkeParameterC");
		hapkeParameterH = DartSAParameters::get<double>(prefix + ".hapkeParameterH");
		hapkeParameterTh = DartSAParameters::get<double>(prefix + ".hapkeParameterTh");
		hapkeParameterW = DartSAParameters::get<double>(prefix + ".hapkeParameterW");
	}
};

class Hapke2012MultiplicativeFactorList : public MultiplicativeFactorList {
public:
	void loadFromParameters(const std::string& prefix) {
		MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

		int count = DartSAParameters::getNbOf(prefix + ".Hapke2MultiplicativeFactorForLUT");
		for (int i = 0; i < count; ++i) {
			auto factor = std::make_shared<Hapke2012MultiplicativeFactor>();
			factor->loadFromParameters(prefix + ".Hapke2MultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(factor);
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE2012MULTIPLICATIVEFACTOR_HPP_ */
