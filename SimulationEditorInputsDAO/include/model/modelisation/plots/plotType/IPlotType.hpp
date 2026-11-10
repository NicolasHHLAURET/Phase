/*
 * IPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_IPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_IPLOTTYPE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPlotType {
	public:
		virtual ~IPlotType() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_IPLOTTYPE_HPP_ */
