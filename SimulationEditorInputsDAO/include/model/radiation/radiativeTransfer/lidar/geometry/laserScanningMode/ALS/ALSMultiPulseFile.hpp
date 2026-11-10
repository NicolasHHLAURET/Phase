/*
 * ALSMultiPulseFile.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEFILE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEFILE_HPP_

#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"
#include "../IPulseFileFunction.hpp"
#include "ALS.hpp"
namespace DartInputs {
	class ALSMultiPulseFile: public ALS, public IPulseFileFunction {
	private:
		std::string pulseFile;

	public:
		ALSMultiPulseFile() {}

		const std::string & getPulseFile() const {
			return pulseFile;
		}

		void operator =(const YAML::Node& node) {
			ALS::operator=(node);
			if (!node.IsNull()) {
				pulseFile = node.get("pulseFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEFILE_HPP_ */
