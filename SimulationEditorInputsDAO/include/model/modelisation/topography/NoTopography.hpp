/*
 * NoTopography.hpp
 *
 *  Created on: 7 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_NOTOPOGRAPHY_HPP_
#define INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_NOTOPOGRAPHY_HPP_

#include "ITopography.hpp"
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class NoTopography: public ITopography {
	private:
	public:
		NoTopography() {}

		void operator =(const YAML::Node& node) {
		}
	};
}



#endif /* INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_NOTOPOGRAPHY_HPP_ */
