/*
 * LidarSolarNoise.hpp
 *
 *  Created on: 21 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARSOLARNOISE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARSOLARNOISE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../FluxTracking.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarSolarNoise {
	public:
		enum SolarRunningMode {
			LIDAR_ONLY, SOLAR_NOISE_ONLY, SOLAR_NOISE_AND_LIDAR
		};

		enum AirborneConfiguration {
			PUSHBROOM, PINHOLE_CAMERA
		};

	private:
		SolarRunningMode runningMode;
		AirborneConfiguration airborneConfiguration;
		shared_ptr<FluxTracking> solarNoiseParameters;

	public:
		LidarSolarNoise() :
				runningMode(LIDAR_ONLY), airborneConfiguration(PUSHBROOM) {}

		const SolarRunningMode & getRunningMode() const {
			return runningMode;
		}

		const AirborneConfiguration & getAirborneConfiguration() const {
			return airborneConfiguration;
		}

		const shared_ptr<FluxTracking> & getSolarNoiseParameters() const {
			return solarNoiseParameters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("runningMode").as<std::string>() == "LIDAR_ONLY") {
					runningMode = LIDAR_ONLY;
				} else if (node.get("runningMode").as<std::string>() == "SOLAR_NOISE_ONLY") {
					runningMode = SOLAR_NOISE_ONLY;
				} else {
					runningMode = SOLAR_NOISE_AND_LIDAR;
				}

				if (node.get("airborneConfiguration").as<std::string>() == "PUSHBROOM") {
					airborneConfiguration = PUSHBROOM;
				} else {
					airborneConfiguration = PINHOLE_CAMERA;
				}

				if (!node.get("solarNoiseParameters").IsNull()) {
					solarNoiseParameters = shared_ptr<FluxTracking>(new FluxTracking());
					(*solarNoiseParameters) = node.get("solarNoiseParameters");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARSOLARNOISE_HPP_ */
