/*
 * TLS.hpp
 *
 *  Created on: 28 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLS_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../../../../../../object/genericObject/TripletDouble.hpp"
#include "../ILaserScanningMode.hpp"

namespace DartInputs {
	class TLS: public ILaserScanningMode {
	private:
		shared_ptr<TripletDouble>	lidarPosition;

	public:
		virtual ~TLS() {}

		const shared_ptr<TripletDouble> & getLidarPosition() const
		{
			return lidarPosition;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("lidarPosition").IsNull()) {
					lidarPosition = shared_ptr<TripletDouble>(new TripletDouble());
					(*lidarPosition) = node.get("lidarPosition");
				}
			}
		}
	};
}




#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLS_HPP_ */
