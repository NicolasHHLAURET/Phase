/*
 * OpticalGeneralProperties.hpp
 *
 *  Created on: 24 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALGENERALPROPERTIES_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALGENERALPROPERTIES_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "opticalProperty/prospect/FluorescenceProperties.hpp"
namespace DartInputs {
	class OpticalGeneralProperties {
	private:
		double integrationStepOnTetha;
		double integrationStepOnPhi;
		bool outputLadFile;
		shared_ptr<FluorescenceProperties> fluorescenceProperties;

	public:
		OpticalGeneralProperties() :
				integrationStepOnTetha(1), integrationStepOnPhi(10), outputLadFile(false) {}

		const double & getIntegrationStepOnTetha() const {
			return integrationStepOnTetha;
		}
		const double & getIntegrationStepOnPhi() const {
			return integrationStepOnPhi;
		}
		const bool & getOutputLadFile() const {
			return outputLadFile;
		}
		const shared_ptr<FluorescenceProperties> & getFluorescenceProperties() const {
			return fluorescenceProperties;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				integrationStepOnTetha = node.get("integrationStepOnTetha").as<double>();
				integrationStepOnPhi = node.get("integrationStepOnPhi").as<double>();
				outputLadFile = node.get("outputLadFile").as<bool>();

				if (!node.get("fluorescenceProperties").IsNull()) {
					fluorescenceProperties = shared_ptr<FluorescenceProperties>(new FluorescenceProperties());
					(*fluorescenceProperties) = node.get("fluorescenceProperties");
				}
			}
		}
	};
}



#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALGENERALPROPERTIES_HPP_ */
