/*
 * LAICalculated.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAICALCULATED_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAICALCULATED_HPP_

#include "yaml-cpp/yaml.h"
#include "ILAIComputing.hpp"
namespace DartInputs {
	class LAICalculated: public ILAIComputing {
	private:
		// No parameters
	public:
		LAICalculated() {}

		void operator =(const YAML::Node& node) {}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAICALCULATED_HPP_ */
