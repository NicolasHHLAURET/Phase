/*
 * BoundedUniform.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_BOUNDEDUNIFORM_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_BOUNDEDUNIFORM_HPP_

#include "yaml-cpp/yaml.h"
#include "ILeafAngleDistribution.hpp"
namespace DartInputs {
	class BoundedUniform: public ILeafAngleDistribution {
	private:
		int numberOfALA;
		int indexOfALA;

	public:
		BoundedUniform() :
				numberOfALA(1), indexOfALA(1) {}

		const int & getNumberOfALA() const {
			return numberOfALA;
		}
		const int & getIndexOfALA() const {
			return indexOfALA;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfALA = node.get("numberOfALA").as<int>();
				indexOfALA = node.get("indexOfALA").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_LEAFANGLEDISTRIBUTION_BOUNDEDUNIFORM_HPP_ */
