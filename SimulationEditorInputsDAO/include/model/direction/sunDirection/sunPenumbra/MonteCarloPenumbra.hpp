/*
 * MonteCarloPenumbra.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_MONTECARLOPENUMBRA_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_MONTECARLOPENUMBRA_HPP_

#include "ISunPenumbra.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class MonteCarloPenumbra: public ISunPenumbra {
	private:
		int numberOfSamples;

	public:
		MonteCarloPenumbra() :
				numberOfSamples(0) {}

		const int & getNumberOfSamples() const {
			return numberOfSamples;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfSamples = node.get("numberOfSamples").as<int>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_MONTECARLOPENUMBRA_HPP_ */
