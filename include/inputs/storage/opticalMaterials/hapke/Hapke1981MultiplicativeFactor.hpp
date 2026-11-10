/*
 * HapkeMultiplicativeFactor.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981MULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981MULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/MultiplicativeFactor.hpp"

namespace phsdata {
class Hapke1981MultiplicativeFactor: public MultiplicativeFactor {
public:
	double hapkeParameterC1;
	double hapkeParameterC2;
	double hapkeParameterC3;
	double hapkeParameterC4;
	double hapkeParameterH1;
	double hapkeParameterH2;
	double hapkeParameterW;

	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactor::loadFromParameters(prefix);  // Load shared fields
		hapkeParameterC1 = DartSAParameters::get<double>(prefix + ".hapkeParameterC1");
		hapkeParameterC2 = DartSAParameters::get<double>(prefix + ".hapkeParameterC2");
		hapkeParameterC3 = DartSAParameters::get<double>(prefix + ".hapkeParameterC3");
		hapkeParameterC4 = DartSAParameters::get<double>(prefix + ".hapkeParameterC4");
		hapkeParameterH1 = DartSAParameters::get<double>(prefix + ".hapkeParameterH1");
		hapkeParameterH2 = DartSAParameters::get<double>(prefix + ".hapkeParameterH2");
		hapkeParameterW = DartSAParameters::get<double>(prefix + ".hapkeParameterW");
	}
};

class Hapke1981MultiplicativeFactorList: public MultiplicativeFactorList {
public:
	void loadFromParameters(const std::string& prefix) override {
		MultiplicativeFactorList::loadFromParameters(prefix);  // Load shared fields

		int count = DartSAParameters::getNbOf(prefix + ".HapkeMultiplicativeFactorForLUT");
		for (int i = 0; i < count; ++i) {
			auto factor = std::make_shared<Hapke1981MultiplicativeFactor>();
			factor->loadFromParameters(prefix + ".HapkeMultiplicativeFactorForLUT[" + std::to_string(i) + "]");
			factors.push_back(factor);
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981MULTIPLICATIVEFACTOR_HPP_ */
