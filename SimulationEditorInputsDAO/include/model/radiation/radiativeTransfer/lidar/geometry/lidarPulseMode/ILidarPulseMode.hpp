/*
 * ILidarPulseMode.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_ILIDARPULSEMODE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_ILIDARPULSEMODE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ILidarPulseMode {
	public:
		virtual ~ILidarPulseMode() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_ILIDARPULSEMODE_HPP_ */
