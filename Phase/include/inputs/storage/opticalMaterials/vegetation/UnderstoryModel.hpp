/*
 * UnderstoryModel.hpp
 *
 *  Foliar (vegetation) model of one UnderstoryMulti face: database/model
 *  pair plus the prospect/fluorescence external module.
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMODEL_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMODEL_HPP_

#include <string>

#include "DartSAParameters.h"
#include "../ProspectExternalModule.hpp"

namespace phsdata {
class UnderstoryModel {
public:
	std::string modelName;
	std::string databaseName;  // Overwritten by prepareProspect() when a prospect database is generated
	ProspectExternalModule prospectExternalModule;

	void loadFromParameters(const std::string& prefix) {
		modelName = DartSAParameters::get<std::string>(prefix + ".modelName");
		databaseName = DartSAParameters::get<std::string>(prefix + ".databaseName");

		prospectExternalModule.loadFromParameters(prefix + ".ProspectExternalModule");
	}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VEGETATION_UNDERSTORYMODEL_HPP_ */
