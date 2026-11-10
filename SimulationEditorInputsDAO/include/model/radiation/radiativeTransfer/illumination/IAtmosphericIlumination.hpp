/*
 * IAtmosphericIlumination.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_ILLUMINATION_IATMOSPHERICILUMINATION_HPP_
#define INCLUDE_MODEL_RADIATION_ILLUMINATION_IATMOSPHERICILUMINATION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IAtmosphericIlumination {
	public:
		virtual ~IAtmosphericIlumination() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_RADIATION_ILLUMINATION_IATMOSPHERICILUMINATION_HPP_ */
