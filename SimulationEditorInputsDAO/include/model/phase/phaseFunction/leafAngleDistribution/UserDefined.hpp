/*
 * UserDefined.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_USERDEFINED_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_USERDEFINED_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class UserDefined: public ILeafAngleDistribution {
	private:
		std::string pathToLADDatabase;
	public:
		UserDefined() {}

		const std::string & getPathToLADDatabase() const {
			return pathToLADDatabase;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("pathToLADDatabase").IsNull()) {
					pathToLADDatabase = node.get("pathToLADDatabase").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_USERDEFINED_HPP_ */
