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

/**
 * Decides, for each spectral band, whether the reflectance values must be
 * weighted by the sun spectral irradiance (PhaseOld rule minus the LiDAR
 * term; same as SurfaceMaterialProcessor::computeUseSpectralIrradiance).
 */
static void testIfUseSun(const PhaseGeneralParametersDTO & genDTO,
		std::vector<bool> & useSpectralIrradianceForReflectance) {
	useSpectralIrradianceForReflectance.assign(genDTO.numberOfSpectralBands, false);
	for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; ++bandIndex) {
		useSpectralIrradianceForReflectance[bandIndex] =
				PhaseProperty::getWeightingOfReflectanceParameters()
				&& PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0;
	}
}

/**
 * Direction set used by one vegetation model: its own DirectionSetLink when
 * valid, else the first direction set.
 */
static unsigned int getUnderstoryDirectionSetIndex(const PhaseGeneralParametersDTO & genDTO, const unsigned int modelPosition) {
	const unsigned int indexDirectionSetLink = genDTO._volumesMaterials.understoryMultiFunctions[modelPosition]->directionSetLink.indexDirectionSetLink;
	if (indexDirectionSetLink < genDTO._directionSet.size()) {
		return indexDirectionSetLink;
	}
	return 0;
}

static void fillFoliarModelFaceMultiplicativeFactors(FoliarModel & foliarModel,
		const phsdata::UnderstoryModel * model, const bool isBottomFace, const unsigned int numberOfBands) {
	if (model == nullptr || !model->useMultiplicativeFactorForLUT || !model->nodeMultiplicativeFactor.is_initialized())
		return;
	const phsdata::MultiplicativeFactorList & mfl = *model->nodeMultiplicativeFactor;
	for (unsigned int sbi = 0; sbi < numberOfBands && sbi < mfl.factors.size(); ++sbi) {
		auto umf = std::static_pointer_cast<const phsdata::UnderstoryMultiplicativeFactor>(mfl.factors[sbi]);
		if (isBottomFace)
			foliarModel.setAbaxialReflectanceForFoliar(sbi, umf->reflectanceFactor);
		else
			foliarModel.setAdaxialReflectanceForFoliar(sbi, umf->reflectanceFactor);
		foliarModel.setLeafTransmittanceForFoliar(sbi, umf->leafTransmittanceFactor, isBottomFace);
	}
}

static void fillFoliarModelMultiplicativeFactors(const phsdata::UnderstoryMulti & understory, FoliarModel & foliarModel) {
	const unsigned int numberOfBands = PhaseProperty::getNumberOfSpectralBands();
	const phsdata::UnderstoryModel * topModel = understory.getTopModel();
	const phsdata::UnderstoryModel * bottomModel = understory.getBottomModel();
	if (bottomModel == nullptr)
		bottomModel = topModel;
	fillFoliarModelFaceMultiplicativeFactors(foliarModel, topModel, false, numberOfBands);
	fillFoliarModelFaceMultiplicativeFactors(foliarModel, bottomModel, true, numberOfBands);
}

/**
 * (Re)initialises the shared foliar objects for one direction set and runs the
 * direction-set-dependent precomputing. Called by VegetationMaterialWriter
 * whenever the direction set changes between models (the per-model data is
 * reset by initFoliarModel, so it is recomputed in treat(), per model).
 */
