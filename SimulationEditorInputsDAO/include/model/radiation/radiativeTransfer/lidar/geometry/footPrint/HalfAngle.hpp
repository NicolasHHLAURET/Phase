/*
 * HalfAngle.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_FOOTPRINT_HALFANGLE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_FOOTPRINT_HALFANGLE_HPP_

#include "IFootPrint.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class HalfAngle: public IFootPrint {
	private:
		double footprint;
		double fieldOfView;
	public:
		HalfAngle() :
				footprint(0.0012), fieldOfView(0.0015) {}

		const double & getFootprint() const {
			return footprint;
		}
		const double & getFieldOfView() const {
			return fieldOfView;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				footprint = node.get("footprint").as<double>();
				fieldOfView = node.get("fieldOfView").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_FOOTPRINT_HALFANGLE_HPP_ */
