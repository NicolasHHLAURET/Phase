/*
 * ITreesPositions.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_ITREESPOSITIONS_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_ITREESPOSITIONS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ITreesPositions {
	public:
		virtual ~ITreesPositions() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_ITREESPOSITIONS_HPP_ */
