/*
 * ILidarType.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_ILIDARTYPE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_ILIDARTYPE_HPP_

#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class ILidarType {
	public:
		virtual ~ILidarType() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_ILIDARTYPE_HPP_ */
