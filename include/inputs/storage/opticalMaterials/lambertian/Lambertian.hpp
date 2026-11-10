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
#include "inputs/storage/opticalMaterials/lambertian/LambertianMultiplicativeFactor.hpp"

namespace phsdata {
class FluorescenceExternParameters {
public:
	bool forceYields = true;
	bool useCombinedYields = false;
	double yieldPSI = 0.02;
	double yieldPSII = 0.;

	void loadFromParameters(const std::string& prefix) {
		forceYields = DartSAParameters::get<bool>(prefix + ".forceYields");

		if (DartSAParameters::exists(prefix + ".Yields.yieldPSI")) {
			useCombinedYields = false;
			yieldPSI = DartSAParameters::get<double>(prefix + ".Yields.yieldPSI");
			yieldPSII = DartSAParameters::get<double>(prefix + ".Yields.yieldPSII");
		}
		if (DartSAParameters::exists(prefix + ".Yield.yieldPS")) {
			useCombinedYields = true; // => yieldPSII is unused
			yieldPSI = DartSAParameters::get<double>(prefix + ".Yield.yieldPS");
		}
	}
};

class ProspectExternParameters {
public:
	double anthocyanin = 0.;
	double cBrown = 0.; // Brown Pigments
	double cab = 40.; // Chlorophyll
	double car = 10.; // Carotenoid
	double cbc = 0.009; //
	double cm = 0.01; // Leaf Mass per Area
	double cp = 0;
	double cw = 0.012; // Water Thickness
	std::string inputProspectFile = "Prospect_Fluspect/Optipar2021_ProspectPRO.txt";
	bool isV2Z = false;
	double v2z = -999;
	double n = 1.5; // Structure Coefficient
	bool useCm = false;

	void loadFromParameters(const std::string& prefix) {
		anthocyanin = DartSAParameters::get<double>(prefix + ".anthocyanin");
		cBrown = DartSAParameters::get<double>(prefix + ".cBrown");
		cab = DartSAParameters::get<double>(prefix + ".cab");
		car = DartSAParameters::get<double>(prefix + ".car");
		cbc = DartSAParameters::get<double>(prefix + ".cbc");
		cm = DartSAParameters::get<double>(prefix + ".cm");
		cp = DartSAParameters::get<double>(prefix + ".cp");
		cw = DartSAParameters::get<double>(prefix + ".cw");
		inputProspectFile = DartSAParameters::get<std::string>(prefix + ".inputProspectFile");
		isV2Z = DartSAParameters::get<bool>(prefix + ".isV2Z");
		n = DartSAParameters::get<double>(prefix + ".n");
		useCm = DartSAParameters::get<bool>(prefix + ".useCm");
		if (isV2Z)
			v2z = DartSAParameters::get<double>(prefix + ".V2Z.v2z");
	}
};

class ProspectExternalModule {
public:
	bool isFluorescent = false;
	bool useProspectExternalModule = false;
	boost::optional<ProspectExternParameters> externParameters;  // Optional child
	boost::optional<FluorescenceExternParameters> fluorescence;  // Optional child

	void loadFromParameters(const std::string& prefix) {
		isFluorescent = DartSAParameters::get<bool>(prefix + ".isFluorescent");
		useProspectExternalModule = DartSAParameters::get<bool>(prefix + ".useProspectExternalModule");

		if (DartSAParameters::exists(prefix + ".ProspectExternParameters")) {
			externParameters = ProspectExternParameters();
			externParameters->loadFromParameters(prefix + ".ProspectExternParameters");
		}
		if (DartSAParameters::exists(prefix + ".FluorescenceYields")) {
			fluorescence = FluorescenceExternParameters();
			fluorescence->loadFromParameters(prefix + ".FluorescenceYields");
		}
	}
};

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
