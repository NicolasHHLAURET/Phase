/*
 * ExactPosition.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_EXACTPOSITION_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_EXACTPOSITION_HPP_

#include "yaml-cpp/yaml.h"
#include "ITreesPositions.hpp"
namespace DartInputs {
	class ExactPosition: public ITreesPositions {
	private:
	public:
		ExactPosition() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_EXACTPOSITION_HPP_ */
