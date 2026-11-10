/*
 * TriDimensionalMultiplicativeFactor.hpp
 *
 *  Created on: 23 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_TRIDIMENSIONALMULTIPLICATIVEFACTOR_TRIDIMENSIONALMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_TRIDIMENSIONALMULTIPLICATIVEFACTOR_TRIDIMENSIONALMULTIPLICATIVEFACTOR_HPP_

#include "yaml-cpp/yaml.h"

class TriDimensionalMultiplicativeFactor {
private:
	std::string triDimensionalmultiplicativeFactorFile;
	bool isDuplicateFirstVoxelLayer;
public:
	TriDimensionalMultiplicativeFactor() : isDuplicateFirstVoxelLayer(false) {}

	const std::string & getTriDimensionalmultiplicativeFactorFile() const {
		return triDimensionalmultiplicativeFactorFile;
	}
	const bool & getIsDuplicateFirstVoxelLayer() const {
		return isDuplicateFirstVoxelLayer;
	}

	void operator =(const YAML::Node& node) {
		if (!node.IsNull()) {
			isDuplicateFirstVoxelLayer = node.get("isDuplicateFirstVoxelLayer").as<bool>();
			if (!node.get("triDimensionalmultiplicativeFactorFile").IsNull()) {
				triDimensionalmultiplicativeFactorFile = node.get("triDimensionalmultiplicativeFactorFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	}
};

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_TRIDIMENSIONALMULTIPLICATIVEFACTOR_TRIDIMENSIONALMULTIPLICATIVEFACTOR_HPP_ */
