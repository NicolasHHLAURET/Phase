/*
 * MixedMaterialWriter.cpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#include "materials/MixedMaterialWriter.hpp"

#include <cstdlib>
#include <iostream>

namespace phase {

MixedMaterialWriter::MixedMaterialWriter(const SurfaceMaterialProcessor & processor) :
	_processor(processor) {}

std::string MixedMaterialWriter::getMaterialName(unsigned int indexFunction) {
	return "mixedMat" + std::to_string(indexFunction);
}

std::string MixedMaterialWriter::referencedMaterialName(const phsdata::Property & link,
														const PhaseGeneralParametersDTO & genDTO) const {
	const int xmlType = link.typeOpticalLink;
	if (xmlType == globalReferences::AIR_FP || xmlType == globalReferences::FOLIAIRE || xmlType == globalReferences::MIX_VOLUME) {
		cerr << "Can not create a mixed material out of a volume." << endl;
		exit(-6);
	}

	// The referenced index is the indexFunction of a surface function of that type.
	const phsdata::SurfaceFunction::Type type = SurfaceMaterialProcessor::surfaceTypeFromXmlType(xmlType);
	bool found = false;
	for (const std::shared_ptr<phsdata::SurfaceFunction> & sf : genDTO._surfaceMaterials.surfaceOpticalFunctions) {
		if (sf->getType() == type && sf->indexFunction == link.indexOpticalLink) {
			found = true;
			break;
		}
	}
	if (!found) {
		cerr << "Optical property " << link.indexOpticalLink << " of type " << xmlType << " referenced by a mixed material doesn't exist." << endl;
		exit(-7);
	}
	return _processor.materialNameFor(xmlType, link.indexOpticalLink);
}

bool MixedMaterialWriter::treat(const phsdata::SurfaceFunction & sf,
								PhaseGeneralParametersDTO & genDTO,
								const std::vector<bool> & useSpectralIrradianceForReflectance,
								unsigned int materialIndex,
								PhaseKeyMappingGenerator & pkmg,
								std::ofstream & outCoeffSpecProperties) const {
	const phsdata::Mixed & m = dynamic_cast<const phsdata::Mixed &>(sf);

	const std::string material1Name = referencedMaterialName(m.firstProperties, genDTO);
	const std::string material2Name = referencedMaterialName(m.secondProperties, genDTO);

	const std::string keyRoot = "scene.materials." + getMaterialName(m.indexFunction) + ".";

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "mix");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "id", std::to_string(materialIndex));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "amount", std::to_string(m.firstProperties.percentFirst / 100.));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material1", material1Name);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material1Type", std::to_string(m.firstProperties.typeOpticalLink));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material1Index", std::to_string(m.firstProperties.indexOpticalLink));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material2", material2Name);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material2Type", std::to_string(m.secondProperties.typeOpticalLink));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "material2Index", std::to_string(m.secondProperties.indexOpticalLink));
	return true;
}
}
