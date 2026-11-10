/*
 * PlotCloudLeafDimensions.hpp
 *
 *  Created on: 8 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFDIMENSIONS_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFDIMENSIONS_HPP_

#include "IPlotCloudLeafDefinition.hpp"
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class PlotCloudLeafDimensions: public IPlotCloudLeafDefinition {
	private:
		double individualLeafArea;
	public:
		PlotCloudLeafDimensions() :
				individualLeafArea(0.3) {}

		const double & getIndividualLeafArea() const {
			return individualLeafArea;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				individualLeafArea = node.get("individualLeafArea").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFDIMENSIONS_HPP_ */
