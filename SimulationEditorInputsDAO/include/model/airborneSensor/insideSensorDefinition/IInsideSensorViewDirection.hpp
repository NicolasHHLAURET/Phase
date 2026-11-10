/*
 * IInsideSensorViewDirection.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_IINSIDESENSORVIEWDIRECTION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_IINSIDESENSORVIEWDIRECTION_HPP_

#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class IInsideSensorViewDirection {
	public:
		virtual ~IInsideSensorViewDirection() {}
		virtual void operator=(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_IINSIDESENSORVIEWDIRECTION_HPP_ */
