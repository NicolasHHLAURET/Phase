/*
 * IPusbroomScannerDefinition.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_IPUSBROOMSCANNERDEFINITION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_IPUSBROOMSCANNERDEFINITION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPusbroomScannerDefinition {
	public:
		virtual ~IPusbroomScannerDefinition() {}
		virtual void operator=(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_IPUSBROOMSCANNERDEFINITION_HPP_ */
