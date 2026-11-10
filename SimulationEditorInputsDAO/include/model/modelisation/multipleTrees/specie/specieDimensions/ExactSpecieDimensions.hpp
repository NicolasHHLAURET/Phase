/*
 * ExactSpecieDimensions.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_EXACTSPECIEDIMENSIONS_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_EXACTSPECIEDIMENSIONS_HPP_

#include "yaml-cpp/yaml.h"
#include "ISpecieDimensions.hpp"
namespace DartInputs {
	class ExactSpecieDimensions: public ISpecieDimensions {
	private:
	public:
		ExactSpecieDimensions() {}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_EXACTSPECIEDIMENSIONS_HPP_ */
