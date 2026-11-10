/*
 * IObject3DNumber.hpp
 *
 *  Created on: 12 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_IOBJECT3DNUMBER_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_IOBJECT3DNUMBER_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IObject3DNumber {
	public:
		virtual ~IObject3DNumber() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_IOBJECT3DNUMBER_HPP_ */
