/*
 * PlotCloudLeafNumberOfTriangles.hpp
 *
 *  Created on: 8 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFNUMBEROFTRIANGLES_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFNUMBEROFTRIANGLES_HPP_

#include "IPlotCloudLeafDefinition.hpp"
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class PlotCloudLeafNumberOfTriangles: public IPlotCloudLeafDefinition {
	private:
		int numberOfTriangles;
	public:
		PlotCloudLeafNumberOfTriangles() :
				numberOfTriangles(10000) {}

		const int & getNumberOfTriangles() const {
			return numberOfTriangles;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfTriangles = node.get("numberOfTriangles").as<int>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTCLOUDLEAFNUMBEROFTRIANGLES_HPP_ */
