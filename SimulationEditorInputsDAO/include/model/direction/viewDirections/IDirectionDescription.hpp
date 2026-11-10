/*
 * IDirectionDescription.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IDIRECTIONDESCRIPTION_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IDIRECTIONDESCRIPTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IDirectionDescription {
	public:
		virtual ~IDirectionDescription() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_IDIRECTIONDESCRIPTION_HPP_ */
