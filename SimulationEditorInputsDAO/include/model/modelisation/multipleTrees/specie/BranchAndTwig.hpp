/*
 * BranchAndTwig.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_BRANCHANDTWIG_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_BRANCHANDTWIG_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class BranchAndTwig {
	private:
		double tai;
		bool isTwigBranchMixing;
		std::string geometryFileName;
		int branchOpticalPropertyId;
		int branchThermalPropertyId;
		int twigVegetationPropertyId;
		int twigThermalPropertyId;
	public:
		BranchAndTwig() :
				tai(1), isTwigBranchMixing(false), branchOpticalPropertyId(1), branchThermalPropertyId(1), twigVegetationPropertyId(1), twigThermalPropertyId(
					1) {}

		const double & getTai() const {
			return tai;
		}
		const bool & getIsTwigBranchMixing() const {
			return isTwigBranchMixing;
		}
		const std::string & getGeometryFileName() const {
			return geometryFileName;
		}
		const int & getBranchOpticalPropertyId() const {
			return branchOpticalPropertyId;
		}
		const int & getBranchThermalPropertyId() const {
			return branchThermalPropertyId;
		}
		const int & getTwigVegetationPropertyId() const {
			return twigVegetationPropertyId;
		}
		const int & getTwigThermalPropertyId() const {
			return twigThermalPropertyId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				tai = node.get("tai").as<double>();
				isTwigBranchMixing = node.get("isTwigBranchMixing").as<bool>();
				if (!node.get("geometryFileName").IsNull()) {
					geometryFileName = node.get("geometryFileName").get("dartFilePath").get("relativePath").as<std::string>();
				}
				branchOpticalPropertyId = node.get("branchOpticalPropertyId").as<int>();
				branchThermalPropertyId = node.get("branchThermalPropertyId").as<int>();
				twigVegetationPropertyId = node.get("twigVegetationPropertyId").as<int>();
				twigThermalPropertyId = node.get("twigThermalPropertyId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_BRANCHANDTWIG_HPP_ */
