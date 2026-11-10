/*
 * ISunPenumbra.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ISUNPENUMBRA_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ISUNPENUMBRA_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ISunPenumbra {
	public:
		virtual ~ISunPenumbra() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ISUNPENUMBRA_HPP_ */
