/*
 * IHouseRoof.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_IHOUSEROOF_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_IHOUSEROOF_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IHouseRoof {
	public:
		virtual ~IHouseRoof() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_IHOUSEROOF_HPP_ */
