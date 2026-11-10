/*
 * IAddedViewDirections.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IADDEDVIEWDIRECTIONS_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IADDEDVIEWDIRECTIONS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IAddedViewDirections {
	private:
		std::string directionName;
		bool isEnableDirection;

	public:
		IAddedViewDirections() :
				isEnableDirection(true) {}
		virtual ~IAddedViewDirections() {}

		const std::string & getDirectionName() const {
			return directionName;
		}

		const bool & getIsEnableDirection() const {
			return isEnableDirection;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				directionName = node.get("directionName").as<std::string>();
				isEnableDirection = node.get("isEnableDirection").as<bool>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IADDEDVIEWDIRECTIONS_HPP_ */
