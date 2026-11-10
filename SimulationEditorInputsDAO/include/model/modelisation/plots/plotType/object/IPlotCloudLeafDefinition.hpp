/*
 * IPlotCloudLeafDefinition.hpp
 *
 *  Created on: 8 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_IPLOTCLOUDLEAFDEFINITION_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_IPLOTCLOUDLEAFDEFINITION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPlotCloudLeafDefinition {
	public:
		virtual ~IPlotCloudLeafDefinition() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_IPLOTCLOUDLEAFDEFINITION_HPP_ */
