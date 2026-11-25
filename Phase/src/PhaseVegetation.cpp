/*
 * PhaseVegetation.cpp
 *
 *  Created on: 20 oct. 2026
 *      Author: lauretn
 */

#include "Phase.hpp"

/**
 * Builds the OpticalPropertyParameters (FOLIAR_PROPERTY) of one vegetation
 * model from its phsdata storage: the top/bottom database+model names (the
 * database name is the prospect-generated uuid when prepareProspect() ran),
 * with the temp output folder and self-destruct flag in that case. Shared by
 * createDatabasesReadingProperties() and VegetationMaterialWriter.
 */
OpticalPropertyParameters buildFoliarOpticalPropertyParameters(const phsdata::UnderstoryMulti & understory, const unsigned int numberOfBands) {
	const phsdata::UnderstoryModel * topModel = understory.getTopModel();
	const phsdata::UnderstoryModel * bottomModel = understory.getBottomModel();

	OpticalPropertyParameters opticalProperty(FOLIAR_PROPERTY, understory.indexFunction, understory.functionName, -1);

	bool useProspectGen = (topModel->prospectExternalModule.useProspectExternalModule
			|| topModel->prospectExternalModule.isFluorescent)
			&& topModel->prospectExternalModule.externParameters.is_initialized();
	std::string databaseName = topModel->databaseName;
	std::string modelName = topModel->modelName;
	std::string databasePath;
	if (useProspectGen) {
		databasePath = DartEnv::getOutputTempDatabasePath(true);
		// Maybe change the model name in the future, but this modelName are not findable by the user.
		// Spaces and special caracters are not allowed in modelName so ...
		modelName = "Model";
	} else {
		databasePath = DartEnv::getDatabasePath(databaseName);
	}

	// Bottom foliar model
	std::string bottomDatabaseName;
	std::string bottomDatabasePath;
	std::string bottomModelName;
	bool bottomProspectGen = false;
	if (bottomModel != nullptr) {
		bottomProspectGen = (bottomModel->prospectExternalModule.useProspectExternalModule
				|| bottomModel->prospectExternalModule.isFluorescent)
				&& bottomModel->prospectExternalModule.externParameters.is_initialized();
		bottomDatabaseName = bottomModel->databaseName;
		bottomModelName = bottomModel->modelName;
		if (bottomProspectGen) {
			bottomDatabasePath = DartEnv::getOutputTempDatabasePath(true);
			bottomModelName = "Model";
		} else {
			bottomDatabasePath = DartEnv::getDatabasePath(bottomDatabaseName);
		}
	}

	shared_ptr<FoliarDatabaseParameters> foliarDatabaseParameters(
		new FoliarDatabaseParameters(databaseName, databasePath, modelName, numberOfBands, understory.hasDifferentModelForBottom,
				bottomDatabaseName, bottomDatabasePath, bottomModelName));
	if (useProspectGen) {
		foliarDatabaseParameters.get()->setSelfDestructDatabase(true);
	}
	if (bottomProspectGen) {
		foliarDatabaseParameters.get()->setSelfDestructBottomDatabase(true);
	}
	opticalProperty.addDatabaseToProperty(foliarDatabaseParameters);

	return opticalProperty;
}
