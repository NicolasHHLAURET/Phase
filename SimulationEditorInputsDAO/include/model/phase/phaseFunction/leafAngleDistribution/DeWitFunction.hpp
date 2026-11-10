/*
 * DeWitFunction.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_DEWITFUNCTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_DEWITFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class DeWitFunction: public ILeafAngleDistribution {
	private:
		double theta0;
		double theta1;
		double theta2;
		double m;
	public:
		DeWitFunction() :
				theta0(0), theta1(0), theta2(0), m(0) {}

		const double & getTheta0() const {
			return theta0;
		}
		const double & getTheta1() const {
			return theta1;
		}
		const double & getTheta2() const {
			return theta2;
		}
		const double & getM() const {
			return m;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				theta0 = node.get("theta0").as<double>();
				theta1 = node.get("theta1").as<double>();
				theta2 = node.get("theta2").as<double>();
				m = node.get("m").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_DEWITFUNCTION_HPP_ */
