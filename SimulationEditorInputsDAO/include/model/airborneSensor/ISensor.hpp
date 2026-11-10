/*
 * ISensor.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_ISENSOR_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_ISENSOR_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ISensor {
	private:
		std::string sensorName;
		bool isEnableSensor;

	public:
		ISensor() :
				isEnableSensor(true) {}
		virtual ~ISensor() {}

		const std::string & getSensorName() const {
			return sensorName;
		}

		const bool & getIsEnableSensor() const {
			return isEnableSensor;
		}

		virtual void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				sensorName = node.get("sensorName").as<std::string>();
				isEnableSensor = node.get("isEnableSensor").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_ISENSOR_HPP_ */
