/*
 * VegetationPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_VEGETATIONPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_VEGETATIONPLOTTYPE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPlotType.hpp"
#include "object/PlotAsTriangleCloud.hpp"

namespace DartInputs {
	class VegetationPlotType: public IPlotType {
	private:
		int vegetationPlotOpticalPropertyId;
		int vegetationPlotThermalPropertyId;

		double lai;

		double bottomHeight;
		double meanPlotHeight;
		double stdDevPlotHeight;

		bool verticalFillMode;

		shared_ptr<PlotAsTriangleCloud>	plotAsTriangleCloud;

	public:
		VegetationPlotType() :
				vegetationPlotOpticalPropertyId(1), vegetationPlotThermalPropertyId(1), lai(-0.3), bottomHeight(0), meanPlotHeight(1.0), stdDevPlotHeight(
					0), verticalFillMode(false) {}

		const shared_ptr<PlotAsTriangleCloud> & getPlotAsTriangleCloud() const
		{
			return plotAsTriangleCloud;
		}

		const int & getVegetationPlotOpticalPropertyId() const {
			return vegetationPlotOpticalPropertyId;
		}
		const int & getVegetationPlotThermalPropertyId() const {
			return vegetationPlotThermalPropertyId;
		}

		const double & getLai() const {
			return lai;
		}

		const double & getBottomHeight() const {
			return bottomHeight;
		}
		const double & getMeanPlotHeight() const {
			return meanPlotHeight;
		}
		const double & getStdDevPlotHeight() const {
			return stdDevPlotHeight;
		}

		const bool & getVerticalFillMode() const {
			return verticalFillMode;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				vegetationPlotOpticalPropertyId = node.get("vegetationPlotOpticalPropertyId").as<int>();
				vegetationPlotThermalPropertyId = node.get("vegetationPlotThermalPropertyId").as<int>();

				lai = node.get("lai").as<double>();

				bottomHeight = node.get("bottomHeight").as<double>();
				meanPlotHeight = node.get("meanPlotHeight").as<double>();
				stdDevPlotHeight = node.get("stdDevPlotHeight").as<double>();
				verticalFillMode = node.get("verticalFillMode").as<bool>();

				if (!node.get("plotAsTriangleCloud").IsNull()) {
					plotAsTriangleCloud = shared_ptr<PlotAsTriangleCloud>(new PlotAsTriangleCloud());
					static_cast<PlotAsTriangleCloud &>(*plotAsTriangleCloud) = node.get("plotAsTriangleCloud");

				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_VEGETATIONPLOTTYPE_HPP_ */
