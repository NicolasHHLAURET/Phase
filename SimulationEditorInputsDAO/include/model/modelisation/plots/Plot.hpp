/*
 * Plot.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOT_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOT_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"

#include "../IDartObject.hpp"

#include "plotType/IPlotType.hpp"
#include "plotType/FluidPlotType.hpp"
#include "plotType/GroundAndVegetationPlotType.hpp"
#include "plotType/GroundPlotType.hpp"
#include "plotType/VegetationPlotType.hpp"
#include "plotType/WaterPlotType.hpp"

#include "plotForm/IPlotForm.hpp"
#include "plotForm/Plot4Corners.hpp"
#include "plotForm/PlotRectangle.hpp"
namespace DartInputs {
	class Plot: public IDartObject {
	private:
		shared_ptr<IPlotForm> plotForm;
		shared_ptr<IPlotType> plotType;
	public:
		Plot() {}

		const shared_ptr<IPlotForm> & getPlotForm() const {
			return plotForm;
		}

		const shared_ptr<IPlotType> getPlotType() const {
			return plotType;
		}

		void operator =(const YAML::Node& node) {
			IDartObject::operator =(node);
			if (!node.IsNull()) {
				if (!node.get("plotForm").IsNull()) {
					if (node.get("plotForm").Tag() == "!modelisation.plots.plotForm.Plot4Corners") {
						plotForm = shared_ptr<IPlotForm>(new Plot4Corners());
						static_cast<Plot4Corners &>(*plotForm) = node.get("plotForm");
					} else if (node.get("plotForm").Tag() == "!modelisation.plots.plotForm.PlotRectangle") {
						plotForm = shared_ptr<IPlotForm>(new PlotRectangle());
						static_cast<PlotRectangle &>(*plotForm) = node.get("plotForm");
					}
				}

				if (!node.get("plotType").IsNull()) {
					if (node.get("plotType").Tag() == "!modelisation.plots.plotForm.FluidPlotType") {
						plotType = shared_ptr<IPlotType>(new FluidPlotType());
						static_cast<FluidPlotType &>(*plotType) = node.get("plotType");
					} else if (node.get("plotType").Tag() == "!modelisation.plots.plotForm.GroundAndVegetationPlotType") {
						plotType = shared_ptr<IPlotType>(new GroundAndVegetationPlotType());
						static_cast<GroundAndVegetationPlotType &>(*plotType) = node.get("plotType");
					} else if (node.get("plotType").Tag() == "!modelisation.plots.plotForm.GroundPlotType") {
						plotType = shared_ptr<IPlotType>(new GroundPlotType());
						static_cast<GroundPlotType &>(*plotType) = node.get("plotType");
					} else if (node.get("plotType").Tag() == "!modelisation.plots.plotForm.VegetationPlotType") {
						plotType = shared_ptr<IPlotType>(new VegetationPlotType());
						static_cast<VegetationPlotType &>(*plotType) = node.get("plotType");
					} else if (node.get("plotType").Tag() == "!modelisation.plots.plotForm.WaterPlotType") {
						plotType = shared_ptr<IPlotType>(new WaterPlotType());
						static_cast<WaterPlotType &>(*plotType) = node.get("plotType");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOT_HPP_ */
