/*
 * ISunAngleDefinition.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_ISUNANGLEDEFINITION_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_ISUNANGLEDEFINITION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ISunAngleDefinition {
	public:
		virtual ~ISunAngleDefinition() {}
		void operator =(const YAML::Node& node) {}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_ISUNANGLEDEFINITION_HPP_ */
