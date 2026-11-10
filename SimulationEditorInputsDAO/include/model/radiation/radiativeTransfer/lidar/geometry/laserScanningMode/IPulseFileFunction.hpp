/*
 * IPulseFileFunction.hpp
 *
 *  Created on: 28 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IPULSEFILEFUNCTION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IPULSEFILEFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPulseFileFunction {
	public:
		virtual ~IPulseFileFunction() {}
		virtual const std::string & getPulseFile() const = 0;
		virtual void operator =(const YAML::Node& node) = 0;
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_IPULSEFILEFUNCTION_HPP_ */
