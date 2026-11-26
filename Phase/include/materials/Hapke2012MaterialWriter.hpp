/*
 * Hapke2012MaterialWriter.hpp
 *
 *  Created on: 5 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_HAPKE2012MATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_HAPKE2012MATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {
class Hapke2012MaterialWriter {
public:
	Hapke2012MaterialWriter(	const FluorescenceService & fluorescenceService,
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

	void writeLuxCoreMaterial(	const phsdata::Hapke2012 & hm,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertyHapke2> & properties,
								unsigned int materialID) const;

private:
	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::Hapke2012 & hm,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;

	void applyMultiplicativeFactors(const phsdata::Hapke2012 & lm,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertyHapke2> & properties) const;

	// --- validation / warnings ---
	void checkAlbedo(	const std::string & modelName,
						PropertyHapke2 & p) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};
}

#endif /* INCLUDE_MATERIALS_HAPKE2012MATERIALWRITER_HPP_ */
