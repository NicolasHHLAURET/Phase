/*
 * NoSunPenumbra.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_NOSUNPENUMBRA_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_NOSUNPENUMBRA_HPP_

#include "ISunPenumbra.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class NoSunPenumbra: public ISunPenumbra {
	public:
		NoSunPenumbra() {}
		void operator =(const YAML::Node& node) {}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_NOSUNPENUMBRA_HPP_ */
