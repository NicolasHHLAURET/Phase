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
int integrationStepOnTheta = 1;
int integrationStepOnPhi = 10;

static int snapToNearestDivider(int value, const int * table, int tableSize) {
	for (int i = 0; i < tableSize - 1; i++) {
		if (value == table[i]) {
			break;
		} else if (value > table[i] && value <= table[i + 1]) {
			if (value - table[i] > table[i + 1] - value) {
				value = table[i + 1];
			} else {
				value = table[i];
			}
			break;
		}
	}
	return value;
}

void loadFromParameters(const std::string& prefix) {
	if (DartSAParameters::exists(prefix + ".UnderstoryMultiFunctions")) {
		const std::string understoryPrefix = prefix + ".UnderstoryMultiFunctions";
		if (DartSAParameters::exists(understoryPrefix + ".integrationStepOnTheta"))
			integrationStepOnTheta = DartSAParameters::get<int>(understoryPrefix + ".integrationStepOnTheta");
		if (DartSAParameters::exists(understoryPrefix + ".integrationStepOnPhi"))
			integrationStepOnPhi = DartSAParameters::get<int>(understoryPrefix + ".integrationStepOnPhi");
		static const int tableDividerOfThetaMax[12] = { 1, 2, 3, 5, 6, 9, 10, 15, 18, 30, 45, 90 };
		static const int tableDividerOfPhiMax[24] = { 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 18, 20, 24, 30, 36, 40, 45, 60, 72, 90, 120, 180, 360 };
		integrationStepOnTheta = snapToNearestDivider(integrationStepOnTheta, tableDividerOfThetaMax, 12);
		integrationStepOnPhi = snapToNearestDivider(integrationStepOnPhi, tableDividerOfPhiMax, 24);
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