static void prepareVegetationDirectionSet(PhaseGeneralParametersDTO & genDTO,
		FoliarModel & foliarModel,
		FoliarComputation & foliarComputation,
		LADComputation & ladComputation,
		FTFoliar & fTFoliar,
		const unsigned int directionSetIndex) {
	const unsigned int numberOfFoliarModels = genDTO._volumesMaterials.understoryMultiFunctions.size();
	const vector<DirectionCartesienne> & listDirections = genDTO._directionSet[directionSetIndex]._directions;
	const unsigned int numberOfDirections = listDirections.size();

	foliarModel.initFoliarModel(PhaseProperty::_numberOfSpectralBands,
			numberOfDirections);

	foliarComputation.initFoliarComputation(numberOfDirections,
			PhaseProperty::_numberOfSpectralBands, numberOfFoliarModels, PhaseProperty::_numberOfThreads,
			PhaseProperty::_threadEnd, foliarModel);

	ladComputation.initLADComputation();

    PhaseProperty::setParametersForPrintingProgress();
    PhaseProperty::resizePhaseParametersVectors();

	readMultiplicativeFactor(genDTO);
    printNumberOfFoliarModels();

    int iterationStepPhi = genDTO._volumesMaterials.integrationStepOnPhi;
    int iterationStepTheta = genDTO._volumesMaterials.integrationStepOnTheta;

	foliarComputation.computeFoliarCosinePsi(listDirections, iterationStepPhi, iterationStepTheta);

    /* FOLIAR precomputing */
    if (PhaseProperty::getNumberOfFoliarModels() > 0) {
		cout << "\tProcessing Vegetation models" << endl;
    }
	double solidAnglesSum = 0.;
	numberOfAngularSectors(listDirections, solidAnglesSum);
	fTFoliar = foliarPrecomputing(genDTO, listDirections, iterationStepTheta, iterationStepPhi, solidAnglesSum);

    if (PhaseProperty::getFluorescence()) {
        PhaseProperty::initYields();
        if (PhaseProperty::getNumberOfFoliarModels() > 0) {
			foliarComputation.initFluorescenceMatrix();
        }
    }
}

/**
 * Copies the LAD parameters of ONE vegetation model from its phsdata storage
 * (UnderstoryMulti) into the per-model FoliarModel parameters and computes its
 * leaf angle distribution into the single-slot LAD vectors of FoliarModel.
 * Called from VegetationMaterialWriter::treat() for the model currently being
 * treated, so no other model's LAD data is ever stored (single-slot storage).
 * outputModelIndex (XML indexFunction) is only used to name the optional LAD
 * output file.
 */
static void computeLadDataForModel(const PhaseGeneralParametersDTO & genDTO,
				LADComputation & ladComputation,
				FoliarModel & foliarModel,
				const phsdata::UnderstoryMulti & understory,
				const unsigned int & outputModelIndex) {
	ofstream writeOutputLeafAngleDistribution;
	ofstream writeOutputElliptical;

	// Port the phsdata storage into the per-model FoliarModel parameters
	foliarModel.setFoliarTypeOfLeafAngleDistribution(understory.lad);
	foliarModel.setFoliarClumpingA(understory.clumpingA);
	foliarModel.setFoliarClumpingB(understory.clumpingB);
	foliarModel.setFoliarOmegaMin(understory.omegaMin);
	foliarModel.setFoliarOmegaMax(understory.omegaMax);
	foliarModel.setFoliarDimension(understory.dimFoliar);
	foliarModel.setHotSpotFactor(understory.thermalHotSpotFactor);

	switch (understory.lad) {
		case globalReferences::ELLIPS:
			if (understory.ellipsoidalAla.is_initialized() && understory.ellipsoidalAla.value() >= 1e-10) {
				foliarModel.setFoliarAverageLeafAngle(understory.ellipsoidalAla.value());
				ladComputation.computeEllipsoidalLadDistribution(understory.ellipsoidalAla.value());
			} else {
				foliarModel.setFoliarTypeOfLeafAngleDistribution(globalReferences::HORIZ);
				ladComputation.computeVerticalHorizontalLadDistribution(0);
			}
			break;
		case globalReferences::BOUNDEDUNI:
			foliarModel.setNumberOfAla(understory.boundedUniformNbAla.value_or(1));
			foliarModel.setNthAla(understory.boundedUniformNthAla.value_or(1));
			ladComputation.computeBoundedUniformLadDistribution(foliarModel.getNumberOfAla(),
					foliarModel.getNthOfAla());
			break;

		case globalReferences::ELLIPT:
			foliarModel.setFoliarEllipsePrincipalAngle(understory.ellipticalThetaM.value_or(0.));
			foliarModel.setFoliarEllipseEccentricity(understory.ellipticalEpsilon.value_or(0.));
			ladComputation.computeEllipticalLadDistribution(foliarModel.getFoliarEllipsePrincipalAngle(),
					foliarModel.getFoliarEllipseEccentricity());
			ladComputation.writeEllipticalLad(writeOutputElliptical);
			break;
		case globalReferences::HORIZ:
			foliarModel.setFoliarTypeOfLeafAngleDistribution(globalReferences::HORIZ);
			ladComputation.computeVerticalHorizontalLadDistribution(0);
			break;
		case globalReferences::VERTI:
			ladComputation.computeVerticalHorizontalLadDistribution(90);
			break;
		case globalReferences::MANUAL: // De Wit
			ladComputation.computeClassicalLadDistribution(understory.manualLad.m, understory.manualLad.b0,
					understory.manualLad.b1, understory.manualLad.b2);
			break;
		case globalReferences::USERDEFINED: {
			const std::string nameDatabaseLAD = understory.userDefinedLadFileName.value_or("");
			const std::string ladFileName = DartEnv::getDatabasePath(nameDatabaseLAD) + nameDatabaseLAD;
			ladComputation.readLadFile(ladFileName);
			break;
		}
		default:
			ladComputation.getLadFile(understory.lad);
			break;
	}
	if (genDTO.isAskedForWritingLadResultsCheck == 1) {
		ladComputation.writeLad(outputModelIndex, writeOutputLeafAngleDistribution,
				foliarModel.getFoliarTypeOfLeafAngleDistribution());
	}
}

