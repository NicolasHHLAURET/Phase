/*
 * IExternalScript.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_IEXTERNALSCRIPT_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_IEXTERNALSCRIPT_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"

#include "scriptLocation/IScriptLocation.hpp"
#include "scriptLocation/AfterDartProcess.hpp"
#include "scriptLocation/BeforeDartProcess.hpp"
#include "scriptLocation/BetweenDartModules.hpp"
namespace DartInputs {
	class IExternalScript {
	private:
		std::string sourceFile;
		std::string arguments;
		shared_ptr<IScriptLocation> location;
		std::string scriptName;
		bool externalScriptEnabled;

	public:
		IExternalScript() :
				externalScriptEnabled(true) {}
		virtual ~IExternalScript() {}

		const std::string & getSourceFile() {
			return sourceFile;
		}
		const std::string & getArguments() {
			return arguments;
		}
		const shared_ptr<IScriptLocation> & getLocation() {
			return location;
		}
		const std::string & getScriptName() {
			return scriptName;
		}
		const bool & getExternalScriptEnabled() {
			return externalScriptEnabled;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				sourceFile = node.get("sourceFile").get("dartFilePath").get("relativePath").as<std::string>();
				arguments = node.get("arguments").as<std::string>();
				scriptName = node.get("scriptName").as<std::string>();
				externalScriptEnabled = node.get("externalScriptEnabled").as<bool>();

				if (!node.get("location").IsNull()) {
					if (node.get("location").Tag() == "!execution.externalScripts.scriptLocation.AfterDartProcess") {
						location = shared_ptr<IScriptLocation>(new AfterDartProcess());
						static_cast<AfterDartProcess &>(*location) = node.get("location");
					} else if (node.get("location").Tag() == "!execution.externalScripts.scriptLocation.BeforeDartProcess") {
						location = shared_ptr<IScriptLocation>(new BeforeDartProcess());
						static_cast<BeforeDartProcess &>(*location) = node.get("location");
					} else if (node.get("location").Tag() == "!execution.externalScripts.scriptLocation.BetweenDartModules") {
						location = shared_ptr<IScriptLocation>(new BetweenDartModules());
						static_cast<BetweenDartModules &>(*location) = node.get("location");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_IEXTERNALSCRIPT_HPP_ */
