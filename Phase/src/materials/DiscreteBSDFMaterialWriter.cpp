/*
 * DiscreteBSDFMaterialWriter.cpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#include "materials/DiscreteBSDFMaterialWriter.hpp"

#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/PhaseExternDatabaseParameters.hpp"

#include <cstdlib>
#include <iostream>

namespace phase {

std::string DiscreteBSDFMaterialWriter::getMaterialName(unsigned int indexFunction) {
	return "DiscreteBSDF" + std::to_string(indexFunction);
}

bool DiscreteBSDFMaterialWriter::treat(const phsdata::SurfaceFunction & sf,
									   PhaseGeneralParametersDTO & genDTO,
									   const std::vector<bool> & useSpectralIrradianceForReflectance,
									   unsigned int materialIndex,
									   PhaseKeyMappingGenerator & pkmg,
									   std::ofstream & outCoeffSpecProperties) const {
	const phsdata::DiscreteBSDF & d = dynamic_cast<const phsdata::DiscreteBSDF &>(sf);

	// v6 novelty: the material references its direction set itself
	// (DirectionSetLink) instead of receiving listDirections as a treat() argument.
	if (d.directionSetLink.indexDirectionSetLink >= genDTO._directionSet.size()) {
		std::cerr << "DiscreteBSDF " << d.indexFunction << " references direction set "
				<< d.directionSetLink.indexDirectionSetLink << " but only " << genDTO._directionSet.size()
				<< " direction set(s) exist." << std::endl;
		std::exit(-7);
	}
	const std::vector<DirectionCartesienne> & listDirections = genDTO._directionSet[d.directionSetLink.indexDirectionSetLink]._directions;

	if (!d.databaseName.is_initialized() || !d.modelName.is_initialized()) {
		std::cerr << "DiscreteBSDF " << d.indexFunction << " has no databaseName/modelName." << std::endl;
		std::exit(-7);
	}

	OpticalPropertyParameters opticalProperty(PHASE_EXTERN_PROPERTY, d.indexFunction, d.functionName);
	auto phaseExternDatabaseParameters = std::make_shared<PhaseExternDatabaseParameters>(
		*d.databaseName, DartEnv::getDatabasePath(*d.databaseName), *d.modelName, genDTO.numberOfSpectralBands);
	opticalProperty.addDatabaseToProperty(phaseExternDatabaseParameters);

	for (unsigned int i = 0; i < opticalProperty.getNbDatabaseForProperty(); ++i)
		PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, opticalProperty.getDatabaseForProperty(i));

	pkmg.setTypeFonctionDePhase(globalReferences::PHASE_EXTERN);

	computePhaseExtern_MultiDirection(opticalProperty, pkmg, outCoeffSpecProperties, genDTO, listDirections);

	writeLuxCoreMaterial(d, genDTO);
	return true;
}

void DiscreteBSDFMaterialWriter::writeLuxCoreMaterial(const phsdata::DiscreteBSDF & d,
													  PhaseGeneralParametersDTO & genDTO) const {
	const std::string keyRoot = "scene.materials." + getMaterialName(d.indexFunction) + ".";
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "type", "DiscreteBSDF");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "dataBase", std::to_string(d.indexFunction) + ".phaseExternData");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot + "generator", "phase");
}

} // namespace phase
