/*
 * ExtrapolatedFromAtmosphereSceneTemperature.hpp
 *
 *  Created on: 20 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_EXTRAPOLATEDFROMATMOSPHERESCENETEMPERATURE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_EXTRAPOLATEDFROMATMOSPHERESCENETEMPERATURE_HPP_

#include "yaml-cpp/yaml.h"
#include "ISceneTemperature.hpp"
namespace DartInputs {
	class ExtrapolatedFromAtmosphereSceneTemperature: public ISceneTemperature {
	private:
	public:
		ExtrapolatedFromAtmosphereSceneTemperature() {}

		void operator =(const YAML::Node& node) {
		}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_EXTRAPOLATEDFROMATMOSPHERESCENETEMPERATURE_HPP_ */
