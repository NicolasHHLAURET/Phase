/*
 * FluorescenceProperties.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEPROPERTIES_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEPROPERTIES_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluorescenceProperties {
	private:
		bool fluorescenceProducts;
		bool yieldsPerTriangle;
		std::string yieldsPerTriangleFile;
	public:
		FluorescenceProperties() :
				fluorescenceProducts(false), yieldsPerTriangle(false) {}

		const bool & getFluorescenceProducts() const {
			return fluorescenceProducts;
		}
		const bool & getYieldsPerTriangle() const {
			return yieldsPerTriangle;
		}
		const std::string & getYieldsPerTriangleFile() const {
			return yieldsPerTriangleFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				fluorescenceProducts = node.get("fluorescenceProducts").as<bool>();
				yieldsPerTriangle = node.get("yieldsPerTriangle").as<bool>();
				if (!node.get("yieldsPerTriangleFile").IsNull()) {
					yieldsPerTriangleFile = node.get("yieldsPerTriangleFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEPROPERTIES_HPP_ */
