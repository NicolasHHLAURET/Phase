/*
 * BeforeDartProcess.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BEFOREDARTPROCESS_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BEFOREDARTPROCESS_HPP_

#include "yaml-cpp/yaml.h"
#include "IScriptLocation.hpp"
namespace DartInputs {
	class BeforeDartProcess: public IScriptLocation {
	private:
		// no parameters
	public:
		BeforeDartProcess() {}
		void operator =(const YAML::Node& node) {}
	};
}

#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BEFOREDARTPROCESS_HPP_ */
