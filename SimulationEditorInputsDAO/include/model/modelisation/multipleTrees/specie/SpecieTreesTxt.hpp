/*
 * SpecieTreesTxt.hpp
 *
 *  Created on: 2 déc. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIETREESTXT_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIETREESTXT_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SpecieTreesTxt {
	private:
		double specieId;
		std::string treesTxt;

	public:
		SpecieTreesTxt() : specieId(0) {}

		const double & getSpecieId() const {
			return specieId;
		}
		const std::string & getTreesTxt() const {
			return treesTxt;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				specieId = node.get("specieId").as<double>();
				if (!node.get("treesTxt").IsNull()) {
					treesTxt = node.get("treesTxt").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}



#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIETREESTXT_HPP_ */
