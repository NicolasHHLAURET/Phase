/*
 * IcospherePenumbra.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ICOSPHEREPENUMBRA_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ICOSPHEREPENUMBRA_HPP_

#include "ISunPenumbra.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IcospherePenumbra: public ISunPenumbra {
	private:
		int numberOfSubdivisions;

	public:
		IcospherePenumbra() :
				numberOfSubdivisions(0) {}

		const int & getNumberOfSubdivisions() const {
			return numberOfSubdivisions;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfSubdivisions = node.get("numberOfSubdivisions").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNPENUMBRA_ICOSPHEREPENUMBRA_HPP_ */
