/*
 * MultiplicativeFactor.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MULTIPLICATIVEFACTOR_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "OpticalFactorMatrix.hpp"

namespace phsdata {
class MultiplicativeFactor {
public:
	virtual ~MultiplicativeFactor() = default;

	// Shared attributes
	double diffuseTransmittanceFactor = 1.;
	double directTransmittanceFactor = 1.;
	bool useOpticalFactorMatrix = false;
	boost::optional<OpticalFactorMatrix> opticalFactorMatrix;  // Optional child

	// Virtual method to load from DartSAParameters
	virtual void loadFromParameters(const std::string& prefix) {
		if (DartSAParameters::exists(prefix + ".diffuseTransmittanceFactor"))
			diffuseTransmittanceFactor = DartSAParameters::get<double>(prefix + ".diffuseTransmittanceFactor");
		if (DartSAParameters::exists(prefix + ".directTransmittanceFactor"))
			directTransmittanceFactor = DartSAParameters::get<double>(prefix + ".directTransmittanceFactor");

		useOpticalFactorMatrix = DartSAParameters::get<bool>(prefix + ".useOpticalFactorMatrix");
		if (useOpticalFactorMatrix && DartSAParameters::exists(prefix + ".OpticalFactorMatrix")) {
			opticalFactorMatrix = OpticalFactorMatrix();
			opticalFactorMatrix->loadFromParameters(prefix + ".OpticalFactorMatrix");
		}
	}
};

class MultiplicativeFactorList {
public:
	// XML layout caveat (producer-side rework pending):
	// - useSameFactorForAllBands: purely a producer-side hint; the XML always
	//   contains one factor per band (the producer copies band 0's values when
	//   the flag is true), so consumers can ignore this flag and iterate `factors`
	//   one-to-one with the spectral bands.
	// - useSameOpticalFactorMatrixForAllBands: when true, the single shared matrix
	//   is stored in bandCommonOpticalFactors and factors[] carry no OFM; when
	//   false, each factor may carry its own optional opticalFactorMatrix.
	virtual ~MultiplicativeFactorList() = default;

	bool useSameFactorForAllBands = false;
	bool useSameOpticalFactorMatrixForAllBands = true;
	boost::optional<OpticalFactorMatrix> bandCommonOpticalFactors;
	std::vector<std::shared_ptr<MultiplicativeFactor> > factors;

	void loadFromParameters(const std::string& prefix) {
		useSameFactorForAllBands = DartSAParameters::get<bool>(prefix + ".useSameFactorForAllBands");
		useSameOpticalFactorMatrixForAllBands = DartSAParameters::get<bool>(prefix + ".useSameOpticalFactorMatrixForAllBands");

		if (useSameOpticalFactorMatrixForAllBands) {
			bandCommonOpticalFactors = OpticalFactorMatrix();
			bandCommonOpticalFactors->loadFromParameters(prefix + ".OpticalFactorMatrix");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_MULTIPLICATIVEFACTOR_HPP_ */
