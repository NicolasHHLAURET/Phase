/*
 * ITopography.hpp
 *
 *  Created on: 7 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_ITOPOGRAPHY_HPP_
#define INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_ITOPOGRAPHY_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ITopography {
	public:
		virtual ~ITopography() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_ITOPOGRAPHY_HPP_ */
