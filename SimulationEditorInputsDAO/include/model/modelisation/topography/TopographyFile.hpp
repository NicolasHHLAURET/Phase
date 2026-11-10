/*
 * Topography.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYFILE_HPP_
#define INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYFILE_HPP_

#include "yaml-cpp/yaml.h"
#include "ITopography.hpp"

namespace DartInputs {
	class TopographyFile : public ITopography  {
	private:
		std::string sourceFile;

	public:
		TopographyFile() {}

		const std::string & getSourceFile() const {
			return sourceFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("sourceFile").IsNull()) {
					sourceFile = node.get("sourceFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_TOPOGRAPHY_TOPOGRAPHYFILE_HPP_ */
