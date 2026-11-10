/*
 * LocalTime.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_LOCALTIME_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_LOCALTIME_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LocalTime {
	private:
		int UTC;
		bool daylightSavingTime;
	public:
		LocalTime() :
				UTC(0), daylightSavingTime(false) {}

		const int & getUTC() const {
			return UTC;
		}
		const bool & getDaylightSavingTime() const {
			return daylightSavingTime;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				UTC = node.get("UTC").as<int>();
				daylightSavingTime = node.get("daylightSavingTime").as<bool>();
			}
		}

	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_LOCALTIME_HPP_ */
