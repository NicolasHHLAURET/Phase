/*
 * LidarGeometry.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARGEOMETRY_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARGEOMETRY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"

#include "footPrint/IFootPrint.hpp"
#include "footPrint/HalfAngle.hpp"
#include "footPrint/Radii.hpp"

#include "waveformStorage/IWaveformStorage.hpp"
#include "waveformStorage/WaveformFromAbsoluteDistance.hpp"
#include "waveformStorage/WaveformFromMinimumAltitude.hpp"

#include "laserScanningMode/ILaserScanningMode.hpp"
#include "laserScanningMode/ALS/ALSSinglePulse.hpp"
#include "laserScanningMode/ALS/ALSMultiPulseFile.hpp"
#include "laserScanningMode/ALS/ALSMultiPulseUserDefined.hpp"
#include "laserScanningMode/TLS/TLSSinglePulse.hpp"
#include "laserScanningMode/TLS/TLSMultiPulseFile.hpp"
#include "laserScanningMode/TLS/TLSMultiPulseUserDefined.hpp"

#include "lidarPulseMode/ILidarPulseMode.hpp"
#include "lidarPulseMode/LidarMultiPulse.hpp"
#include "lidarPulseMode/LidarSinglePulse.hpp"

namespace DartInputs {
	class LidarGeometry {
	private:
		shared_ptr<ILidarPulseMode> lidarPulseMode;
		double areaOfLidarSensor;
		shared_ptr<ILaserScanningMode> illuminationSource;
		shared_ptr<IWaveformStorage> waveformStorageOptions;
		shared_ptr<IFootPrint> footPrint;

	public:
		LidarGeometry() :
				areaOfLidarSensor(0.1) {}

		const shared_ptr<ILidarPulseMode> & getLidarPulseMode() const {
			return lidarPulseMode;
		}

		const double & getAreaOfLidarSensor() const {
			return areaOfLidarSensor;
		}

		const shared_ptr<ILaserScanningMode> & getIlluminationSource() const {
			return illuminationSource;
		}

		const shared_ptr<IWaveformStorage> & getWaveformStorageOptions() const {
			return waveformStorageOptions;
		}

		const shared_ptr<IFootPrint> & getFootPrint() const {
			return footPrint;
		}


		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				areaOfLidarSensor = node.get("areaOfLidarSensor").as<double>();

				if (!node.get("lidarPulseMode").IsNull()) {
					if (node.get("lidarPulseMode").Tag() == "!radiation.radiativeTransfer.lidar.geometry.lidarPulseMode.LidarSinglePulse") {
						lidarPulseMode = shared_ptr<ILidarPulseMode>(new LidarSinglePulse());
						static_cast<LidarSinglePulse &>(*lidarPulseMode) = node.get("lidarPulseMode");
					} else 	if (node.get("lidarPulseMode").Tag() == "!radiation.radiativeTransfer.lidar.geometry.lidarPulseMode.LidarSinglePulse") {
						lidarPulseMode = shared_ptr<ILidarPulseMode>(new LidarMultiPulse());
						static_cast<LidarMultiPulse &>(*lidarPulseMode) = node.get("lidarPulseMode");
					}
				}

				if (!node.get("illuminationSource").IsNull()) {
					if (node.get("illuminationSource").Tag() == "!radiation.radiativeTransfer.lidar.geometry.laserScanningMode.ALS.ALSSinglePulse") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new ALSSinglePulse());
						static_cast<ALSSinglePulse &>(*illuminationSource) = node.get("illuminationSource");
					} else if (node.get("illuminationSource").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.laserScanningMode.ALS.ALSMultiPulseFile") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new ALSMultiPulseFile());
						static_cast<ALSMultiPulseFile &>(*illuminationSource) = node.get("illuminationSource");
					} else if (node.get("illuminationSource").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.laserScanningMode.ALS.ALSMultiPulseUserDefined") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new ALSMultiPulseUserDefined());
						static_cast<ALSMultiPulseUserDefined &>(*illuminationSource) = node.get("illuminationSource");
					} else if (node.get("illuminationSource").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.laserScanningMode.TLS.TLSSinglePulse") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new TLSSinglePulse());
						static_cast<TLSSinglePulse &>(*illuminationSource) = node.get("illuminationSource");
					} else if (node.get("illuminationSource").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.laserScanningMode.TLS.TLSMultiPulseFile") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new TLSMultiPulseFile());
						static_cast<TLSMultiPulseFile &>(*illuminationSource) = node.get("illuminationSource");
					} else if (node.get("illuminationSource").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.laserScanningMode.TLS.TLSMultiPulseUserDefined") {
						illuminationSource = shared_ptr<ILaserScanningMode>(new TLSMultiPulseUserDefined());
						static_cast<TLSMultiPulseUserDefined &>(*illuminationSource) = node.get("illuminationSource");
					}
				}

				if (!node.get("waveformStorageOptions").IsNull()) {
					if (node.get("waveformStorageOptions").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.waveformStorage.WaveformFromAbsoluteDistance") {
						waveformStorageOptions = shared_ptr<IWaveformStorage>(new WaveformFromAbsoluteDistance());
						static_cast<WaveformFromAbsoluteDistance &>(*waveformStorageOptions) = node.get("waveformStorageOptions");
					} else if (node.get("waveformStorageOptions").Tag()
							== "radiation.radiativeTransfer.lidar.geometry.waveformStorage.WaveformFromMinimumAltitude") {
						waveformStorageOptions = shared_ptr<IWaveformStorage>(new WaveformFromMinimumAltitude());
						static_cast<WaveformFromMinimumAltitude &>(*waveformStorageOptions) = node.get("waveformStorageOptions");
					}
				}

				if (!node.get("footPrint").IsNull()) {
					if (node.get("footPrint").Tag() == "!radiation.radiativeTransfer.lidar.geometry.footPrint.HalfAngle") {
						footPrint = shared_ptr<IFootPrint>(new HalfAngle());
						static_cast<HalfAngle &>(*footPrint) = node.get("footPrint");
					} else if (node.get("footPrint").Tag() == "!radiation.radiativeTransfer.lidar.geometry.footPrint.Radii") {
						footPrint = shared_ptr<IFootPrint>(new Radii());
						static_cast<Radii &>(*footPrint) = node.get("footPrint");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARGEOMETRY_HPP_ */