static void readBioClimaticFile(	string bcFilePath, vector<double> & bioClimaticThetaValues,
						vector<double> & bioClimaticPhiValues,
						vector<vector<vector<double> > > & bioClimaticProfileEta) {
	bioClimaticThetaValues.clear();
	bioClimaticPhiValues.clear();
	bool binary = boost::algorithm::ends_with(bcFilePath, ".bin");
	ifstream bcStream;
	if (binary) {
		bcStream.open(bcFilePath.c_str(), ifstream::in | ifstream::binary);
	} else {
		bcStream.open(bcFilePath.c_str(), ifstream::in);
	}
	if (bcStream) {
		unsigned int nbLayer, nbTheta, nbPhi;
		if (binary) {
			bcStream.read((char*) &nbLayer, sizeof(unsigned int));
			bcStream.read((char*) &nbTheta, sizeof(unsigned int));
			bcStream.read((char*) &nbPhi, sizeof(unsigned int));
		} else {
			bcStream >> nbLayer >> nbTheta >> nbPhi;
		}
		if (nbTheta != 1) {
			// Recover the median value of each range
			bioClimaticThetaValues.resize(nbTheta);
			for (unsigned int i = 0; i < nbTheta; ++i) {
				if (binary) {
					bcStream.read((char*) &bioClimaticThetaValues[i], sizeof(double));
				} else {
					bcStream >> bioClimaticThetaValues[i];
				}
			}
		}
		if (nbPhi != 1) {
			// Recover the median value of each range
			bioClimaticPhiValues.resize(nbPhi);
			for (unsigned int i = 0; i < nbPhi; ++i) {
				if (binary) {
					bcStream.read((char*) &bioClimaticPhiValues[i], sizeof(double));
				} else {
					bcStream >> bioClimaticPhiValues[i];
				}
			}
		}
		bioClimaticProfileEta.clear();
		bioClimaticProfileEta.resize(nbLayer, std::vector<std::vector<double> >(nbTheta, std::vector<double>(nbPhi)));
		for (unsigned int l = 0; l < nbLayer; ++l) {
			for (unsigned int t = 0; t < nbTheta; ++t) {
				for (unsigned int p = 0; p < nbPhi; ++p) {
					if (binary) {
						bcStream.read((char*) &bioClimaticProfileEta[l][t][p], sizeof(double));
					} else {
						bcStream >> bioClimaticProfileEta[l][t][p];
					}
				}
			}
		}
		bcStream.close();
	}
}

