/*
 * MultiplicativeFactorGlobal.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORGLOBAL_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORGLOBAL_HPP_

#include "yaml-cpp/yaml.h"
#include "IMultiplicativeFactorQuantifier.hpp"
#include "IMultiplicativeFactor.hpp"
namespace DartInputs {
	class MultiplicativeFactorGlobal: public IMultiplicativeFactorQuantifier {
	public:
		MultiplicativeFactorGlobal() {}
		void operator =(const YAML::Node& node) {
			IMultiplicativeFactorQuantifier::operator =(node);
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORGLOBAL_HPP_ */
