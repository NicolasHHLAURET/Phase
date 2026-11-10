/*
 * PhaseExtern.hpp
 *
 *  Created on: 16 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PHASEEXTERN_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PHASEEXTERN_HPP_

#include "yaml-cpp/yaml.h"
#include "IPhaseProperty.hpp"
namespace DartInputs {
	class PhaseExtern: public IPhaseProperty {
	private:
	public:
		PhaseExtern() {}

		void operator =(const YAML::Node& node) {
			IPhaseProperty::operator=(node);
		}
	};
}



#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PHASEEXTERN_HPP_ */
