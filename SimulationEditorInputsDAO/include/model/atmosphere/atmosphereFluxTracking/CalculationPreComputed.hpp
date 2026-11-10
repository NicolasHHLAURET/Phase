/*
 * CalculationPreComputed.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONPRECOMPUTED_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONPRECOMPUTED_HPP_

#include "ICalculationMode.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class CalculationPreComputed: public ICalculationMode {
	private:
		std::string preComputedFile;

	public:
		CalculationPreComputed() {
			preComputedFile = "atmosphereTranfertFunctions.txt";
		}

		const std::string & getPreComputedFile() const {
			return preComputedFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("preComputedFile").IsNull()) {
					preComputedFile = node.get("preComputedFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONPRECOMPUTED_HPP_ */
