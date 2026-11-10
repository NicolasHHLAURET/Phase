/*
 * IDartGroundArea.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_GROUND_IDARTGROUNDAREA_HPP_
#define INCLUDE_MODEL_MODELISATION_GROUND_IDARTGROUNDAREA_HPP_

#include "yaml-cpp/yaml.h"
#include "../IDartObject.hpp"
namespace DartInputs {
	class IDartGroundArea: public IDartObject {
	private:
		int groundAreaOpticalPropertyId;
		int groundAreaThermalPropertyId;
		int itemId;
	public:
		IDartGroundArea() :
				groundAreaOpticalPropertyId(1), groundAreaThermalPropertyId(1), itemId(1) {}
		virtual ~IDartGroundArea() {}

		const int & getGroundAreaOpticalPropertyId() const {
			return groundAreaOpticalPropertyId;
		}
		const int & getGroundAreaThermalPropertyId() const {
			return groundAreaThermalPropertyId;
		}
		const int & getItemId() const {
			return itemId;
		}

		virtual void operator =(const YAML::Node& node) {
			IDartObject::operator=(node);
			if (!node.IsNull()) {
				groundAreaOpticalPropertyId = node.get("groundAreaOpticalPropertyId").as<int>();
				groundAreaThermalPropertyId = node.get("groundAreaThermalPropertyId").as<int>();
				itemId = node.get("itemId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_GROUND_IDARTGROUNDAREA_HPP_ */
