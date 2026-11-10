/*
 * UserDefinedSceneTemperature.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_TEMPERATURE_USERDEFINEDSCENETEMPERATURE_HPP_
#define INCLUDE_MODEL_RADIATION_TEMPERATURE_USERDEFINEDSCENETEMPERATURE_HPP_

#include "yaml-cpp/yaml.h"
#include "ISceneTemperature.hpp"
namespace DartInputs {
	class UserDefinedSceneTemperature: public ISceneTemperature {
	private:
		double skylForTemperatureAssignation;
		double histogramThreshold;
		double distanceBetweenIlluminationSubCenters;
	public:
		UserDefinedSceneTemperature() :
			skylForTemperatureAssignation(0.0),histogramThreshold(5.0),distanceBetweenIlluminationSubCenters(0.1)  {}

		const double & getSkylForTemperatureAssignation() const {
			return skylForTemperatureAssignation;
		}
		const double & getHistogramThreshold() const {
			return histogramThreshold;
		}
		const double & getDistanceBetweenIlluminationSubCenters() const {
			return distanceBetweenIlluminationSubCenters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				skylForTemperatureAssignation = node.get("skylForTemperatureAssignation").as<double>();
				histogramThreshold = node.get("histogramThreshold").as<double>();
				distanceBetweenIlluminationSubCenters = node.get("distanceBetweenIlluminationSubCenters").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_TEMPERATURE_USERDEFINEDSCENETEMPERATURE_HPP_ */
