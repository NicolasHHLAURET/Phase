/*
 * IMultiplicativeFactor.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTOR_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IMultiplicativeFactor {
	public:
		virtual ~IMultiplicativeFactor() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTOR_HPP_ */
