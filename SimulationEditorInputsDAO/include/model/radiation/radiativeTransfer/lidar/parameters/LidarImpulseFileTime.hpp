/*
 * LidarImpulseFileTime.hpp
 *
 *  Created on: 7 jan. 2020
 *      Author: xuebo
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILETIME_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILETIME_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class LidarImpulseFileTime {
	private:
		std::string file;


	public:
		LidarImpulseFileTime() {}

		const std::string & getFile() const {
			return file;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("file").IsNull()) {
					file = node.get("file").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}


#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILETIME_HPP_ */
