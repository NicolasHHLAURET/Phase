/*
 * HotspotDirections.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONS_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "HotspotRegion.hpp"
#include "HotspotDirectionsSet.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class HotspotDirections {
	private:
		shared_ptr<HotspotRegion> upwardSunRegion;
		shared_ptr<HotspotRegion> downwardSunRegion;

		shared_ptr<HotspotDirectionsSet> sunPlaneDirections;
		shared_ptr<HotspotDirectionsSet> perpendicularSunPlaneDirections;

	public:
		HotspotDirections() {}

		const shared_ptr<HotspotRegion> & getUpwardSunRegion() const {
			return upwardSunRegion;
		}
		const shared_ptr<HotspotRegion> & getDownwardSunRegion() const {
			return downwardSunRegion;
		}

		const shared_ptr<HotspotDirectionsSet> & getSunPlaneDirections() const {
			return sunPlaneDirections;
		}
		const shared_ptr<HotspotDirectionsSet> & getPerpendicularSunPlaneDirections() const {
			return perpendicularSunPlaneDirections;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("upwardSunRegion").IsNull()) {
					upwardSunRegion = shared_ptr<HotspotRegion>(new HotspotRegion());
					(*upwardSunRegion) = node.get("upwardSunRegion");
				}
				if (!node.get("downwardSunRegion").IsNull()) {
					downwardSunRegion = shared_ptr<HotspotRegion>(new HotspotRegion());
					(*downwardSunRegion) = node.get("downwardSunRegion");
				}

				if (!node.get("sunPlaneDirections").IsNull()) {
					sunPlaneDirections = shared_ptr<HotspotDirectionsSet>(new HotspotDirectionsSet());
					(*sunPlaneDirections) = node.get("sunPlaneDirections");
				}
				if (!node.get("perpendicularSunPlaneDirections").IsNull()) {
					perpendicularSunPlaneDirections = shared_ptr<HotspotDirectionsSet>(new HotspotDirectionsSet());
					(*perpendicularSunPlaneDirections) = node.get("perpendicularSunPlaneDirections");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_HOTSPOT_HOTSPOTDIRECTIONS_HPP_ */
