/*
 * IDirectionalWeightOfGFunction.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_IDIRECTIONALWEIGHTOFGFUNCTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_IDIRECTIONALWEIGHTOFGFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IDirectionalWeightOfGFunction {
	public:
		virtual ~IDirectionalWeightOfGFunction() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_VEGETATIONFUNCTION_IDIRECTIONALWEIGHTOFGFUNCTION_HPP_ */
