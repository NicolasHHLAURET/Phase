/*
 * VegetationMaterialWriter.cpp
 *
 *  Ported from the old treatFoliar() (Phase.cpp), with per-model direction
 *  sets (DirectionSetLink) and the phsdata storage instead of PhaseDAO reads
 *  for the database/prospect/fluorescence data.
 */

#include "materials/VegetationMaterialWriter.hpp"

#include <algorithm>
#include <limits>
#include <numeric>

#include <boost/lexical_cast.hpp>

#include "Phase.hpp"
#include "PhaseDAO.hpp"
#include "PhasePolarization.hpp"
#include "PhaseUtils.hpp"
#include "materials/LambertianMaterialWriter.hpp"
#include "database/model/FoliarDatabaseParameters.hpp"

namespace phase {

VegetationMaterialWriter::VegetationMaterialWriter(	const FluorescenceService & fluorescenceService,
							const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}

void VegetationMaterialWriter::processAll(	PhaseGeneralParametersDTO & genDTO,
						VegetationVolumeMaterials & vegetation,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties) {
	const std::vector<std::shared_ptr<phsdata::UnderstoryMulti> > & models = genDTO._volumesMaterials.understoryMultiFunctions;
	if (models.empty())
		return;

	// Iterate grouped by direction set (stable within a group) so that the
	// direction-set-dependent precomputing is redone only when the direction
	// set changes. Output model indices come from UnderstoryMulti::indexFunction,
	// so the grouping does not change the emitted keys.
	std::vector<unsigned int> order(models.size());
	std::iota(order.begin(), order.end(), 0);
	std::stable_sort(order.begin(), order.end(), [&](unsigned int a, unsigned int b) {
		return getUnderstoryDirectionSetIndex(genDTO, a) < getUnderstoryDirectionSetIndex(genDTO, b);
	});

	unsigned int lastDirectionSetIndex = std::numeric_limits<unsigned int>::max();
	for (unsigned int position : order) {
		const unsigned int directionSetIndex = getUnderstoryDirectionSetIndex(genDTO, position);
		if (directionSetIndex != lastDirectionSetIndex) {
			prepareVegetationDirectionSet(genDTO, vegetation, directionSetIndex);
			lastDirectionSetIndex = directionSetIndex;
			if (!_sphereMap)
				_sphereMap.reset(new SphereDiscretizationMap(1.0, vegetation.fTFoliar.iterationStepPhi));
		}
		treat(*models[position], position, genDTO, vegetation, genDTO._directionSet[directionSetIndex]._directions, pkmg, outCoeffSpecProperties);
	}
}

void VegetationMaterialWriter::computeFaceFluorescence(	FluorescenceResults & results,
								std::vector<std::vector<std::vector<double> > > & fluorescenceMatrix,
								const phsdata::UnderstoryModel & model,
								unsigned int modelIndex,
								const PhaseGeneralParametersDTO & genDTO,
								unsigned int face) const {
	phase::InterpolatedMatrices interpolatedMatrices(genDTO.numberOfSpectralBands);
	_fluorescenceService.computeMatrices(results, interpolatedMatrices, globalReferences::FOLIAIRE, modelIndex,
		*model.prospectExternalModule.externParameters, *model.prospectExternalModule.fluorescence, genDTO, face);

	for (unsigned int sbi1 = 0; sbi1 < genDTO.numberOfSpectralBands; ++sbi1) {
		for (unsigned int sbi2 = 0; sbi2 < genDTO.numberOfSpectralBands; ++sbi2) {
			for (unsigned int k = 0; k < 4; ++k) {
				fluorescenceMatrix[sbi1][sbi2][k] = interpolatedMatrices.at(sbi1, sbi2, k);
			}
		}
	}
}

bool VegetationMaterialWriter::treat(	const phsdata::UnderstoryMulti & understory,
						unsigned int foliarIndice,
						PhaseGeneralParametersDTO & genDTO,
						VegetationVolumeMaterials & vegetation,
						const std::vector<DirectionCartesienne> & listDirections,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties) {
	FoliarComputation & foliarComputer = *vegetation.foliarComputation;
	LADComputation & ladComputation = *vegetation.ladComputation;
	FoliarModel & foliarModel = *vegetation.foliarModel;
	FTFoliar & fTFoliar = vegetation.fTFoliar;

	const unsigned int modelIndex = understory.indexFunction;
	const phsdata::UnderstoryModel * topModel = understory.getTopModel();
	const phsdata::UnderstoryModel * bottomModel = understory.getBottomModel();
	if (topModel == nullptr)
		return false;

	foliarComputer.reInitFoliarComputation();

	vector<FoliarProperty> foliarProperty;
	foliarProperty.resize(genDTO.numberOfSpectralBands);

	// Specular foliar database dropped (old XML handled by the external upgrader)
	for (unsigned int numBand = 0; numBand < genDTO.numberOfSpectralBands; ++numBand) {
		foliarProperty[numBand].specularFoliarData._isSpecular = false;
	}

	OpticalPropertyParameters foliarPropertyParameters = buildFoliarOpticalPropertyParameters(understory, genDTO.numberOfSpectralBands);
	for (unsigned int numPropertyDatabase = 0; numPropertyDatabase < foliarPropertyParameters.getNbDatabaseForProperty(); ++numPropertyDatabase) {
		if (foliarPropertyParameters.getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_FILE
				&& foliarPropertyParameters.getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_UP
				&& foliarPropertyParameters.getDatabaseForProperty(numPropertyDatabase).get()->getDatabaseType() != WATER_NO_DATABASE_USER_DEFINED_DOWN) {
			shared_ptr<PhaseDatabaseParameters> currentDatabase = foliarPropertyParameters.getDatabaseForProperty(numPropertyDatabase);
			PhaseProperty::readReflectanceParameters(vegetation.useSpectralIrradianceForReflectance, currentDatabase);
		}
	}

	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		foliarProperty[spectralBandIndex].typeOfLeafAngleDistribution = foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarIndice);
		foliarProperty[spectralBandIndex].foliarDimension = foliarModel.getFoliarDimension(foliarIndice);
	}
	fTFoliar.casPDF = ladComputation.initialisationLadProbabilityDensityFunctions(foliarIndice);
	ostringstream val;
	val << modelIndex; //on ajoute le numero de la fx de phase dans le nom du fichier : sprintf (val, "%u.",j)
	string name = val.str() + "." + foliarPropertyParameters.getModelName();

	createDirectory(DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder());

	pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	outCoeffSpecProperties << pkmg.cleThermalHotSpotFactor() << foliarModel.getHotSpotFactor(foliarIndice) << endl;
	phase::FluorescenceResults topFluorescenceResults;
	topFluorescenceResults.useBioClimaticProfileEta = false;
	topFluorescenceResults.isFluorescent = false;
	phase::FluorescenceResults bottomFluorescenceResults;
	bottomFluorescenceResults.useBioClimaticProfileEta = false;
	bottomFluorescenceResults.isFluorescent = false;
	foliarComputer.setTopFluorescenceComputation(false);
	foliarComputer.setBottomFluorescenceComputation(false);
	const bool hasDifferentBottom = understory.hasDifferentModelForBottom && bottomModel != nullptr;
	if (PhaseProperty::getFluorescence()) {
		foliarComputer.resetFrontFaceFluorescenceMatrix(genDTO.numberOfSpectralBands);
		if (topModel != nullptr && topModel->prospectExternalModule.isFluorescent
				&& topModel->prospectExternalModule.externParameters.is_initialized()
				&& topModel->prospectExternalModule.fluorescence.is_initialized()) {
			computeFaceFluorescence(topFluorescenceResults, foliarComputer.getFrontFaceFluorescenceMatrix(), *topModel, modelIndex,
					genDTO, 0);
			foliarComputer.setTopFluorescenceComputation(true);
			outCoeffSpecProperties << pkmg.cleTopFluorescenceBioClimaticUsage() << (topFluorescenceResults.useBioClimaticProfileEta ? "1" : "0") << endl;
		}
		foliarComputer.resetBackFaceFluorescenceMatrix(genDTO.numberOfSpectralBands);
		if (hasDifferentBottom) {
			if (bottomModel->prospectExternalModule.isFluorescent
					&& bottomModel->prospectExternalModule.externParameters.is_initialized()
					&& bottomModel->prospectExternalModule.fluorescence.is_initialized()) {
				computeFaceFluorescence(bottomFluorescenceResults, foliarComputer.getBackFaceFluorescenceMatrix(), *bottomModel, modelIndex,
						genDTO, 1);
				foliarComputer.setBottomFluorescenceComputation(true);
				outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << (bottomFluorescenceResults.useBioClimaticProfileEta ? "1" : "0") << endl;
			}
		} else {
			if (foliarComputer.isTopFluorescenceComputation()) {
				bottomFluorescenceResults = topFluorescenceResults;
				foliarComputer.getBackFaceFluorescenceMatrix() = foliarComputer.getFrontFaceFluorescenceMatrix();
				foliarComputer.setBottomFluorescenceComputation(true);
				outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticUsage() << (topFluorescenceResults.useBioClimaticProfileEta ? "1" : "0") << endl;
			}
		}
		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		if (foliarComputer.isTopFluorescenceComputation() || foliarComputer.isBottomFluorescenceComputation()) {
			outCoeffSpecProperties << pkmg.cleUseFluorescence() << "1" << endl;
		} else {
			outCoeffSpecProperties << pkmg.cleUseFluorescence() << "0" << endl;
		}
		if (foliarComputer.isTopFluorescenceComputation()) {
			if (topFluorescenceResults.useBioClimaticProfileEta) {
				string topBCFilePath = DartEnv::getFilePathForRead(INPUT_DIR, topFluorescenceResults.bioClimaticFileRelativePath);
				vector<double> topBioClimaticThetaValues, topBioClimaticPhiValues;
				vector<vector<vector<double> > > topBioClimaticProfileEta;
				readBioClimaticFile(topBCFilePath, topBioClimaticThetaValues, topBioClimaticPhiValues, topBioClimaticProfileEta);
				string topAverageBioClimaticProfileFileName = "top_average_eta_" + std::to_string(modelIndex) + ".txt";
				string topAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + topAverageBioClimaticProfileFileName;
				ofstream ofstreamTopAverageEta;
				ofstreamTopAverageEta.open(topAverageBioClimaticProfileFilePath.c_str(), ofstream::app);
				computeAndWriteAverageBioClimaticProfile(modelIndex, foliarComputer, topBioClimaticThetaValues, topBioClimaticPhiValues, topBioClimaticProfileEta, ofstreamTopAverageEta);
				outCoeffSpecProperties << pkmg.cleTopFluorescenceBioClimaticFile() << topAverageBioClimaticProfileFileName << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceTopBioClimaticLinkedPropertyType() << topFluorescenceResults.linkedOpticalPropertyType << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceTopBioClimaticLinkedPropertyIndex() << topFluorescenceResults.linkedOpticalPropertyIndex << endl;
			}
		}
		if (foliarComputer.isBottomFluorescenceComputation()) {
			if (bottomFluorescenceResults.useBioClimaticProfileEta) {
				string bottomBCFilePath = DartEnv::getFilePathForRead(INPUT_DIR, bottomFluorescenceResults.bioClimaticFileRelativePath);
				vector<double> bottomBioClimaticThetaValues, bottomBioClimaticPhiValues;
				vector<vector<vector<double> > > bottomBioClimaticProfileEta;
				readBioClimaticFile(bottomBCFilePath, bottomBioClimaticThetaValues, bottomBioClimaticPhiValues, bottomBioClimaticProfileEta);
				string bottomAverageBioClimaticProfileFileName = "bottom_average_eta_" + std::to_string(modelIndex) + ".txt";
				string bottomAverageBioClimaticProfileFilePath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + bottomAverageBioClimaticProfileFileName;
				ofstream ofstreamBottomAverageEta;
				ofstreamBottomAverageEta.open(bottomAverageBioClimaticProfileFilePath.c_str(), ofstream::app);
				computeAndWriteAverageBioClimaticProfile(modelIndex, foliarComputer, bottomBioClimaticThetaValues, bottomBioClimaticPhiValues, bottomBioClimaticProfileEta, ofstreamBottomAverageEta);
				outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticFile() << bottomAverageBioClimaticProfileFileName << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyType() << bottomFluorescenceResults.linkedOpticalPropertyType << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyIndex() << bottomFluorescenceResults.linkedOpticalPropertyIndex << endl;
			}
		}
	}

	if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		foliarModel.setEquivalentFrontLambertianIndex(foliarIndice, PhaseProperty::getNewEquivalentLambertianModelIndex());
		if (!hasDifferentBottom) {
			foliarModel.setEquivalentBackLambertianIndex(foliarIndice, foliarModel.getEquivalentFrontLambertianIndex(foliarIndice));
		} else {
			foliarModel.setEquivalentBackLambertianIndex(foliarIndice, PhaseProperty::getNewEquivalentLambertianModelIndex());
		}

		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		outCoeffSpecProperties << pkmg.cleEquivalentFrontLambertianIndex() << (foliarModel.getEquivalentFrontLambertianIndex(foliarIndice)) << endl;
		outCoeffSpecProperties << pkmg.cleEquivalentBackLambertianIndex() << (foliarModel.getEquivalentBackLambertianIndex(foliarIndice)) << endl;
		pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
		pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentFrontLambertianIndex(foliarIndice));
		_fluorescenceService.writeSimulationProperties(topFluorescenceResults, pkmg, outCoeffSpecProperties);
		if (foliarModel.getEquivalentBackLambertianIndex(foliarIndice) != foliarModel.getEquivalentFrontLambertianIndex(foliarIndice)) {
			pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentBackLambertianIndex(foliarIndice));
			_fluorescenceService.writeSimulationProperties(bottomFluorescenceResults, pkmg, outCoeffSpecProperties);
		}
	}

	string pathPhaseSpec = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_SpecularFunction";
	string pathPhaseDiff = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_ScatteringFunction";
	string pathPhaseTransm = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
			+ "_TransmittanceFunction";
	string pathPhaseHotSpotFactor = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
			+ "_ThermalHotSpot";
	string pathPhaseFluorescence = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name + "_fluorescence";

	PointsListsNCIntegration<DirectionCartesienne> pointsListSpec;
	PointsListsNCIntegration<DirectionCartesienne> pointsListDiff;
	foliarComputer.computeFRFoliarSpecularForModelIndex(pointsListSpec, pointsListDiff, listDirections, fTFoliar, foliarIndice, foliarProperty[0]);
	foliarComputer.computeClumpingIndex(listDirections, genDTO.solarDirection, foliarIndice);
	foliarComputer.runComputeFoliarPerThread(foliarProperty, foliarIndice, fTFoliar, foliarPropertyParameters, listDirections, *_sphereMap,
		pointsListSpec, pointsListDiff);
	foliarComputer.computeFoliarHotSpotDirectionalFactor(foliarIndice, listDirections, fTFoliar.iterationStepPhi, fTFoliar.iterationStepTheta);

	/*
	 * Nous avons deux cas : soit on ecrase le fichier car on est au premier passage de boucle,
	 * soit on continue le remplissage du fichier
	 */
	std::ostringstream keyRootScatObj;
	keyRootScatObj << "scene.vegscatter." << "vegetation_" <<  modelIndex << ".";
	if (PhaseProperty::getFluorescence() && topModel != nullptr && topModel->prospectExternalModule.isFluorescent) {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "type", "homogeneous_vegetation_fluo");
	} else {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "type", "homogeneous_vegetation");
	}
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "opIndex", std::to_string(modelIndex));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "leafdimension", std::to_string(foliarModel.getFoliarDimension(foliarIndice)));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "LAD", std::to_string(foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarIndice)));
	if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarIndice) == globalReferences::ELLIPS)
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "ala", std::to_string(foliarModel.getFoliarAverageLeafAngle(foliarIndice)));

	foliarComputer.writeTransmittance(fTFoliar, listDirections, genDTO.radiativeTransferMethod, 2, foliarProperty[0].specularFoliarData._isSpecular,
		pathPhaseTransm, pathPhaseDiff, pathPhaseSpec, pathPhaseHotSpotFactor, pathPhaseFluorescence);
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "transferFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_ScatteringFunction" + "_mc");
	if (PhaseProperty::getFluorescence() && topModel != nullptr && topModel->prospectExternalModule.isFluorescent) {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "fluorescenceTransferFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_fluorescence" + "_mc");
	}
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "transmFunctionPath", PhaseUtils::getOutputFolder() + pathSeparator + name + "_TransmittanceFunction");
	genDTO.luxCoreRenderSceneProperties.ajouterCle("scene.simPropertiesPath", "simulation.properties.txt");
	if (PhaseProperty::getFluorescence() && topModel != nullptr && topModel->prospectExternalModule.isFluorescent) {
		if (topFluorescenceResults.useBioClimaticProfileEta) {
			const string topAverageBioClimaticProfileFileName = "top_average_eta_" + std::to_string(modelIndex) + ".txt";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topUseBioClimaticProfileEta", "1");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topBioClimaticFilePath", PhaseUtils::getOutputFolder() + pathSeparator + topAverageBioClimaticProfileFileName);
		} else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "topUseBioClimaticProfileEta", "0");
		}
		if (bottomFluorescenceResults.useBioClimaticProfileEta) {
			const string bottomAverageBioClimaticProfileFileName = "bottom_average_eta_" + std::to_string(modelIndex) + ".txt";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomUseBioClimaticProfileEta", "1");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomBioClimaticFilePath", PhaseUtils::getOutputFolder() + pathSeparator + bottomAverageBioClimaticProfileFileName);
		} else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "bottomUseBioClimaticProfileEta", "0");
		}
	}

	OpticalFactorParameters opticalFactorParameters;
	if (PhaseDAO::readCoeffDiffParametersForFoliarOpticalFactors(foliarIndice, opticalFactorParameters)) {
		std::string textureName = "v" + boost::lexical_cast<std::string>(foliarIndice) + "OF";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "opticalFactors", textureName);

		std::string textureKeyRoot = "scene.textures." + textureName;
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", textureName + ".bin");

		if (opticalFactorParameters.sameOpticalFactorForAllBands) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", opticalFactorParameters.fileNames[0]);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", opticalFactorParameters.duplicateFirstLayer[0] ? "1" : "0");
		} else {
			std::ostringstream inputsOSS, duplicationOSS;
			inputsOSS << opticalFactorParameters.fileNames[0];
			duplicationOSS << opticalFactorParameters.duplicateFirstLayer[0];
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
				duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
			}

			if (PhaseProperty::getPolarization()) {
				// 3 * numberOfSpectralBands are added to simulate Q, U, V
				// The optical properties are repeated in these bands
				for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
						duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
					}
				}
			}

			if (PhaseProperty::getFluorescence()) {
				// 2 * numberOfSpectralBands are added to simulate PSI and PSII
				// The optical properties are repeated in these bands
				for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
						inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
						duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
					}
				}
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
		}
	}
	foliarComputer.writeTransmittance(fTFoliar, listDirections, genDTO.radiativeTransferMethod, 2, foliarProperty[0].specularFoliarData._isSpecular,
		pathPhaseTransm, pathPhaseDiff, pathPhaseSpec, pathPhaseHotSpotFactor, pathPhaseFluorescence);

	std::vector<PropertyLambertian> frontEquivalentLambertianProperties(genDTO.numberOfSpectralBands);
	std::vector<PropertyLambertian> backEquivalentLambertianProperties(genDTO.numberOfSpectralBands);
	unsigned int numberOfFresnelCoefficient = 20;
	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		foliarComputer.testAlbedo(spectralBandIndex, foliarIndice);
		foliarComputer.writeFoliar(foliarIndice, spectralBandIndex, foliarPropertyParameters.getModelName(), pkmg, outCoeffSpecProperties,
			foliarProperty);
		if (PhaseProperty::getPolarization()) {
			PhasePolarization::computeAndWriteFoliarPolarizationMatrix(genDTO, foliarPropertyParameters, spectralBandIndex, listDirections, genDTO.solarDirection,
				numberOfFresnelCoefficient, foliarModel, ladComputation);
		}
		if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
			frontEquivalentLambertianProperties[spectralBandIndex].reflectance = foliarProperty[spectralBandIndex].reflectanceTop;
			frontEquivalentLambertianProperties[spectralBandIndex].directTransmittance = 0.;
			frontEquivalentLambertianProperties[spectralBandIndex].diffuseTransmittance = foliarProperty[spectralBandIndex].transmittance;
			backEquivalentLambertianProperties[spectralBandIndex].reflectance = foliarProperty[spectralBandIndex].reflectanceBottom;
			backEquivalentLambertianProperties[spectralBandIndex].directTransmittance = 0.;
			backEquivalentLambertianProperties[spectralBandIndex].diffuseTransmittance = foliarProperty[spectralBandIndex].bottomTransmittance;
		}
	}

	/* Equivalent lambertian phase functions generated from the foliar model: the LuxCore
	 * material keys are written by the same writer as for real lambertians (the emissivity
	 * is padded with zeros on the extra simulated Q,U,V / PSI,PSII bands). Thermal keys
	 * are handled separately by the EmissionMaterial. */
	if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		const phase::LambertianMaterialWriter lambertianMaterialWriter(_fluorescenceService, _opticalFactorsWriter);

		const unsigned int frontEquivalentLambertianIndex = foliarModel.getEquivalentFrontLambertianIndex(foliarIndice);
		const unsigned int backEquivalentLambertianIndex = foliarModel.getEquivalentBackLambertianIndex(foliarIndice);

		lambertianMaterialWriter.writeLuxCoreMaterial(genDTO, frontEquivalentLambertianIndex, frontEquivalentLambertianIndex,
				frontEquivalentLambertianProperties, topFluorescenceResults);

		if (backEquivalentLambertianIndex != frontEquivalentLambertianIndex) {
			lambertianMaterialWriter.writeLuxCoreMaterial(genDTO, backEquivalentLambertianIndex, backEquivalentLambertianIndex,
					backEquivalentLambertianProperties, bottomFluorescenceResults);
		}

		/* Optical factors of the equivalent lambertians come from the foliar model */
		OpticalFactorParameters opticalFactorParameters;
		if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(foliarIndice, opticalFactorParameters)) {
			const std::string textureName = "v" + boost::lexical_cast<std::string>(foliarIndice) + "OF";
			genDTO.luxCoreRenderSceneProperties.ajouterCle(
					"scene.materials.lambertian" + std::to_string(frontEquivalentLambertianIndex) + ".opticalFactors", textureName);
			if (backEquivalentLambertianIndex != frontEquivalentLambertianIndex) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(
						"scene.materials.lambertian" + std::to_string(backEquivalentLambertianIndex) + ".opticalFactors", textureName);
			}
		}
	}
	return true;
}

} // namespace phase
