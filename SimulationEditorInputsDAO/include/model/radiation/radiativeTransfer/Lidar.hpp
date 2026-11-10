/*
 * Lidar.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_LIDAR_HPP_
#define INCLUDE_MODEL_RADIATION_LIDAR_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "lidar/LidarParameters.hpp"
#include "lidar/LidarSolarNoise.hpp"
#include "lidar/geometry/LidarGeometry.hpp"
#include "IRadiativeTransferMethod.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class Lidar: public IRadiativeTransferMethod {
	private:
		shared_ptr<LidarParameters> lidarParameters;
		shared_ptr<LidarSolarNoise> solarNoise;
		shared_ptr<LidarGeometry> lidarGeometry;

	public:
		Lidar() {}

		const shared_ptr<LidarSolarNoise> & getSolarNoise() const {
			return solarNoise;
		}

		const shared_ptr<LidarGeometry> & getLidarGeometry() const {
			return lidarGeometry;
		}

		const shared_ptr<LidarParameters> & getLidarParameters() const {
			return lidarParameters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("lidarParameters").IsNull()) {
					if (node.get("lidarParameters").Tag() == "!radiation.radiativeTransfer.lidar.LidarParameters") {
						lidarParameters = shared_ptr<LidarParameters>(new LidarParameters());
						(*lidarParameters) = node.get("lidarParameters");
					}
				}

				if (!node.get("solarNoise").IsNull()) {
					if (node.get("solarNoise").Tag() == "!radiation.radiativeTransfer.lidar.LidarSolarNoise") {
						solarNoise = shared_ptr<LidarSolarNoise>(new LidarSolarNoise());
						(*solarNoise) = node.get("solarNoise");
					}
				}

				if (!node.get("lidarGeometry").IsNull()) {
					if (node.get("lidarGeometry").Tag() == "!radiation.radiativeTransfer.lidar.geometry.LidarGeometry") {
						lidarGeometry = shared_ptr<LidarGeometry>(new LidarGeometry());
						(*lidarGeometry) = node.get("lidarGeometry");
					}
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_LIDAR_HPP_ */
