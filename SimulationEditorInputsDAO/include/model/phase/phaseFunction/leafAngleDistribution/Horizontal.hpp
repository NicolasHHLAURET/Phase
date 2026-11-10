/*
 * Horizontal.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_HORIZONTAL_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_HORIZONTAL_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Horizontal: public ILeafAngleDistribution {
	private:
		// Aucun parametre
	public:
		Horizontal() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_HORIZONTAL_HPP_ */
