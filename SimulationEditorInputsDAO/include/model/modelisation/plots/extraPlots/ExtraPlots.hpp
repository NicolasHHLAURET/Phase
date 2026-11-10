/*
 * ExtraPlots.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_EXTRAPLOTS_EXTRAPLOTS_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_EXTRAPLOTS_EXTRAPLOTS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ExtraPlots {
	private:
		std::string extraPlotsFile;
	public:
		ExtraPlots() {}

		const std::string & getExtraPlotsFile() const {
			return extraPlotsFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("extraPlotsFile").IsNull()) {
					extraPlotsFile = node.get("extraPlotsFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_EXTRAPLOTS_EXTRAPLOTS_HPP_ */
