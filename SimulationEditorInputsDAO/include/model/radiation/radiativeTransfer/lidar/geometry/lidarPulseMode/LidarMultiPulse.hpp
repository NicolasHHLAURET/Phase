/*
 * LidarMultiPulse.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARMULTIPULSE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARMULTIPULSE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ILidarPulseMode.hpp"
#include "multiPulse/ILidarType.hpp"
#include "multiPulse/LidarTypePhotonCounting.hpp"
#include "multiPulse/LidarTypeDiscreteReturn.hpp"
#include "multiPulse/LidarTypeWaveform.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarMultiPulse : public ILidarPulseMode {
	private:
		shared_ptr<ILidarType> lidarType;

	public:
		LidarMultiPulse() {}

		const shared_ptr<ILidarType> & getLidarType() const {
			return lidarType;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("lidarType").IsNull()) {
					if (node.get("lidarType").Tag()	== "radiation.radiativeTransfer.lidar.geometry.lidarPulseMode.multiPulse.LidarTypeDiscreteReturn") {
						lidarType = shared_ptr<ILidarType>(new LidarTypeDiscreteReturn());
						static_cast<LidarTypeDiscreteReturn &>(*lidarType) = node.get("lidarType");
					} else if (node.get("lidarType").Tag() == "radiation.radiativeTransfer.lidar.geometry.lidarPulseMode.multiPulse.LidarTypePhotonCounting") {
						lidarType = shared_ptr<ILidarType>(new LidarTypePhotonCounting());
						static_cast<LidarTypePhotonCounting &>(*lidarType) = node.get("lidarType");
					} else if (node.get("lidarType").Tag() == "radiation.radiativeTransfer.lidar.geometry.lidarPulseMode.multiPulse.LidarTypeWaveform") {
						lidarType = shared_ptr<ILidarType>(new LidarTypeWaveform());
						static_cast<LidarTypeWaveform &>(*lidarType) = node.get("lidarType");
					}
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_LIDARMULTIPULSE_HPP_ */
