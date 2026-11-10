/*
 * FluorescenceFunction.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEFUNCTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEFUNCTION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluorescenceFunction {
	private:
		double photoSystemI;
		double photoSystemII;
		bool forceYields;
		bool bioClimaticWeighting;
		std::string bioClimaticFile;
		bool opticalPropertyGrouping;
		int groupingOpticalPropertyId;
	public:
		FluorescenceFunction() :
				photoSystemI(0.002), photoSystemII(0.01), forceYields(false), bioClimaticWeighting(false), opticalPropertyGrouping(false), groupingOpticalPropertyId(
					1) {}

		const double & getPhotoSystemI() const {
			return photoSystemI;
		}
		const double & getPhotoSystemII() const {
			return photoSystemII;
		}
		const bool & getForceYields() const {
			return forceYields;
		}
		const bool & getBioClimaticWeighting() const {
			return bioClimaticWeighting;
		}
		const std::string & getBioClimaticFile() const {
			return bioClimaticFile;
		}
		const bool & getOpticalPropertyGrouping() const {
			return opticalPropertyGrouping;
		}
		const int & getGroupingOpticalPropertyId() const {
			return groupingOpticalPropertyId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				photoSystemI = node.get("photoSystemI").as<double>();
				photoSystemII = node.get("photoSystemII").as<double>();
				forceYields = node.get("forceYields").as<bool>();
				bioClimaticWeighting = node.get("bioClimaticWeighting").as<bool>();
				if (!node.get("bioClimaticFile").IsNull()) {
					bioClimaticFile = node.get("bioClimaticFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
				opticalPropertyGrouping = node.get("opticalPropertyGrouping").as<bool>();
				groupingOpticalPropertyId = node.get("groupingOpticalPropertyId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_FLUORESCENCEFUNCTION_HPP_ */
