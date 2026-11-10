/*
 * ILaserScanningMode.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ILASERSCANNINGMODE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ILASERSCANNINGMODE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ILaserScanningMode {
	public:
		virtual ~ILaserScanningMode() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ILASERSCANNINGMODE_HPP_ */
