/*
 * DartModelDatabase.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_DATABASE_OPTICALDATABASE_DARTMODELDATABASE_HPP_
#define INCLUDE_MODEL_OBJECT_DATABASE_OPTICALDATABASE_DARTMODELDATABASE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class DartModelDatabase {
	private:
		std::string modelName;
		std::string dartFilePath;
	public:
		DartModelDatabase() {}

		const std::string & getModelName() const {
			return modelName;
		}

		const std::string & getDartFilePath() const {
			return dartFilePath;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				modelName = node.get("modelName").as<std::string>();
				dartFilePath = node.get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_DATABASE_OPTICALDATABASE_DARTMODELDATABASE_HPP_ */
