/*
 * LidarTypeDiscreteReturn.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEPHOTONCOUNTING_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEPHOTONCOUNTING_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ILidarType.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarTypePhotonCounting : public ILidarType {
	private:
		double quantumEfficiency;
		double deadTime;
		double darkCountRate;

	public:
		LidarTypePhotonCounting() :
			quantumEfficiency(15.0), deadTime(2.5), darkCountRate(100.0) {}

		const double & getQuantumEfficiency() const {
			return quantumEfficiency;
		}
		const double & getDeadTime() const {
			return deadTime;
		}
		const double getDarkCountRate() const {
			return darkCountRate;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				quantumEfficiency = node.get("quantumEfficiency").as<double>();
				deadTime = node.get("deadTime").as<double>();
				darkCountRate = node.get("darkCountRate").as<double>();
			}
		}
	};
}




#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEPHOTONCOUNTING_HPP_ */
