/*
 * DartsRGB.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTSRGB_HPP_
#define INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTSRGB_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class DartsRGB {
	private:
		double red;
		double green;
		double blue;
		double opacity;
	public:
		DartsRGB() :
				red(0.8), green(0.7), blue(1), opacity(0.8) {}

		const double & getRed() const {
			return red;
		}
		const double & getGreen() const {
			return green;
		}
		const double & getBlue() const {
			return blue;
		}
		const double & getOpacity() const {
			return opacity;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				red = node.get("red").as<double>();
				green = node.get("green").as<double>();
				blue = node.get("blue").as<double>();
				opacity = node.get("opacity").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTSRGB_HPP_ */
