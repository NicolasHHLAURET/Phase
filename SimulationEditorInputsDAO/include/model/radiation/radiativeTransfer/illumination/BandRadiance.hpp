/*
 * BandRadiance.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDRADIANCE_HPP_
#define INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDRADIANCE_HPP_

#include "yaml-cpp/yaml.h"
#include "IAtmosphericIlumination.hpp"
namespace DartInputs {
	class BandRadiance: public IAtmosphericIlumination {
	private:
		std::string radianceFile;

	public:
		BandRadiance() {}

		const std::string & getRadianceFile() const {
			return radianceFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				radianceFile = node.get("radianceFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDRADIANCE_HPP_ */
