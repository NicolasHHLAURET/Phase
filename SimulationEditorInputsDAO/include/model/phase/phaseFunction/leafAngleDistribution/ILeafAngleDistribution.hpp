/*
 * ILeafAngleDistribution.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ILEAFANGLEDISTRIBUTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ILEAFANGLEDISTRIBUTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ILeafAngleDistribution {
	public:
		virtual ~ILeafAngleDistribution() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_ILEAFANGLEDISTRIBUTION_HPP_ */
