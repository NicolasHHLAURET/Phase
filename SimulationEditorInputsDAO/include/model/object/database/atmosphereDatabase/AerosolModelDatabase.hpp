/*
 * AerosolModelDatabase.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_AEROSOLMODELDATABASE_HPP_
#define INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_AEROSOLMODELDATABASE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AerosolModelDatabase {
	private:
		std::string dartFilePath;

		std::string aerosolOpticalProperty;
		std::string aerosolVerticalProfile;
		std::string aerosolHenyeyGreensteinParameters;
	public:
		AerosolModelDatabase() {}

		const std::string & getDartFilePath() const {
			return dartFilePath;
		}

		const std::string & getAerosolOpticalProperty() const {
			return aerosolOpticalProperty;
		}

		const std::string & getAerosolVerticalProfile() const {
			return aerosolVerticalProfile;
		}

		const std::string & getAerosolHenyeyGreensteinParameters() const {
			return aerosolHenyeyGreensteinParameters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				aerosolOpticalProperty = node.get("aerosolOpticalProperty").as<std::string>();
				aerosolVerticalProfile = node.get("aerosolVerticalProfile").as<std::string>();
				aerosolHenyeyGreensteinParameters = node.get("aerosolHenyeyGreensteinParameters").as<std::string>();
				dartFilePath = node.get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_OBJECT_DATABASE_ATMOSPHEREDATABASE_AEROSOLMODELDATABASE_HPP_ */
