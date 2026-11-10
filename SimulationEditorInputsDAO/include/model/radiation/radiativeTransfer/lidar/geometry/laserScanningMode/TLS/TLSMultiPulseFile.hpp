/*
 * TLSMultiPulseFile.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEFILE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEFILE_HPP_

#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"
#include "../IPulseFileFunction.hpp"
#include "TLS.hpp"
namespace DartInputs {
	class TLSMultiPulseFile: public IPulseFileFunction, public TLS {
	private:
		std::string pulseFile;

	public:
		TLSMultiPulseFile() {}

		const std::string & getPulseFile() const {
			return pulseFile;
		}

		void operator =(const YAML::Node& node) {
			TLS::operator=(node);
			if (!node.IsNull()) {
				pulseFile = node.get("pulseFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEFILE_HPP_ */
