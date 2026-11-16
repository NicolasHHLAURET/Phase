/*
 * PhaseIOEmissionMaterial.cpp
 *
 *  Created on: 22 sept. 2026
 *      Author: lauretn
 */
#include "PhaseDAO.hpp"

#include <string>
#include <stdexcept>
#include <sstream>

void PhaseDAO::parseOpticalMaterials(PhaseGeneralParametersDTO & genDTO) {
	if (DartSAParameters::exists("Materials.Surfaces"))
		genDTO._surfaceMaterials.loadFromParameters("Materials.Surfaces");
	if (DartSAParameters::exists("Materials.Volumes"))
		genDTO._volumesMaterials.loadFromParameters("Materials.Volumes");
}
