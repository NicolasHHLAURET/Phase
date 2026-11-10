/*
 * IRandomSpatialDistribution.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_IRANDOMSPATIALDISTRIBUTION_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_IRANDOMSPATIALDISTRIBUTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IRandomSpatialDistribution {
	public:
		virtual ~IRandomSpatialDistribution() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_IRANDOMSPATIALDISTRIBUTION_HPP_ */
