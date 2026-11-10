/*
 * WaterPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_WATERPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_WATERPLOTTYPE_HPP_

#include "yaml-cpp/yaml.h"
#include "IPlotType.hpp"
namespace DartInputs {
	class WaterPlotType: public IPlotType {
	private:
		int waterOpticalPropertyId;
		int waterThermalPropertyId;

		double extinctionCoeffcient;
		double numberComponentWaterVolume;

		double waterHeight;
		double waterDepth;
		double stdDevWaterDepth;
	public:
		WaterPlotType() :
				waterOpticalPropertyId(1), waterThermalPropertyId(1), extinctionCoeffcient(0.5), numberComponentWaterVolume(1), waterHeight(0), waterDepth(
					10.0), stdDevWaterDepth(0) {}

		const int & getWaterOpticalPropertyId() const {
			return waterOpticalPropertyId;
		}
		const int & getWaterThermalPropertyId() const {
			return waterThermalPropertyId;
		}

		const double & getExtinctionCoeffcient() const {
			return extinctionCoeffcient;
		}
		const double & getNumberComponentWaterVolume() const {
			return numberComponentWaterVolume;
		}

		const double & getWaterHeight() const {
			return waterHeight;
		}
		const double & getWaterDepth() const {
			return waterDepth;
		}
		const double & getStdDevWaterDepth() const {
			return stdDevWaterDepth;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				waterOpticalPropertyId = node.get("waterOpticalPropertyId").as<int>();
				waterThermalPropertyId = node.get("waterThermalPropertyId").as<int>();

				extinctionCoeffcient = node.get("extinctionCoeffcient").as<double>();
				numberComponentWaterVolume = node.get("numberComponentWaterVolume").as<double>();

				waterHeight = node.get("waterHeight").as<double>();
				waterDepth = node.get("waterDepth").as<double>();
				stdDevWaterDepth = node.get("stdDevWaterDepth").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_WATERPLOTTYPE_HPP_ */
