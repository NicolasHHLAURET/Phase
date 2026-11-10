/*
 * Uniform.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_UNIFORM_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_UNIFORM_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Uniform: public ILeafAngleDistribution {
	private:
		// Aucun parametre
	public:
		Uniform() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_UNIFORM_HPP_ */
