/*
 * IWaveformStorage.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_IWAVEFORMSTORAGE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_IWAVEFORMSTORAGE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IWaveformStorage {
	public:
		virtual ~IWaveformStorage() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_WAVEFORMSTORAGE_IWAVEFORMSTORAGE_HPP_ */
