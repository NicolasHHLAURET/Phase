/*
 * LambertianMaterialWriter.hpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_LAMBERTIANMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_LAMBERTIANMATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {

class LambertianMaterialWriter {
public:
	LambertianMaterialWriter(	const FluorescenceService & fluorescenceService,
								const OpticalFactorsTextureWriter & opticalFactorsWriter);

	// Full Lambertian treatment: database preparation, optical properties,
	// LuxCore scene keys and fluorescence. Returns false if nothing could be
	// processed (no Lambertian node, Marmit failure, ...).
	// LuxCore material name (the part after "scene.materials."): shared with
	// SurfaceMaterialProcessor so mixed materials can reference this material.
	static std::string getMaterialName(unsigned int indexFunction);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

	void writeLuxCoreMaterial(	const phsdata::LambertianMulti & lm,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertyLambertian> & properties,
								const FluorescenceResults & fluorescence,
								unsigned int materialID) const;

	// Writes the LuxCore scene keys of a lambertian material. materialIndex is
	// the phase-function index (opIndex), materialID the LuxCore material id.
	// Usable for any lambertian-like material, including the equivalent
	// lambertians generated from the foliar models (which have no LambertianMulti
	// storage: the optical factors, if any, are written by the caller).
	void writeLuxCoreMaterial(	PhaseGeneralParametersDTO & genDTO,
									unsigned int materialIndex,
									unsigned int materialID,
									const std::vector<PropertyLambertian> & lambertianProperties,
									const FluorescenceResults & fluorescenceResults) const;

private:

	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::LambertianMulti & lm,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;
	FluorescenceResults computeFluorescence(const phsdata::LambertianMulti & lm,
											const PhaseGeneralParametersDTO & genDTO) const;
	void applyMultiplicativeFactors(const phsdata::LambertianMulti & lm,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertyLambertian> & properties) const;

	// --- validation / warnings ---
	void checkAlbedo(	const std::string & modelName,
						PropertyLambertian & p) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};

} // namespace phase

#endif /* INCLUDE_MATERIALS_LAMBERTIANMATERIALWRITER_HPP_ */
