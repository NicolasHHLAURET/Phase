/*
 * MultiplicativeFactor.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTOR_HPP_

#include "yaml-cpp/yaml.h"

#include "property/IMultiplicativeFactor.hpp"
namespace DartInputs {
	class MultiplicativeFactor {
	public:
		virtual ~MultiplicativeFactor() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTOR_HPP_ */
