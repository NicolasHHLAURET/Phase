/*
 * PinholeCameraNumberPixelsInFOV.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERANUMBERPIXELSINFOV_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERANUMBERPIXELSINFOV_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class PinholeCameraNumberPixelsInFOV {
	private:
		int nbPixelsLength;
		int nbPixelsWidth;

	public:
		PinholeCameraNumberPixelsInFOV() :
				nbPixelsLength(60), nbPixelsWidth(50) {}

		const int & getNbPixelsLength() const {
			return nbPixelsLength;
		}

		const int & getNbPixelsWidth() const {
			return nbPixelsWidth;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				nbPixelsLength = node.get("nbPixelsLength").as<int>();
				nbPixelsWidth = node.get("nbPixelsWidth").as<int>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERANUMBERPIXELSINFOV_HPP_ */
