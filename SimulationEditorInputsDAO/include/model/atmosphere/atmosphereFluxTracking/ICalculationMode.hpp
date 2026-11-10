/*
 * ICalculationMode.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ICALCULATIONMODE_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ICALCULATIONMODE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ICalculationMode {
	public:
		virtual ~ICalculationMode() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ICALCULATIONMODE_HPP_ */
