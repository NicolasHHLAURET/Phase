/*
 * LeavesAndHolesUniform.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLESUNIFORM_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLESUNIFORM_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownLeafDistribution.hpp"
namespace DartInputs {
	class LeavesAndHolesUniform: public ICrownLeafDistribution {
	private:
		// No parameters
	public:
		LeavesAndHolesUniform() {}
		void operator =(const YAML::Node& node) {}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLESUNIFORM_HPP_ */
