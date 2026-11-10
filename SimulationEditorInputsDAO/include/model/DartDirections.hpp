/*
 * DartDirections.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTDIRECTIONS_HPP_
#define INCLUDE_MODEL_DARTDIRECTIONS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "direction/sunDirection/SunDirection.hpp"
#include "direction/viewDirections/IAddedViewDirections.hpp"
#include "direction/viewDirections/SingleDirection.hpp"
#include "direction/viewDirections/Region.hpp"
#include "direction/viewDirections/OversampledPlane.hpp"
#include "direction/viewDirections/hotspot/HotspotDirections.hpp"
#include <vector>
#include "../yaml/YaMLReader.hpp"
namespace DartInputs {
	class DartDirections {
	private:
		shared_ptr<SunDirection> sunDirection;
		std::vector<shared_ptr<IAddedViewDirections> > addedViewDirections;
		shared_ptr<HotspotDirections> hotspotDirections;

		int numberOfAngularSector;
		int numberOfDirectionsLayer;
		int nbDirections;

		double sunAzimuthalOffset;
		double directionalAzimuthalOffset;

	public:
		const int & getNbDirections() const {
			return nbDirections;
		}
		const shared_ptr<SunDirection> & getSunDirection() const {
			return sunDirection;
		}
		const std::vector<shared_ptr<IAddedViewDirections> > & getAddedViewDirections() const {
			return addedViewDirections;
		}
		const shared_ptr<HotspotDirections> & getHotspotDirections() const {
			return hotspotDirections;
		}
		const int & getNumberOfAngularSector() const {
			return numberOfAngularSector;
		}
		const int & getNumberOfDirectionsLayer() const {
			return numberOfDirectionsLayer;
		}
		const double & getSunAzimuthalOffset() const
		{
			return sunAzimuthalOffset;
		}
		const double & getDirectionalAzimuthalOffset() const
		{
			return directionalAzimuthalOffset;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			nbDirections = yamlReader.getFirstNode("nbDirections").as<int>();
			numberOfAngularSector = yamlReader.getFirstNode("numberOfAngularSector").as<int>();
			numberOfDirectionsLayer = yamlReader.getFirstNode("numberOfDirectionsLayer").as<int>();

			sunAzimuthalOffset = yamlReader.getFirstNode("sunAzimuthalOffset").as<double>();
			directionalAzimuthalOffset = yamlReader.getFirstNode("directionalAzimuthalOffset").as<double>();

			if (!yamlReader.getFirstNode("sunDirection").IsNull()) {
				sunDirection = shared_ptr<SunDirection>(new SunDirection());
				(*sunDirection) = yamlReader.getFirstNode("sunDirection");
			}
			if (!yamlReader.getFirstNode("hotspotDirections").IsNull()) {
				hotspotDirections = shared_ptr<HotspotDirections>(new HotspotDirections());
				(*hotspotDirections) = yamlReader.getFirstNode("hotspotDirections");
			}

			const YAML::Node & node = yamlReader.getFirstNode("addedViewDirections");
			if (!node.IsNull() && node.IsSequence()) {
				for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
					shared_ptr<IAddedViewDirections> addedViewDirection;
					if (it->Tag() == "!direction.viewDirections.SingleDirection") {
						addedViewDirection = shared_ptr<IAddedViewDirections>(new SingleDirection());
						static_cast<SingleDirection &>(*addedViewDirection) = *it;
					} else if (it->Tag() == "!direction.viewDirections.Region") {
						addedViewDirection = shared_ptr<IAddedViewDirections>(new Region());
						static_cast<Region &>(*addedViewDirection) = *it;
					} else if (it->Tag() == "!direction.viewDirections.OversampledPlane") {
						addedViewDirection = shared_ptr<IAddedViewDirections>(new OversampledPlane());
						static_cast<OversampledPlane &>(*addedViewDirection) = *it;
					} else {
						break;
					}
					addedViewDirections.push_back(addedViewDirection);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTDIRECTIONS_HPP_ */