static void computeAndWriteAverageBioClimaticProfile(	const FoliarComputation & foliarComputer,
												const vector<double> & centralBioClimaticThetaValues,
												const vector<double> & centralBioClimaticPhiValues,
												const vector<vector<vector<double> > > & bioClimaticProfileEta,
												ofstream & ofstreamAverageEta) {
	vector<double> averageBioClimaticProfileEta;
	unsigned int nbLayers = bioClimaticProfileEta.size(), nbTheta = centralBioClimaticThetaValues.size(), nbPhi = centralBioClimaticPhiValues.size();
	nbTheta = max(1, nbTheta);
	nbPhi = max(1, nbPhi);
	if (nbTheta == 1 && nbPhi == 1) {
		// nbTheta == 1 && nbPhi == 1 => The biochemical profile eta is aleady an average
		averageBioClimaticProfileEta.resize(nbLayers);
		for (unsigned int i = 0; i < nbLayers; i++) {
			averageBioClimaticProfileEta[i] = bioClimaticProfileEta[i][0][0];
		}
	}
	else {
		// We compute an average profile of bioclimatic eta per layer
		averageBioClimaticProfileEta.resize(nbLayers);
		vector<double> thetaValues, phiValues;
		thetaValues.resize(nbTheta + 1);
		phiValues.resize(nbPhi + 1);
		thetaValues[0] = 0;
		thetaValues[1] = 2 * centralBioClimaticThetaValues[0];
		phiValues[0] = 0;
		phiValues[1] = 2 * centralBioClimaticPhiValues[0];
		for (unsigned int i = 2; i < thetaValues.size(); i++) {
			thetaValues[i] = 2 * centralBioClimaticThetaValues[i - 1] - thetaValues[i - 1];
		}
		for (unsigned int i = 2; i < phiValues.size(); i++) {
			phiValues[i] = 2 * centralBioClimaticPhiValues[i - 1] - phiValues[i - 1];
		}
		vector<double> dThetaValues, dPhiValues;
		dThetaValues.resize(nbTheta);
		dPhiValues.resize(nbPhi);
		for (unsigned int i = 0; i < dThetaValues.size(); i++) {
			dThetaValues[i] = thetaValues[i+1] - thetaValues[i];
		}
		for (unsigned int i = 0; i < dPhiValues.size(); i++) {
			dPhiValues[i] = phiValues[i+1] - phiValues[i];
		}
		for (unsigned int layer = 0; layer < nbLayers; layer++) {
			double average = 0.0;
			for (unsigned int phiIndex = 0; phiIndex < nbPhi; phiIndex++) {
				double thetaIntegral = 0.0;
				for (unsigned int thetaIndex = 0; thetaIndex < nbTheta; thetaIndex++) {
					thetaIntegral += bioClimaticProfileEta[layer][thetaIndex][phiIndex] *
							foliarComputer.getLeafAngleDistributionForAngle(centralBioClimaticThetaValues[thetaIndex]) *
							dThetaValues[thetaIndex];
				}
				average += thetaIntegral * dPhiValues[phiIndex];
			}
			averageBioClimaticProfileEta[layer] = average / 360;
		}
	}
	if (ofstreamAverageEta) {
		ofstreamAverageEta << nbLayers << " 1 1" << std::endl;
		for (unsigned int layer = 0; layer < nbLayers; layer++){
			ofstreamAverageEta << averageBioClimaticProfileEta[layer] << std::endl;
		}
	}
	ofstreamAverageEta.close();

}

VegetationMaterialWriter::VegetationMaterialWriter(	const FluorescenceService & fluorescenceService,
							const OpticalFactorsTextureWriter & opticalFactorsWriter) :
		_fluorescenceService(fluorescenceService), _opticalFactorsWriter(opticalFactorsWriter) {}

