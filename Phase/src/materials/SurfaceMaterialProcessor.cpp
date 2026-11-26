/*
 * SurfaceMaterialProcessor.cpp
 *
 *  Created on: 2 nov. 2026
 *      Author: lauretn
 */

#include "materials/SurfaceMaterialProcessor.hpp"
#include "PhaseProperty.hpp"

#include <cstdlib>
#include <iostream>

namespace phase {
void SurfaceMaterialProcessor::registerWriter(phsdata::SurfaceFunction::Type type, MaterialHandler handler) {
	if (!handler) {
		throw std::invalid_argument("SurfaceMaterialProcessor: null material handler");
	}
	_handlers[type] = std::move(handler);
}

void SurfaceMaterialProcessor::registerNameProvider(phsdata::SurfaceFunction::Type type, NameProvider provider) {
	if (!provider) {
		throw std::invalid_argument("SurfaceMaterialProcessor: null name provider");
	}
	_nameProviders[type] = std::move(provider);
}

phsdata::SurfaceFunction::Type SurfaceMaterialProcessor::surfaceTypeFromXmlType(int xmlType) {
	switch (xmlType) {
		case globalReferences::LAMBERTIEN:
			return phsdata::SurfaceFunction::Type::Lambertian;
		case globalReferences::HAPKE_SPEC:
			return phsdata::SurfaceFunction::Type::Hapke1981;
		case globalReferences::HAPKE2_SPEC:
			return phsdata::SurfaceFunction::Type::Hapke2012;
		case globalReferences::RPV_PHASE:
			return phsdata::SurfaceFunction::Type::RPV;
		case globalReferences::PHASE_EXTERN:
			return phsdata::SurfaceFunction::Type::DiscreteBSDF;
		case globalReferences::SPECULAR_LUX:
			return phsdata::SurfaceFunction::Type::Specular;
		case globalReferences::MIX_MATERIAL:
			return phsdata::SurfaceFunction::Type::Mixed;
		default:
			cerr << "Unknown optical link type " << xmlType << endl;
			exit(-7);
	}
}

std::string SurfaceMaterialProcessor::materialNameFor(int xmlType, unsigned int indexFunction) const {
	const phsdata::SurfaceFunction::Type type = surfaceTypeFromXmlType(xmlType);
	const auto it = _nameProviders.find(type);
	if (it == _nameProviders.cend()) {
		cerr << "No material writer registered for optical link type " << xmlType << endl;
		exit(-7);
	}
	return it->second(indexFunction);
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
