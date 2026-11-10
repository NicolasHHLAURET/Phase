/*
 * PinholeCameraRotation.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_PINHOLECAMERAROTATION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_PINHOLECAMERAROTATION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class PinholeCameraRotation {
	private:
		double precessionAzimuth;
		double nutationZenith;
	public:
		PinholeCameraRotation() :
				precessionAzimuth(225), nutationZenith(15) {}

		const double & getPrecessionAzimuth() const {
			return precessionAzimuth;
		}

		const double & getNutationZenith() const {
			return nutationZenith;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				precessionAzimuth = node.get("precessionAzimuth").as<double>();
				nutationZenith = node.get("nutationZenith").as<double>();
			}
		}

	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_PINHOLECAMERAROTATION_HPP_ */
