/*
 * AngleDirection.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GEOMETRY_ANGLEDIRECTION_HPP_
#define INCLUDE_MODEL_OBJECT_GEOMETRY_ANGLEDIRECTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AngleDirection {
	private:
		double zenithAngle;
		double azimuthAngle;

	public:
		AngleDirection() :
				zenithAngle(0), azimuthAngle(0) {}

		const double & getZenithAngle() const {
			return zenithAngle;
		}

		const double & getAzimuthAngle() const {
			return azimuthAngle;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				zenithAngle = node.get("zenithAngle").as<double>();
				azimuthAngle = node.get("azimuthAngle").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GEOMETRY_ANGLEDIRECTION_HPP_ */
