/*
 * IScriptLocation.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_ISCRIPTLOCATION_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_ISCRIPTLOCATION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IScriptLocation {
	public:
		virtual ~IScriptLocation() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_ISCRIPTLOCATION_HPP_ */
