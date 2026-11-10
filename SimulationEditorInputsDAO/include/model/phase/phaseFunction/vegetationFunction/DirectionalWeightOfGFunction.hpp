/*
 * DirectionalWeightOfGFunction.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_DIRECTIONALWEIGHTOFGFUNCTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_DIRECTIONALWEIGHTOFGFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
#include "IDirectionalWeightOfGFunction.hpp"
namespace DartInputs {
	class DirectionalWeightOfGFunction: public IDirectionalWeightOfGFunction {
	private:
		double omegaMin;
		double omegaMax;
		double a;
		double b;
	public:
		DirectionalWeightOfGFunction() :
				omegaMin(1), omegaMax(0), a(0), b(0) {}

		const double & getOmegaMin() const {
			return omegaMin;
		}
		const double & getOmegaMax() const {
			return omegaMax;
		}
		const double & getA() const {
			return a;
		}
		const double & getB() const {
			return b;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				omegaMin = node.get("omegaMin").as<double>();
				omegaMax = node.get("omegaMax").as<double>();
				a = node.get("a").as<double>();
				b = node.get("b").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_DIRECTIONALWEIGHTOFGFUNCTION_HPP_ */
