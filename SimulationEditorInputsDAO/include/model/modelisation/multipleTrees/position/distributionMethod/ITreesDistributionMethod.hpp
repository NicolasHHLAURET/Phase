/*
 * ITreesDistributionMethod.hpp
 *
 *  Created on: 3 déc. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_ITREESDISTRIBUTIONMETHOD_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_ITREESDISTRIBUTIONMETHOD_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ITreesDistributionMethod {
	public:
		virtual ~ITreesDistributionMethod() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}


#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_ITREESDISTRIBUTIONMETHOD_HPP_ */
