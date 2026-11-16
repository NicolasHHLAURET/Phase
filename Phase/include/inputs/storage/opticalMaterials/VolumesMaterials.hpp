/*
 * VolumesMaterials.hpp
 *
 *  Created on: 10 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VOLUMESMATERIALS_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VOLUMESMATERIALS_HPP_

#include <memory>
#include <string>
#include <vector>

#include "DartSAParameters.h"
#include "vegetation/UnderstoryMulti.hpp"

namespace phsdata {
class VolumesMaterials {
public:
std::vector<std::shared_ptr<UnderstoryMulti> > understoryMultiFunctions;

void loadFromParameters(const std::string& prefix) {
	if (DartSAParameters::exists(prefix + ".UnderstoryMultiFunctions")) {
		const std::string understoryPrefix = prefix + ".UnderstoryMultiFunctions";
		int count = DartSAParameters::getNbOf(understoryPrefix + ".UnderstoryMulti");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<UnderstoryMulti>(new UnderstoryMulti());
			func->loadFromParameters(understoryPrefix + ".UnderstoryMulti[" + std::to_string(i) + "]");
			understoryMultiFunctions.push_back(func);
		}
	}
}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_VOLUMESMATERIALS_HPP_ */
