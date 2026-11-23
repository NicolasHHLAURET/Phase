/*
 * UnderstoryMulti.hpp
 *
 *  Created on: 10 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_

#include <string>
#include <boost/optional.hpp>

#include "DartSAParameters.h"
#include "../../DirectionSetLink.hpp"
#include "UnderstoryModel.hpp"

namespace phsdata {
class UnderstoryMulti {
public:
	bool hasDifferentModelForBottom = false;
	std::string functionName = "undefined";
	unsigned int indexFunction = 0;

	int lad = 0; // type of leaf angle distribution (values per the v6 template: 8=Ellipsoidal, 9=Elliptical, 10=UserDefined, 11=Manual, 12=BoundedUniform, ...)
	double dimFoliar = 0.;
	double thermalHotSpotFactor = 0.;
	// DirectionalClumpingIndexProperties
	double clumpingA = 0., clumpingB = 0., omegaMin = 0., omegaMax = 0.;
	// LAD-type-specific children: present in the XML only for the matching lad value
	boost::optional<double> ellipsoidalAla;              // Ellipsoidal.ala
	boost::optional<double> ellipticalThetaM;            // Elliptical.thetam
	boost::optional<double> ellipticalEpsilon;           // Elliptical.epsilon
	boost::optional<int> boundedUniformNbAla;            // BoundedUniform.nbAla
	boost::optional<int> boundedUniformNthAla;           // BoundedUniform.nthAla
	boost::optional<std::string> userDefinedLadFileName; // UserDefined.fileName
	struct ManualLadParameters {
		double m = 0., b0 = 0., b1 = 0., b2 = 0.; // Manual.lad_m / lad_b0 / lad_b1 / lad_b2
	} manualLad;
	bool hasManualLad = false;

	boost::optional<UnderstoryModel> understoryMultiModel;       // Single model (top and bottom)
	boost::optional<UnderstoryModel> understoryMultiTopModel;    // Used when hasDifferentModelForBottom
	boost::optional<UnderstoryModel> understoryMultiBottomModel; // Used when hasDifferentModelForBottom

	DirectionSetLink directionSetLink;

	void loadFromParameters(const std::string & prefix) {
		hasDifferentModelForBottom = DartSAParameters::get<bool>(prefix + ".hasDifferentModelForBottom");
		functionName = DartSAParameters::get<std::string>(prefix + ".functionName");
		indexFunction = DartSAParameters::get<unsigned int>(prefix + ".indexFunction");
		lad = DartSAParameters::get<int>(prefix + ".lad");
		dimFoliar = DartSAParameters::get<double>(prefix + ".dimFoliar");
		thermalHotSpotFactor = DartSAParameters::get<double>(prefix + ".thermalHotSpotFactor");

		if (DartSAParameters::exists(prefix + ".DirectionalClumpingIndexProperties")) {
			const std::string clumpingPrefix = prefix + ".DirectionalClumpingIndexProperties";
			clumpingA = DartSAParameters::get<double>(clumpingPrefix + ".clumpinga");
			clumpingB = DartSAParameters::get<double>(clumpingPrefix + ".clumpingb");
			omegaMin = DartSAParameters::get<double>(clumpingPrefix + ".omegaMin");
			omegaMax = DartSAParameters::get<double>(clumpingPrefix + ".omegaMax");
		}
		// LAD-type-specific children exist only for the matching lad value
		if (DartSAParameters::exists(prefix + ".Ellipsoidal"))
			ellipsoidalAla = DartSAParameters::get<double>(prefix + ".Ellipsoidal.ala");
		if (DartSAParameters::exists(prefix + ".Elliptical")) {
			ellipticalThetaM = DartSAParameters::get<double>(prefix + ".Elliptical.thetam");
			ellipticalEpsilon = DartSAParameters::get<double>(prefix + ".Elliptical.epsilon");
		}
		if (DartSAParameters::exists(prefix + ".BoundedUniform")) {
			boundedUniformNbAla = DartSAParameters::get<int>(prefix + ".BoundedUniform.nbAla");
			boundedUniformNthAla = DartSAParameters::get<int>(prefix + ".BoundedUniform.nthAla");
		}
		if (DartSAParameters::exists(prefix + ".UserDefined"))
			userDefinedLadFileName = DartSAParameters::get<std::string>(prefix + ".UserDefined.fileName");
		if (DartSAParameters::exists(prefix + ".Manual")) {
			hasManualLad = true;
			manualLad.m = DartSAParameters::get<double>(prefix + ".Manual.lad_m");
			manualLad.b0 = DartSAParameters::get<double>(prefix + ".Manual.lad_b0");
			manualLad.b1 = DartSAParameters::get<double>(prefix + ".Manual.lad_b1");
			manualLad.b2 = DartSAParameters::get<double>(prefix + ".Manual.lad_b2");
		}

		if (DartSAParameters::exists(prefix + ".UnderstoryMultiModel")) {
			understoryMultiModel = UnderstoryModel();
			understoryMultiModel->loadFromParameters(prefix + ".UnderstoryMultiModel");
		}
		if (DartSAParameters::exists(prefix + ".UnderstoryMultiTopModel")) {
			understoryMultiTopModel = UnderstoryModel();
			understoryMultiTopModel->loadFromParameters(prefix + ".UnderstoryMultiTopModel");
		}
		if (DartSAParameters::exists(prefix + ".UnderstoryMultiBottomModel")) {
			understoryMultiBottomModel = UnderstoryModel();
			understoryMultiBottomModel->loadFromParameters(prefix + ".UnderstoryMultiBottomModel");
		}

		if (DartSAParameters::exists(prefix + ".DirectionSetLink")) {
			directionSetLink.loadFromParameters(prefix + ".DirectionSetLink");
		}
	}

	UnderstoryModel * getTopModel() {
		if (hasDifferentModelForBottom)
			return understoryMultiTopModel.is_initialized() ? &understoryMultiTopModel.value() : nullptr;
		return understoryMultiModel.is_initialized() ? &understoryMultiModel.value() : nullptr;
	}

	const UnderstoryModel * getTopModel() const {
		if (hasDifferentModelForBottom)
			return understoryMultiTopModel.is_initialized() ? &understoryMultiTopModel.value() : nullptr;
		return understoryMultiModel.is_initialized() ? &understoryMultiModel.value() : nullptr;
	}

	UnderstoryModel * getBottomModel() {
		if (hasDifferentModelForBottom && understoryMultiBottomModel.is_initialized())
			return &understoryMultiBottomModel.value();
		return nullptr;
	}

	const UnderstoryModel * getBottomModel() const {
		if (hasDifferentModelForBottom && understoryMultiBottomModel.is_initialized())
			return &understoryMultiBottomModel.value();
		return nullptr;
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMULTI_HPP_ */
