/*
 * lambertian.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIAN_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIAN_HPP_

#include <string>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "inputs/storage/opticalMaterials/SurfaceFunction.hpp"
#include "inputs/storage/opticalMaterials/ProspectExternalModule.hpp"
#include "inputs/storage/opticalMaterials/lambertian/LambertianMultiplicativeFactor.hpp"

namespace phsdata {
class Marmit {
public:
	std::string marmitDatabase;
	std::string marmitModel;
	double d = 0.005;
	double smc = 10;
	bool keepOutputFile = false;

	void loadFromParameters(const std::string& prefix) {
		marmitDatabase = DartSAParameters::get<std::string>(prefix + ".marmitDatabase");
		marmitModel = DartSAParameters::get<std::string>(prefix + ".marmitModel");
		keepOutputFile = DartSAParameters::get<bool>(prefix + ".keepOutputFile");
		smc = DartSAParameters::get<double>(prefix + ".smc");
		d = DartSAParameters::get<double>(prefix + ".d");
	}
};

class Lambertian {
public:
	std::string databaseName;
	std::string modelName;
	ProspectExternalModule prospectExternalModule;

	void loadFromParameters(const std::string& prefix) {
		databaseName = DartSAParameters::get<std::string>(prefix + ".databaseName");
		modelName = DartSAParameters::get<std::string>(prefix + ".modelName");

		prospectExternalModule.loadFromParameters(prefix + ".ProspectExternalModule");
	}
};

class LambertianMulti: public SurfaceFunction {
public:
	int lambertianDefinition = 0;
	boost::optional<Lambertian> lambertian;  // Child node
	boost::optional<Marmit> marmit;  // Child node
	boost::optional<LambertianMultiplicativeFactorList> nodeMultiplicativeFactor;  // Child node

	Type getType() override {
		return SurfaceFunction::Type::Lambertian;
	}

	void loadFromParameters(const std::string& prefix) override {
		SurfaceFunction::loadFromParameters(prefix);

		lambertianDefinition = DartSAParameters::get<int>(prefix + ".lambertianDefinition");

		if (DartSAParameters::exists(prefix + ".Lambertian")) {
			lambertian = Lambertian();
			lambertian->loadFromParameters(prefix + ".Lambertian");
		}
		if (DartSAParameters::exists(prefix + ".Marmit")) {
			marmit = Marmit();
			marmit->loadFromParameters(prefix + ".Marmit");
		}

		if (DartSAParameters::exists(prefix + ".LambertianNodeMultiplicativeFactorForLUT")) {
			nodeMultiplicativeFactor = LambertianMultiplicativeFactorList();
			nodeMultiplicativeFactor->loadFromParameters(prefix + ".LambertianNodeMultiplicativeFactorForLUT");
		}
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_LAMBERTIAN_LAMBERTIAN_HPP_ */
