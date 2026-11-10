/*
 * FastDiscretization.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_FASTDISCRETIZATION_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_FASTDISCRETIZATION_HPP_

#include "IDiscretizationMode.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FastDiscretization: public IDiscretizationMode {
	private:
		// No parameters on FAST discretization
	public:
		FastDiscretization() {}
		void operator =(const YAML::Node& node) {}
	};
}

#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_FASTDISCRETIZATION_HPP_ */
