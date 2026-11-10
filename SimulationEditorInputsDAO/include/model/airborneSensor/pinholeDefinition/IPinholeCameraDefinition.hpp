/*
 * IPinholeCameraDefinition.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_IPINHOLECAMERADEFINITION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_IPINHOLECAMERADEFINITION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPinholeCameraDefinition {
	public:
		virtual ~IPinholeCameraDefinition() {}
		virtual void operator=(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_IPINHOLECAMERADEFINITION_HPP_ */
