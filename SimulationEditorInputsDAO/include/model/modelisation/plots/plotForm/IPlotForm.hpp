/*
 * IPlotForm.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_IPLOTFORM_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_IPLOTFORM_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IPlotForm {
	public:
		virtual ~IPlotForm() {}
		virtual void operator =(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_IPLOTFORM_HPP_ */
