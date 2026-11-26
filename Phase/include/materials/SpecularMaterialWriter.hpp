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
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {
class SpecularMaterialWriter {
public:
	SpecularMaterialWriter(	const FluorescenceService & fluorescenceService,
							const OpticalFactorsTextureWriter & opticalFactorsWriter);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

	void writeLuxCoreMaterial(	const phsdata::Specular & s,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertySpecular> & properties,
								const std::string & baseMaterial,
								unsigned int materialID) const;

private:
	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::Specular & s,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;

	void applyMultiplicativeFactors(const phsdata::Specular & s,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertySpecular> & properties,
									std::string & baseMaterial) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};
}

#endif /* INCLUDE_MATERIALS_SPECULARMATERIALWRITER_HPP_ */
