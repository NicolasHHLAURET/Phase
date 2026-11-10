/*
 * IThermalProperty.hpp
 *
 *  Created on: 2 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_THERMALFUNCTION_ITHERMALPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_THERMALFUNCTION_ITHERMALPROPERTY_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IThermalProperty {
	private:
		std::string thermalPropertyName;
		int itemId;
	public:
		IThermalProperty() :
				itemId(0) {}
		virtual ~IThermalProperty() {}

		const std::string & getThermalPropertyName() const {
			return thermalPropertyName;
		}

		const int & getItemId() const {
			return itemId;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				thermalPropertyName = node.get("thermalPropertyName").as<std::string>();
				itemId = node.get("itemId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_THERMALFUNCTION_ITHERMALPROPERTY_HPP_ */
