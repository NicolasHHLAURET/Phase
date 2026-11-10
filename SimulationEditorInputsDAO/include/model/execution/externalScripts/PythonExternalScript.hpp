/*
 * PythonExternalScript.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_PYTHONEXTERNALSCRIPT_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_PYTHONEXTERNALSCRIPT_HPP_

#include "yaml-cpp/yaml.h"
#include "IExternalScript.hpp"
namespace DartInputs {
	class PythonExternalScript: public IExternalScript {
	private:
		// no parameters
	public:
		PythonExternalScript() {}
		void operator =(const YAML::Node& node) {
			IExternalScript::operator=(node);
		}
	};
}
#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_PYTHONEXTERNALSCRIPT_HPP_ */
