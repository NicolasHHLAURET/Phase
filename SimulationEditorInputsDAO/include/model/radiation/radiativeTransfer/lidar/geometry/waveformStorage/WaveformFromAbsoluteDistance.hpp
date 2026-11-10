/*
 * WaveformFromAbsoluteDistance.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMABSOLUTEDISTANCE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMABSOLUTEDISTANCE_HPP_

#include "IWaveformStorage.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class WaveformFromAbsoluteDistance: public IWaveformStorage {
	private:
		double distanceToReferencePoint;
		double distanceBeforeReferencePoint;
		double distanceAfterReferencePoint;
	public:
		WaveformFromAbsoluteDistance() :
				distanceToReferencePoint(-1.0), distanceBeforeReferencePoint(-1.0), distanceAfterReferencePoint(-1.0) {}

		const double & getDistanceToReferencePoint() const {
			return distanceToReferencePoint;
		}
		const double & getDistanceBeforeReferencePoint() const {
			return distanceBeforeReferencePoint;
		}
		const double & getDistanceAfterReferencePoint() const {
			return distanceAfterReferencePoint;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				distanceToReferencePoint = node.get("distanceToReferencePoint").as<double>();
				distanceBeforeReferencePoint = node.get("distanceBeforeReferencePoint").as<double>();
				distanceAfterReferencePoint = node.get("distanceAfterReferencePoint").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_WAVEFORMFROMABSOLUTEDISTANCE_HPP_ */
