/*
 * VegetationMaterialWriter.hpp
 *
 *  Vegetation (foliar) counterpart of the surface MaterialWriters: loops the
 *  phsdata::UnderstoryMulti models, resolves each model's direction set through
 *  its DirectionSetLink, and redoes the direction-set-dependent precomputing
 *  only when the direction set changes.
 */

#ifndef INCLUDE_MATERIALS_VEGETATIONMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_VEGETATIONMATERIALWRITER_HPP_

#include <memory>
#include <vector>

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"
#include "PhaseStructure.hpp"
#include "containers/SphereDiscretizationMap.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"

struct VegetationVolumeMaterials;

namespace phase {

class VegetationMaterialWriter {
public:
	VegetationMaterialWriter(	const FluorescenceService & fluorescenceService,
							const OpticalFactorsTextureWriter & opticalFactorsWriter);

	// Loops genDTO._volumesMaterials.understoryMultiFunctions grouped by their
	// DirectionSetLink (original order kept within a group; the output model
	// index is UnderstoryMulti::indexFunction). Calls
	// prepareVegetationDirectionSet() only when the direction set changes.
	void processAll(	PhaseGeneralParametersDTO & genDTO,
					VegetationVolumeMaterials & vegetation,
					PhaseKeyMappingGenerator & pkmg,
					std::ofstream & outCoeffSpecProperties);

private:
	// Full treatment of one vegetation model (ported from the old treatFoliar).
	// foliarIndice is the position in understoryMultiFunctions (used by the
	// FoliarModel/FoliarComputation/LADComputation accessors); the output model
	// index is understory.indexFunction.
	bool treat(	const phsdata::UnderstoryMulti & understory,
			unsigned int foliarIndice,
			PhaseGeneralParametersDTO & genDTO,
			VegetationVolumeMaterials & vegetation,
			const std::vector<DirectionCartesienne> & listDirections,
			PhaseKeyMappingGenerator & pkmg,
			std::ofstream & outCoeffSpecProperties);

	// Computes the fluorescence matrices of one face from the phsdata prospect
	// module of the model node (replaces the old computeFoliarFluorescenceMatrices).
	void computeFaceFluorescence(	FluorescenceResults & results,
							std::vector<std::vector<std::vector<double> > > & fluorescenceMatrix,
							const phsdata::UnderstoryModel & model,
							unsigned int modelIndex,
							const PhaseGeneralParametersDTO & genDTO,
							unsigned int face) const;

	const FluorescenceService & _fluorescenceService;
	const OpticalFactorsTextureWriter & _opticalFactorsWriter;
	std::unique_ptr<SphereDiscretizationMap> _sphereMap; // built once from fTFoliar.iterationStepPhi (direction-set independent)
};

} // namespace phase

#endif /* INCLUDE_MATERIALS_VEGETATIONMATERIALWRITER_HPP_ */
