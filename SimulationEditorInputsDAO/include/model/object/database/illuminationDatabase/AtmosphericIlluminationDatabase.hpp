/*
 * AtmosphericIlluminationDatabase.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_DATABASE_ILLUMINATIONDATABASE_ATMOSPHERICILLUMINATIONDATABASE_HPP_
#define INCLUDE_MODEL_OBJECT_DATABASE_ILLUMINATIONDATABASE_ATMOSPHERICILLUMINATIONDATABASE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AtmosphericIlluminationDatabase {
	private:
		bool weiggtingAtmosphericParameters;
		bool weiggtingReflectanceParameters;
		std::string modelName;
		std::string dartFilePath;
	public:
		AtmosphericIlluminationDatabase() :
				weiggtingAtmosphericParameters(true), weiggtingReflectanceParameters(true) {}

		const bool & getWeiggtingAtmosphericParameters() const {
			return weiggtingAtmosphericParameters;
		}

		const bool & getWeiggtingReflectanceParameters() const {
			return weiggtingReflectanceParameters;
		}

		const std::string & getModelName() const {
			return modelName;
		}

		const std::string & getDartFilePath() const {
			return dartFilePath;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				weiggtingAtmosphericParameters = node.get("weiggtingAtmosphericParameters").as<bool>();
				weiggtingReflectanceParameters = node.get("weiggtingReflectanceParameters").as<bool>();
				modelName = node.get("modelName").as<std::string>();
				dartFilePath = node.get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_DATABASE_ILLUMINATIONDATABASE_ATMOSPHERICILLUMINATIONDATABASE_HPP_ */
