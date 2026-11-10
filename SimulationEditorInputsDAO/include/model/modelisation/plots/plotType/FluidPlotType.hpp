/*
 * FluidPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_FLUIDPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_FLUIDPLOTTYPE_HPP_

#include "yaml-cpp/yaml.h"
#include "IPlotType.hpp"
namespace DartInputs {
	class FluidPlotType: public IPlotType {
	private:
		int fluidOpticalPropertyId;
		int fluidThermalPropertyId;

		double particleDensity;
		double numberOfGazParticle;

		double bottomHeight;
		double meanPlotHeight;
		double stdDevPlotHeight;

		bool verticalFillMode;
	public:
		FluidPlotType() :
				fluidOpticalPropertyId(1), fluidThermalPropertyId(1), particleDensity(0.5), numberOfGazParticle(1), bottomHeight(0), meanPlotHeight(
					1.0), stdDevPlotHeight(0), verticalFillMode(false)  {}

		const int & getFluidOpticalPropertyId() const {
			return fluidOpticalPropertyId;
		}
		const int & getFluidThermalPropertyId() const {
			return fluidThermalPropertyId;
		}

		const double & getParticleDensity() const {
			return particleDensity;
		}
		const double & getNumberOfGazParticle() const {
			return numberOfGazParticle;
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
				fluidOpticalPropertyId = node.get("fluidOpticalPropertyId").as<int>();
				fluidThermalPropertyId = node.get("fluidThermalPropertyId").as<int>();

				particleDensity = node.get("particleDensity").as<double>();
				numberOfGazParticle = node.get("numberOfGazParticle").as<double>();

				bottomHeight = node.get("bottomHeight").as<double>();
				meanPlotHeight = node.get("meanPlotHeight").as<double>();
				stdDevPlotHeight = node.get("stdDevPlotHeight").as<double>();

				verticalFillMode = node.get("verticalFillMode").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_FLUIDPLOTTYPE_HPP_ */
