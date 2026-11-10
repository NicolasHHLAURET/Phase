/*
 * Elliptical.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPTICAL_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPTICAL_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Elliptical: public ILeafAngleDistribution {
	private:
		double thetam;
		double epsilon;
	public:
		Elliptical() :
				thetam(0), epsilon(0) {}

		const double & getThetam() const {
			return thetam;
		}
		const double & getEpsilon() const {
			return epsilon;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				thetam = node.get("thetam").as<double>();
				epsilon = node.get("epsilon").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPTICAL_HPP_ */
