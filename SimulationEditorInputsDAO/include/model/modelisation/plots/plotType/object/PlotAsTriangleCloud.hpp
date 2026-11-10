/*
 * PlotAsTriangleCloud.hpp
 *
 *  Created on: 8 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTASTRIANGLECLOUD_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTASTRIANGLECLOUD_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPlotCloudLeafDefinition.hpp"
#include "PlotCloudLeafDimensions.hpp"
#include "PlotCloudLeafNumberOfTriangles.hpp"

namespace DartInputs {
	class PlotAsTriangleCloud {
	public:
		enum PLOT_CLOUD_DISTRIBUTION {
			PURE_RANDOM, HOMOGENEOUS_GRID
		};

	private:
		PLOT_CLOUD_DISTRIBUTION leavesDistribution;
		shared_ptr<IPlotCloudLeafDefinition> leavesDefinition;

	public:
		PlotAsTriangleCloud() :
				leavesDistribution(PURE_RANDOM) {}

		const PLOT_CLOUD_DISTRIBUTION & getLeavesDistribution() const {
			return leavesDistribution;
		}
		const shared_ptr<IPlotCloudLeafDefinition> & getLeavesDefinition() const {
			return leavesDefinition;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("leavesDistribution").as<std::string>() == "PURE_RANDOM") {
					leavesDistribution = PURE_RANDOM;
				} else {
					leavesDistribution = HOMOGENEOUS_GRID;
				}

				if (!node.get("leavesDefinition").IsNull()) {
					if (node.get("leavesDefinition").Tag() == "!modelisation.plots.plotType.object.PlotCloudLeafDimensions") {
						leavesDefinition = shared_ptr<IPlotCloudLeafDefinition>(new PlotCloudLeafDimensions());
						static_cast<PlotCloudLeafDimensions &>(*leavesDefinition) = node.get("leavesDefinition");
					} else if (node.get("leavesDefinition").Tag() == "!modelisation.plots.plotType.object.PlotCloudLeafNumberOfTriangles") {
						leavesDefinition = shared_ptr<IPlotCloudLeafDefinition>(new PlotCloudLeafNumberOfTriangles());
						static_cast<PlotCloudLeafNumberOfTriangles &>(*leavesDefinition) = node.get("leavesDefinition");
					}
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTTYPE_OBJECT_PLOTASTRIANGLECLOUD_HPP_ */
