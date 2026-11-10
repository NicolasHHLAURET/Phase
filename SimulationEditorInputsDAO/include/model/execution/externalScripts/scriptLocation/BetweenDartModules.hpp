/*
 * BetweenDartModules.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BETWEENDARTMODULES_HPP_
#define INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BETWEENDARTMODULES_HPP_

#include "yaml-cpp/yaml.h"
#include "IScriptLocation.hpp"
namespace DartInputs {
	class BetweenDartModules: public IScriptLocation {
	public:
		enum BetweenDartModule {
			BETWEENDIRECTIONPHASE, BETWEENPHASEMAKET, BETWEENMAKETDART
		};
	private:
		BetweenDartModule betweenDartModules;
	public:
		BetweenDartModules() :
				betweenDartModules(BETWEENDIRECTIONPHASE) {}

		const BetweenDartModule & getBetweenDartModules() const {
			return betweenDartModules;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("betweenDartModules").as<std::string>() == "BETWEENDIRECTIONPHASE") {
					betweenDartModules = BETWEENDIRECTIONPHASE;
				} else if (node.get("betweenDartModules").as<std::string>() == "BETWEENPHASEMAKET") {
					betweenDartModules = BETWEENPHASEMAKET;
				} else {
					betweenDartModules = BETWEENMAKETDART;
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_EXECUTION_EXTERNALSCRIPTS_SCRIPTLOCATION_BETWEENDARTMODULES_HPP_ */
