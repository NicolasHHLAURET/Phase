/*
 * ExternalScripts.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "yaml-cpp/yaml.h"
#include "externalScripts/IExternalScript.hpp"
#include "externalScripts/PythonExternalScript.hpp"
namespace DartInputs {
	class ExternalScripts {
	private:
		std::vector<shared_ptr<IExternalScript> > externalScripts;
	public:
		ExternalScripts() {}

		const std::vector<shared_ptr<IExternalScript> > & getExternalScripts() const {
			return externalScripts;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("externalScripts").IsNull() && node.get("externalScripts").IsSequence()) {
					for (YAML::const_iterator it = node.get("externalScripts").begin(); it != node.get("externalScripts").end(); ++it) {
						shared_ptr<IExternalScript> script;
						if (it->Tag() == "!execution.externalScripts.PythonExternalScript") {
							script = shared_ptr<IExternalScript>(new PythonExternalScript());
							static_cast<PythonExternalScript &>(*script) = (*it);
						} else {
							break;
						}
						externalScripts.push_back(script);
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_HPP_ */
