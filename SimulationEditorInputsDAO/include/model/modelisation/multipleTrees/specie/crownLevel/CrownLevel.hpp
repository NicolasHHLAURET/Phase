/*
 * CrownLevel.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEVEL_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEVEL_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "crownLeafDistribution/ICrownLeafDistribution.hpp"
#include "crownLeafDistribution/Holes.hpp"
#include "crownLeafDistribution/Leaves.hpp"
#include "crownLeafDistribution/LeavesAndHoles.hpp"
#include "crownLeafDistribution/LeavesAndHolesUniform.hpp"
namespace DartInputs {
	class CrownLevel {
	private:
		int trunkOpticalPropertyId;
		int trunkThermalPropertyId;
		int leafOpticalPropertyId;
		int leafThermalPropertyId;
		shared_ptr<ICrownLeafDistribution> leafDistribution;
		double relativeTrunkDiameter;
		double relativeHeightVsCrownHeight;
		double verticalWeightForUf;
		bool crownEnabled;
		std::string crownName;
	public:
		CrownLevel() :
				trunkOpticalPropertyId(1), trunkThermalPropertyId(1), leafOpticalPropertyId(1), leafThermalPropertyId(1), relativeTrunkDiameter(0.5), relativeHeightVsCrownHeight(
					1), verticalWeightForUf(1), crownEnabled(true) {}

		const int & getTrunkOpticalPropertyId() const {
			return trunkOpticalPropertyId;
		}
		const int & getTrunkThermalPropertyId() const {
			return trunkThermalPropertyId;
		}
		const int & getLeafOpticalPropertyId() const {
			return leafOpticalPropertyId;
		}
		const int & getLeafThermalPropertyId() const {
			return leafThermalPropertyId;
		}
		const shared_ptr<ICrownLeafDistribution> & getLeafDistribution() const {
			return leafDistribution;
		}
		const double & getRelativeTrunkDiameter() const {
			return relativeTrunkDiameter;
		}
		const double & getRelativeHeightVsCrownHeight() const {
			return relativeHeightVsCrownHeight;
		}
		const double & getVerticalWeightForUf() const {
			return verticalWeightForUf;
		}
		const bool & getCrownEnabled() const {
			return crownEnabled;
		}
		const std::string & getCrownName() const {
			return crownName;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				trunkOpticalPropertyId = node.get("trunkOpticalPropertyId").as<int>();
				trunkThermalPropertyId = node.get("trunkThermalPropertyId").as<int>();
				leafOpticalPropertyId = node.get("leafOpticalPropertyId").as<int>();
				leafThermalPropertyId = node.get("leafThermalPropertyId").as<int>();
				relativeTrunkDiameter = node.get("relativeTrunkDiameter").as<double>();
				relativeHeightVsCrownHeight = node.get("relativeHeightVsCrownHeight").as<double>();
				verticalWeightForUf = node.get("verticalWeightForUf").as<double>();
				crownEnabled = node.get("crownEnabled").as<bool>();
				crownName = node.get("crownName").as<std::string>();

				if (!node.get("leafDistribution").IsNull()) {
					if (node.get("leafDistribution").Tag() == "!modelisation.multipleTrees.specie.crownLevel.crownLeafDistribution.Holes") {
						leafDistribution = shared_ptr<ICrownLeafDistribution>(new Holes());
						static_cast<Holes &>(*leafDistribution) = node.get("leafDistribution");
					} else if (node.get("leafDistribution").Tag() == "!modelisation.multipleTrees.specie.crownLevel.crownLeafDistribution.Leaves") {
						leafDistribution = shared_ptr<ICrownLeafDistribution>(new Leaves());
						static_cast<Leaves &>(*leafDistribution) = node.get("leafDistribution");
					} else if (node.get("leafDistribution").Tag()
							== "modelisation.multipleTrees.specie.crownLevel.crownLeafDistribution.LeavesAndHoles") {
						leafDistribution = shared_ptr<ICrownLeafDistribution>(new LeavesAndHoles());
						static_cast<LeavesAndHoles &>(*leafDistribution) = node.get("leafDistribution");
					} else if (node.get("leafDistribution").Tag()
							== "modelisation.multipleTrees.specie.crownLevel.crownLeafDistribution.LeavesAndHolesUniform") {
						leafDistribution = shared_ptr<ICrownLeafDistribution>(new LeavesAndHolesUniform());
						static_cast<LeavesAndHolesUniform &>(*leafDistribution) = node.get("leafDistribution");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEVEL_HPP_ */
