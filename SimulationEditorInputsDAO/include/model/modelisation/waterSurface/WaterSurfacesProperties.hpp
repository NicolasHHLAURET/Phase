/*
 * WaterSurfacesProperties.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_WATERSURFACE_WATERSURFACESPROPERTIES_HPP_
#define INCLUDE_MODEL_MODELISATION_WATERSURFACE_WATERSURFACESPROPERTIES_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class WaterSurfacesProperties {
	private:
		bool priorityOnPlots;
		bool priorityOnTrees;

	public:
		WaterSurfacesProperties() :
				priorityOnPlots(false), priorityOnTrees(false) {}

		const bool & getPriorityOnPlots() const {
			return priorityOnPlots;
		}
		const bool & getPriorityOnTrees() const {
			return priorityOnTrees;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				priorityOnPlots = node.get("priorityOnPlots").as<bool>();
				priorityOnTrees = node.get("priorityOnTrees").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_WATERSURFACE_WATERSURFACESPROPERTIES_HPP_ */
