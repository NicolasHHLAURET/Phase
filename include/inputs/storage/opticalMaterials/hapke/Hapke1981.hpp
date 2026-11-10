/*
 * HapkeSpecularMulti.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "Hapke1981MultiplicativeFactor.hpp"
#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"

namespace phsdata {
class Hapke1981: public SurfaceFunction {
public:
	std::string transmittanceDatabaseName;
	std::string transmittanceModelName;
	bool useExternalModule;
	boost::optional<Hapke1981MultiplicativeFactorList> nodeMultiplicativeFactor;

	virtual Type getType() override {
		return SurfaceFunction::Type::Hapke1981;
	}

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);
		transmittanceDatabaseName = DartSAParameters::get<std::string>(prefix + ".transmittanceDatabaseName");
		transmittanceModelName = DartSAParameters::get<std::string>(prefix + ".transmittanceModelName");
		useExternalModule = DartSAParameters::get<bool>(prefix + ".useExternalModule");

		if (DartSAParameters::exists(prefix + ".HapkeNodeMultiplicativeFactorForLUT")) {
			nodeMultiplicativeFactor = Hapke1981MultiplicativeFactorList();
			nodeMultiplicativeFactor->loadFromParameters(prefix + ".HapkeNodeMultiplicativeFactorForLUT");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_HAPKE_HAPKE1981_HPP_ */
