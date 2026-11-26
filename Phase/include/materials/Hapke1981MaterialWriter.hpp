/*
 * Hapke1981MaterialWriter.hpp
 *
 *  Created on: 2 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_HAPKE1981MATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_HAPKE1981MATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {
class Hapke1981MaterialWriter {
public:
	Hapke1981MaterialWriter(	const FluorescenceService & fluorescenceService,
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

	void writeLuxCoreMaterial(	const phsdata::Hapke1981 & hm,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertyHapke> & properties,
								unsigned int materialID) const;

private:
	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::Hapke1981 & hm,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;

	void applyMultiplicativeFactors(const phsdata::Hapke1981 & lm,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertyHapke> & properties) const;

	// --- validation / warnings ---
	void checkAlbedo(	const std::string & modelName,
						PropertyHapke & p) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};
}

#endif /* INCLUDE_MATERIALS_HAPKE1981MATERIALWRITER_HPP_ */
