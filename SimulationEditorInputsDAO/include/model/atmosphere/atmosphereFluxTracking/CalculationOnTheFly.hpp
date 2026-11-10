/*
 * CalculationOnTheFly.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONONTHEFLY_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONONTHEFLY_HPP_

#include "ICalculationMode.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class CalculationOnTheFly: public ICalculationMode {
	private:
		bool writeTransferFunction;

	public:
		CalculationOnTheFly() :
				writeTransferFunction(false) {}

		const bool & getWriteTransferFunction() const {
			return writeTransferFunction;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				writeTransferFunction = node.get("writeTransferFunction").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_CALCULATIONONTHEFLY_HPP_ */
