/*
 * TopographyBuilder.hpp
 *
 *  Created on: 7 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYBUILDER_HPP_
#define INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYBUILDER_HPP_

#include "ITopography.hpp"
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class TopographyBuilder: public ITopography {
	private:
	public:
		TopographyBuilder() {}

		void operator =(const YAML::Node& node) {
//			std::cerr << ".get(WARNING) Topography builder is curently not read on YamlCpp"<<std::endl;
		}
	};
}



#endif /* INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYBUILDER_HPP_ */
