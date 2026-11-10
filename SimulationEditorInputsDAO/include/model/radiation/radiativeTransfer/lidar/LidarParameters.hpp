/*
 * LidarParameters.hpp
 *
 *  Created on: 21 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARPARAMETERS_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARPARAMETERS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "parameters/LidarImpulseFileSpatial.hpp"
#include "parameters/LidarImpulseFileTime.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarParameters {
	private:
		// Lidar pulsation power
		double energyOfEachPulse;
		double halfPulseDuration;
		double pulseRelativePower; // Expert

		// parameter
		double halfPulseDurationAtRelativePower;

		// Lidar pulsation parameter
		double totalNumberOfPhotonPerPulsation;
		double fractionOfPhotonsAtLidarRadius;

		// Lidar acquisition
		bool isDartResutsIdenticalWithConstantParam;
		int maximumOrderOfScattering;
		bool isFilterSignal;
		int lidarAcquisitionRate;
		int maximumRAMOfCalculator;

		// Paramètre expert dont on ne sait pas trop à quoi ça sert :3
		int numberOfRandomSignalPerInterceptionInAtmosphere;

		// Impulse file
		shared_ptr<LidarImpulseFileSpatial> impulseFileSpatial;
		shared_ptr<LidarImpulseFileTime> impulseFileTime;

	public:
		LidarParameters() :
				energyOfEachPulse(1.0), halfPulseDuration(3.0), pulseRelativePower(0.5), halfPulseDurationAtRelativePower(2.0),
					totalNumberOfPhotonPerPulsation(100000.0), fractionOfPhotonsAtLidarRadius(0.368), isDartResutsIdenticalWithConstantParam(
					false), maximumOrderOfScattering(10), isFilterSignal(false), lidarAcquisitionRate(1), maximumRAMOfCalculator(1000), numberOfRandomSignalPerInterceptionInAtmosphere(
					1) {}

		const double & getEnergyOfEachPulse() const {
			return energyOfEachPulse;
		}

		const double & getHalfPulseDuration() const {
			return halfPulseDuration;
		}

		const double & getPulseRelativePower() const {
			return pulseRelativePower;
		}

		const double & getHalfPulseDurationAtRelativePower() const {
			return halfPulseDurationAtRelativePower;
		}

		const double & getTotalNumberOfPhotonPerPulsation() const {
			return totalNumberOfPhotonPerPulsation;
		}

		const double & getFractionOfPhotonsAtLidarRadius() const {
			return fractionOfPhotonsAtLidarRadius;
		}

		const shared_ptr<LidarImpulseFileSpatial> & getImpulseFileSpatial() const {
			return impulseFileSpatial;
		}

		const shared_ptr<LidarImpulseFileTime> & getImpulseFileTime() const {
			return impulseFileTime;
		}

		const bool & getIsDartResutsIdenticalWithConstantParam() const {
			return isDartResutsIdenticalWithConstantParam;
		}

		const int & getMaximumOrderOfScattering() const {
			return maximumOrderOfScattering;
		}

		const bool & getIsFilterSignal() const {
			return isFilterSignal;
		}

		const int & getLidarAcquisitionRate() const {
			return lidarAcquisitionRate;
		}

		const int & getMaximumRAMOfCalculator() const {
			return maximumRAMOfCalculator;
		}

		const int & getNumberOfRandomSignalPerInterceptionInAtmosphere() const {
			return numberOfRandomSignalPerInterceptionInAtmosphere;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				energyOfEachPulse = node.get("energyOfEachPulse").as<double>();
				halfPulseDuration = node.get("halfPulseDuration").as<double>();
				pulseRelativePower = node.get("pulseRelativePower").as<double>();

				halfPulseDurationAtRelativePower = node.get("halfPulseDurationAtRelativePower").as<double>();

				totalNumberOfPhotonPerPulsation = node.get("totalNumberOfPhotonPerPulsation").as<double>();
				fractionOfPhotonsAtLidarRadius = node.get("fractionOfPhotonsAtLidarRadius").as<double>();

				if (!node.get("impulseFileSpatial").IsNull()) {
					impulseFileSpatial = shared_ptr<LidarImpulseFileSpatial>(new LidarImpulseFileSpatial());
					(*impulseFileSpatial) = node.get("impulseFileSpatial");
				}

				if (!node.get("impulseFileTime").IsNull()) {
					impulseFileTime = shared_ptr<LidarImpulseFileTime>(new LidarImpulseFileTime());
					(*impulseFileTime) = node.get("impulseFileTime");
				}

				isDartResutsIdenticalWithConstantParam = node.get("isDartResutsIdenticalWithConstantParam").as<bool>();
				maximumOrderOfScattering = node.get("maximumOrderOfScattering").as<int>();
				isFilterSignal = node.get("isFilterSignal").as<bool>();
				lidarAcquisitionRate = node.get("lidarAcquisitionRate").as<int>();
				maximumRAMOfCalculator = node.get("maximumRAMOfCalculator").as<int>();

				numberOfRandomSignalPerInterceptionInAtmosphere = node.get("numberOfRandomSignalPerInterceptionInAtmosphere").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_LIDARPARAMETERS_HPP_ */
