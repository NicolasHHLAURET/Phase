/*
 * OpticalFactorMatrix.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_OPTICALFACTORMATRIX_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_OPTICALFACTORMATRIX_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "DartSAParameters.h"

namespace phsdata {
class VoxelNumber {
public:
	unsigned int numberX = 1;
	unsigned int numberY = 1;
	unsigned int numberZ = 1;

	void loadFromParameters(const std::string& prefix) {
		numberX = DartSAParameters::get<unsigned int>(prefix + ".numberX");
		numberY = DartSAParameters::get<unsigned int>(prefix + ".numberY");
		numberZ = DartSAParameters::get<unsigned int>(prefix + ".numberZ");
	}
};

class CellDimensions {
public:
	double cellXY = 1.;
	double cellZ = 1.;
	bool roundToExact = false;

	void loadFromParameters(const std::string& prefix) {
		cellXY = DartSAParameters::get<double>(prefix + ".cellX");
		cellZ = DartSAParameters::get<double>(prefix + ".cellZ");
		roundToExact = DartSAParameters::get<bool>(prefix + ".roundToExact");
	}
};

class OpticalFactorMatrix {
public:
	std::string opticalFactorMatrixFile;
	bool useDifferentCellDimensions;
	VoxelNumber voxelNumber;
	boost::optional<CellDimensions> cellDimensions;  // Optional child

	void loadFromParameters(const std::string& prefix) {
		opticalFactorMatrixFile = DartSAParameters::get<std::string>(prefix + ".opticalFactorMatrixFile");
		useDifferentCellDimensions = DartSAParameters::get<bool>(prefix + ".useDifferentCellDimensions");

		voxelNumber.loadFromParameters(prefix + ".VoxelNumber");

		if (DartSAParameters::exists(prefix + ".CellDimensions") && useDifferentCellDimensions) {
			cellDimensions = CellDimensions();
			cellDimensions->loadFromParameters(prefix + ".CellDimensions");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_OPTICALFACTORMATRIX_HPP_ */
