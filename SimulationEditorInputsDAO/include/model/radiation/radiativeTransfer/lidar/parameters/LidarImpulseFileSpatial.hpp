/*
 * LidarImpulseFileSpatial.hpp
 *
 *  Created on: 8 oct. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILESPATIAL_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILESPATIAL_HPP_

#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class LidarImpulseFileSpatial {
	private:
		std::string file;

		double matrixResolution;
		double offsetX;
		double offsetY;

	public:
		LidarImpulseFileSpatial() :
				matrixResolution(1.), offsetX(0.), offsetY(0.) {}

		const std::string & getFile() const {
			return file;
		}
		const double & getMatrixResolution() const {
			return matrixResolution;
		}
		const double & getOffsetX() const {
			return offsetX;
		}

		const double & getOffsetY() const {
			return offsetY;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("file").IsNull()) {
					file = node.get("file").get("dartFilePath").get("relativePath").as<std::string>();
				}
				matrixResolution = node.get("matrixResolution").as<double>();
				offsetX = node.get("offsetX").as<double>();
				offsetY = node.get("offsetY").as<double>();
			}
		}
	};
}


#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_PARAMETERS_LIDARIMPULSEFILE_HPP_ */
