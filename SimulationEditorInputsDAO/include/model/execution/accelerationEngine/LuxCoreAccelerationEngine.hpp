/*
 * LuxCoreAccelerationEngine.hpp
 *
 *  Created on: 21 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_LUXCOREACCELERATIONENGINE_HPP_
#define INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_LUXCOREACCELERATIONENGINE_HPP_

#include "yaml-cpp/yaml.h"
#include "IAccelerationEngine.hpp"
namespace DartInputs {
	class LuxCoreAccelerationEngine: public IAccelerationEngine {
	private:
		double pixelSize;
		int timeLimit;
		int periodicSaveTime;
		int densityTarget;
		bool useTimeLimit;
		bool usePeriodicSave;
		bool useDensityTarget;

	public:
		LuxCoreAccelerationEngine() :
				pixelSize(1.0), timeLimit(60), periodicSaveTime(20), densityTarget(50), useTimeLimit(true), usePeriodicSave(false), useDensityTarget(true) {}

		const double & getPixelSize() const {
			return pixelSize;
		}
		const bool & getUseTimeLimit() const {
			return useTimeLimit;
		}
		const bool & getUsePeriodicSave() const {
			return usePeriodicSave;
		}
		const bool & getUseDensityTarget() const {
			return useDensityTarget;
		}
		const int & getTimeLimit() const {
			return timeLimit;
		}
		const int & getPeriodicSaveTime() const {
			return periodicSaveTime;
		}
		const int & getDensityTarget() const {
			return densityTarget;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				pixelSize = node.get("pixelSize").as<double>();
				timeLimit = node.get("timeLimit").as<int>();
				periodicSaveTime = node.get("periodicSaveTime").as<int>();
				densityTarget = node.get("densityTarget").as<int>();
				useTimeLimit = node.get("useTimeLimit").as<bool>();
				usePeriodicSave = node.get("usePeriodicSave").as<bool>();
				useDensityTarget = node.get("useDensityTarget").as<bool>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_LUXCOREACCELERATIONENGINE_HPP_ */
