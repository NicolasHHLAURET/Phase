/*
 * Spherical.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_SPHERICAL_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_SPHERICAL_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Spherical: public ILeafAngleDistribution {
	private:
		// Aucun parametre
	public:
		Spherical() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_SPHERICAL_HPP_ */
