/*
 * HotspotRegion.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTREGION_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTREGION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class HotspotRegion {
	private:
		int numberDirectionInRegion;
		double regionSolidAngle;

	public:
		HotspotRegion() :
				numberDirectionInRegion(25), regionSolidAngle(0.01) {}

		const int & getNumberDirectionInRegion() const {
			return numberDirectionInRegion;
		}
		const double & getRegionSolidAngle() const {
			return regionSolidAngle;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberDirectionInRegion = node.get("numberDirectionInRegion").as<int>();
				regionSolidAngle = node.get("regionSolidAngle").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTREGION_HPP_ */
