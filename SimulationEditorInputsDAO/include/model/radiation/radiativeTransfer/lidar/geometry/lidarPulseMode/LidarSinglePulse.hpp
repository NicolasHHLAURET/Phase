/*
 * LidarSinglePulse.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARSINGLEPULSE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARSINGLEPULSE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ILidarPulseMode.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarSinglePulse : public ILidarPulseMode {
	private:
	public:
		LidarSinglePulse() {}
		void operator =(const YAML::Node& node) {}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARSINGLEPULSE_HPP_ */
