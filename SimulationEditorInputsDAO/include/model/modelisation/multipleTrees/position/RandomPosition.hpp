/*
 * RandomPosition.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ITreesPositions.hpp"

#include "randomPosition/IRandomSpatialDistribution.hpp"
#include "randomPosition/GridOnEllipticalSubZone.hpp"
#include "randomPosition/GridOnQuadrilateralSubZone.hpp"
#include "randomPosition/GridOnTotalScene.hpp"
#include "distributionMethod/ITreesDistributionMethod.hpp"
#include "distributionMethod/IntervalBetweenTreesDistributionMethod.hpp"
#include "distributionMethod/NumberOfTreesDistributionMethod.hpp"

namespace DartInputs {
	class RandomPosition: public ITreesPositions {
	private:
		shared_ptr<IRandomSpatialDistribution> randomSpatialDistribution;
		shared_ptr<ITreesDistributionMethod> treesDistributionMethod;
		double probabilityOfPresence;
		double radiusForTreePresence;
	public:
		RandomPosition() :
				probabilityOfPresence(0.9), radiusForTreePresence(5) {}

		const shared_ptr<IRandomSpatialDistribution> & getRandomSpatialDistribution() const {
			return randomSpatialDistribution;
		}
		const shared_ptr<ITreesDistributionMethod> & getTreesDistributionMethod() const {
			return treesDistributionMethod;
		}
		const double & getProbabilityOfPresence() const {
			return probabilityOfPresence;
		}
		const double & getRadiusForTreePresence() const {
			return radiusForTreePresence;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				probabilityOfPresence = node.get("probabilityOfPresence").as<double>();
				radiusForTreePresence = node.get("radiusForTreePresence").as<double>();

				if (!node.get("randomSpatialDistribution").IsNull()) {
					if (node.get("randomSpatialDistribution").Tag() == "!modelisation.multipleTrees.position.randomPosition.GridOnEllipticalSubZone") {
						randomSpatialDistribution = shared_ptr<IRandomSpatialDistribution>(new GridOnEllipticalSubZone());
						static_cast<GridOnEllipticalSubZone &>(*randomSpatialDistribution) = node.get("randomSpatialDistribution");
					} else if (node.get("randomSpatialDistribution").Tag()
							== "modelisation.multipleTrees.position.randomPosition.GridOnQuadrilateralSubZone") {
						randomSpatialDistribution = shared_ptr<IRandomSpatialDistribution>(new GridOnQuadrilateralSubZone());
						static_cast<GridOnQuadrilateralSubZone &>(*randomSpatialDistribution) = node.get("randomSpatialDistribution");
					} else if (node.get("randomSpatialDistribution").Tag() == "!modelisation.multipleTrees.position.randomPosition.GridOnTotalScene") {
						randomSpatialDistribution = shared_ptr<IRandomSpatialDistribution>(new GridOnTotalScene());
						static_cast<GridOnTotalScene &>(*randomSpatialDistribution) = node.get("randomSpatialDistribution");
					}
				}

				if (!node.get("treesDistributionMethod").IsNull()) {
					if (node.get("treesDistributionMethod").Tag() == "!modelisation.multipleTrees.position.distributionMethod.IntervalBetweenTreesDistributionMethod") {
						treesDistributionMethod = shared_ptr<ITreesDistributionMethod>(new IntervalBetweenTreesDistributionMethod());
						static_cast<IntervalBetweenTreesDistributionMethod &>(*treesDistributionMethod) = node.get("treesDistributionMethod");
					} else if (node.get("treesDistributionMethod").Tag() == "!modelisation.multipleTrees.position.distributionMethod.NumberOfTreesDistributionMethod") {
						treesDistributionMethod = shared_ptr<ITreesDistributionMethod>(new NumberOfTreesDistributionMethod());
						static_cast<NumberOfTreesDistributionMethod &>(*treesDistributionMethod) = node.get("treesDistributionMethod");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_HPP_ */
