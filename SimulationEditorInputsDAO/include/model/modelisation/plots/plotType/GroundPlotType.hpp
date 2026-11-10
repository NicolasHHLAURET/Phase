/*
 * GroundPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDPLOTTYPE_HPP_

#include "yaml-cpp/yaml.h"
#include "IPlotType.hpp"
namespace DartInputs {
	class GroundPlotType: public IPlotType {
	private:
		int groundPlotOpticalPropertyId;
		int groundPlotThermalPropertyId;

	public:
		GroundPlotType() :
				groundPlotOpticalPropertyId(1), groundPlotThermalPropertyId(1) {}

		const int & getGroundPlotOpticalPropertyId() const {
			return groundPlotOpticalPropertyId;
		}
		const int & getGroundPlotThermalPropertyId() const {
			return groundPlotThermalPropertyId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				groundPlotOpticalPropertyId = node.get("groundPlotOpticalPropertyId").as<int>();
				groundPlotThermalPropertyId = node.get("groundPlotThermalPropertyId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDPLOTTYPE_HPP_ */
