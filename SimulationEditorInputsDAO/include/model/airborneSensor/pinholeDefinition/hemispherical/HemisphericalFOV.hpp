/*
 * HemisphericalFOV.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALFOV_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALFOV_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class HemisphericalFOV {
	private:
		double maximalAngleImage;
	public:
		HemisphericalFOV() :
				maximalAngleImage(90) {}

		const double & getMaximalAngleImage() const {
			return maximalAngleImage;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				maximalAngleImage = node.get("maximalAngleImage").as<double>();
			}
		}

	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALFOV_HPP_ */
