/*
 * SolarIrradiance.hpp
 *
 *  Created on: 18 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_SOLARIRRADIANCE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_SOLARIRRADIANCE_HPP_

#include "yaml-cpp/yaml.h"
#include "ISolarRadiationDefinition.hpp"

/**
 * (Aucun paramètre)
 */
namespace DartInputs {
	class SolarIrradiance: public ISolarRadiationDefinition {
	public:
		SolarIrradiance() {}

		void operator =(const YAML::Node& node) {
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_SOLARIRRADIANCE_HPP_ */
