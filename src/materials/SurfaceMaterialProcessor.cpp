/*
 * SurfaceMaterialProcessor.cpp
 *
 *  Created on: 2 nov. 2026
 *      Author: lauretn
 */

#include "materials/SurfaceMaterialProcessor.hpp"
#include "PhaseProperty.hpp"

namespace phase {
void SurfaceMaterialProcessor::registerWriter(phsdata::SurfaceFunction::Type type, MaterialHandler handler) {
	if (!handler) {
		throw std::invalid_argument("SurfaceMaterialProcessor: null material handler");
	}
	_handlers[type] = std::move(handler);
}

void SurfaceMaterialProcessor::processAll(	PhaseGeneralParametersDTO & genDTO,
											PhaseKeyMappingGenerator & pkmg,
											std::ofstream & outCoeffSpecProperties) const {
	const std::vector<bool> useSpectralIrradianceForReflectance = computeUseSpectralIrradiance(genDTO);

	unsigned int materialIndex = 0;
	for (const std::shared_ptr<phsdata::SurfaceFunction> & sm : genDTO._surfaceMaterials.surfaceOpticalFunctions) {
		const auto it = _handlers.find(sm->getType());
		if (it != _handlers.cend()) {
			it->second(*sm, genDTO, useSpectralIrradianceForReflectance, materialIndex, pkmg, outCoeffSpecProperties);
		}
		++materialIndex;
	}
}

/*
 * Depending of the mode optical properties are weighted according to the solar spectrum:
 * In mode R, in mode R+T if the ratio of energy from R/T is greater than factorRatioModesRTuseSun
 */
std::vector<bool> SurfaceMaterialProcessor::computeUseSpectralIrradiance(	const PhaseGeneralParametersDTO & genDTO) const {
	std::vector<bool> spectralIrradianceUsageVector(genDTO.numberOfSpectralBands, false);
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		spectralIrradianceUsageVector[spectralBandIndex] = PhaseProperty::getWeightingOfReflectanceParameters() && PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0;
	}
	return spectralIrradianceUsageVector;
}
}
