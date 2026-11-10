/*
 * PrecomputedIPARs.hpp
 *
 *  Created on: Apr 18, 2019
 *      Author: lauretn
 */

#ifndef INCLUDE_MODEL_PHASE_THERMALFUNCTION_PRECOMPUTEDIPARS_PRECOMPUTEDIPARS_HPP_
#define INCLUDE_MODEL_PHASE_THERMALFUNCTION_PRECOMPUTEDIPARS_PRECOMPUTEDIPARS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class PrecomputedIPARs {
	private:
		double minIPAR;
		double maxIPAR;
	public:
		PrecomputedIPARs() :
				minIPAR(0.), maxIPAR(1000.) {
		}

		const double & getMinIPAR() {
			return minIPAR;
		}

		const double & getMaxIPAR() {
			return maxIPAR;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				minIPAR = node.get("minIPAR").as<double>();
				maxIPAR = node.get("maxIPAR").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_THERMALFUNCTION_PRECOMPUTEDIPARS_PRECOMPUTEDIPARS_HPP_ */
