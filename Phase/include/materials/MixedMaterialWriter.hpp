/*
 * MixedMaterialWriter.hpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_MIXEDMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_MIXEDMATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "materials/SurfaceMaterialProcessor.hpp"

namespace phase {
class MixedMaterialWriter {
public:
	explicit MixedMaterialWriter(const SurfaceMaterialProcessor & processor);

	// LuxCore material name (the part after "scene.materials."): shared with
	// SurfaceMaterialProcessor so mixed materials can reference this material.
	static std::string getMaterialName(unsigned int indexFunction);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

private:
	// Resolves one Mixed link (FirstProperty/SecondProperty) to the LuxCore
	// material name of the referenced material, via the shared processor
	// resolver. Exits (-6) on volume links, (-7) on unknown/unregistered
	// references.
	std::string referencedMaterialName(const phsdata::Property & link,
										const PhaseGeneralParametersDTO & genDTO) const;

	const SurfaceMaterialProcessor & _processor;
};
}

#endif /* INCLUDE_MATERIALS_MIXEDMATERIALWRITER_HPP_ */
