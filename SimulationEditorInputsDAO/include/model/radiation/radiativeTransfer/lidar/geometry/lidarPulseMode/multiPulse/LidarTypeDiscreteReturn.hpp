/*
 * LidarTypePhotonCounting.hpp
 *
 *  Created on: 14 janv. 2020
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEDISCRETERETURN_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEDISCRETERETURN_HPP_

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
	class LidarTypeDiscreteReturn : public ILidarType {
	public:
		enum OUTPUT_INTENSITY_VALUE_TYPE
		{
			FAST_PEAK_AMPLITUDE, GAUSS_PEAK_AMPLITUDE, GAUSS_INTEGRAL, GAUSS_SIGMA, GAUSS_REFLECTANCE, GAUSS_ALL
		};

	private:
		double minimumTargetReflectance;
		double numberOfPointsPerPulse;
		OUTPUT_INTENSITY_VALUE_TYPE outputIntensityValueType;

	public:
		LidarTypeDiscreteReturn() :
			minimumTargetReflectance(0.01), numberOfPointsPerPulse(1), outputIntensityValueType(FAST_PEAK_AMPLITUDE) {}

		const double & getMinimumTargetReflectance() const {
			return minimumTargetReflectance;
		}

		const double & getNumberOfPointsPerPulse() const {
			return numberOfPointsPerPulse;
		}

		const OUTPUT_INTENSITY_VALUE_TYPE & getOutputIntensityValueType() const {
			return outputIntensityValueType;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				minimumTargetReflectance = node.get("minimumTargerReflectance").as<double>();
				numberOfPointsPerPulse = node.get("numberOfPointsPerPulse").as<double>();

				if (node.get("outputIntensityValueType").as<std::string>() == "FAST_PEAK_AMPLITUDE") {
					outputIntensityValueType = FAST_PEAK_AMPLITUDE;
				} else if (node.get("outputIntensityValueType").as<std::string>() == "GAUSS_PEAK_AMPLITUDE") {
					outputIntensityValueType = FAST_PEAK_AMPLITUDE;
				} else if (node.get("outputIntensityValueType").as<std::string>() == "GAUSS_INTEGRAL") {
					outputIntensityValueType = GAUSS_INTEGRAL;
				} else if (node.get("outputIntensityValueType").as<std::string>() == "GAUSS_SIGMA") {
					outputIntensityValueType = GAUSS_SIGMA;
				} else if (node.get("outputIntensityValueType").as<std::string>() == "GAUSS_REFLECTANCE") {
					outputIntensityValueType = GAUSS_REFLECTANCE;
				} else if (node.get("outputIntensityValueType").as<std::string>() == "GAUSS_ALL") {
					outputIntensityValueType = GAUSS_ALL;
				}
			}
		}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LIDARPULSEMODE_MULTIPULSE_LIDARTYPEDISCRETERETURN_HPP_ */
