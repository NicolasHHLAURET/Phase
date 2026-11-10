/*
 * PinholeCameraAngleOfView.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_ANGLEDEFINITION_PINHOLECAMERAANGLEOFVIEW_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_ANGLEDEFINITION_PINHOLECAMERAANGLEOFVIEW_HPP_

#include "yaml-cpp/yaml.h"
#include "../IPinholeCameraAngleDefinition.hpp"
namespace DartInputs {
	class PinholeCameraAngleOfView: public IPinholeCameraAngleDefinition {
	private:
		double length;
		double width;
	public:
		PinholeCameraAngleOfView() :
				length(30), width(25) {}

		const double & getLength() const {
			return length;
		}

		const double & getWidth() const {
			return width;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				length = node.get("length").as<double>();
				width = node.get("width").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_ANGLEDEFINITION_PINHOLECAMERAANGLEOFVIEW_HPP_ */