void VegetationMaterialWriter::processAll(	PhaseGeneralParametersDTO & genDTO,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties) {
	const std::vector<std::shared_ptr<phsdata::UnderstoryMulti> > & models = genDTO._volumesMaterials.understoryMultiFunctions;
	if (models.empty())
		return;

	// FoliarComputation and LADComputation hold references into foliarModel:
	// foliarModel is declared first so that it is destroyed last.
	FoliarModel foliarModel;
	FoliarComputation foliarComputation(foliarModel);
	LADComputation ladComputation(foliarModel);
	FTFoliar fTFoliar;
	std::vector<bool> useSpectralIrradianceForReflectance;
	testIfUseSun(genDTO, useSpectralIrradianceForReflectance);

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
			prepareVegetationDirectionSet(genDTO, foliarModel, foliarComputation, ladComputation, fTFoliar, directionSetIndex);
			lastDirectionSetIndex = directionSetIndex;
			if (!_sphereMap)
				_sphereMap.reset(new SphereDiscretizationMap(1.0, fTFoliar.iterationStepPhi));
		}
		treat(*models[position], position, genDTO, foliarModel, foliarComputation, ladComputation, fTFoliar,
				useSpectralIrradianceForReflectance, genDTO._directionSet[directionSetIndex]._directions, pkmg, outCoeffSpecProperties);
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
				FoliarModel & foliarModel,
				FoliarComputation & foliarComputation,
				LADComputation & ladComputation,
				FTFoliar & fTFoliar,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
						const std::vector<DirectionCartesienne> & listDirections,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties) {
	FoliarComputation & foliarComputer = foliarComputation;

	const unsigned int modelIndex = understory.indexFunction;
	const phsdata::UnderstoryModel * topModel = understory.getTopModel();
	const phsdata::UnderstoryModel * bottomModel = understory.getBottomModel();
	if (topModel == nullptr)
		return false;

	foliarComputer.reInitFoliarComputation();

	// Per-model LAD: read the LAD parameters of this model and compute its leaf
	// angle distribution into the single-slot LAD vectors (no other model's LAD
	// data is stored). Must run before the getFoliarTypeOfLeafAngleDistribution()
	// reads below (the ELLIPS/ALA==0 case rewrites the type to HORIZ).
	computeLadDataForModel(genDTO, ladComputation, foliarModel, understory, modelIndex);
	fillFoliarModelMultiplicativeFactors(understory, foliarModel);

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
			PhaseProperty::readReflectanceParameters(useSpectralIrradianceForReflectance, currentDatabase);
		}
	}

	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		foliarProperty[spectralBandIndex].typeOfLeafAngleDistribution = foliarModel.getFoliarTypeOfLeafAngleDistribution();
		foliarProperty[spectralBandIndex].foliarDimension = foliarModel.getFoliarDimension();
	}
	fTFoliar.casPDF = ladComputation.initialisationLadProbabilityDensityFunctions();
	ostringstream val;
	val << modelIndex; //on ajoute le numero de la fx de phase dans le nom du fichier : sprintf (val, "%u.",j)
	string name = val.str() + "." + foliarPropertyParameters.getModelName();

	createDirectory(DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder());

	pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
	pkmg.setNumeroFonctionDePhase(modelIndex);
	outCoeffSpecProperties << pkmg.cleThermalHotSpotFactor() << foliarModel.getHotSpotFactor() << endl;
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
				computeAndWriteAverageBioClimaticProfile(foliarComputer, topBioClimaticThetaValues, topBioClimaticPhiValues, topBioClimaticProfileEta, ofstreamTopAverageEta);
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
				computeAndWriteAverageBioClimaticProfile(foliarComputer, bottomBioClimaticThetaValues, bottomBioClimaticPhiValues, bottomBioClimaticProfileEta, ofstreamBottomAverageEta);
				outCoeffSpecProperties << pkmg.cleBottomFluorescenceBioClimaticFile() << bottomAverageBioClimaticProfileFileName << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyType() << bottomFluorescenceResults.linkedOpticalPropertyType << endl;
				outCoeffSpecProperties << pkmg.cleFluorescenceBottomBioClimaticLinkedPropertyIndex() << bottomFluorescenceResults.linkedOpticalPropertyIndex << endl;
			}
		}
	}

	if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		foliarModel.setEquivalentFrontLambertianIndex(PhaseProperty::getNewEquivalentLambertianModelIndex());
		if (!hasDifferentBottom) {
			foliarModel.setEquivalentBackLambertianIndex(foliarModel.getEquivalentFrontLambertianIndex());
		} else {
			foliarModel.setEquivalentBackLambertianIndex(PhaseProperty::getNewEquivalentLambertianModelIndex());
		}

		pkmg.setTypeFonctionDePhase(globalReferences::FOLIAIRE);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		outCoeffSpecProperties << pkmg.cleEquivalentFrontLambertianIndex() << (foliarModel.getEquivalentFrontLambertianIndex()) << endl;
		outCoeffSpecProperties << pkmg.cleEquivalentBackLambertianIndex() << (foliarModel.getEquivalentBackLambertianIndex()) << endl;
		pkmg.setTypeFonctionDePhase(globalReferences::LAMBERTIEN);
		pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentFrontLambertianIndex());
		_fluorescenceService.writeSimulationProperties(topFluorescenceResults, pkmg, outCoeffSpecProperties);
		if (foliarModel.getEquivalentBackLambertianIndex() != foliarModel.getEquivalentFrontLambertianIndex()) {
			pkmg.setNumeroFonctionDePhase(foliarModel.getEquivalentBackLambertianIndex());
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
	foliarComputer.computeClumpingIndex(listDirections, genDTO.solarDirection);
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
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "leafdimension", std::to_string(foliarModel.getFoliarDimension()));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "LAD", std::to_string(foliarModel.getFoliarTypeOfLeafAngleDistribution()));
	if (foliarModel.getFoliarTypeOfLeafAngleDistribution() == globalReferences::ELLIPS)
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "ala", std::to_string(foliarModel.getFoliarAverageLeafAngle()));

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

	// Optical factors texture of the foliar model, written directly by Phase via
	// OpticalFactorsTextureWriter (like the surface materials), from the phsdata storage.
	std::string opticalFactorsTextureName;
	if (topModel->nodeMultiplicativeFactor.is_initialized()) {
		const phsdata::MultiplicativeFactorList & mfl = *topModel->nodeMultiplicativeFactor;
		bool useOpticalFactorMatrix = mfl.useSameOpticalFactorMatrixForAllBands && mfl.bandCommonOpticalFactors.is_initialized();
		if (!useOpticalFactorMatrix) {
			const unsigned int nbFactors = std::min<unsigned int>(genDTO.numberOfSpectralBands, mfl.factors.size());
			for (unsigned int sbi = 0; sbi < nbFactors && !useOpticalFactorMatrix; sbi++)
				useOpticalFactorMatrix = mfl.factors[sbi]->useOpticalFactorMatrix && mfl.factors[sbi]->opticalFactorMatrix.is_initialized();
		}
		if (useOpticalFactorMatrix) {
			opticalFactorsTextureName = "v" + boost::lexical_cast<std::string>(foliarIndice) + "OF";
			const boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
			if (!outputDir.empty() && _opticalFactorsWriter.writeTexture(genDTO, opticalFactorsTextureName, mfl, outputDir.string()))
				genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRootScatObj.str() + "opticalFactors", opticalFactorsTextureName);
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
			// The LAD of the current model was already computed once by
			// computeLadDataForModel() above; no recompute here.
			PhasePolarization::computeAndWriteFoliarPolarizationMatrix(genDTO, foliarPropertyParameters, spectralBandIndex, listDirections, genDTO.solarDirection,
				numberOfFresnelCoefficient, foliarModel);
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

		const unsigned int frontEquivalentLambertianIndex = foliarModel.getEquivalentFrontLambertianIndex();
		const unsigned int backEquivalentLambertianIndex = foliarModel.getEquivalentBackLambertianIndex();

		lambertianMaterialWriter.writeLuxCoreMaterial(genDTO, frontEquivalentLambertianIndex, frontEquivalentLambertianIndex,
				frontEquivalentLambertianProperties, topFluorescenceResults);

		if (backEquivalentLambertianIndex != frontEquivalentLambertianIndex) {
			lambertianMaterialWriter.writeLuxCoreMaterial(genDTO, backEquivalentLambertianIndex, backEquivalentLambertianIndex,
					backEquivalentLambertianProperties, bottomFluorescenceResults);
		}

		/* Optical factors of the equivalent lambertians: same foliar texture as the
		 * vegscatter one above, read from the phsdata storage. (The old code wrongly
		 * read the LAMBERTIAN optical factors with the foliar model index.) */
		if (!opticalFactorsTextureName.empty()) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(
					"scene.materials.lambertian" + std::to_string(frontEquivalentLambertianIndex) + ".opticalFactors", opticalFactorsTextureName);
			if (backEquivalentLambertianIndex != frontEquivalentLambertianIndex) {
				genDTO.luxCoreRenderSceneProperties.ajouterCle(
						"scene.materials.lambertian" + std::to_string(backEquivalentLambertianIndex) + ".opticalFactors", opticalFactorsTextureName);
			}
		}
	}
	return true;
}

} // namespace phase
