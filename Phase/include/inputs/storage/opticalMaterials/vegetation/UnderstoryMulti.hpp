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

	boost::optional<UnderstoryModel> understoryMultiModel;       // Single model (top and bottom)
	boost::optional<UnderstoryModel> understoryMultiTopModel;    // Used when hasDifferentModelForBottom
	boost::optional<UnderstoryModel> understoryMultiBottomModel; // Used when hasDifferentModelForBottom

	DirectionSetLink directionSetLink;

	void loadFromParameters(const std::string & prefix) {
		hasDifferentModelForBottom = DartSAParameters::get<bool>(prefix + ".hasDifferentModelForBottom");
		functionName = DartSAParameters::get<std::string>(prefix + ".functionName");
		indexFunction = DartSAParameters::get<unsigned int>(prefix + ".indexFunction");

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
