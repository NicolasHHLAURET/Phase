/*
 * IDiscretizationMode.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_IDISCRETIZATIONMODE_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_IDISCRETIZATIONMODE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IDiscretizationMode {
	public:
		virtual ~IDiscretizationMode() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_IDISCRETIZATIONMODE_HPP_ */
