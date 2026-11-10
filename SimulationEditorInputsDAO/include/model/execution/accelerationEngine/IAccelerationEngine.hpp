/*
 * IAccelerationEngine.hpp
 *
 *  Created on: 21 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_IACCELERATIONENGINE_HPP_
#define INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_IACCELERATIONENGINE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IAccelerationEngine {
	public:
		virtual ~IAccelerationEngine() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_IACCELERATIONENGINE_HPP_ */
