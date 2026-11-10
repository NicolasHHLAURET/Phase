/*
 * GridOnTotalScene.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONTOTALSCENE_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONTOTALSCENE_HPP_

#include "yaml-cpp/yaml.h"
#include "IRandomSpatialDistribution.hpp"
namespace DartInputs {
	class GridOnTotalScene: public IRandomSpatialDistribution {
	private:
		// No parameters
	public:
		GridOnTotalScene() {}
		void operator =(const YAML::Node& node) {}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONTOTALSCENE_HPP_ */
