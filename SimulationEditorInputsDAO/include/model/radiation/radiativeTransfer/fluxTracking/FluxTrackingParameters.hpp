/*
 * FluxTrackingParameters.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGPARAMETERS_HPP_
#define INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGPARAMETERS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "FluxTrackingAdvancedParameters.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluxTrackingParameters {
	private:
		bool gaussSiedel;
		int numberOfIterations;
		shared_ptr<FluxTrackingAdvancedParameters> advancedParameters;
	public:
		FluxTrackingParameters() :
				gaussSiedel(false), numberOfIterations(5) {}

		const bool & getGaussSiedel() const {
			return gaussSiedel;
		}

		const int & getNumberOfIterations() const {
			return numberOfIterations;
		}
		const shared_ptr<FluxTrackingAdvancedParameters> & getAdvancedParameters() const {
			return advancedParameters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("advancedParameters").IsNull()) {
					advancedParameters = shared_ptr<FluxTrackingAdvancedParameters>(new FluxTrackingAdvancedParameters());
					(*advancedParameters) = node.get("advancedParameters");
				}
				gaussSiedel = node.get("gaussSiedel").as<bool>();
				numberOfIterations = node.get("numberOfIterations").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGPARAMETERS_HPP_ */
