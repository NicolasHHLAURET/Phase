/*
 * ALS.hpp
 *
 *  Created on: 28 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALS_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALS_HPP_

#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"

namespace DartInputs {
	class ALS: public ILaserScanningMode {
	private:
		double lidarAltitude;
	public:
		ALS() : lidarAltitude(10) {}
		virtual ~ALS() {}

		const double & getLidarAltitude() const
		{
			return lidarAltitude;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				lidarAltitude = node.get("lidarAltitude").as<double>();
			}
		}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALS_HPP_ */
