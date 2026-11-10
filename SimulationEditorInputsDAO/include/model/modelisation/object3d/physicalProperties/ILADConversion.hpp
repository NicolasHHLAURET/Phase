/*
 * ILADConversion.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_ILADCONVERSION_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_ILADCONVERSION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ILADConversion {
	public:
		virtual ~ILADConversion() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_ILADCONVERSION_HPP_ */
