/*
 * SpecularMaterialWriter.hpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_SPECULARMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_SPECULARMATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "materials/SurfaceMaterialProcessor.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {
class SpecularMaterialWriter {
public:
	SpecularMaterialWriter(	const SurfaceMaterialProcessor & processor,
							const FluorescenceService & fluorescenceService,
							const OpticalFactorsTextureWriter & opticalFactorsWriter);

	// LuxCore material name (the part after "scene.materials."): shared with
	// SurfaceMaterialProcessor so mixed materials can reference this material.
	static std::string getMaterialName(unsigned int indexFunction);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

	void writeLuxCoreMaterial(	const phsdata::Specular & s,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertySpecular> & properties,
								unsigned int materialID) const;

private:
	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::Specular & s,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;

	void applyMultiplicativeFactors(const phsdata::Specular & s,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertySpecular> & properties) const;

	const SurfaceMaterialProcessor & _processor;
	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};
}

#endif /* INCLUDE_MATERIALS_SPECULARMATERIALWRITER_HPP_ */
