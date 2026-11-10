/*
 * IMultiPulseFunction.hpp
 *
 *  Created on: 14 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IMULTIPULSEFUNCTION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IMULTIPULSEFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IMultiPulseFunction {
	public:
		virtual ~IMultiPulseFunction() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}




#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IMULTIPULSEFUNCTION_HPP_ */
