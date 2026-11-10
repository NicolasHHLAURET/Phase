/*
 * VerticalProfile1DSceneTemperature.hpp
 *
 *  Created on: 20 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE1DSCENETEMPERATURE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE1DSCENETEMPERATURE_HPP_

#include "yaml-cpp/yaml.h"
#include "ISceneTemperature.hpp"
namespace DartInputs {
	class VerticalProfile1DSceneTemperature: public ISceneTemperature {
	private:
		std::string temperatureSceneFile;

	public:
		VerticalProfile1DSceneTemperature() {}

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



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_TEMPERATURE_VERTICALPROFILE1DSCENETEMPERATURE_HPP_ */
