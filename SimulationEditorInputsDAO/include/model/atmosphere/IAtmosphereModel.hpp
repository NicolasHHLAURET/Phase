/*
 * IAtmosphereModel.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_IATMOSPHEREMODEL_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_IATMOSPHEREMODEL_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IAtmosphereModel {
	public:
		virtual ~IAtmosphereModel() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_IATMOSPHEREMODEL_HPP_ */
