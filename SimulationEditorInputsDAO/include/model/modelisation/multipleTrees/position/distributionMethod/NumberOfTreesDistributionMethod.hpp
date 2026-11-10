/*
 * NumberOfTreesDistributionMethod.hpp
 *
 *  Created on: 3 déc. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_NUMBEROFTREESDISTRIBUTIONMETHOD_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_NUMBEROFTREESDISTRIBUTIONMETHOD_HPP_

#include "yaml-cpp/yaml.h"
#include "ITreesDistributionMethod.hpp"

namespace DartInputs {
	class NumberOfTreesDistributionMethod: public ITreesDistributionMethod {
	private:
		int numberOfTrees;

	public:
		NumberOfTreesDistributionMethod() :
				numberOfTrees(12) {}

		const int & getNumberOfTrees() const {
			return numberOfTrees;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfTrees = node.get("numberOfTrees").as<int>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_NUMBEROFTREESDISTRIBUTIONMETHOD_HPP_ */
