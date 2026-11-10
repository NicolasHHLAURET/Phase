/*
 * WaveformFromMinimumAltitude.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMMINIMUMALTITUDE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMMINIMUMALTITUDE_HPP_

#include "IWaveformStorage.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class WaveformFromMinimumAltitude: public IWaveformStorage {
	private:
		double heightAboveSceneMinimumAltitude;
		double heightBelowSceneMinimumAltitude;
	public:
		WaveformFromMinimumAltitude() :
				heightAboveSceneMinimumAltitude(50.0), heightBelowSceneMinimumAltitude(50.0) {}

		const double & getHeightAboveSceneMinimumAltitude() const {
			return heightAboveSceneMinimumAltitude;
		}
		const double & getHeightBelowSceneMinimumAltitude() const {
			return heightBelowSceneMinimumAltitude;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				heightAboveSceneMinimumAltitude = node.get("heightAboveSceneMinimumAltitude").as<double>();
				heightBelowSceneMinimumAltitude = node.get("heightBelowSceneMinimumAltitude").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMMINIMUMALTITUDE_HPP_ */
