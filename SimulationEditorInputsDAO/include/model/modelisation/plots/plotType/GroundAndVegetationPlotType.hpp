/*
 * GroundAndVegetationPlotType.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDANDVEGETATIONPLOTTYPE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDANDVEGETATIONPLOTTYPE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPlotType.hpp"
#include "GroundPlotType.hpp"
#include "VegetationPlotType.hpp"
namespace DartInputs {
	class GroundAndVegetationPlotType: public IPlotType {
	private:
		shared_ptr<GroundPlotType> groundPlot;
		shared_ptr<VegetationPlotType> vegetationPlot;
	public:
		GroundAndVegetationPlotType() {}

		const shared_ptr<GroundPlotType> & getGroundPlot() const {
			return groundPlot;
		}
		const shared_ptr<VegetationPlotType> & getVegetationPlot() const {
			return vegetationPlot;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("groundPlot").IsNull()) {
					groundPlot = shared_ptr<GroundPlotType>(new GroundPlotType());
					(*groundPlot) = node.get("groundPlot");
				}
				if (!node.get("vegetationPlot").IsNull()) {
					vegetationPlot = shared_ptr<VegetationPlotType>(new VegetationPlotType());
					(*vegetationPlot) = node.get("vegetationPlot");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_GROUNDANDVEGETATIONPLOTTYPE_HPP_ */
