/*
 * RPVMaterialWriter.hpp
 *
 *  Created on: 9 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_RPVMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_RPVMATERIALWRITER_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "PhaseStructure.hpp"

namespace phase {
class RPVMaterialWriter {
public:
	RPVMaterialWriter(	const FluorescenceService & fluorescenceService,
								const OpticalFactorsTextureWriter & opticalFactorsWriter);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

	void writeLuxCoreMaterial(	const phsdata::RPVMulti & hm,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<PropertyRpv> & properties,
								unsigned int materialID) const;

private:
	// --- pipeline steps (private, in call order) ---
	bool prepareDatabase(	const phsdata::RPVMulti & hm,
							const PhaseGeneralParametersDTO & genDTO,
							OpticalPropertyParameters & opticalProperty) const;

	void applyMultiplicativeFactors(const phsdata::RPVMulti & lm,
									OpticalPropertyParameters & opticalProperty,
									std::vector<PropertyRpv> & properties) const;

	// --- validation / warnings ---
	void checkAlbedo(	const std::string & modelName,
						PropertyRpv & p) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
};
}

#endif /* INCLUDE_MATERIALS_RPVMATERIALWRITER_HPP_ */
