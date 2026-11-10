/*
 * VerticalProfile3DSceneTemperature.hpp
 *
 *  Created on: 20 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE3DSCENETEMPERATURE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE3DSCENETEMPERATURE_HPP_

#include "yaml-cpp/yaml.h"
#include "ISceneTemperature.hpp"
namespace DartInputs {
	class VerticalProfile3DSceneTemperature: public ISceneTemperature {
	private:
		std::string temperatureSceneFile;

	public:
		VerticalProfile3DSceneTemperature() {}

		const std::string & getTemperatureSceneFile() const {
			return temperatureSceneFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				temperatureSceneFile = node.get("temperatureSceneFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE3DSCENETEMPERATURE_HPP_ */
