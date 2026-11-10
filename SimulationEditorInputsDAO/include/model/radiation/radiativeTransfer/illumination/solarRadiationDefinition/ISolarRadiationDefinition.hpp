/*
 * ISolarRadiationDefinition.hpp
 *
 *  Created on: 18 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_ISOLARRADIATIONDEFINITION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_ISOLARRADIATIONDEFINITION_HPP_

#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class ISolarRadiationDefinition {
	private:
	public:
		virtual ~ISolarRadiationDefinition() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_ILLUMINATION_SOLARRADIATIONDEFINITION_ISOLARRADIATIONDEFINITION_HPP_ */
