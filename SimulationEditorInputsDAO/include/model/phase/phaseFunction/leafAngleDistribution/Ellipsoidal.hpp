/*
 * Ellipsoidal.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPSOIDAL_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPSOIDAL_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Ellipsoidal: public ILeafAngleDistribution {
	private:
		double ALA;
	public:
		Ellipsoidal() :
				ALA(0) {}

		const double & getALA() const {
			return ALA;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				ALA = node.get("ALA").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ELLIPSOIDAL_HPP_ */
