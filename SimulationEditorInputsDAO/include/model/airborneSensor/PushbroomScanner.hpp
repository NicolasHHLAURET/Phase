/*
 * PushbroomScanner.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMSCANNER_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMSCANNER_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ISensor.hpp"
#include "pushbroomDefinition/IPusbroomScannerDefinition.hpp"
#include "pushbroomDefinition/PushbroomScannerWithAngleDistributionPath.hpp"
#include "pushbroomDefinition/PushbroomScannerWithLinearPath.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class PushbroomScanner: public ISensor {
	private:
		shared_ptr<IPusbroomScannerDefinition> pushbroomScannerDefinition;

	public:
		PushbroomScanner() {}

		const shared_ptr<IPusbroomScannerDefinition> & getPushbroomScannerDefinition() const {
			return pushbroomScannerDefinition;
		}

		void operator=(const YAML::Node& node) {
			ISensor::operator =(node); // <-> super.method();
			if (!node.IsNull()) {
				if (!node.get("pushbroomScannerDefinition").IsNull()) {
					if (node.get("pushbroomScannerDefinition").Tag() == "!airborneSensor.pushbroomDefinition.PushbroomScannerWithAngleDistributionPath") {
						pushbroomScannerDefinition = shared_ptr<IPusbroomScannerDefinition>(new PushbroomScannerWithAngleDistributionPath());
						static_cast<PushbroomScannerWithAngleDistributionPath &>(*pushbroomScannerDefinition) = node.get("pushbroomScannerDefinition");
					} else if (node.get("pushbroomScannerDefinition").Tag() == "!airborneSensor.pushbroomDefinition.PushbroomScannerWithLinearPath") {
						pushbroomScannerDefinition = shared_ptr<IPusbroomScannerDefinition>(new PushbroomScannerWithLinearPath());
						static_cast<PushbroomScannerWithLinearPath &>(*pushbroomScannerDefinition) = node.get("pushbroomScannerDefinition");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMSCANNER_HPP_ */
