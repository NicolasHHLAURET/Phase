/*
 * Planophile.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_PLANOPHILE_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_PLANOPHILE_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class Planophile: public ILeafAngleDistribution {
	private:
		// Aucun parametre
	public:
		Planophile() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_PLANOPHILE_HPP_ */
