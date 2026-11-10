/*
 * GasModelDatabase.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_GASMODELDATABASE_HPP_
#define INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_GASMODELDATABASE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class GasModelDatabase {
	private:
		std::string dartFilePath;

		std::string gasOpticalProperty;
		std::string gasTemperatureProfile;
		std::string gasOtherGasProfile;
	public:
		GasModelDatabase() {}

		const std::string & getDartFilePath() const {
			return dartFilePath;
		}

		const std::string & getGasOpticalProperty() const {
			return gasOpticalProperty;
		}
		const std::string & getGasTemperatureProfile() const {
			return gasTemperatureProfile;
		}
		const std::string & getGasOtherGasProfile() const {
			return gasOtherGasProfile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				gasOpticalProperty = node.get("gasOpticalProperty").as<std::string>();
				gasTemperatureProfile = node.get("gasTemperatureProfile").as<std::string>();
				gasOtherGasProfile = node.get("gasOtherGasProfile").as<std::string>();
				dartFilePath = node.get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_GASMODELDATABASE_HPP_ */
