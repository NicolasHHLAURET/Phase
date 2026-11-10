/*
 * HotspotDirectionsSet.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONSSET_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONSSET_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class HotspotDirectionsSet {
	private:
		double zenithStep;
		double solidAngle;

	public:
		HotspotDirectionsSet() :
				zenithStep(10), solidAngle(0.001) {}

		const double & getZenithStep() const {
			return zenithStep;
		}
		const double & getSolidAngle() const {
			return solidAngle;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				zenithStep = node.get("zenithStep").as<double>();
				solidAngle = node.get("solidAngle").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONSSET_HPP_ */
