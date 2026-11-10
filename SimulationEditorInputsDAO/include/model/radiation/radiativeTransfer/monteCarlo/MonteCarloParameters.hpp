/*
 * MonteCarloParameters.hpp
 *
 *  Created on: 21 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOPARAMETERS_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOPARAMETERS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"

#include "MonteCarloAdvancedParameters.hpp"
namespace DartInputs {
	class MonteCarloParameters {
	private:
		int maximumRAMofCalculator;
		int photonNumber;
		int orderOfScattering;
		shared_ptr<MonteCarloAdvancedParameters> advancedParameters;
	public:
		MonteCarloParameters() :
				maximumRAMofCalculator(5), photonNumber(5), orderOfScattering(5) {}

		const int & getMaximumRAMofCalculator() const {
			return maximumRAMofCalculator;
		}

		const int & getPhotonNumber() const {
			return photonNumber;
		}

		const int & getOrderOfScattering() const {
			return orderOfScattering;
		}

		const shared_ptr<MonteCarloAdvancedParameters> & getAdvancedParameters() const {
			return advancedParameters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("advancedParameters").IsNull()) {
					advancedParameters = shared_ptr<MonteCarloAdvancedParameters>(new MonteCarloAdvancedParameters());
					(*advancedParameters) = node.get("advancedParameters");
				}
				maximumRAMofCalculator = node.get("maximumRAMofCalculator").as<int>();
				photonNumber = node.get("photonNumber").as<int>();
				orderOfScattering = node.get("orderOfScattering").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOPARAMETERS_HPP_ */
