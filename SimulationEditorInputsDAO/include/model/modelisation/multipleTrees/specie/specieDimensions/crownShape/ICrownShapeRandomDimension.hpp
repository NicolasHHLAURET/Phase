/*
 * ICrownShapeRandomDimension.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_ICROWNSHAPERANDOMDIMENSION_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_ICROWNSHAPERANDOMDIMENSION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ICrownShapeRandomDimension {
	public:
		virtual ~ICrownShapeRandomDimension() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_ICROWNSHAPERANDOMDIMENSION_HPP_ */
