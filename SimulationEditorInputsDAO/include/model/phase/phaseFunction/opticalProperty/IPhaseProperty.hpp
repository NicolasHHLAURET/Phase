/*
 * IPhaseProperty.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IPHASEPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IPHASEPROPERTY_HPP_

#include "IOpticalProperty.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPhaseProperty: public IOpticalProperty {
	public:
		virtual ~IPhaseProperty() {}
		virtual void operator =(const YAML::Node& node) {
			IOpticalProperty::operator=(node);
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IPHASEPROPERTY_HPP_ */
