/*! Time-stamp: <@(#) DART_DIR_ReadPhaseParameters.c   10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : DART_DIR_ReadDirectionsParameters.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description :  Implementation des Nouvelles methodes d'acces aux parametres Phase
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.4.0 21/04/2008  MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture)
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0  07/03/2005  VBT : MAG-TT-004 : Implementation nouvelles fonctions de lecture des parametres Phase
 *
 *********************************************************************
 * CVS $Log : DART_DIR_ReadPhaseParameters.c,c$
 */
#include <fstream>

#include "../include/PhaseDAO.hpp"
#include "Phase.hpp"
#include "GeneralDef.hpp"

//#include "../../libLog/include/log4cplus/logger.hapkeParameterh"
//#include "../../libLog/include/log4cplus/configurator.hapkeParameterh"

bool PhaseDAO::_useYaML = true;
DartInputs::DartInputsDAO PhaseDAO::_dartInputsDAO;

/**!
 * Permet de ne faire qu'une seule initialisation des fichiers pour ceux que l'on ouvre plusieurs fois.
 */
void PhaseDAO::initPhase(bool fromStream) {
	DartSAParameters::initialize(DartEnv::getCurrentSimulationInputPath());
	// ouverture des fichiers
	if (_useYaML) {
		initYaMLs();
	} else {
		initXMLs(fromStream);
	}
}

void PhaseDAO::initXMLs(bool fromStream) {
	if (!fromStream) {
		DartSAParameters::openFile("atmosphere.xml");
		DartSAParameters::openFile("directions.xml");
		DartSAParameters::openFile("coeff_diff.xml");
		DartSAParameters::openFile("phase.xml");
		DartSAParameters::openFile("maket.xml");
		DartSAParameters::openFile("trees.xml");
		DartSAParameters::openFile("plots.xml");
	} else {
		ifstream atmosphereStream((DartEnv::getCurrentSimulationInputPath() + "atmosphere.xml").c_str());
		DartSAParameters::parseStream(atmosphereStream, "atmosphere.xml");
		atmosphereStream.close();

		ifstream directionsStream((DartEnv::getCurrentSimulationInputPath() + "directions.xml").c_str());
		DartSAParameters::parseStream(directionsStream, "directions.xml");
		directionsStream.close();

		ifstream coeffStream((DartEnv::getCurrentSimulationInputPath() + "coeff_diff.xml").c_str());
		DartSAParameters::parseStream(coeffStream, "coeff_diff.xml");
		coeffStream.close();

		ifstream phaseStream((DartEnv::getCurrentSimulationInputPath() + "phase.xml").c_str());
		DartSAParameters::parseStream(phaseStream, "phase.xml");
		phaseStream.close();

		ifstream maketStream((DartEnv::getCurrentSimulationInputPath() + "maket.xml").c_str());
		DartSAParameters::parseStream(maketStream, "maket.xml");
		maketStream.close();

		ifstream treesStream((DartEnv::getCurrentSimulationInputPath() + "trees.xml").c_str());
		DartSAParameters::parseStream(treesStream, "trees.xml");
		maketStream.close();

		ifstream plotsStream((DartEnv::getCurrentSimulationInputPath() + "plots.xml").c_str());
		DartSAParameters::parseStream(plotsStream, "plots.xml");
		maketStream.close();
	}
}

void PhaseDAO::initYaMLs() {
	// Triangle File is still an XML
	_dartInputsDAO.loadInputsFromYaML();
}

// -----------------------------------------------------------------------------
/**!
 * Permet de ne faire qu'une seule fermeture des fichiers pour ceux que l'on ouvre plusieurs fois.
 */
void PhaseDAO::closePhase() {
	DartSAParameters::terminate();
}

bool PhaseDAO::readPhaseParameters(PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readPhaseParametersYaML(genDTO);
	} else {
		return readPhaseParametersXML(genDTO);
	}
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres de Phase
 *
 * @param int numberOfSpectralBands 			: nombre de bandes spectrales.
 * @param vector<double> lowerBandWavelength 	: vecteur contenant les valeurs des bandes spectrales minimum.
 * @param vector<double> upperBandWavelength 	: vecteur contenant les valeurs des bandes spectrales maximales.
 * @param double dAS 					: angle azimuthal solaire
 * @param double dZS 					: angle zenithal solaire
 * @param string sfich_angle 		: fichier contenant toutes directions.
 * @param int mode 						: mode dans lequel va se lancer la simulation.
 * @param int MonteCarlo 				: 1 si la simulation tourne en mode Monte Carlo.
 * @param int Specul 					: 1 si effet speculaire demande.
 *
 * @return 1 si okay
 */

bool PhaseDAO::readPhaseParametersXML(PhaseGeneralParametersDTO & genDTO) {
	try {
		//		DART_PHASE_init() ;//DartEnv::getCurrentSimulationInputPath()) ;

		//	DartSAParameters::get("Directions.ExpertModeZone.directionFileName", fich_angle) ;
		DartSAParameters::get("Phase.calculatorMethod", genDTO.radiativeTransferMethod);
		DartSAParameters::get("Phase.ExpertModeZone.nbThreads", genDTO.numberOfThreads);
		if (DartSAParameters::exists("Phase.accelerationEngine")) {
			DartSAParameters::get("Phase.accelerationEngine", genDTO.accelerationEngine);
			if(DartSAParameters::exists("Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer"))
				genDTO.atmosphereRTmode = DartSAParameters::get<unsigned int>(
						"Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer");
		}
		if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR /*LIDAR*/) {
			DartSAParameters::get("Phase.DartInputParameters.Lidar.simulateSolarNoise", genDTO.isSimulatedSolarNoiseCheck);
			if (genDTO.isSimulatedSolarNoiseCheck) {
				DartSAParameters::get("Phase.DartInputParameters.Lidar.RunningMode.snMode", genDTO.SNrunningmode);
			}
			PhaseProperty::setIfSimuSolarNoise(genDTO.isSimulatedSolarNoiseCheck);
		}
		int polarization = 0;
		if (DartSAParameters::exists("Phase.DartProduct.dartModuleProducts.CommonProducts.polarizationProducts")) {
			DartSAParameters::get("Phase.DartProduct.dartModuleProducts.CommonProducts.polarizationProducts", polarization);
		}
		PhaseProperty::setPolarization(polarization);
		int fluorescence = 0;
		DartSAParameters::get("Coeff_diff.fluorescenceProducts", fluorescence);
		PhaseProperty::setFluorescence(fluorescence);
		if (PhaseProperty::getFluorescence()) {
			int combinedPS = 0;
			if (DartSAParameters::exists("Coeff_diff.useCombinedYield")) {
				DartSAParameters::get("Coeff_diff.useCombinedYield", combinedPS);
			}
			PhaseProperty::setCombinedPS(combinedPS);
			if (DartSAParameters::exists("Coeff_diff.fluorescenceFile")) {
				bool fluorescenceFile;
				DartSAParameters::get("Coeff_diff.fluorescenceFile", fluorescenceFile);
				if (fluorescenceFile) {
					string fluorescenceFileName;
					DartSAParameters::get("Coeff_diff.FluorescenceProductsProperties.fluorescenceFileName", fluorescenceFileName);
					PhaseProperty::setFluorescenceFileName(fluorescenceFileName);
				} else {
					PhaseProperty::setFluorescenceFileName("");
				}
			} else {
				PhaseProperty::setFluorescenceFileName("");
			}
			PhaseProperty::setPolarization(false);
		}

		string fileNameDP = DartEnv::getFilePathForRead(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties donneesDirections(fileNameDP);

		if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
				|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO
				|| (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == true)) {
			// Flux-Tracking ou Monte-Carlo
			double ZSTmp, AS;
			donneesDirections.valeurProperties<double>("direction.sunTheta", ZSTmp);
			donneesDirections.valeurProperties<double>("direction.sunPhi", AS);
			donneesDirections.valeurProperties<int>("direction.sunCalMethod", genDTO.sunCalMethod);
			donneesDirections.valeurProperties<int>("direction.dayOfTheYear", genDTO.dayOfTheYear);
			genDTO.solarDirection.setBeginTheta(deg2rad(ZSTmp)).setCenterTheta(deg2rad(ZSTmp)).setEndTheta(deg2rad(ZSTmp));
			genDTO.solarDirection.setBeginPhi(deg2rad(AS)).setCenterPhi(deg2rad(AS)).setEndPhi(deg2rad(AS));
			genDTO.solarDirection.setCosThetaDOmega(0.0);
			genDTO.solarDirection.setSolidAngle(0.0).setProperty(DirectionRoot::propertyOfString("V"));
			genDTO.solarDirection.calculerCoordonneesCartesiennes();

			// Rays are emitted from the surface of the sun => non parallel rays
			unsigned int useParallelSunRays = 0;
			if (donneesDirections.valeurProperties<unsigned int>("direction.penumbra.mode", useParallelSunRays)) {
				if (useParallelSunRays != 0) {
					Point3<double> temp;
					genDTO.penumbraEffects = true;
					temp.setX(donneesDirections.valeurProperties<double>("direction.penumbra.sunCenter.x"));
					temp.setY(donneesDirections.valeurProperties<double>("direction.penumbra.sunCenter.y"));
					temp.setZ(donneesDirections.valeurProperties<double>("direction.penumbra.sunCenter.z"));
					genDTO.sunDistance = temp.getDistanceToOrigin();

					genDTO.sunRadius = (donneesDirections.valeurProperties<double>("direction.penumbra.sunRadius"));
				} else {
					genDTO.penumbraEffects = false;
				}
			} else {
				genDTO.penumbraEffects = false;
			}
		}

		if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR) {
			// Lidar
			double ZL, AL;
			donneesDirections.valeurProperties<double>("direction.lidarTransmitterTheta", ZL);
			donneesDirections.valeurProperties<double>("direction.lidarTransmitterPhi", AL);
			genDTO.lidarDirection.setBeginTheta(deg2rad(ZL)).setCenterTheta(deg2rad(ZL)).setEndTheta(deg2rad(ZL));
			genDTO.lidarDirection.setBeginPhi(deg2rad(AL)).setCenterPhi(deg2rad(AL)).setEndPhi(deg2rad(AL));
			genDTO.lidarDirection.setCosThetaDOmega(0.0);
			genDTO.lidarDirection.setSolidAngle(0.0).setProperty(DirectionRoot::propertyOfString("V"));
			genDTO.lidarDirection.calculerCoordonneesCartesiennes();
		}
		//DartSAParameters::get("Coeff_diff.Volumes.UnderstoryMultiFunctions.specularEffects", genDTO.specularCheck);
		DartSAParameters::get("Coeff_diff.Volumes.UnderstoryMultiFunctions.outputLADFile", genDTO.isAskedForWritingLadResultsCheck);

		// Checks if a tree has to be generated as a triangle cloud
		if (DartSAParameters::exists("Trees.TreeGeneralOptions.triangleTreeRepresentation")) {
			DartSAParameters::get("Trees.TreeGeneralOptions.triangleTreeRepresentation",
				genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation);
			if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
				// Maket will need the LAD to compute the orientation of the triangles representing the leaves
				genDTO.isAskedForWritingLadResultsCheck = true;
			}
		}
		if (!genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
			// Checks if a plot in the xml has to be generated as a triangle cloud
			const int nbVegetationPlot = DartSAParameters::getNbOf("Plots.Plot");
			if (nbVegetationPlot > 0) {
				for (int i = 0; i < nbVegetationPlot; ++i) {
					ostringstream oss;
					oss << i;
					const string plotRootNode = "Plots.Plot[" + oss.str() + "].";

					unsigned int mediumType = DartSAParameters::get<int>(plotRootNode + "type");
					if (mediumType == 1 || mediumType == 2) {
						int representation = DartSAParameters::get<int>(plotRootNode + "PlotVegetationProperties.trianglePlotRepresentation");
						if (representation == 1) {
							genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation = true;
							genDTO.isAskedForWritingLadResultsCheck = true;
						}
					}
				}
			}
		}
		if (!genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
			// Checks if a plot in the txt file has to be generated as a triangle cloud
			if (DartSAParameters::get<bool>("Plots.addExtraPlotsTextFile")) {
				readPlotTxt(DartSAParameters::get<std::string>("Plots.ExtraPlotsTextFileDefinition.extraPlotsFileName"), genDTO);
			}
		}

		genDTO.numberOfSpectralBands = DartSAParameters::getNbOf("Phase.DartInputParameters.SpectralIntervals.SpectralIntervalsProperties");
		double lowerBandWavelength;
		bool thermalMode = false;
		for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
			double meanLambda = 0.0;
			double deltaLambda = 0.0;
			int domaineSpectralCourant = 0, loiEmissionCourante = -1;
			ostringstream oss;
			oss << i;
			string temp = "Phase.DartInputParameters.SpectralIntervals.SpectralIntervalsProperties[" + oss.str();
			DartSAParameters::get(temp + "].meanLambda", meanLambda);
			DartSAParameters::get(temp + "].deltaLambda", deltaLambda);
			DartSAParameters::get(temp + "].spectralDartMode", domaineSpectralCourant);
			genDTO.spectralDomain.push_back(domaineSpectralCourant);

			lowerBandWavelength = meanLambda - deltaLambda / 2;
			if (lowerBandWavelength < 0.25) {
				lowerBandWavelength = 0.25;
			}
			genDTO.lowerBandWavelength.push_back(lowerBandWavelength);
			genDTO.upperBandWavelength.push_back(meanLambda + deltaLambda / 2);
			if (domaineSpectralCourant == 2) {
				DartSAParameters::get(temp + "].emissionLawNode.emissionLaw", loiEmissionCourante);
				thermalMode = true;
			} else if (domaineSpectralCourant == 1) {
				loiEmissionCourante = PLANCK;
				thermalMode = true;
			}
			genDTO.emissionLaw.push_back(loiEmissionCourante);
		}
		if (thermalMode) {
			// Reads the mean temperature provided by the user if it exists
			if (DartSAParameters::exists(
				"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation")) {
				DartSAParameters::get(
					"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation",
					genDTO.meanTemperature);
			} else if (DartSAParameters::exists(
				"Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation")) {
				DartSAParameters::get(
					"Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation",
					genDTO.meanTemperature);
			}
			// Write the LAD files in order to compute the thermal hotspot effect
			genDTO.isAskedForWritingLadResultsCheck = true;
		}

		//		DartSAParameters::terminate() ;

		genDTO.isThermalEmissionEnabled = false;
		for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
			if (genDTO.spectralDomain[bandIndex] == 1 || genDTO.spectralDomain[bandIndex] == 2) { // mode T+R or T
				genDTO.isThermalEmissionEnabled = true;
			}
		}
		if (genDTO.isThermalEmissionEnabled) {
			if (DartSAParameters::exists("Phase.DartInputParameters.SpectralDomainTir.temperatureMode")) {
				DartSAParameters::get("Phase.DartInputParameters.SpectralDomainTir.temperatureMode", genDTO.temperatureDeterminationMode);
			}
			if (genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_MATRICE_3D ||
					genDTO.temperatureDeterminationMode == PhaseGeneralParametersDTO::MODE_DETERMINATION_VERTICAL_1D) {
				if (DartSAParameters::exists("Phase.DartInputParameters.SpectralDomainTir.TemperatureFile.temperatureFileName")) {
					genDTO.temperatureFile = DartSAParameters::get<std::string>("Phase.DartInputParameters.SpectralDomainTir.TemperatureFile.temperatureFileName");
				}
			}
			if (DartSAParameters::exists("Phase.DartInputParameters.temperatureAtmosphere.atmosphericApparentTemperature")) {
				genDTO.atmosphericApparentTemperature = DartSAParameters::get<double>("Phase.DartInputParameters.temperatureAtmosphere.atmosphericApparentTemperature");
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
/**
 * Read the general parameters of the simulation from YaML files
 * @return 1 si okay
 */
bool PhaseDAO::readPhaseParametersYaML(PhaseGeneralParametersDTO & genDTO) {
	try {
		//		DART_PHASE_init() ;//DartEnv::getCurrentSimulationInputPath()) ;
		if (_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod()) {
			if (dynamic_cast<DartInputs::FluxTracking*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::FLUXTRACKING;
			} else if (dynamic_cast<DartInputs::MonteCarlo*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::MONTECARLO;
			} else if (DartInputs::Lidar* lidar =
					dynamic_cast<DartInputs::Lidar*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				genDTO.radiativeTransferMethod = PhaseGeneralParametersDTO::LIDAR;
				// J'sais pas si il peut cast le pointeur directement en bool ?
				genDTO.isSimulatedSolarNoiseCheck = lidar->getSolarNoise() ? true : false;
			}
		}

		if (_dartInputsDAO.getDartExecution().getExecutionParameters()) {
			genDTO.numberOfThreads = _dartInputsDAO.getDartExecution().getExecutionParameters().get()->getNumberOfThreads();
			genDTO.accelerationEngine = 0;
			if (dynamic_cast<DartInputs::LuxCoreAccelerationEngine*>(_dartInputsDAO.getDartExecution().getExecutionParameters()->getAccelerationEngine().get())) {
				genDTO.accelerationEngine = 2;
			}
		}

		int polarization = 0;
		// TODO:
//		DartSAParameters::get("Phase.DartProduct.dartModuleProducts.polarizationProducts", polarization);
		PhaseProperty::setPolarization(polarization);

		// Fluorescence
		PhaseProperty::setFluorescence(false);
		PhaseProperty::setFluorescenceFileName("");
		if (_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()) {
			if (_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getFluorescenceProperties()) {
				if (_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getFluorescenceProperties().get()->getFluorescenceProducts()) {
					PhaseProperty::setFluorescence(true);
					if (_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getFluorescenceProperties()->getYieldsPerTriangle()) {
						PhaseProperty::setFluorescenceFileName(
							_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getFluorescenceProperties()->getYieldsPerTriangleFile());
					}
				}
			}
		}

		// On charge le fichier simulation properties au milieu du YaML ...
		string fileNameDP = DartEnv::getFilePathForRead(OUTPUT_DIR, SIMULATIONPROPERTIESFILE);
		FichierProperties donneesDirections(fileNameDP);
		if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::FLUXTRACKING
				|| genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::MONTECARLO
				|| (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR && genDTO.isSimulatedSolarNoiseCheck == true)) {
			// Flux-Tracking ou Monte-Carlo
			double ZSTmp, AS;
			donneesDirections.valeurProperties<double>("direction.sunTheta", ZSTmp);
			donneesDirections.valeurProperties<double>("direction.sunPhi", AS);
			donneesDirections.valeurProperties<int>("direction.sunCalMethod", genDTO.sunCalMethod);
			donneesDirections.valeurProperties<int>("direction.dayOfTheYear", genDTO.dayOfTheYear);
			genDTO.solarDirection.setBeginTheta(deg2rad(ZSTmp)).setCenterTheta(deg2rad(ZSTmp)).setEndTheta(deg2rad(ZSTmp));
			genDTO.solarDirection.setBeginPhi(deg2rad(AS)).setCenterPhi(deg2rad(AS)).setEndPhi(deg2rad(AS));
			genDTO.solarDirection.setCosThetaDOmega(0.0);
			genDTO.solarDirection.setSolidAngle(0.0).setProperty(DirectionRoot::propertyOfString("V"));
			genDTO.solarDirection.calculerCoordonneesCartesiennes();
		}
		if (genDTO.radiativeTransferMethod == PhaseGeneralParametersDTO::LIDAR) {
			// Lidar
			double ZL, AL;
			donneesDirections.valeurProperties<double>("direction.lidarTransmitterTheta", ZL);
			donneesDirections.valeurProperties<double>("direction.lidarTransmitterPhi", AL);
			genDTO.lidarDirection.setBeginTheta(deg2rad(ZL)).setCenterTheta(deg2rad(ZL)).setEndTheta(deg2rad(ZL));
			genDTO.lidarDirection.setBeginPhi(deg2rad(AL)).setCenterPhi(deg2rad(AL)).setEndPhi(deg2rad(AL));
			genDTO.lidarDirection.setCosThetaDOmega(0.0);
			genDTO.lidarDirection.setSolidAngle(0.0).setProperty(DirectionRoot::propertyOfString("V"));
			genDTO.lidarDirection.calculerCoordonneesCartesiennes();
		}

		// TODO:
		//		DartSAParameters::get("Coeff_diff.Volumes.UnderstoryMultiFunctions.outputLADFile", genDTO.isAskedForWritingLadResultsCheck);
		// Checks if a tree has to be generated as a triangle cloud
		//		TODO:
		//		if (DartSAParameters::exists("Trees.TreeGeneralOptions.triangleTreeRepresentation")) {
		//			DartSAParameters::get("Trees.TreeGeneralOptions.triangleTreeRepresentation",
		//				genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation);
		//			if (genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		//				// Maket will need the LAD to compute the orientation of the triangles representing the leaves
		//				genDTO.isAskedForWritingLadResultsCheck = true;
		//			}
		//		}
		//		if (!genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		//			// Checks if a plot in the xml has to be generated as a triangle cloud
		//			const int nbVegetationPlot = DartSAParameters::getNbOf("Plots.Plot");
		//			if (nbVegetationPlot > 0) {
		//				for (int i = 0; i < nbVegetationPlot; ++i) {
		//					ostringstream oss;
		//					oss << i;
		//					const string plotRootNode = "Plots.Plot[" + oss.str() + "].";
		//
		//					unsigned int mediumType = DartSAParameters::get<int>(plotRootNode + "type");
		//					if (mediumType == 1 || mediumType == 2) {
		//						int representation = DartSAParameters::get<int>(plotRootNode + "PlotVegetationProperties.trianglePlotRepresentation");
		//						if (representation == 1) {
		//							genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation = true;
		//							genDTO.isAskedForWritingLadResultsCheck = true;
		//						}
		//					}
		//				}
		//			}
		//			initYaMLs();
		//		}
		//		if (!genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation) {
		//			// Checks if a plot in the txt file has to be generated as a triangle cloud
		//			if (DartSAParameters::get<bool>("Plots.addExtraPlotsTextFile")) {
		//				readPlotTxt(DartSAParameters::get<std::string>("Plots.ExtraPlotsTextFileDefinition.extraPlotsFileName"), genDTO);
		//			}
		//		}

		// Traitement des bandes spectrales
		bool thermalMode = false;
		for (const shared_ptr<DartInputs::SpectralBand> &spectralBand : _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand()) {
			int loiEmissionCourante = -1;
			int spectralDomain = 0;
			if (spectralBand.get()->getSpectralDomain()) {
				if (DartInputs::ModeT* modeT = dynamic_cast<DartInputs::ModeT*>(spectralBand.get()->getSpectralDomain().get())) {
					spectralDomain = 3;
					if (modeT->getThermalEmissionLaw() == DartInputs::ModeT::BOLTSZMAN_LAW) {
						loiEmissionCourante = STEPHAN;
					} else if (modeT->getThermalEmissionLaw() == DartInputs::ModeT::PLANK_LAW) {
						loiEmissionCourante = PLANCK;
					}
					thermalMode = true;
				} else if (dynamic_cast<DartInputs::ModeR*>(spectralBand.get()->getSpectralDomain().get())) {
					spectralDomain = 0;
				} else if (dynamic_cast<DartInputs::ModeRT*>(spectralBand.get()->getSpectralDomain().get())) {
					spectralDomain = 1;
					loiEmissionCourante = PLANCK;
					thermalMode = true;
				}
			}

			int yamlSpectralBandIndex = spectralBand.get()->getItemId();
			double meanLambda = spectralBand.get()->getCentralWavelength();
			double deltaLambda = spectralBand.get()->getSpectralBandWidth();
			double lowerBandWavelength = meanLambda - deltaLambda / 2;
			lowerBandWavelength = lowerBandWavelength < 0.25 ? 0.25 : lowerBandWavelength;
			if (DartInputs::SpectralRange* spectralRange = dynamic_cast<DartInputs::SpectralRange*>(spectralBand.get())) {
				unsigned int nbBandInRange = spectralRange->getNumberOfBand();
				double interval = spectralRange->getIntervalBetweenBands();
				for (unsigned int i = 0; i < nbBandInRange; ++i) {
					meanLambda = meanLambda + i * interval;
					genDTO.lowerBandWavelength.push_back(lowerBandWavelength);
					genDTO.upperBandWavelength.push_back(meanLambda + deltaLambda / 2);
					genDTO.emissionLaw.push_back(loiEmissionCourante);
					genDTO.spectralDomain.push_back(spectralDomain);
					genDTO.yamlSpectralBandIndex.push_back(yamlSpectralBandIndex);
				}
			} else {
				genDTO.lowerBandWavelength.push_back(lowerBandWavelength);
				genDTO.upperBandWavelength.push_back(meanLambda + deltaLambda / 2);
				genDTO.emissionLaw.push_back(loiEmissionCourante);
				genDTO.spectralDomain.push_back(spectralDomain);
				genDTO.yamlSpectralBandIndex.push_back(yamlSpectralBandIndex);
			}
		}
		genDTO.numberOfSpectralBands = genDTO.spectralDomain.size();

		// TODO:
		if (thermalMode) {
//			// Reads the mean temperature provided by the user if it exists
//			if (DartSAParameters::exists(
//				"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation")) {
//				DartSAParameters::get(
//					"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation",
//					genDTO.meanTemperature);
//			} else if (DartSAParameters::exists(
//				"Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation")) {
//				DartSAParameters::get(
//					"Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.WeightingParameters.sceneAverageTemperatureForPonderation",
//					genDTO.meanTemperature);
//			}
		}
	} catch (YAML::Exception &E) {
		cerr << E.msg << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readPlotTxt(	const std::string & fileName,
							PhaseGeneralParametersDTO & genDTO) {
	string filePath = DartEnv::getFilePathForRead(INPUT_DIR, fileName);
	ifstream stream(filePath.c_str(), ifstream::in);
	skipBOM(stream);

	if (stream) {
		// Stream and global initialization
		string line;
		bool headerRead = false;
		bool plotAsTriangleColumnPresent = false;
		unsigned int plotAsTriangleColumnIndex = 0;
		bool plotAsTriangleFound = false;
		int plotAsTriangle = 0;

		while (getline(stream, line) && (!headerRead || plotAsTriangleColumnPresent) && !plotAsTriangleFound) {
			// Line begin with '*' character is user commentary line
			if (line.compare("") != 0 && line.at(0) != '*') {
				istringstream iss_line(line);
				string temp;
				if (!headerRead) {
					while (iss_line >> temp && !plotAsTriangleColumnPresent) {
						if (temp.compare("VEG_AS_TRI") == 0) {
							plotAsTriangleColumnPresent = true;
						} else {
							++plotAsTriangleColumnIndex;
						}
					}
					headerRead = true;
				} else {
					for (unsigned int i = 0; i < plotAsTriangleColumnIndex; ++i) {
						iss_line >> temp;
					}
					iss_line >> plotAsTriangle;
					if (plotAsTriangle == 1) {
						genDTO.generateEquivalentLambertianPhaseFunctionsFromVegetation = true;
						genDTO.isAskedForWritingLadResultsCheck = true;
						plotAsTriangleFound = true;
					}
				}
			}
		}
	} else {
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des paramètres de l'atmosphere dans Phase
 *
 * @param unsigned int aruflag 			: 1 si l'atmosphere a deja ete calcule ; 0 si on doit la calculer ; -1 s'il n'y a pas d'atmosphere.
 * @param unsigned int isAtmosphereDatabase 			: 1 si fichier de la database, 0 sinon.
 * @param string FichData 		: nom du fichier de la database.
 * @param double lambda_max 		: borne superieure de l'intervalle spectral.
 * @param double lambda_min 		: borne inferieure de l'intervalle spectral.
 * @param unsigned int atmosphericModel 	: type de modele atmospherique.
 * @param unsigned int H_model 		: modele des facteurs d'echelle
 * @param unsigned int ozonModel 	: modele pour l'ozone
 * @param unsigned int temperatureModel 	: modele pour la distribution des temperatures.
 * @param unsigned int vaporModel 		: modele pour la vapeur d'eau
 * @param double d 				: quantite de vapeur d'eau precipitable
 * @param unsigned int aerosolModel 		: modele pour les aerosols.
 * @param double Es 				: constante solaire.
 * @param double aerosolScaleFactor 			: facteur d'echelle pour les gazs
 * @param double trans_diff_m 		: epaisseur optique pour les gazs
 * @param double aerosolOpticalDepthForScattering 		: epaisseur optique pour les aerosols
 * @param double aerosolSingleScatteringAlbedo 		: albedo pour les aerosols
 * @param double g1_p 			: Parametre de la fonction de phase de Henyey Greenstein (entre 0 et 1). Diffusion vers l'avant
 * @param double g2_p 			: Parametre de la fonction de phase de Henyey Greenstein (entre 0 et 1). Retrodiffusion
 * @param double a_p 			: facteur multiplicatif de la fonction de phase de Henyey Greenstein
 * @param double gasScaleFactor 			: facteur d'echelle pour les aerosols
 * @param double gasTotalTransmittanceForAbsoption 		: transmittance totale pour les gazs
 * @param double sensorHeight 		: altitude du capteur
 *
 * @return 1 si okay
 */
bool PhaseDAO::readAtmosphereParameters(AtmosphereParameters& atmosphereParameters) {
	if (_useYaML) {
		return readAtmosphereParametersYaML(atmosphereParameters);
	} else {
		return readAtmosphereParametersXML(atmosphereParameters);
	}
}

bool PhaseDAO::readAtmosphereParametersXML(AtmosphereParameters& atmosphereParameters) {
	try {
		/*
		 * isAtmosphereDatabase 0:User defined parameters - Atmosphere radiative transfer (Monospectral);1:Atmospheric database;2:No radiative transfer or only within the air of Bottom Atmosphere (BA)
		 */
		//		DartSAParameters::initialize(DartEnv::getCurrentSimulationInputPath()) ;
		DartSAParameters::get("Phase.AtmosphereRadiativeTransfer.TOAtoBOA", atmosphereParameters.computationMethodTOAtoBOA);
		int atmosphereRTmode = 0;
		double gas_u = 0.5;
		double aerosols_u = 0.16667;
		if(DartSAParameters::exists("Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer"))
			atmosphereRTmode = DartSAParameters::get<unsigned int>("Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer");

		if (atmosphereParameters.computationMethodTOAtoBOA == 0) {
			atmosphereParameters.isAtmosphereDefined = 0;
		} else {
			atmosphereParameters.isAtmosphereDefined = 1;
		}
		if (atmosphereParameters.computationMethodTOAtoBOA == 1)
		{
			if(DartSAParameters::exists("Phase.AtmosphereRadiativeTransfer.ForwardScatteringFunction.gas_u"))
			{
				DartSAParameters::get("Phase.AtmosphereRadiativeTransfer.ForwardScatteringFunction.gas_u", gas_u);
			}
			if(DartSAParameters::exists("Phase.AtmosphereRadiativeTransfer.ForwardScatteringFunction.aerosols_u"))
			{
				DartSAParameters::get("Phase.AtmosphereRadiativeTransfer.ForwardScatteringFunction.aerosols_u", aerosols_u);
			}
		}
		atmosphereParameters.gas_u = gas_u;
		atmosphereParameters.aerosols_u = aerosols_u;

		DartSAParameters::get("Maket.LatLon.altitude", atmosphereParameters.sceneAltitude);

		DartSAParameters::get("Atmosphere.isRadiativeTransfertInBottomAtmosphereDefined",
			atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer);
		int irradianceFromDatabase = 0;
		std::string illuminationNodeKey;
		if (PhaseProperty::getIfSimuSolarNoise()) {
			illuminationNodeKey = "Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.";
		} else {
			illuminationNodeKey = "Phase.DartInputParameters.nodeIlluminationMode.";
		}
		if (DartSAParameters::exists(illuminationNodeKey + "irradianceMode")) {
			DartSAParameters::get(illuminationNodeKey + "irradianceMode", irradianceFromDatabase);
			if (irradianceFromDatabase == 3) {
				// Mode 3 works like mode 1, except with where it gets its radiation data from (from a file instead of from the xml)
				irradianceFromDatabase = 1;
			}
		}

		if (atmosphereParameters.computationMethodTOAtoBOA == 2 && irradianceFromDatabase == 2) {
			atmosphereParameters.fullyPrecomputed = DartSAParameters::exists(
				"Atmosphere.IsAtmosphere.AtmosphereIterations.AtmosphereTransfertFunctions.ImportedTransferFunctions.transferFunctionsFile")
					&& DartSAParameters::exists(illuminationNodeKey + "illumination.illuminationFileName")
					&& DartSAParameters::exists(illuminationNodeKey + "ftiImportation.ftiFileName");
		} else {
			atmosphereParameters.fullyPrecomputed = false;
		}

		if (atmosphereParameters.isAtmosphereDefined == 1 || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer == 1) {

			DartSAParameters::get("Atmosphere.IsAtmosphere.typeOfAtmosphere", atmosphereParameters.isAtmosphereDatabase);

			if (atmosphereParameters.isAtmosphereDatabase == 1) {

				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.databaseName", atmosphereParameters.databaseName);

//				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.aerosolOptDepthFactor",
//					atmosphereParameters.aerosolOpticalDepthFactor);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.co2MixRate", atmosphereParameters.co2MixRate);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.scaleOtherGases",
					atmosphereParameters.ifDefAmountOtherGases);

				if (atmosphereParameters.ifDefAmountOtherGases) {
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.coMulFactor",
						atmosphereParameters.coMulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.ch4MulFactor",
						atmosphereParameters.ch4MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.n2oMulFactor",
						atmosphereParameters.n2oMulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.o2MulFactor",
						atmosphereParameters.o2MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.n2MulFactor",
						atmosphereParameters.n2MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.o3MulFactor",
						atmosphereParameters.o3MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.nh3MulFactor",
						atmosphereParameters.nh3MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.noMulFactor",
						atmosphereParameters.noMulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.no2MulFactor",
						atmosphereParameters.no2MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.so2MulFactor",
						atmosphereParameters.so2MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.hno3MulFactor",
						atmosphereParameters.hno3MulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.othersMulFactor",
						atmosphereParameters.othersMulFactor);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.scatMulFactor",
						atmosphereParameters.mulScatMulFactor);
				}
				atmosphereParameters.numberOfAerosol = DartSAParameters::getNbOf("Atmosphere.Aerosol.AerosolProperties");
				std::string aerosolsModelName, aerosolCumulativeModelName, hgParametersModelName;
				double aerosolOptDepthFactor;
				for (int index = 0; index < atmosphereParameters.numberOfAerosol; index++) {
					ostringstream oss;
					oss << index;
					string temp = "Atmosphere.Aerosol.AerosolProperties[" + oss.str();
					DartSAParameters::get(temp + "].aerosolCumulativeModelName", aerosolCumulativeModelName);
					DartSAParameters::get(temp + "].aerosolOptDepthFactor", aerosolOptDepthFactor);
					DartSAParameters::get(temp + "].aerosolsModelName", aerosolsModelName);
					DartSAParameters::get(temp + "].hgParametersModelName", hgParametersModelName);
					atmosphereParameters.aerosolsModelName.push_back(aerosolsModelName);
					atmosphereParameters.aerosolCumulativeModelName.push_back(aerosolCumulativeModelName);
					atmosphereParameters.hgParametersModelName.push_back(hgParametersModelName);
					atmosphereParameters.aerosolOpticalDepthFactor.push_back(aerosolOptDepthFactor);
				}

//				for(int i=0;i<atmosphereParameters.numberOfAerosol;i++){
//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.aerosolsModelName",
//						aerosolsModelName);
//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.aerosolCumulativeModelName",
//						aerosolCumulativeModelName);
//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.hgParametersModelName",
//						hgParametersModelName);
//					atmosphereParameters.aerosolsModelName.push_back(aerosolsModelName);
//					atmosphereParameters.aerosolCumulativeModelName.push_back(aerosolCumulativeModelName);
//					atmosphereParameters.hgParametersModelName.push_back(hgParametersModelName);
//				}

				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.gasModelName", atmosphereParameters.gasModelName);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.gasCumulativeModelName",
					atmosphereParameters.gasCumulativeModelName);
//				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.gasParametersModelName",
//					atmosphereParameters.gasParametersModelName);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.temperatureModelName",
					atmosphereParameters.temperatureModelName);

//				cout<<"atmosphereParameters.temperatureModelName: "<<atmosphereParameters.temperatureModelName<<endl;

				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.precipitableWaterAmountCkeckbox",
					atmosphereParameters.h2oAmountCkeckbox);
//				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.aerosolsModelName",
//					atmosphereParameters.aerosolsModelName);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.redefTemperature",
					atmosphereParameters.redefTemperature);
				if (atmosphereParameters.redefTemperature) {
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.tempParameters.temp",
						atmosphereParameters.redef_temp);
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.tempParameters.altitude",
						atmosphereParameters.redef_altitude);
				}

				if (atmosphereParameters.h2oAmountCkeckbox) {
					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.defWaterAmount",
						atmosphereParameters.h2oAmountDef);
					switch (atmosphereParameters.h2oAmountDef) {
						case 0:
							cout << "Defining water vapour amount by multiplicative factor" << endl;
							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.M_factor.mulFactorH2O",
								atmosphereParameters.mulFactorH2O);
							break;
						case 1:
							cout << "Defining water vapour amount in g/cm2" << endl;
							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Amount_g_per_cm2.g_per_cm2",
								atmosphereParameters.def_amount_H2O_g_per_cm2);
							break;
						case 2:
							cout << "Defining water vapour amount in atm-cm" << endl;
							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Amount_atm_cm.atm_cm",
								atmosphereParameters.def_amount_H2O_atm_cm);
							break;
						case 3:
							cout << "Defining water vapour density in atm-cm/km" << endl;
							DartSAParameters::get(
								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_atm_cm_per_km_altitude.atm_cm_per_km",
								atmosphereParameters.def_density_atm_cm_per_km);
							DartSAParameters::get(
								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_atm_cm_per_km_altitude.altitude",
								atmosphereParameters.def_altitude);
							break;
						case 4:
							cout << "Defining water vapour density with relative humidity" << endl;
							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.rh",
								atmosphereParameters.def_rh);
							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.temp",
								atmosphereParameters.def_temp);
							DartSAParameters::get(
								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.altitude",
								atmosphereParameters.def_altitude);
							break;
						default:
							cerr << "Option is not implemented" << endl;
							abort();
					}

				}
			} else {
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.aerosolScaleFactor",
					atmosphereParameters.aerosolScaleFactor);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.aerosolOpticalDepth",
					atmosphereParameters.aerosolOpticalDepthForScattering);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.aerosolAlbedo",
					atmosphereParameters.aerosolSingleScatteringAlbedo);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.g1", atmosphereParameters.henleyGreensteinParameters[0].g1);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.g2", atmosphereParameters.henleyGreensteinParameters[0].g2);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.a_HG",
					atmosphereParameters.henleyGreensteinParameters[0].a);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.gasOpticalDepth",
					atmosphereParameters.gasOpticalDepthForScattering);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.gasScaleFactor", atmosphereParameters.gasScaleFactor);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalProperty.transmittanceOfGases",
					atmosphereParameters.gasTotalTransmittanceForAbsoption);
				DartSAParameters::get("Atmosphere.IsAtmosphere.TemperatureFile.atmosphereTemperatureFileName",
					atmosphereParameters.atmosphereTemperatureFileName);
			}
			if (atmosphereParameters.isAtmosphereDefined) {
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphereGeometry.heightOfSensor", atmosphereParameters.sensorHeight);
				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphereGeometry.minimumNumberOfDivisions",
					atmosphereParameters.minimumNumberOfDivisions);

			}

		} else {
			cout << "   -> No atmosphere simulation" << endl;
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readAtmosphereParametersYaML(AtmosphereParameters& atmosphereParameters) {
	try {
		/*
		 * isAtmosphereDatabase 0:User defined parameters - Atmosphere radiative transfer (Monospectral);1:Atmospheric database;2:No radiative transfer or only within the air of Bottom Atmosphere (BA)
		 */
		//		DartSAParameters::initialize(DartEnv::getCurrentSimulationInputPath()) ;
//		int computationMethodTOAtoBOA = 0;
		atmosphereParameters.isAtmosphereDefined = 0;
		if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::NO_ATMOSPHERE) {
//			computationMethodTOAtoBOA = 0;
			atmosphereParameters.isAtmosphereDefined = 0;
		} else if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::ANALYTIC) {
//			computationMethodTOAtoBOA = 1;
			atmosphereParameters.isAtmosphereDefined = 1;
			atmosphereParameters.sensorHeight = _dartInputsDAO.getDartAtmosphere().getAtmosphereGeometry().get()->getSensorAltitude();
			atmosphereParameters.minimumNumberOfDivisions =
					_dartInputsDAO.getDartAtmosphere().getAtmosphereGeometry().get()->getIntegralStepForComputionAerosolPhaseFunction();
		} else if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::RADIATIVE_TRANSFER) {
//			computationMethodTOAtoBOA = 2;
			atmosphereParameters.isAtmosphereDefined = 1;
			atmosphereParameters.sensorHeight = _dartInputsDAO.getDartAtmosphere().getAtmosphereGeometry().get()->getSensorAltitude();
			atmosphereParameters.minimumNumberOfDivisions =
					_dartInputsDAO.getDartAtmosphere().getAtmosphereGeometry().get()->getIntegralStepForComputionAerosolPhaseFunction();
		}

		if (_dartInputsDAO.getDartEarthSceneProperties().getEarthSceneProperties()) {
			if (_dartInputsDAO.getDartEarthSceneProperties().getEarthSceneProperties().get()->getEarthSceneCoordinates()) {
				atmosphereParameters.sceneAltitude =
						_dartInputsDAO.getDartEarthSceneProperties().getEarthSceneProperties().get()->getEarthSceneCoordinates().get()->getAltitude();
			}
		}
		atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer = _dartInputsDAO.getDartAtmosphere().getAirWithinScene();
		if (atmosphereParameters.isAtmosphereDefined == 1 || atmosphereParameters.airInEarthSceneDefinedForRadiativeTransfer == 1) {
			if (const shared_ptr<DartInputs::IAtmosphereModel> &atmModel = _dartInputsDAO.getDartAtmosphere().getAtmosphereModel()) {
				if (DartInputs::DatabaseAtmosphereModel* databModel = dynamic_cast<DartInputs::DatabaseAtmosphereModel*>(atmModel.get())) {
					atmosphereParameters.isAtmosphereDatabase = 1;
					atmosphereParameters.databaseName = databModel->getAtmosphereDatabase().get()->getDartFilePath();
					atmosphereParameters.gasModelName = databModel->getAtmosphereDatabase().get()->getGasOpticalProperty();
					atmosphereParameters.gasCumulativeModelName = databModel->getAtmosphereDatabase().get()->getGasOtherGasProfile();
					atmosphereParameters.temperatureModelName = databModel->getAtmosphereDatabase().get()->getGasTemperatureProfile();
					atmosphereParameters.h2oAmountCkeckbox = databModel->getPrecipitableAmountWater() ? true : false;

					//					atmosphereParameters.aerosolOpticalDepthFactor.push_back(
//						boost::lexical_cast<double>(atmosphereModelNode["aerosolOpticalDepthMultiplicativeFactor"].Scalar()));
					//				TODO: DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.co2MixRate", atmosphereParameters.co2MixRate);
					//				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.scaleOtherGases",
					//					atmosphereParameters.ifDefAmountOtherGases);
					//
					//				if (atmosphereParameters.ifDefAmountOtherGases) {
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.coMulFactor",
					//						atmosphereParameters.coMulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.ch4MulFactor",
					//						atmosphereParameters.ch4MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.n2oMulFactor",
					//						atmosphereParameters.n2oMulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.o2MulFactor",
					//						atmosphereParameters.o2MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.n2MulFactor",
					//						atmosphereParameters.n2MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.o3MulFactor",
					//						atmosphereParameters.o3MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.nh3MulFactor",
					//						atmosphereParameters.nh3MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.noMulFactor",
					//						atmosphereParameters.noMulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.no2MulFactor",
					//						atmosphereParameters.no2MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.so2MulFactor",
					//						atmosphereParameters.so2MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.hno3MulFactor",
					//						atmosphereParameters.hno3MulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.othersMulFactor",
					//						atmosphereParameters.othersMulFactor);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.OtherGasesScale.scatMulFactor",
					//						atmosphereParameters.mulScatMulFactor);
					//				}

					atmosphereParameters.numberOfAerosol = _dartInputsDAO.getDartAtmosphere().getAerosolsModels().size();
					for (const shared_ptr<DartInputs::AerosolModel> &aeroModel : _dartInputsDAO.getDartAtmosphere().getAerosolsModels()) {
						atmosphereParameters.aerosolsModelName.push_back(aeroModel.get()->getAerosolDatabase()->getAerosolOpticalProperty());
						atmosphereParameters.aerosolCumulativeModelName.push_back(aeroModel.get()->getAerosolDatabase()->getAerosolVerticalProfile());
						atmosphereParameters.hgParametersModelName.push_back(
							aeroModel.get()->getAerosolDatabase()->getAerosolHenyeyGreensteinParameters());
						atmosphereParameters.aerosolOpticalDepthFactor.push_back(aeroModel.get()->getAerosolOpticalDepthMultiplicativeFactor());
					}

					if (atmosphereParameters.h2oAmountCkeckbox) {
						// TODO:
						//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.defWaterAmount",
						//						atmosphereParameters.h2oAmountDef);
						//					switch (atmosphereParameters.h2oAmountDef) {
						//						case 0:
						//							cout << "Defining water vapour amount by multiplicative factor" << endl;
						//							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.M_factor.mulFactorH2O",
						//								atmosphereParameters.mulFactorH2O);
						//							break;
						//						case 1:
						//							cout << "Defining water vapour amount in g/cm2" << endl;
						//							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Amount_g_per_cm2.g_per_cm2",
						//								atmosphereParameters.def_amount_H2O_g_per_cm2);
						//							break;
						//						case 2:
						//							cout << "Defining water vapour amount in atm-cm" << endl;
						//							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Amount_atm_cm.atm_cm",
						//								atmosphereParameters.def_amount_H2O_atm_cm);
						//							break;
						//						case 3:
						//							cout << "Defining water vapour density in atm-cm/km" << endl;
						//							DartSAParameters::get(
						//								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_atm_cm_per_km_altitude.atm_cm_per_km",
						//								atmosphereParameters.def_density_atm_cm_per_km);
						//							DartSAParameters::get(
						//								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_atm_cm_per_km_altitude.altitude",
						//								atmosphereParameters.def_altitude);
						//							break;
						//						case 4:
						//							cout << "Defining water vapour density with relative humidity" << endl;
						//							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.rh",
						//								atmosphereParameters.def_rh);
						//							DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.temp",
						//								atmosphereParameters.def_temp);
						//							DartSAParameters::get(
						//								"Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.WaterAmount.Density_RH_temp_altitude.altitude",
						//								atmosphereParameters.def_altitude);
						//							break;
						//						default:
						//							cerr << "Option is not implemented" << endl;
						//							abort();
						//					}
					}

					// TODO:
					//				DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.redefTemperature",
					//					atmosphereParameters.redefTemperature);
					//				if (atmosphereParameters.redefTemperature) {
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.tempParameters.temp",
					//						atmosphereParameters.redef_temp);
					//					DartSAParameters::get("Atmosphere.IsAtmosphere.AtmosphericOpticalPropertyModel.tempParameters.altitude",
					//						atmosphereParameters.redef_altitude);
					//				}
				} else if (DartInputs::UserDefinedAtmosphereModel* userModel = dynamic_cast<DartInputs::UserDefinedAtmosphereModel*>(atmModel.get())) {
					atmosphereParameters.isAtmosphereDatabase = 0;

					atmosphereParameters.aerosolScaleFactor = userModel->getAerosolScaleFactor();
					atmosphereParameters.aerosolOpticalDepthForScattering = userModel->getAerosolOpticalDepth();
					atmosphereParameters.aerosolSingleScatteringAlbedo = userModel->getAerosolAlbedo();

					atmosphereParameters.henleyGreensteinParameters[0].a = userModel->getHenyeyGreensteinParameterA();
					atmosphereParameters.henleyGreensteinParameters[0].g1 = userModel->getHenyeyGreensteinParameterG1();
					atmosphereParameters.henleyGreensteinParameters[0].g2 = userModel->getHenyeyGreensteinParameterG2();

					atmosphereParameters.gasOpticalDepthForScattering = userModel->getGasOpticalDepth();
					atmosphereParameters.gasTotalTransmittanceForAbsoption = userModel->getGasTransmittance();
					atmosphereParameters.gasScaleFactor = userModel->getGasScaleFactor();

					atmosphereParameters.atmosphereTemperatureFileName = userModel->getAtmosphereTemperatureFile();
				}
			}
		} else {
			cout << "   -> No atmosphere simulation" << endl;
		}

	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**!
 * Lecture du nombre de fonctions de phase dans coeff_diff (mode multispectral)
 *
 * @param int nb_lambertien 				: nombre de fonctions lambertiennes.
 * @param int nb_lambertien_specular 	: nombre de fonctions lambertienne-speculaires.
 * @param int nb_hapke_specular 			: nombre de fonctions hapke-speculaire.
 * @param int nb_phase 					: nombre de fonctions de phase.
 * @param int nb_leaf 					: nombre de fonctions pour les leafs.
 *
 * @return 1 si okay
 */
bool PhaseDAO::readNumberOfOpticalPropertiesModels() {
	if (_useYaML) {
		return readNumberOfOpticalPropertiesModelsYaML();
	} else {
		return readNumberOfOpticalPropertiesModelsXML();
	}
}

bool PhaseDAO::readNumberOfOpticalPropertiesModelsXML() {
	try {
		PhaseProperty::setNumberOfLambertianModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti"));
		PhaseProperty::setNumberOfSpecularModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti"));
		PhaseProperty::setNumberOfHapkeModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti"));
		PhaseProperty::setNumberOfHapke2Models(DartSAParameters::getNbOf("Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti2"));
		PhaseProperty::setNumberOfFoliarModels(DartSAParameters::getNbOf("Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti"));
		PhaseProperty::setNumberOfRPVModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.RPVMultiFunctions.RPVMulti"));
		PhaseProperty::setNumberOfWaterInterfaceModels(DartSAParameters::getNbOf("Coeff_diff.WaterInterfaceMultiFunctions.WaterInterfaceFunction"));
		PhaseProperty::setNumberOfPhaseExternModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.PhaseExternMultiFunctions.PhaseExtern"));
		PhaseProperty::setNumberOfMixedMaterialModels(DartSAParameters::getNbOf("Coeff_diff.Surfaces.MixedMultiFunctions.MixedMulti"));
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readNumberOfOpticalPropertiesModelsYaML() {
	PhaseProperty::setNumberOfLambertianModels(_dartInputsDAO.getDartOpticalProperties().getLambertianProperties().size());
//	PhaseProperty::setNumberOfSpecularModels(_dartInputsDAO.getDartOpticalProperties().getSpecularProperties().size());
	PhaseProperty::setNumberOfHapkeModels(_dartInputsDAO.getDartOpticalProperties().getHapkeProperties().size());
	PhaseProperty::setNumberOfHapke2Models(_dartInputsDAO.getDartOpticalProperties().getHapke2Properties().size());
	PhaseProperty::setNumberOfRPVModels(_dartInputsDAO.getDartOpticalProperties().getRPVProperties().size());
	PhaseProperty::setNumberOfPhaseExternModels(_dartInputsDAO.getDartOpticalProperties().getPhaseExternProperties().size());
//	PhaseProperty::setNumberOfAirModels(_dartInputsDAO.getDartOpticalProperties().getFluidProperties().size());
	PhaseProperty::setNumberOfFoliarModels(_dartInputsDAO.getDartOpticalProperties().getVegetationProperties().size());
	PhaseProperty::setNumberOfWaterInterfaceModels(0);
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForIsProspectUseLambertian(	const int & index,
																	bool& useProspectGen,
																	int& lambertianDefinition) {
	useProspectGen = false;
	if (_useYaML) {
		return true;
	} else {
		try {
			ostringstream oss;
			oss << index;
			string temp = "Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str();
			DartSAParameters::get(temp + "].lambertianDefinition", lambertianDefinition);
			if (lambertianDefinition == 0) {
				DartSAParameters::get(temp + "].Lambertian.ProspectExternalModule.useProspectExternalModule", useProspectGen);
			}
		} catch (DartException &E) {
			cerr << E.getOriginMessage() << endl;
			return false;
		}
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForIsProspectUseFoliar(	const int & index,
																bool& useProspectGen,
																bool& bottomProspectGen,
																bool& hasDifferentModelForBottom) {
	useProspectGen = false;
	bottomProspectGen = false;
	if (_useYaML) {
		return true;
	} else {
		string temp, modelKey;
		ostringstream oss;
		try {
			oss << index;
			temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
			DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentModelForBottom);
			modelKey = hasDifferentModelForBottom ? "UnderstoryMultiTopModel" : "UnderstoryMultiModel";
			DartSAParameters::get(temp + "]." + modelKey + ".ProspectExternalModule.useProspectExternalModule", useProspectGen);
			if (hasDifferentModelForBottom) {
				string bottomModelKey;
				bottomModelKey = "UnderstoryMultiBottomModel";
				DartSAParameters::get(temp + "]." + bottomModelKey + ".ProspectExternalModule.useProspectExternalModule", bottomProspectGen);
			}
		} catch (DartException &E) {
			cerr << E.getOriginMessage() << endl;
			return false;
		}
		return true;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase lambertiennes dans coeff_diff (mode multispectral)
 *
 * @param int indice 					: numero de la fonction de phase en cours de traitement.
 * @param int arivar_lamb 			: flag permettant de savoir s'il faut ou non calculer cette fonction de phase.
 * @param string nom_lamb 	: nom de la fonction de phase lambertienne.
 * @param double ecart_type 		: valeur de l'ecart type.
 *
 * @return 1 si okay
 */
bool PhaseDAO::readCoeffDiffParametersForLambertian(const int & index,
													string & phaseName,
													string & modelName,
													string & databaseName,
													double & standardDeviation,
													bool& isSpecular,
													bool& useProspectGen,
													int& lambertianDefinition,
													double & smc, double & delta,
													bool & keepOutputFile) {
	if (_useYaML) {
		return readCoeffDiffParametersForLambertianYaML(index, phaseName, modelName, databaseName, standardDeviation, isSpecular);
	} else {
		return readCoeffDiffParametersForLambertianXML(index, phaseName, modelName, databaseName, standardDeviation, isSpecular, useProspectGen, lambertianDefinition, smc, delta, keepOutputFile);
	}
}
bool PhaseDAO::readCoeffDiffParametersForLambertianXML(	const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName,
														double & standardDeviation,
														bool& isSpecular,
														bool& useProspectGen,
														int& lambertianDefinition,
														double & smc, double & delta,
														bool & keepOutputFile) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].lambertianDefinition", lambertianDefinition);
		DartSAParameters::get(temp + "].roStDev", standardDeviation);
		if (lambertianDefinition == 0) {
			DartSAParameters::get(temp + "].Lambertian.ModelName", modelName);
			DartSAParameters::get(temp + "].Lambertian.databaseName", databaseName);
			DartSAParameters::get(temp + "].Lambertian.useSpecular", isSpecular);
			DartSAParameters::get(temp + "].Lambertian.ProspectExternalModule.useProspectExternalModule", useProspectGen);
		} else if (lambertianDefinition == 1) {
			// Marmit
			DartSAParameters::get(temp + "].Marmit.marmitDatabase", databaseName);
			DartSAParameters::getStringWithPossibleWhiteSpace(temp + "].Marmit.marmitModel", modelName);
			DartSAParameters::get(temp + "].Marmit.smc", smc);
			DartSAParameters::get(temp + "].Marmit.d", delta);
			DartSAParameters::get(temp + "].Marmit.keepOutputFile", keepOutputFile);
			isSpecular = false;
		}

	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForLambertianYaML(const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName,
														double & standardDeviation,
														bool& isSpecular) {

	DartInputs::LambertianProperty* lambertianProperty = _dartInputsDAO.getDartOpticalProperties().getLambertianProperties()[index];
	phaseName = lambertianProperty->getOpticalPropertyName();
	databaseName = lambertianProperty->getOpticalDatabase()->getDartFilePath();
	modelName = lambertianProperty->getOpticalDatabase()->getModelName();
	standardDeviation = lambertianProperty->getReflectanceVariability();
	isSpecular = lambertianProperty->getSpecularParameters() ? true : false;
	return true;
}

/***
 * LECTURE DE PHASE EXTERN
 */
bool PhaseDAO::readCoeffDiffParametersForPhaseExtern(	const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName) {
	if (_useYaML) {
		return readCoeffDiffParametersForPhaseExternYaML(index, phaseName, modelName, databaseName);
	} else {
		return readCoeffDiffParametersForPhaseExternXML(index, phaseName, modelName, databaseName);
	}

}
bool PhaseDAO::readCoeffDiffParametersForPhaseExternXML(const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.PhaseExternMultiFunctions.PhaseExtern[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].ModelName", modelName);
		DartSAParameters::get(temp + "].databaseName", databaseName);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForPhaseExternYaML(	const int & index,
															string & phaseName,
															string & modelName,
															string & databaseName) {
	DartInputs::PhaseExtern* phaseExtern = _dartInputsDAO.getDartOpticalProperties().getPhaseExternProperties()[index];
	phaseName = phaseExtern->getOpticalPropertyName();
	databaseName = phaseExtern->getOpticalDatabase()->getDartFilePath();
	modelName = phaseExtern->getOpticalDatabase()->getModelName();
	return true;
}



/*
 *    LAMBERTIEN
 */
bool PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(const int & index,
																  OpticalFactorParameters & opticalFactorParameters) {
	if (_useYaML) {
		return readCoeffDiffParametersOpticalFactorsYaML(index, globalReferences::LAMBERTIEN, opticalFactorParameters);
	} else {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersOpticalFactorsXML("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str() + "].lambertianNodeMultiplicativeFactorForLUT.", "lambertianMultiplicativeFactorForLUT", opticalFactorParameters);
	}
}

/*
 * Foliar
 */
bool PhaseDAO::readCoeffDiffParametersForFoliarOpticalFactors(const int & index,
																  OpticalFactorParameters & opticalFactorParameters) {
	if (_useYaML) {
		return readCoeffDiffParametersOpticalFactorsYaML(index, globalReferences::FOLIAIRE, opticalFactorParameters);
	} else {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersOpticalFactorsXML("Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str() + "].UnderstoryMultiModel.understoryNodeMultiplicativeFactorForLUT.", "understoryMultiplicativeFactorForLUT", opticalFactorParameters);
	}
}

bool PhaseDAO::readCoeffDiffParametersForFluidOpticalFactors(const int & index,
																  OpticalFactorParameters & opticalFactorParameters) {
	if (_useYaML) {
		return readCoeffDiffParametersOpticalFactorsYaML(index, globalReferences::FOLIAIRE, opticalFactorParameters);
	} else {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersOpticalFactorsXML("Coeff_diff.Volumes.AirMultiFunctions.AirFunction[" + oss.str() + "].AirFunctionNodeMultiplicativeFactorForLut.", "AirFunctionMultiplicativeFactorForLut", opticalFactorParameters);
	}
}


bool PhaseDAO::readCoeffDiffParametersOpticalFactorsXML(const string & key,
														const string & subNodeKey,
															OpticalFactorParameters & opticalFactorParameters) {
	bool useOpticalFactor = false;
	opticalFactorParameters.duplicateFirstLayer.resize(0);
	opticalFactorParameters.fileNames.resize(0);
	if (DartSAParameters::exists(key + "useSameOpticalFactorMatrixForAllBands")) {
		DartSAParameters::get(key + "useSameOpticalFactorMatrixForAllBands", opticalFactorParameters.sameOpticalFactorForAllBands);
		if (opticalFactorParameters.sameOpticalFactorForAllBands) {
			opticalFactorParameters.duplicateFirstLayer.resize(1);
			opticalFactorParameters.fileNames.resize(1);

			opticalFactorParameters.duplicateFirstLayer[0] = DartSAParameters::get<bool>(key + "opticalFactorMatrix.duplicateFirstMatrixLayer");
			DartSAParameters::get(key + "opticalFactorMatrix.opticalFactorMatrixFile", opticalFactorParameters.fileNames[0]);

			useOpticalFactor = true;
		} else {
			unsigned int nbMultiplicativeFactorNode = DartSAParameters::getNbOf(key + subNodeKey);
			opticalFactorParameters.duplicateFirstLayer.resize(nbMultiplicativeFactorNode, false);
			opticalFactorParameters.fileNames.resize(nbMultiplicativeFactorNode, "");
			for (unsigned int mfNodeIndex = 0; mfNodeIndex < nbMultiplicativeFactorNode; ++mfNodeIndex) {
				ostringstream oss;
				oss << mfNodeIndex;
				if (DartSAParameters::get<bool>(key + subNodeKey + "[" + oss.str() + "].useOpticalFactorMatrix")) {
					opticalFactorParameters.duplicateFirstLayer[mfNodeIndex] = DartSAParameters::get<bool>(key + subNodeKey + "[" + oss.str() + "].opticalFactorMatrix.duplicateFirstMatrixLayer");
					DartSAParameters::get(key + subNodeKey + "[" + oss.str() + "].opticalFactorMatrix.opticalFactorMatrixFile", opticalFactorParameters.fileNames[mfNodeIndex]);
					useOpticalFactor = true;
				}
			}
		}
	}
	return useOpticalFactor;
}

bool PhaseDAO::readCoeffDiffParametersOpticalFactorsYaML(const int & index,
											const int & TYPE,
									OpticalFactorParameters & opticalFactorParameters) {
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForLambertianFluspect(const int & index,
															FluorescenceParameters & fluorescenceParameters) {
	if (_useYaML) {
		return readCoeffDiffParameterForFluspectYaML(index, globalReferences::LAMBERTIEN, fluorescenceParameters);
	} else {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersForFluspect("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str() + "].Lambertian.ProspectExternalModule.",
			fluorescenceParameters);
	}
}

bool PhaseDAO::readCoeffDiffParametersForFoliarFluspect(const int & index,
														FluorescenceParameters & fluorescenceParameters,
														bool & hasDifferentBottom,
														bool bottom) {
	if (_useYaML) {
		return readCoeffDiffParameterForFluspectYaML(index, globalReferences::FOLIAIRE, fluorescenceParameters);
	} else {
		ostringstream oss;
		oss << index;

		hasDifferentBottom = false;
		string temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
		DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentBottom);
		string modelKey = hasDifferentBottom ? (bottom ? "UnderstoryMultiBottomModel" : "UnderstoryMultiTopModel") : "UnderstoryMultiModel";
		return readCoeffDiffParametersForFluspect(temp + "]." + modelKey + ".ProspectExternalModule.", fluorescenceParameters);
	}
}

bool PhaseDAO::readCoeffDiffParameterForFluspectYaML(	const int & index,
														const int & TYPE,
														FluorescenceParameters & fluorescenceParameters) {
	fluorescenceParameters.isFluorescent = false;
	fluorescenceParameters.forceYield = false;
	shared_ptr<DartInputs::ProspectFunction> inputProspect;
	if (TYPE == globalReferences::LAMBERTIEN) {
		DartInputs::LambertianProperty* lambertianProperty = _dartInputsDAO.getDartOpticalProperties().getLambertianProperties()[index];
		inputProspect = lambertianProperty->getProspectFunction();
	} else if (TYPE == globalReferences::FOLIAIRE) {
		shared_ptr<DartInputs::VegetationProperty> vegetationProperty = _dartInputsDAO.getDartOpticalProperties().getVegetationProperties()[index];
		inputProspect = vegetationProperty->getProspectFunction();
	}

	if (inputProspect) {
		if (const shared_ptr<DartInputs::FluorescenceFunction> inputFluo = inputProspect->getFluorescence()) {
			fluorescenceParameters.isFluorescent = true;
			fluorescenceParameters.forceYield = inputFluo->getForceYields();
			if (fluorescenceParameters.forceYield) {
				fluorescenceParameters.yieldPSI = inputFluo->getPhotoSystemI();
				fluorescenceParameters.yieldPSII = inputFluo->getPhotoSystemII();
			} else {
				fluorescenceParameters.yieldPSI = 0.002;
				fluorescenceParameters.yieldPSII = 0.01;
			}
			fluorescenceParameters.databaseName = inputProspect->getOpticalParametersFile();
			fluorescenceParameters.structureCoeff = inputProspect->getStructureCoefficient();
			fluorescenceParameters.chlorophyll = inputProspect->getClorophyll();
			fluorescenceParameters.carotenoid = inputProspect->getCarotenoid();
			fluorescenceParameters.brownPigment = inputProspect->getBrownPigment();
			fluorescenceParameters.waterThickness = inputProspect->getEquivalentWaterThickness();
			if (inputProspect->getLeafMassPerArea()) {
				fluorescenceParameters.leafMassPerArea = *inputProspect->getLeafMassPerArea().get();
			}
			fluorescenceParameters.anthocyanin = inputProspect->getAnthocyanin();

			fluorescenceParameters.useBioCLimaticProfileEta = inputFluo->getBioClimaticWeighting();
			if (fluorescenceParameters.useBioCLimaticProfileEta) {
				fluorescenceParameters.bioClimaticFileRelativePath = inputFluo->getBioClimaticFile();
				if (inputFluo->getOpticalPropertyGrouping()) {
					int itemid = inputFluo->getGroupingOpticalPropertyId();
					bool found = false;
					for (unsigned int i = 0; i < _dartInputsDAO.getDartOpticalProperties().getLambertianProperties().size(); ++i) {
						if (itemid == _dartInputsDAO.getDartOpticalProperties().getLambertianProperties()[i]->getItemId()) {
							fluorescenceParameters.linkedOpticalPropertyIndex = i;
							fluorescenceParameters.linkedOpticalPropertyType = globalReferences::LAMBERTIEN;
							found = true;
							break;
						}
					}
					if (!found) {
						for (unsigned int i = 0; i < _dartInputsDAO.getDartOpticalProperties().getHapkeProperties().size(); ++i) {
							if (itemid == _dartInputsDAO.getDartOpticalProperties().getHapkeProperties()[i]->getItemId()) {
								fluorescenceParameters.linkedOpticalPropertyIndex = i;
								fluorescenceParameters.linkedOpticalPropertyType = globalReferences::HAPKE_SPEC;
								found = true;
								break;
							}
						}
					}
					if (!found) {
						for (unsigned int i = 0; i < _dartInputsDAO.getDartOpticalProperties().getHapke2Properties().size(); ++i) {
							if (itemid == _dartInputsDAO.getDartOpticalProperties().getHapke2Properties()[i]->getItemId()) {
								fluorescenceParameters.linkedOpticalPropertyIndex = i;
								fluorescenceParameters.linkedOpticalPropertyType = globalReferences::HAPKE2_SPEC;
								found = true;
								break;
							}
						}
					}
					if (!found) {
						for (unsigned int i = 0; i < _dartInputsDAO.getDartOpticalProperties().getRPVProperties().size(); ++i) {
							if (itemid == _dartInputsDAO.getDartOpticalProperties().getRPVProperties()[i]->getItemId()) {
								fluorescenceParameters.linkedOpticalPropertyIndex = i;
								fluorescenceParameters.linkedOpticalPropertyType = globalReferences::RPV_PHASE;
								found = true;
								break;
							}
						}
					}
					if (!found) {
						for (unsigned int i = 0; i < _dartInputsDAO.getDartOpticalProperties().getPhaseExternProperties().size(); ++i) {
							cout<<"i"<<flush;
							if (itemid == _dartInputsDAO.getDartOpticalProperties().getPhaseExternProperties()[i]->getItemId()) {
								fluorescenceParameters.linkedOpticalPropertyIndex = i;
								fluorescenceParameters.linkedOpticalPropertyType = globalReferences::PHASE_EXTERN;
								found = true;
								break;
							}
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool PhaseDAO::readCoeffDiffParametersForProspect(		const int & modelIndex,
														const int & prospectFunction,
														ProspectParameters & prospectParameters) {
	string temp;
	ostringstream oss;
	oss << modelIndex;
	switch(prospectFunction) {
		case globalReferences::PROSPECT_LAMBERTIEN:
			temp = "Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str();
			temp += "].Lambertian.ProspectExternalModule.";
			return readCoeffDiffParametersForProspectXML(temp, prospectParameters);
		case globalReferences::PROSPECT_FOLIAIRE:
			temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
			temp += "].UnderstoryMultiModel.ProspectExternalModule.";
			return readCoeffDiffParametersForProspectXML(temp, prospectParameters);
		case globalReferences::PROSPECT_FOLIAIRE_TOP:
			temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
			temp += "].UnderstoryMultiTopModel.ProspectExternalModule.";
			return readCoeffDiffParametersForProspectXML(temp, prospectParameters);
		case globalReferences::PROSPECT_FOLIAIRE_BOTTOM:
			temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
			temp += "].UnderstoryMultiBottomModel.ProspectExternalModule.";
			return readCoeffDiffParametersForProspectXML(temp, prospectParameters);
		default:
			break;
	}
	return false;
}


bool PhaseDAO::readCoeffDiffParametersForProspectXML(	const std::string & prefixKey,
														ProspectParameters & prospectParameters) {
	try {
		bool useProspectGen = false;
		DartSAParameters::get(prefixKey + "useProspectExternalModule", useProspectGen);
		if (useProspectGen) {
			prospectParameters.pythonScript = "Fluspect_B_CX_PRO.py";

			string prospectKey = prefixKey + "ProspectExternParameters.";
			DartSAParameters::get(prospectKey + "inputProspectFile", prospectParameters.inputProspectFile);
			DartSAParameters::get(prospectKey + "N", prospectParameters.structureCoeff);
			DartSAParameters::get(prospectKey + "Cab", prospectParameters.chlorophyll);
			DartSAParameters::get(prospectKey + "Car", prospectParameters.carotenoid);
			DartSAParameters::get(prospectKey + "CBrown", prospectParameters.brownPigment);
			DartSAParameters::get(prospectKey + "Cw", prospectParameters.waterThickness);

			DartSAParameters::get(prospectKey + "useCm", prospectParameters.useNewCm);

			bool isv2z = false;
			DartSAParameters::get(prospectKey + "isV2Z", isv2z);
			if (isv2z) {
				DartSAParameters::get(prospectKey + "V2Z.v2z", prospectParameters.v2z);
			} else {
				prospectParameters.v2z = -999;
			}

			if (prospectParameters.useNewCm) {
				DartSAParameters::get(prospectKey + "Cbc", prospectParameters.Cbc);
				DartSAParameters::get(prospectKey + "Cp", prospectParameters.Cp);
				// Retrocompatibility if need to use the old script
				prospectParameters.leafMassPerArea = prospectParameters.Cbc+prospectParameters.Cp;
			} else {
				DartSAParameters::get(prospectKey + "Cm", prospectParameters.leafMassPerArea);
				prospectParameters.Cbc = 0;
				prospectParameters.Cp = 0;
			}

			DartSAParameters::get(prospectKey + "Cm", prospectParameters.leafMassPerArea);
			if (DartSAParameters::exists(prospectKey + "anthocyanin")) {
				DartSAParameters::get(prospectKey + "anthocyanin", prospectParameters.anthocyanin);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForFluspect(	const std::string & key,
													FluorescenceParameters & fluorescenceParameters) {
	try {
		fluorescenceParameters.isFluorescent = false;
		fluorescenceParameters.forceYield = false;
		if (DartSAParameters::exists(key + "isFluorescent")) // With MARMIT, key doesn't exists & fluorescence is impossible
		{
			DartSAParameters::get(key + "isFluorescent", fluorescenceParameters.isFluorescent);
			DartSAParameters::get("Coeff_diff.useCombinedYield", fluorescenceParameters.useCombinedYields);

			if (fluorescenceParameters.isFluorescent) {
				DartSAParameters::get(key + "FluorescenceYields.forceYields", fluorescenceParameters.forceYield);
				if (fluorescenceParameters.forceYield) {
	//				DartSAParameters::get(key + "FluorescenceYields.Yields.useCombinedYield", fluorescenceParameters.useCombinedYields);
					if (fluorescenceParameters.useCombinedYields) {
						DartSAParameters::get(key + "FluorescenceYields.Yield.yieldPS", fluorescenceParameters.yieldPSI);
						fluorescenceParameters.yieldPSII = 0.;
						fluorescenceParameters.pythonScript = "Fluspect_B_CX_PRO.py";
					} else {
						DartSAParameters::get(key + "FluorescenceYields.Yields.yieldPSI", fluorescenceParameters.yieldPSI);
						DartSAParameters::get(key + "FluorescenceYields.Yields.yieldPSII", fluorescenceParameters.yieldPSII);
						fluorescenceParameters.pythonScript = "Fluspect_B_CX_P6.py";
					}
				} else {
					if (fluorescenceParameters.useCombinedYields) {
						fluorescenceParameters.yieldPSI = 0.012;
						fluorescenceParameters.yieldPSII = 0.0;
						fluorescenceParameters.pythonScript = "Fluspect_B_CX_PRO.py";
					} else {
						fluorescenceParameters.yieldPSI = 0.002;
						fluorescenceParameters.yieldPSII = 0.01;
						fluorescenceParameters.pythonScript = "Fluspect_B_CX_P6.py";
					}
				}

				string subKey = key + "ProspectExternParameters.";
				DartSAParameters::get(subKey + "inputProspectFile", fluorescenceParameters.databaseName);
				DartSAParameters::get(subKey + "N", fluorescenceParameters.structureCoeff);
				DartSAParameters::get(subKey + "Cab", fluorescenceParameters.chlorophyll);
				DartSAParameters::get(subKey + "Car", fluorescenceParameters.carotenoid);
				DartSAParameters::get(subKey + "CBrown", fluorescenceParameters.brownPigment);
				DartSAParameters::get(subKey + "Cw", fluorescenceParameters.waterThickness);

				bool isv2z = false;
				DartSAParameters::get(subKey + "isV2Z", isv2z);
				if (isv2z) {
					DartSAParameters::get(subKey + "V2Z.v2z", fluorescenceParameters.v2z);
				} else {
					fluorescenceParameters.v2z = -999;
				}

				DartSAParameters::get(subKey + "useCm", fluorescenceParameters.useNewCm);
				if (fluorescenceParameters.useNewCm) {
					DartSAParameters::get(subKey + "Cbc", fluorescenceParameters.Cbc);
					DartSAParameters::get(subKey + "Cp", fluorescenceParameters.Cp);
					// Retrocompatibility if need to use the old script
					fluorescenceParameters.leafMassPerArea = fluorescenceParameters.Cbc+fluorescenceParameters.Cp;
				} else {
					DartSAParameters::get(subKey + "Cm", fluorescenceParameters.leafMassPerArea);
				}

				DartSAParameters::get(subKey + "Cm", fluorescenceParameters.leafMassPerArea);
				if (DartSAParameters::exists(subKey + "anthocyanin")) {
					DartSAParameters::get(subKey + "anthocyanin", fluorescenceParameters.anthocyanin);
				}

				if (DartSAParameters::exists(key + "WindProfileEta.useBioClimaticWeighting")) {
					DartSAParameters::get(key + "WindProfileEta.useBioClimaticWeighting", fluorescenceParameters.useBioCLimaticProfileEta);
					if (fluorescenceParameters.useBioCLimaticProfileEta) {
						DartSAParameters::get(key + "WindProfileEta.BioClimaticWeighting.profileFilePath",
							fluorescenceParameters.bioClimaticFileRelativePath);
						bool hasLinkedProperty = false;
						DartSAParameters::get(key + "WindProfileEta.BioClimaticWeighting.linkedOpticalProperty", hasLinkedProperty);
						if (hasLinkedProperty) {
							DartSAParameters::get(key + "WindProfileEta.BioClimaticWeighting.OpticalPropertyLink.indexFctPhase",
								fluorescenceParameters.linkedOpticalPropertyIndex);
							DartSAParameters::get(key + "WindProfileEta.BioClimaticWeighting.OpticalPropertyLink.type",
								fluorescenceParameters.linkedOpticalPropertyType);
						} else {
							fluorescenceParameters.linkedOpticalPropertyIndex = -1;
							fluorescenceParameters.linkedOpticalPropertyType = -1;
						}
					}
				} else {
					fluorescenceParameters.useBioCLimaticProfileEta = false;
				}
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForSpecularLambertian(const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution) {
	if (_useYaML) {
		return readCoeffDiffParametersForSpecularLambertianYaML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	} else {
		return readCoeffDiffParametersForSpecularLambertianXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	}
}
bool PhaseDAO::readCoeffDiffParametersForSpecularLambertianXML(	const int & index,
																string & modelName,
																string & databaseName,
																bool & useRefraction,
																int & specularRadianceDistribution) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str();
		DartSAParameters::get(temp + "].Lambertian.SpecularData.specularModelName", modelName);
		DartSAParameters::get(temp + "].Lambertian.SpecularData.specularDatabaseName", databaseName);
		DartSAParameters::get(temp + "].Lambertian.SpecularData.useRefraction", useRefraction);
		DartSAParameters::get(temp + "].Lambertian.SpecularData.specularRadianceDistribution", specularRadianceDistribution);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForSpecularLambertianYaML(const int & index,
																string & modelName,
																string & databaseName,
																bool & useRefraction,
																int & specularRadianceDistribution) {
	DartInputs::LambertianProperty* lambertianProperty = _dartInputsDAO.getDartOpticalProperties().getLambertianProperties()[index];
	if (lambertianProperty->getSpecularParameters()) {
		modelName = lambertianProperty->getSpecularParameters()->getSpecularDatabase()->getModelName();
		databaseName = lambertianProperty->getSpecularParameters()->getSpecularDatabase()->getDartFilePath();
//			useRefraction = !lambertianNode["specularParameters"].IsNull();
		// TODO : specularRadianceDistribution
	} else {
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForRPVThetaMax(	const int & index,
														double & theta_max) {
	if (_useYaML) {
		return readCoeffDiffParametersForRPVThetaMaxYaML(index, theta_max);
	} else {
		return readCoeffDiffParametersForRPVThetaMaxXML(index, theta_max);
	}
}

bool PhaseDAO::readCoeffDiffParametersForRPVThetaMaxXML(	const int & index,
															double & theta_max) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.RPVMultiFunctions.RPVMulti[" + oss.str();
		DartSAParameters::get(temp + "].theta_max", theta_max);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForRPVThetaMaxYaML(	const int & index,
															double & theta_max) {
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForRPVSpecular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution) {
	if (_useYaML) {
		return readCoeffDiffParametersForRPVSpecularYaML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	} else {
		return readCoeffDiffParametersForRPVSpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	}
}

bool PhaseDAO::readCoeffDiffParametersForRPVSpecularXML(const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.RPVMultiFunctions.RPVMulti[" + oss.str();
		DartSAParameters::get(temp + "].SpecularData.specularModelName", modelName);
		DartSAParameters::get(temp + "].SpecularData.specularDatabaseName", databaseName);
		DartSAParameters::get(temp + "].SpecularData.useRefraction", useRefraction);
		DartSAParameters::get(temp + "].SpecularData.specularRadianceDistribution", specularRadianceDistribution);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForRPVSpecularYaML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution) {
	DartInputs::RPVProperty* rpvProperty = _dartInputsDAO.getDartOpticalProperties().getRPVProperties()[index];
	if (rpvProperty->getSpecularParameters()) {
		modelName = rpvProperty->getSpecularParameters()->getSpecularDatabase()->getModelName();
		databaseName = rpvProperty->getSpecularParameters()->getSpecularDatabase()->getDartFilePath();
//			useRefraction = !lambertianNode["specularParameters"].IsNull();
		// TODO : specularRadianceDistribution
	} else {
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase de type "Hapke + Speculaire" dans coeff_diff (mode multispectral).
 *
 * @param int indice        			: numero de la fonction de phase en cours de traitement.
 * @param int arivar_hapke 			: flag permettant de savoir s'il faut ou non calculer cette fonction de phase.
 * @param string nom_hapke 	: nom de la fonction de phase hapke.
 *
 * @return 1 si okay.
 */
bool PhaseDAO::readCoeffDiffParametersForHapke(	const int & index,
												string & phaseName,
												string & modelName,
												string & databaseName,
												string & transmittanceModelName,
												string & transmittanceDatabaseName,
												bool& isSpecular,
												int &indexOrder) {

	if (_useYaML) {
		return readCoeffDiffParametersForHapkeYaML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular);
	} else {
		return readCoeffDiffParametersForHapkeXML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular, indexOrder);
	}
}
bool PhaseDAO::readCoeffDiffParametersForHapkeXML(	const int & index,
													string & phaseName,
													string & modelName,
													string & databaseName,
													string & transmittanceModelName,
													string & transmittanceDatabaseName,
													bool& isSpecular,
													int &indexOrder) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].ModelName", modelName);
		DartSAParameters::get(temp + "].databaseName", databaseName);
		DartSAParameters::get(temp + "].transmittanceModelName", transmittanceModelName);
		DartSAParameters::get(temp + "].transmittanceDatabaseName", transmittanceDatabaseName);
		DartSAParameters::get(temp + "].useSpecular", isSpecular);
		indexOrder = DartSAParameters::getKeyIndex(temp+"]");      // IndexOrder gives the lecture rank of the model taking into account hapke2
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForHapkeYaML(	const int & index,
													string & phaseName,
													string & modelName,
													string & databaseName,
													string & transmittanceModelName,
													string & transmittanceDatabaseName,
													bool& isSpecular) {
	try {
		DartInputs::HapkeProperty* hapkeProperty = _dartInputsDAO.getDartOpticalProperties().getHapkeProperties()[index];
		phaseName = hapkeProperty->getOpticalPropertyName();
		modelName = hapkeProperty->getOpticalDatabase()->getModelName();
		databaseName = hapkeProperty->getOpticalDatabase()->getDartFilePath();
		transmittanceModelName = hapkeProperty->getTransmitanceDatabase()->getModelName();
		transmittanceDatabaseName = hapkeProperty->getTransmitanceDatabase()->getDartFilePath();
		isSpecular = hapkeProperty->getSpecularParameters() ? true : false;
	} catch (YAML::Exception &E) {
		cerr << E.msg << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForHapkeSpecular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int &indexOrder) {
	if (_useYaML) {
		return readCoeffDiffParametersForHapkeSpecularYaML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	} else {
		return readCoeffDiffParametersForHapkeSpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution, indexOrder);
	}
}
bool PhaseDAO::readCoeffDiffParametersForHapkeSpecularXML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution,
															int & indexOrder) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].SpecularData.specularModelName", modelName);
		DartSAParameters::get(temp + "].SpecularData.specularDatabaseName", databaseName);
		DartSAParameters::get(temp + "].SpecularData.useRefraction", useRefraction);
		DartSAParameters::get(temp + "].SpecularData.specularRadianceDistribution", specularRadianceDistribution);
		indexOrder= DartSAParameters::getKeyIndex(temp+"]");

	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForHapkeSpecularYaML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution) {
	DartInputs::HapkeProperty* hapkeProperty = _dartInputsDAO.getDartOpticalProperties().getHapkeProperties()[index];
	if (hapkeProperty->getSpecularParameters()) {
		modelName = hapkeProperty->getSpecularParameters()->getSpecularDatabase()->getModelName();
		databaseName = hapkeProperty->getSpecularParameters()->getSpecularDatabase()->getDartFilePath();
//			useRefraction = !lambertianNode["specularParameters"].IsNull();
		// TODO : specularRadianceDistribution
	} else {
		return false;
	}
	return true;
}



/*
 * for hapke2012
 */
bool PhaseDAO::readCoeffDiffParametersForHapke2(	const int & index,
												string & phaseName,
												string & modelName,
												string & databaseName,
												string & transmittanceModelName,
												string & transmittanceDatabaseName,
												bool& isSpecular,
												int &indexOrder) {
	if (_useYaML) {
		return readCoeffDiffParametersForHapke2YaML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular);
	} else {
		return readCoeffDiffParametersForHapke2XML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular, indexOrder);
	}
}
bool PhaseDAO::readCoeffDiffParametersForHapke2XML(	const int & index,
													string & phaseName,
													string & modelName,
													string & databaseName,
													string & transmittanceModelName,
													string & transmittanceDatabaseName,
													bool& isSpecular,
													int & indexOrder) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti2[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].ModelName", modelName);
		DartSAParameters::get(temp + "].databaseName", databaseName);
		DartSAParameters::get(temp + "].transmittanceModelName", transmittanceModelName);
		DartSAParameters::get(temp + "].transmittanceDatabaseName", transmittanceDatabaseName);
		DartSAParameters::get(temp + "].useSpecular", isSpecular);
		indexOrder = DartSAParameters::getKeyIndex(temp+"]");    // IndexOrder gives the lecture rank of the model taking into account hapke

	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForHapke2YaML(	const int & index,
													string & phaseName,
													string & modelName,
													string & databaseName,
													string & transmittanceModelName,
													string & transmittanceDatabaseName,
													bool& isSpecular) {
	try {
		DartInputs::Hapke2Property* hapke2Property = _dartInputsDAO.getDartOpticalProperties().getHapke2Properties()[index];
		phaseName = hapke2Property->getOpticalPropertyName();
		modelName = hapke2Property->getOpticalDatabase()->getModelName();
		databaseName = hapke2Property->getOpticalDatabase()->getDartFilePath();
		transmittanceModelName = hapke2Property->getTransmitanceDatabase()->getModelName();
		transmittanceDatabaseName = hapke2Property->getTransmitanceDatabase()->getDartFilePath();
		isSpecular = hapke2Property->getSpecularParameters() ? true : false;
	} catch (YAML::Exception &E) {
		cerr << E.msg << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForHapke2Specular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int &indexOrder) {
	if (_useYaML) {
		return readCoeffDiffParametersForHapke2SpecularYaML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
	} else {
		return readCoeffDiffParametersForHapke2SpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution, indexOrder);
	}
}
bool PhaseDAO::readCoeffDiffParametersForHapke2SpecularXML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution,
															int & indexOrder) {
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti2[" + oss.str();
		DartSAParameters::get(temp + "].SpecularData.specularModelName", modelName);
		DartSAParameters::get(temp + "].SpecularData.specularDatabaseName", databaseName);
		DartSAParameters::get(temp + "].SpecularData.useRefraction", useRefraction);
		DartSAParameters::get(temp + "].SpecularData.specularRadianceDistribution", specularRadianceDistribution);
		indexOrder = DartSAParameters::getKeyIndex(temp+"]");
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForHapke2SpecularYaML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution) {
	DartInputs::Hapke2Property* hapke2Property = _dartInputsDAO.getDartOpticalProperties().getHapke2Properties()[index];
	if (hapke2Property->getSpecularParameters()) {
		modelName = hapke2Property->getSpecularParameters()->getSpecularDatabase()->getModelName();
		databaseName = hapke2Property->getSpecularParameters()->getSpecularDatabase()->getDartFilePath();
//			useRefraction = !lambertianNode["specularParameters"].IsNull();
		// TODO : specularRadianceDistribution
	} else {
		return false;
	}
	return true;
}

/* read parameters of specular scatteirng function
 *
 * */
bool PhaseDAO::readCoeffDiffParametersForSpecular(	const int & index,
											std::string & phaseName,
											std::string & databaseName,
											std::string & niModelName,
											std::string & ntModelName){
	if (_useYaML) {
		return readCoeffDiffParametersForSpecularYaML(index, phaseName, databaseName, niModelName, ntModelName);
	} else {
		return readCoeffDiffParametersForSpecularXML(index, phaseName, databaseName, niModelName, ntModelName);
	}
}
bool PhaseDAO::readCoeffDiffParametersForSpecularXML(	const int & index,
											std::string & phaseName,
											std::string & databaseName,
											std::string & niModelName,
											std::string & ntModelName){
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].databaseName", databaseName);
		DartSAParameters::get(temp + "].ni", niModelName);
		DartSAParameters::get(temp + "].nt", ntModelName);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readCoeffDiffParametersForSpecularYaML(	const int & index,
											std::string & phaseName,
											std::string & databaseName,
											std::string & niModelName,
											std::string & ntModelName){
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForSpecularMaterial(const int & index, int & type, int & baseBSDF, int & baseBSDFindex){
	if (_useYaML) {
		return readCoeffDiffParametersForSpecularMaterialYaML(index, type, baseBSDF, baseBSDFindex);
	} else {
		return readCoeffDiffParametersForSpecularMaterialXML(index, type, baseBSDF, baseBSDFindex);
	}
}
bool PhaseDAO::readCoeffDiffParametersForSpecularMaterialXML(const int & index, int & type, int & baseBSDF, int & baseBSDFindex){
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].materialCharacter", type);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	if(type == 2){
		try {
			ostringstream oss;
			oss << index;
			string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
			DartSAParameters::get(temp + "].OpticalPropertyLink.type", baseBSDF);
		} catch (DartException &E) {
			cerr << E.getOriginMessage() << endl;
			return false;
		}

		try {
			ostringstream oss;
			oss << index;
			string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
			DartSAParameters::get(temp + "].OpticalPropertyLink.indexFctPhase", baseBSDFindex);
		} catch (DartException &E) {
			cerr << E.getOriginMessage() << endl;
			return false;
		}
	}else{
		baseBSDF = -1;
		baseBSDFindex = -1;
	}

	return true;
}
bool PhaseDAO::readCoeffDiffParametersForSpecularMaterialYaML(const int & index, int & type, int & baseBSDF, int & baseBSDFindex){
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForSpecularRoughness(const int & index, double & roughness){
	if (_useYaML) {
		return readCoeffDiffParametersForSpecularRoughnessYaML(index, roughness);
	} else {
		return readCoeffDiffParametersForSpecularRoughnessXML(index, roughness);
	}
}
bool PhaseDAO::readCoeffDiffParametersForSpecularRoughnessXML(const int & index, double & roughness){
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].roughnessFactor", roughness);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForSpecularRoughnessYaML(const int & index, double & roughness){
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForSpecularTau(const int & index, double & tau){
	if (_useYaML) {
		return readCoeffDiffParametersForSpecularTauYaML(index, tau);
	} else {
		return readCoeffDiffParametersForSpecularTauXML(index, tau);
	}
}
bool PhaseDAO::readCoeffDiffParametersForSpecularTauXML(const int & index, double & tau){
	try {
		ostringstream oss;
		oss << index;
		string temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
		DartSAParameters::get(temp + "].opticalDepth", tau);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForSpecularTauYaML(const int & index, double & Tau){
	return true;
}

// Mixed materials
bool PhaseDAO::readCoeffDiffParametersForMixedMaterial(unsigned int modelIndex,
													std::string & phaseName,
													unsigned int & material1Type,
													unsigned int & material1Index,
													std::string & material1Name,
													unsigned int & material2Type,
													unsigned int & material2Index,
													std::string & material2Name,
													double & percentOfMaterial1) {
	if (_useYaML) {
		return readCoeffDiffParametersForMixedMaterialYaML(modelIndex, phaseName, material1Type, material1Index, material1Name, material2Type, material2Index, material2Name, percentOfMaterial1);
	} else {
		return readCoeffDiffParametersForMixedMaterialXML(modelIndex, phaseName, material1Type, material1Index, material1Name, material2Type, material2Index, material2Name, percentOfMaterial1);
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForMixedMaterialXML(unsigned int modelIndex,
													std::string & phaseName,
													unsigned int & material1Type,
													unsigned int & material1Index,
													std::string & material1Name,
													unsigned int & material2Type,
													unsigned int & material2Index,
													std::string & material2Name,
													double & percentOfMaterial1) {
	try {
		ostringstream oss;
		oss << modelIndex;
		string temp = "Coeff_diff.Surfaces.MixedMultiFunctions.MixedMulti[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].FirstProperty.percentFirst", percentOfMaterial1);
		DartSAParameters::get(temp + "].FirstProperty.OpticalPropertyLink.type", material1Type);
		DartSAParameters::get(temp + "].FirstProperty.OpticalPropertyLink.indexFctPhase", material1Index);
		DartSAParameters::get(temp + "].FirstProperty.OpticalPropertyLink.ident", material1Name);
		DartSAParameters::get(temp + "].SecondProperty.OpticalPropertyLink.type", material2Type);
		DartSAParameters::get(temp + "].SecondProperty.OpticalPropertyLink.indexFctPhase", material2Index);
		DartSAParameters::get(temp + "].SecondProperty.OpticalPropertyLink.ident", material2Name);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readCoeffDiffParametersForMixedMaterialYaML(unsigned int modelIndex,
													std::string & phaseName,
													unsigned int & material1Type,
													unsigned int & material1Index,
													std::string & material1Name,
													unsigned int & material2Type,
													unsigned int & material2Index,
													std::string & material2Name,
													double & percentOfMaterial1) {
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions phase RPV dans coeff_diff (mode multispectral)
 *
 * @param int indice        			: numero de la fonction de phase en cours de traitement.
 * @param int arivar_phase 			: flag permettant de savoir s'il faut ou non calculer cette fonction de phase.
 * @param string nom_phase 	: nom de la fonction de phase.
 * @param int aritype_diff_phase 	: type de diffusion ("Lambertien + Speculaire" ou "Hapke + speculaire")
 *
 * @return 1 si okay.
 */
bool PhaseDAO::readCoeffDiffParametersForRPV(	const int & indice,
												string & phaseName,
												string & modelName,
												string & databaseName,
												bool& isSpecular,
												string & transmittanceModelName,
												string & transmittanceDatabaseName) {
	if (_useYaML) {
		return readCoeffDiffParametersForRPVYaML(indice, phaseName, modelName, databaseName, isSpecular, transmittanceModelName,
			transmittanceDatabaseName);
	} else {
		return readCoeffDiffParametersForRPVXML(indice, phaseName, modelName, databaseName, isSpecular, transmittanceModelName,
			transmittanceDatabaseName);
	}
}
bool PhaseDAO::readCoeffDiffParametersForRPVXML(const int & indice,
												string & phaseName,
												string & modelName,
												string & databaseName,
												bool& isSpecular,
												string & transmittanceModelName,
												string & transmittanceDatabaseName) {
	try {
		ostringstream oss;
		oss << indice;
		string temp = "Coeff_diff.Surfaces.RPVMultiFunctions.RPVMulti[" + oss.str();
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].ModelName", modelName);
		DartSAParameters::get(temp + "].databaseName", databaseName);
		DartSAParameters::get(temp + "].useSpecular", isSpecular);
		DartSAParameters::get(temp + "].transmittanceModelName", transmittanceModelName);
		DartSAParameters::get(temp + "].transmittanceDatabaseName", transmittanceDatabaseName);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readCoeffDiffParametersForRPVYaML(	const int & indice,
													string & phaseName,
													string & modelName,
													string & databaseName,
													bool& isSpecular,
													string & transmittanceModelName,
													string & transmittanceDatabaseName) {
	try {
		DartInputs::RPVProperty* rpvProperty = _dartInputsDAO.getDartOpticalProperties().getRPVProperties()[indice];
		phaseName = rpvProperty->getOpticalPropertyName();
		modelName = rpvProperty->getOpticalDatabase()->getModelName();
		databaseName = rpvProperty->getOpticalDatabase()->getDartFilePath();
		transmittanceModelName = rpvProperty->getTransmitanceDatabase()->getModelName();
		transmittanceDatabaseName = rpvProperty->getTransmitanceDatabase()->getDartFilePath();
		isSpecular = rpvProperty->getSpecularParameters() ? true : false;
	} catch (YAML::Exception &E) {
		cerr << E.msg << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readCoeffDiffParametersForFoliarDatabases(	const unsigned int & modelIndex,
															string & phaseName,
															string & modelName,
															string & databaseName,
															bool & isSpecular,
															bool & useProspectGen,
															bool & hasDifferentBottom,
															string & specularDatabaseName,
															string & specularDatabaseModel,
															string & bottomDatabase,
															string & bottomModel,
															bool & bottomProspectGen) {
	if (_useYaML) {
		return readCoeffDiffParametersForFoliarDatabasesYaML(modelIndex, phaseName, modelName, databaseName, isSpecular, specularDatabaseName,
			specularDatabaseModel);
	} else {
		return readCoeffDiffParametersForFoliarDatabasesXML(modelIndex, phaseName, modelName, databaseName, isSpecular, useProspectGen, hasDifferentBottom,
			specularDatabaseName, specularDatabaseModel, bottomDatabase, bottomModel, bottomProspectGen);
	}
}
bool PhaseDAO::readCoeffDiffParametersForFoliarDatabasesXML(const unsigned int & modelIndex,
															string & phaseName,
															string & modelName,
															string & databaseName,
															bool & isSpecular,
															bool & useProspectGen,
															bool & hasDifferentBottom,
															string & specularDatabaseName,
															string & specularDatabaseModel,
															string & bottomDatabase,
															string & bottomModel,
															bool & bottomProspectGen) {
	string temp, modelKey;
	ostringstream oss;

	try {
		oss << modelIndex;
		temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
		DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentBottom);
		modelKey = hasDifferentBottom ? "UnderstoryMultiTopModel" : "UnderstoryMultiModel";
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "]." + modelKey + ".ModelName", modelName);
		DartSAParameters::get(temp + "]." + modelKey + ".databaseName", databaseName);
		DartSAParameters::get(temp + "]." + modelKey + ".useSpecular", isSpecular);
		DartSAParameters::get(temp + "]." + modelKey + ".ProspectExternalModule.useProspectExternalModule", useProspectGen);
		if (isSpecular) {
			DartSAParameters::get(temp + "]." + modelKey + ".SpecularData.specularModelName", specularDatabaseModel);
			DartSAParameters::get(temp + "]." + modelKey + ".SpecularData.specularDatabaseName", specularDatabaseName);
		}
		if (hasDifferentBottom) {
			string bottomModelKey;
			bottomModelKey = "UnderstoryMultiBottomModel";
			DartSAParameters::get(temp + "]." + bottomModelKey + ".ModelName", bottomModel);
			DartSAParameters::get(temp + "]." + bottomModelKey + ".databaseName", bottomDatabase);
			DartSAParameters::get(temp + "]." + bottomModelKey + ".ProspectExternalModule.useProspectExternalModule", bottomProspectGen);
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForFoliarDatabasesYaML(	const unsigned int & modelIndex,
																string & phaseName,
																string & modelName,
																string & databaseName,
																bool & isSpecular,
																string & specularDatabaseName,
																string & specularDatabaseModel) {
	try {
		shared_ptr<DartInputs::VegetationProperty> vegetationProperty =
				_dartInputsDAO.getDartOpticalProperties().getVegetationProperties()[modelIndex];
		phaseName = vegetationProperty->getOpticalPropertyName();
		modelName = vegetationProperty->getOpticalDatabase()->getModelName();
		databaseName = vegetationProperty->getOpticalDatabase()->getDartFilePath();
		isSpecular = vegetationProperty->getSpecularParameters() ? true : false;
		if (isSpecular) {
			specularDatabaseName = vegetationProperty->getSpecularParameters()->getSpecularDatabase()->getModelName();
			specularDatabaseModel = vegetationProperty->getSpecularParameters()->getSpecularDatabase()->getDartFilePath();
		}
	} catch (YAML::Exception &E) {
		cerr << E.msg << endl;
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase turbide dans coeff_diff (mode multispectral).
 *
 * @param int indice 				 : numero de la fonction de phase en cours de traitement.
 * @param int arivar_leaf 		 : flag permettant de savoir s'il faut ou non calculer cette fonction de phase.
 * @param string nom_leaf : nom de la fonction de phase pour les leafs.
 * @param int  parametersForFoliarModels.typeOfLeafAngleDistribution 		 : valeur du typeOfLeafAngleDistribution.
 * @param double &omegaMin 	 : parametre du clumping index
 * @param double &omegaMax 	 : parametre du clumping index
 * @param double &clumpingA 	 : parametre du clumping index
 * @param double &clumpingB 	 : parametre du clumping index
 * @param double averageLeafAngle           : Angle foliaire moyen pour la distribution angulaire moyenne ellipsoidale (�)
 * @param double ellipsePrincipalAngle        : angle de l'axe principal de l'ellipse (� typeOfLeafAngleDistribution elliptique)
 * @param double ellipseEccentricity  : excentricit� de la distribution angulaire foliaire elliptique
 *
 * @return 1 si okay.
 */
bool PhaseDAO::readCoeffDiffParametersForFoliar(const unsigned int & foliarModelindice,
												double &m,
												double &b0,
												double &b1,
												double &b2,
												string& nameDatabaseLAD,
												FoliarModel & foliarModel) {
	if (_useYaML) {
		return readCoeffDiffParametersForFoliarYaML(foliarModelindice, m, b0, b1, b2, nameDatabaseLAD, foliarModel);
	} else {
		return readCoeffDiffParametersForFoliarXML(foliarModelindice, m, b0, b1, b2, nameDatabaseLAD, foliarModel);
	}
}
bool PhaseDAO::readCoeffDiffParametersForFoliarXML(	const unsigned int & foliarModelindice,
													double &m,
													double &b0,
													double &b1,
													double &b2,
													string& nameDatabaseLAD,
													FoliarModel & foliarModel) {
	string tempString;
	double tempDouble;
	int tempInt;
	try {
		ostringstream oss;
		oss << foliarModelindice;
		string temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str() + "].";
		DartSAParameters::get(temp + "lad", tempInt);
		foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, tempInt);
		DartSAParameters::get(temp + "DirectionalClumpingIndexProperties.clumpinga", tempDouble);
		foliarModel.setFoliarClumpingA(foliarModelindice, tempDouble);
		DartSAParameters::get(temp + "DirectionalClumpingIndexProperties.clumpingb", tempDouble);
		foliarModel.setFoliarClumpingB(foliarModelindice, tempDouble);
		DartSAParameters::get(temp + "DirectionalClumpingIndexProperties.omegaMin", tempDouble);
		foliarModel.setFoliarOmegaMin(foliarModelindice, tempDouble);
		DartSAParameters::get(temp + "DirectionalClumpingIndexProperties.omegaMax", tempDouble);
		foliarModel.setFoliarOmegaMax(foliarModelindice, tempDouble);
		DartSAParameters::get(temp + "dimFoliar", tempDouble);
		foliarModel.setFoliarDimension(foliarModelindice, tempDouble);
		DartSAParameters::get(temp + "thermalHotSpotFactor", tempDouble);
		foliarModel.setHotSpotFactor(foliarModelindice, tempDouble);
		if (tempInt == globalReferences::ELLIPS) {
			DartSAParameters::get(temp + "Ellipsoidal.ala", tempDouble);
			foliarModel.setFoliarAverageLeafAngle(foliarModelindice, tempDouble);
		} else if (tempInt == globalReferences::ELLIPT) {
			DartSAParameters::get(temp + "Elliptical.thetam", tempDouble);
			foliarModel.setFoliarEllipsePrincipalAngle(foliarModelindice, tempDouble);
			DartSAParameters::get(temp + "Elliptical.epsilon", tempDouble);
			foliarModel.setFoliarEllipseEccentricity(foliarModelindice, tempDouble);
		} else if (tempInt == globalReferences::BOUNDEDUNI) {
			DartSAParameters::get(temp + "BoundedUniform.nbAla", tempInt);
			foliarModel.setNumberOfAla(foliarModelindice, tempInt);
//			cout << "read nbALA: " << tempInt << endl;
			DartSAParameters::get(temp + "BoundedUniform.nthAla", tempInt);
			foliarModel.setNthAla(foliarModelindice, tempInt);
		} else if (tempInt == globalReferences::USERDEFINED) {
			DartSAParameters::get(temp + "UserDefined.fileName", nameDatabaseLAD);
		} else if (tempInt == globalReferences::MANUAL) {
			DartSAParameters::get(temp + "Manual.LAD_m", m);
			DartSAParameters::get(temp + "Manual.LAD_b0", b0);
			DartSAParameters::get(temp + "Manual.LAD_b1", b1);
			DartSAParameters::get(temp + "Manual.LAD_b2", b2);
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readCoeffDiffParametersForFoliarYaML(const unsigned int & foliarModelindice,
													double &m,
													double &b0,
													double &b1,
													double &b2,
													string& nameDatabaseLAD,
													FoliarModel & foliarModel) {
	try {

		shared_ptr<DartInputs::VegetationProperty> vegetationProperty =
				_dartInputsDAO.getDartOpticalProperties().getVegetationProperties()[foliarModelindice];
		foliarModel.setFoliarDimension(foliarModelindice, vegetationProperty->getLeafDimension());
		foliarModel.setHotSpotFactor(foliarModelindice, vegetationProperty->getThermalHotspotFactor());

		if (DartInputs::BoundedUniform* boundedUnicorn =
				dynamic_cast<DartInputs::BoundedUniform*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::BOUNDEDUNI);
			foliarModel.setNumberOfAla(foliarModelindice, boundedUnicorn->getNumberOfALA());
			foliarModel.setNthAla(foliarModelindice, boundedUnicorn->getIndexOfALA());
		} else if (DartInputs::DeWitFunction* dewitFunc =
				dynamic_cast<DartInputs::DeWitFunction*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::MANUAL);
			m = dewitFunc->getM();
			b0 = dewitFunc->getTheta0();
			b1 = dewitFunc->getTheta1();
			b2 = dewitFunc->getTheta2();
		} else if (DartInputs::Ellipsoidal* ellips = dynamic_cast<DartInputs::Ellipsoidal*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::ELLIPS);
			foliarModel.setFoliarAverageLeafAngle(foliarModelindice, ellips->getALA());
		} else if (DartInputs::Elliptical* ellipt = dynamic_cast<DartInputs::Elliptical*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::ELLIPT);
			foliarModel.setFoliarEllipsePrincipalAngle(foliarModelindice, ellipt->getThetam());
			foliarModel.setFoliarEllipseEccentricity(foliarModelindice, ellipt->getEpsilon());
		} else if (dynamic_cast<DartInputs::Erectophile*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::ERECTO);
		} else if (dynamic_cast<DartInputs::Extremophile*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::EXTREMO);
		} else if (dynamic_cast<DartInputs::Horizontal*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::HORIZ);
		} else if (dynamic_cast<DartInputs::Plagiophile*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::PLAGIO);
		} else if (dynamic_cast<DartInputs::Planophile*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::PLANO);
		} else if (dynamic_cast<DartInputs::Spherical*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::SPHERIC);
		} else if (dynamic_cast<DartInputs::Uniform*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::UNIFORM);
		} else if (DartInputs::UserDefined* userDef = dynamic_cast<DartInputs::UserDefined*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::USERDEFINED);
			nameDatabaseLAD = userDef->getPathToLADDatabase();
		} else if (dynamic_cast<DartInputs::Vertical*>(vegetationProperty->getLeafAngleDistribution().get())) {
			foliarModel.setFoliarTypeOfLeafAngleDistribution(foliarModelindice, globalReferences::VERTI);
		}

		if (DartInputs::DirectionalWeightOfGFunction* dirFunc =
				dynamic_cast<DartInputs::DirectionalWeightOfGFunction*>(vegetationProperty->getFunctionGParameters().get())) {
			foliarModel.setFoliarClumpingA(foliarModelindice, dirFunc->getA());
			foliarModel.setFoliarClumpingB(foliarModelindice, dirFunc->getB());
			foliarModel.setFoliarOmegaMin(foliarModelindice, dirFunc->getOmegaMin());
			foliarModel.setFoliarOmegaMax(foliarModelindice, dirFunc->getOmegaMax());
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

/**
 * Lecture des parametres des fonctions de phase lambertiennes dans coeff_diff (mode multispectral)
 *
 * @param int indice 					: numero de la fonction de phase en cours de traitement.

 * @return 1 si okay
 */
bool PhaseDAO::readMultiplicativeFactorLambertien(const PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readMultiplicativeFactorLambertienYaML(genDTO);
	} else {
		return readMultiplicativeFactorLambertienXML();
	}
}
bool PhaseDAO::readMultiplicativeFactorLambertienXML() {
	string temp;
	int useMultiplicatorFactor;
	unsigned int sameFactorForAll = 0;
	double reflectanceFactor = 1;
	double directTransmittanceFactor = 1;
	double diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1;
	double diffuseTransmittanceAcceleration = 0.;

	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfLambertianModels(); modelIndex++) {
			// /!\ Initialisation à chaque boucles /!\ //
			reflectanceFactor = 1;
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			sameFactorForAll = 0;
			specularIntensityFactor = 1;
			useMultiplicatorFactor = 0;

			ostringstream oss;
			oss << modelIndex;
			temp = "Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str();

			DartSAParameters::get(temp + "].useMultiplicativeFactorForLUT", useMultiplicatorFactor);
			if (useMultiplicatorFactor == 1) {
				DartSAParameters::get(temp + "].lambertianNodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
			}
			if (sameFactorForAll == 1) {
				DartSAParameters::get(temp + "].lambertianNodeMultiplicativeFactorForLUT.reflectanceFactor", reflectanceFactor);
				DartSAParameters::get(temp + "].lambertianNodeMultiplicativeFactorForLUT.directTransmittanceFactor", directTransmittanceFactor);
				DartSAParameters::get(temp + "].lambertianNodeMultiplicativeFactorForLUT.diffuseTransmittanceFactor", diffuseTransmittanceFactor);
				DartSAParameters::get(temp + "].lambertianNodeMultiplicativeFactorForLUT.specularIntensityFactor", specularIntensityFactor);
			}
			diffuseTransmittanceAcceleration = 0.;
			if (DartSAParameters::exists(temp + "].lambertianNodeMultiplicativeFactorForLUT.diffuseTransmittanceAcceleration")) {
				diffuseTransmittanceAcceleration = DartSAParameters::get<double>(
					temp + "].lambertianNodeMultiplicativeFactorForLUT.diffuseTransmittanceAcceleration");
			}
			PhaseProperty::setDiffuseTransmittanceAccelerationForLambertian(modelIndex, diffuseTransmittanceAcceleration);

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
				if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
					ostringstream oss2;
					oss2 << spectralBandIndex;
					DartSAParameters::get(
						temp + "].lambertianNodeMultiplicativeFactorForLUT.lambertianMultiplicativeFactorForLUT[" + oss2.str()
								+ "].reflectanceFactor", reflectanceFactor);
					DartSAParameters::get(
						temp + "].lambertianNodeMultiplicativeFactorForLUT.lambertianMultiplicativeFactorForLUT[" + oss2.str()
								+ "].directTransmittanceFactor", directTransmittanceFactor);
					DartSAParameters::get(
						temp + "].lambertianNodeMultiplicativeFactorForLUT.lambertianMultiplicativeFactorForLUT[" + oss2.str()
								+ "].diffuseTransmittanceFactor", diffuseTransmittanceFactor);
					DartSAParameters::get(
						temp + "].lambertianNodeMultiplicativeFactorForLUT.lambertianMultiplicativeFactorForLUT[" + oss2.str()
								+ "].specularIntensityFactor", specularIntensityFactor);
				}
				PhaseProperty::setMultiplicativeFactorReflectanceForLambertian(modelIndex, spectralBandIndex, reflectanceFactor);
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactoForLambertian(modelIndex, spectralBandIndex, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactoForLambertian(modelIndex, spectralBandIndex,
					diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForLambertian(modelIndex, spectralBandIndex, specularIntensityFactor);

			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readMultiplicativeFactorLambertienYaML(const PhaseGeneralParametersDTO & genDTO) {
	double reflectanceFactor = 1;
	double directTransmittanceFactor = 1;
	double diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1;
	bool sameFactorForAll = false;
	bool useMultiplicatorFactor = false;

	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfLambertianModels(); ++modelIndex) {
			DartInputs::LambertianProperty* lambertianProperty = _dartInputsDAO.getDartOpticalProperties().getLambertianProperties()[modelIndex];

			// /!\ Initialisation à chaque boucles /!\ //
			reflectanceFactor = 1;
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;
			sameFactorForAll = false;
			useMultiplicatorFactor = lambertianProperty->getLambertianMultiplicativeFactor() ? true : false;
			if (useMultiplicatorFactor) {
				if (DartInputs::MultiplicativeFactorForAllBands* mfAllBands =
						dynamic_cast<DartInputs::MultiplicativeFactorForAllBands*>(lambertianProperty->getLambertianMultiplicativeFactor().get())) {
					sameFactorForAll = true;
					if (DartInputs::LambertianMultiplicativeFactor* lambertianMF =
							dynamic_cast<DartInputs::LambertianMultiplicativeFactor*>(mfAllBands->getMultiplicativeFactor()->getMultiplicativeFactor().get())) {
						reflectanceFactor = lambertianMF->getReflectanceFactor();
						directTransmittanceFactor = lambertianMF->getDirectTransmitance();
						diffuseTransmittanceFactor = lambertianMF->getDiffuseTransmitance();
						specularIntensityFactor = lambertianMF->getSpecularIntensity();
					}
				}
			}

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); ++spectralBandIndex) {
				if (useMultiplicatorFactor && !sameFactorForAll) {
					if (DartInputs::MultiplicativeFactorPerBand* mfPerBand =
							dynamic_cast<DartInputs::MultiplicativeFactorPerBand*>(lambertianProperty->getLambertianMultiplicativeFactor().get())) {
						for (unsigned int i = 0; i < mfPerBand->getMultiplicativeFactor().size(); ++i) {
							int idAssocBand = mfPerBand->getMultiplicativeFactor()[i]->getAssociatedSpectralBand();
							if (idAssocBand == genDTO.yamlSpectralBandIndex[spectralBandIndex]) {
								if (DartInputs::LambertianMultiplicativeFactor* lambertianMF =
										dynamic_cast<DartInputs::LambertianMultiplicativeFactor*>(mfPerBand->getMultiplicativeFactor()[i]->getMultiplicativeFactor().get())) {
									reflectanceFactor = lambertianMF->getReflectanceFactor();
									directTransmittanceFactor = lambertianMF->getDirectTransmitance();
									diffuseTransmittanceFactor = lambertianMF->getDiffuseTransmitance();
									specularIntensityFactor = lambertianMF->getSpecularIntensity();
									break;
								}
							}
						}
					}
				}
				PhaseProperty::setMultiplicativeFactorReflectanceForLambertian(modelIndex, spectralBandIndex, reflectanceFactor);
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactoForLambertian(modelIndex, spectralBandIndex, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactoForLambertian(modelIndex, spectralBandIndex,
					diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForLambertian(modelIndex, spectralBandIndex, specularIntensityFactor);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

// read Specular multiplicative factor

bool PhaseDAO::readMultiplicativeFactorSpecular(const PhaseGeneralParametersDTO & genDTO){
	if (_useYaML) {
		return readMultiplicativeFactorSpecularYaML(genDTO);
	} else {
		return readMultiplicativeFactorSpecularXML();
	}
}
bool PhaseDAO::readMultiplicativeFactorSpecularXML(){
	string temp;
	int useMultiplicatorFactor;
	unsigned int sameFactorForAll = 0;
	double fni = 1;
	double fki = 1;
	double fnt = 1;
	double fkt = 1;
	double fR = 1;
	double fT = 1;
	double fTau = 1;

	try {
		for (unsigned int indice = 0; indice < PhaseProperty::getNumberOfSpecularModels(); indice++) {
			// /!\ Initialisation à chaque boucles /!\ //
			useMultiplicatorFactor = 0;
			sameFactorForAll = 0;
			fni = 1;
			fnt = 1;
			fR = 1;
			fT = 1;
			fTau = 1;

			ostringstream oss;
			oss << indice;
			temp = "Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + oss.str();
			DartSAParameters::get(temp + "].useMultiplicativeFactorForLUT", useMultiplicatorFactor);
			if (useMultiplicatorFactor == 1) {
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
			}
			if (sameFactorForAll == 1) {
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.niFactor", fni);
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.ntFactor", fnt);
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.reflectionIntensityFactor", fR);
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.transmissionIntensityFactor", fT);
				DartSAParameters::get(temp + "].SpecularNodeMultiplicativeFactorForLUT.opticalDepthFactor", fTau);
			}
			for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
				if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
					ostringstream oss2;
					oss2 << i;
					DartSAParameters::get(
						temp + "].SpecularNodeMultiplicativeFactorForLUT.SpecularMultiplicativeFactorForLUT[" + oss2.str() + "].niFactor", fni);
					DartSAParameters::get(
						temp + "].SpecularNodeMultiplicativeFactorForLUT.SpecularMultiplicativeFactorForLUT[" + oss2.str() + "].ntFactor", fnt);
					DartSAParameters::get(
						temp + "].SpecularNodeMultiplicativeFactorForLUT.SpecularMultiplicativeFactorForLUT[" + oss2.str() + "].reflectionIntensityFactor", fR);
					DartSAParameters::get(
						temp + "].SpecularNodeMultiplicativeFactorForLUT.SpecularMultiplicativeFactorForLUT[" + oss2.str() + "].transmissionIntensityFactor", fT);
					DartSAParameters::get(
						temp + "].SpecularNodeMultiplicativeFactorForLUT.SpecularMultiplicativeFactorForLUT[" + oss2.str() + "].opticalDepthFactor", fTau);
				}

				PhaseProperty::setMultiplicativeFactorNiForSpecular(indice, i, fni);
				PhaseProperty::setMultiplicativeFactorNtForSpecular(indice, i, fnt);
				PhaseProperty::setMultiplicativeFactorTauForSpecular(indice, i, fTau);
				PhaseProperty::setMultiplicativeFactorReflectionForSpecular(indice, i, fR);
				PhaseProperty::setMultiplicativeFactorTransmissionForSpecular(indice, i, fT);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readMultiplicativeFactorSpecularYaML(const PhaseGeneralParametersDTO & genDTO){
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase "Lambertien + Speculaire" dans coeff_diff (mode multispectral).
 *
 * @param int indice 						: numero de la fonction de phase en cours de traitement.

 * @return 1 si okay.
 */
// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase de type "Hapke + Speculaire" dans coeff_diff (mode multispectral).
 *
 * @param int indice        			: numero de la fonction de phase en cours de traitement.

 * @return 1 si okay.
 */
bool PhaseDAO::readMultiplicativeFactorHapke(const PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readMultiplicativeFactorHapkeYaML(genDTO);
	} else {
		return readMultiplicativeFactorHapkeXML();
	}
}
bool PhaseDAO::readMultiplicativeFactorHapkeXML() {
	string temp;
	int useMultiplicatorFactor;
	unsigned int sameFactorForAll = 0;
	double directTransmittanceFactor = 1, diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1;
	double W = 1.0, C1 = 1.0, C2 = 1.0, C3 = 1.0, C4 = 1.0, H1 = 1.0, H2 = 1.0;
	try {
		for (unsigned int indice = 0; indice < PhaseProperty::getNumberOfHapkeModels(); indice++) {
			// /!\ Initialisation à chaque boucles /!\ //
			specularIntensityFactor = 1;
			useMultiplicatorFactor = 0;
			sameFactorForAll = 0;
			W = 1.0;
			C1 = 1.0;
			C2 = 1.0;
			C3 = 1.0;
			C4 = 1.0;
			H1 = 1.0;
			H2 = 1.0;
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;

			ostringstream oss;
			oss << indice;
			temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti[" + oss.str();

			DartSAParameters::get(temp + "].useMultiplicativeFactorForLUT", useMultiplicatorFactor);
			if (useMultiplicatorFactor == 1) {
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
			}
			if (sameFactorForAll == 1) {
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.directTransmittanceFactor", directTransmittanceFactor);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.diffuseTransmittanceFactor", diffuseTransmittanceFactor);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.specularIntensityFactor", specularIntensityFactor);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterW", W);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterC1", C1);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterC2", C2);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterC3", C3);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterC4", C4);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterH1", H1);
				DartSAParameters::get(temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeParameterH2", H2);
			}
			for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
				if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
					ostringstream oss2;
					oss2 << i;
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].directTransmittanceFactor",
						directTransmittanceFactor);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].diffuseTransmittanceFactor",
						diffuseTransmittanceFactor);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].specularIntensityFactor",
						specularIntensityFactor);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterW", W);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterC1", C1);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterC2", C2);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterC3", C3);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterC4", C4);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterH1", H1);
					DartSAParameters::get(
						temp + "].hapkeNodeMultiplicativeFactorForLUT.hapkeMultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterH2", H2);
				}
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactorForHapke(indice, i, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactorForHapke(indice, i, diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForHapke(indice, i, specularIntensityFactor);
				PhaseProperty::setMultiplicativeFactorWForHapke(indice, i, W);
				PhaseProperty::setMultiplicativeFactorC1ForHapke(indice, i, C1);
				PhaseProperty::setMultiplicativeFactorC2ForHapke(indice, i, C2);
				PhaseProperty::setMultiplicativeFactorC3ForHapke(indice, i, C3);
				PhaseProperty::setMultiplicativeFactorC4ForHapke(indice, i, C4);
				PhaseProperty::setMultiplicativeFactorH1ForHapke(indice, i, H1);
				PhaseProperty::setMultiplicativeFactorH2ForHapke(indice, i, H2);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readMultiplicativeFactorHapkeYaML(const PhaseGeneralParametersDTO & genDTO) {
	double directTransmittanceFactor = 1;
	double diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1, factorW = 1, factorC1 = 1, factorC2 = 1, factorC3 = 1, factorC4 = 1, factorH1 = 1, factorH2 = 1;
	bool sameFactorForAll = false;
	bool useMultiplicatorFactor = false;

	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfHapkeModels(); modelIndex++) {
			DartInputs::HapkeProperty* hapkeProperty = _dartInputsDAO.getDartOpticalProperties().getHapkeProperties()[modelIndex];

			// /!\ Initialisation à chaque boucles /!\ //
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;
			factorW = 1;
			factorC1 = 1;
			factorC2 = 1;
			factorC3 = 1;
			factorC4 = 1;
			factorH1 = 1;
			factorH2 = 1;
			sameFactorForAll = false;
			useMultiplicatorFactor = hapkeProperty->getHapkeMultiplicativeFactor() ? true : false;
			if (useMultiplicatorFactor) {
				if (DartInputs::MultiplicativeFactorForAllBands* mfAllBands =
						dynamic_cast<DartInputs::MultiplicativeFactorForAllBands*>(hapkeProperty->getHapkeMultiplicativeFactor().get())) {
					sameFactorForAll = true;
					if (DartInputs::HapkeMultiplicativeFactor* hapkeMF =
							dynamic_cast<DartInputs::HapkeMultiplicativeFactor*>(mfAllBands->getMultiplicativeFactor()->getMultiplicativeFactor().get())) {
						directTransmittanceFactor = hapkeMF->getDirectTransmittance();
						diffuseTransmittanceFactor = hapkeMF->getDiffuseTransmittance();
						specularIntensityFactor = hapkeMF->getSpecularIntensity();
						factorW = hapkeMF->getW();
						factorC1 = hapkeMF->getC1();
						factorC2 = hapkeMF->getC2();
						factorC3 = hapkeMF->getC3();
						factorC4 = hapkeMF->getC4();
						factorH1 = hapkeMF->getH1();
						factorH2 = hapkeMF->getH2();
					}
				}
			}

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); ++spectralBandIndex) {
				if (useMultiplicatorFactor && !sameFactorForAll) {
					if (DartInputs::MultiplicativeFactorPerBand* mfPerBand =
							dynamic_cast<DartInputs::MultiplicativeFactorPerBand*>(hapkeProperty->getHapkeMultiplicativeFactor().get())) {
						for (unsigned int i = 0; i < mfPerBand->getMultiplicativeFactor().size(); ++i) {
							int idAssocBand = mfPerBand->getMultiplicativeFactor()[i]->getAssociatedSpectralBand();
							if (idAssocBand == genDTO.yamlSpectralBandIndex[spectralBandIndex]) {
								if (DartInputs::HapkeMultiplicativeFactor* hapkeMF =
										dynamic_cast<DartInputs::HapkeMultiplicativeFactor*>(mfPerBand->getMultiplicativeFactor()[i]->getMultiplicativeFactor().get())) {
									directTransmittanceFactor = hapkeMF->getDirectTransmittance();
									diffuseTransmittanceFactor = hapkeMF->getDiffuseTransmittance();
									specularIntensityFactor = hapkeMF->getSpecularIntensity();
									factorW = hapkeMF->getW();
									factorC1 = hapkeMF->getC1();
									factorC2 = hapkeMF->getC2();
									factorC3 = hapkeMF->getC3();
									factorC4 = hapkeMF->getC4();
									factorH1 = hapkeMF->getH1();
									factorH2 = hapkeMF->getH2();
									break;
								}
							}
						}
					}
					PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactorForHapke(modelIndex, spectralBandIndex, directTransmittanceFactor);
					PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactorForHapke(modelIndex, spectralBandIndex,
						diffuseTransmittanceFactor);
					PhaseProperty::setMultiplicativeFactorSpecularIntensityForHapke(modelIndex, spectralBandIndex, specularIntensityFactor);
					PhaseProperty::setMultiplicativeFactorWForHapke(modelIndex, spectralBandIndex, factorW);
					PhaseProperty::setMultiplicativeFactorC1ForHapke(modelIndex, spectralBandIndex, factorC1);
					PhaseProperty::setMultiplicativeFactorC2ForHapke(modelIndex, spectralBandIndex, factorC2);
					PhaseProperty::setMultiplicativeFactorC3ForHapke(modelIndex, spectralBandIndex, factorC3);
					PhaseProperty::setMultiplicativeFactorC4ForHapke(modelIndex, spectralBandIndex, factorC4);
					PhaseProperty::setMultiplicativeFactorH1ForHapke(modelIndex, spectralBandIndex, factorH1);
					PhaseProperty::setMultiplicativeFactorH2ForHapke(modelIndex, spectralBandIndex, factorH2);

				}
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}


/*
 * 							FOR HAPKE 2
 */
bool PhaseDAO::readMultiplicativeFactorHapke2(const PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readMultiplicativeFactorHapke2YaML(genDTO);
	} else {
		return readMultiplicativeFactorHapke2XML();
	}
}
bool PhaseDAO::readMultiplicativeFactorHapke2XML() {
	string temp;
	int useMultiplicatorFactor;
	unsigned int sameFactorForAll = 0;
	double directTransmittanceFactor = 1, diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1;
	double W = 1.0, C = 1.0, B = 1.0, H = 1.0, B0 = 1.0, Th = 1.0;
	try {
		for (unsigned int indice = 0; indice < PhaseProperty::getNumberOfHapke2Models(); indice++) {
			// /!\ Initialisation à chaque boucles /!\ //
			specularIntensityFactor = 1;
			useMultiplicatorFactor = 0;
			sameFactorForAll = 0;
			W = 1.0;
			C = 1.0;
			B = 1.0;
			H = 1.0;
			B0 = 1.0;
			Th = 1.0;
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;

			ostringstream oss;
			oss << indice;
			temp = "Coeff_diff.Surfaces.HapkeSpecularMultiFunctions.HapkeSpecularMulti2[" + oss.str();

			DartSAParameters::get(temp + "].useMultiplicativeFactorForLUT", useMultiplicatorFactor);
			if (useMultiplicatorFactor == 1) {
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
			}
			if (sameFactorForAll == 1) {
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.directTransmittanceFactor", directTransmittanceFactor);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.diffuseTransmittanceFactor", diffuseTransmittanceFactor);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.specularIntensityFactor", specularIntensityFactor);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterW", W);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterC", C);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterB", B);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterH", H);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterB0", B0);
				DartSAParameters::get(temp + "].hapke2NodeMultiplicativeFactorForLUT.hapkeParameterTh", Th);
			}
			for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
				if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
					ostringstream oss2;
					oss2 << i;
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].directTransmittanceFactor",
						directTransmittanceFactor);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].diffuseTransmittanceFactor",
						diffuseTransmittanceFactor);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].specularIntensityFactor",
						specularIntensityFactor);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterW", W);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterC", C);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterB", B);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterH", H);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterB0", B0);
					DartSAParameters::get(
						temp + "].hapke2NodeMultiplicativeFactorForLUT.hapke2MultiplicativeFactorForLUT[" + oss2.str() + "].hapkeParameterTh", Th);
				}
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactorForHapke2(indice, i, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactorForHapke2(indice, i, diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForHapke2(indice, i, specularIntensityFactor);
				PhaseProperty::setMultiplicativeFactorWForHapke2(indice, i, W);
				PhaseProperty::setMultiplicativeFactorCForHapke2(indice, i, C);
				PhaseProperty::setMultiplicativeFactorBForHapke2(indice, i, B);
				PhaseProperty::setMultiplicativeFactorHForHapke2(indice, i, H);
				PhaseProperty::setMultiplicativeFactorB0ForHapke2(indice, i, B0);
				PhaseProperty::setMultiplicativeFactorThForHapke2(indice, i, Th);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readMultiplicativeFactorHapke2YaML(const PhaseGeneralParametersDTO & genDTO) {
	double directTransmittanceFactor = 1;
	double diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1, factorW = 1, factorC = 1, factorB = 1, factorH = 1, factorB0 = 1, factorTh = 1;
	bool sameFactorForAll = false;
	bool useMultiplicatorFactor = false;

	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfHapke2Models(); modelIndex++) {
			DartInputs::Hapke2Property* hapke2Property = _dartInputsDAO.getDartOpticalProperties().getHapke2Properties()[modelIndex];

			// /!\ Initialisation à chaque boucles /!\ //
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;
			factorW = 1;
			factorC = 1;
			factorB = 1;
			factorH = 1;
			factorB0 = 1;
			factorTh = 1;
			sameFactorForAll = false;
			useMultiplicatorFactor = hapke2Property->getHapke2MultiplicativeFactor() ? true : false;
			if (useMultiplicatorFactor) {
				if (DartInputs::MultiplicativeFactorForAllBands* mfAllBands =
						dynamic_cast<DartInputs::MultiplicativeFactorForAllBands*>(hapke2Property->getHapke2MultiplicativeFactor().get())) {
					sameFactorForAll = true;
					if (DartInputs::Hapke2MultiplicativeFactor* hapke2MF =
							dynamic_cast<DartInputs::Hapke2MultiplicativeFactor*>(mfAllBands->getMultiplicativeFactor()->getMultiplicativeFactor().get())) {
						directTransmittanceFactor = hapke2MF->getDirectTransmittance();
						diffuseTransmittanceFactor = hapke2MF->getDiffuseTransmittance();
						specularIntensityFactor = hapke2MF->getSpecularIntensity();
						factorW = hapke2MF->getW();
						factorC = hapke2MF->getC();
						factorB = hapke2MF->getB();
						factorH = hapke2MF->getH();
						factorB0 = hapke2MF->getB0();
						factorTh = hapke2MF->getTh();
					}
				}
			}

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); ++spectralBandIndex) {
				if (useMultiplicatorFactor && !sameFactorForAll) {
					if (DartInputs::MultiplicativeFactorPerBand* mfPerBand =
							dynamic_cast<DartInputs::MultiplicativeFactorPerBand*>(hapke2Property->getHapke2MultiplicativeFactor().get())) {
						for (unsigned int i = 0; i < mfPerBand->getMultiplicativeFactor().size(); ++i) {
							int idAssocBand = mfPerBand->getMultiplicativeFactor()[i]->getAssociatedSpectralBand();
							if (idAssocBand == genDTO.yamlSpectralBandIndex[spectralBandIndex]) {
								if (DartInputs::Hapke2MultiplicativeFactor* hapke2MF =
										dynamic_cast<DartInputs::Hapke2MultiplicativeFactor*>(mfPerBand->getMultiplicativeFactor()[i]->getMultiplicativeFactor().get())) {
									directTransmittanceFactor = hapke2MF->getDirectTransmittance();
									diffuseTransmittanceFactor = hapke2MF->getDiffuseTransmittance();
									specularIntensityFactor = hapke2MF->getSpecularIntensity();
									factorW = hapke2MF->getW();
									factorC = hapke2MF->getC();
									factorB = hapke2MF->getB();
									factorH = hapke2MF->getH();
									factorB0 = hapke2MF->getB0();
									factorTh = hapke2MF->getTh();
									break;
								}
							}
						}
					}
					PhaseProperty::setMultiplicativeFactorDirectTransmittanceFactorForHapke2(modelIndex, spectralBandIndex, directTransmittanceFactor);
					PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceFactorForHapke2(modelIndex, spectralBandIndex,
						diffuseTransmittanceFactor);
					PhaseProperty::setMultiplicativeFactorSpecularIntensityForHapke2(modelIndex, spectralBandIndex, specularIntensityFactor);
					PhaseProperty::setMultiplicativeFactorWForHapke2(modelIndex, spectralBandIndex, factorW);
					PhaseProperty::setMultiplicativeFactorCForHapke2(modelIndex, spectralBandIndex, factorC);
					PhaseProperty::setMultiplicativeFactorBForHapke2(modelIndex, spectralBandIndex, factorB);
					PhaseProperty::setMultiplicativeFactorHForHapke2(modelIndex, spectralBandIndex, factorH);
					PhaseProperty::setMultiplicativeFactorB0ForHapke2(modelIndex, spectralBandIndex, factorB0);
					PhaseProperty::setMultiplicativeFactorThForHapke2(modelIndex, spectralBandIndex, factorTh);
				}
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}


/* Water mode
 *
 */

bool PhaseDAO::readCoeffDiffWaterMode(	const unsigned int & modelIndex,
										unsigned int & waterMode,
										string & phaseName) {
	string temp = "";
	ostringstream oss;

	oss << modelIndex;
	temp = "Coeff_diff.WaterInterfaceMultiFunctions.WaterInterfaceFunction[" + oss.str();
	try {
		DartSAParameters::get(temp + "].ident", phaseName);
		DartSAParameters::get(temp + "].givenPhaseFunction", waterMode);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readCoeffDiffParametersWaterUserDefinedForBand(	const unsigned int & modelIndex,
																const unsigned int & numBand,
																double & upwardRefractionIndex,
																double & downwardRefractionIndex) {
	string temp = "";
	ostringstream oss, oss2;
	unsigned int sameFactorForAll = 0;

	oss << modelIndex;
	temp = "Coeff_diff.WaterInterfaceMultiFunctions.WaterInterfaceFunction[" + oss.str();
	try {
		// /!\ Initialisation à chaque boucles /!\ //
		sameFactorForAll = 0;

		DartSAParameters::get(temp + "].WaterInterfaceFunctionRefractionIndexNode.sameRefractionIndexForAllBands", sameFactorForAll);
		if (sameFactorForAll == 1) {
			DartSAParameters::get(
				temp + "].WaterInterfaceFunctionRefractionIndexNode.WaterInterfaceFunctionRefractionIndex[0].downwardRefractionIndex",
				downwardRefractionIndex);
			DartSAParameters::get(temp + "].WaterInterfaceFunctionRefractionIndexNode.WaterInterfaceFunctionRefractionIndex[0].upwardRefractionIndex",
				upwardRefractionIndex);
		} else {
			oss2 << numBand;
			if (sameFactorForAll == 0) {
				DartSAParameters::get(
					temp + "].WaterInterfaceFunctionRefractionIndexNode.WaterInterfaceFunctionRefractionIndex[" + oss2.str()
							+ "].downwardRefractionIndex", downwardRefractionIndex);
				DartSAParameters::get(
					temp + "].WaterInterfaceFunctionRefractionIndexNode.WaterInterfaceFunctionRefractionIndex[" + oss2.str()
							+ "].upwardRefractionIndex", upwardRefractionIndex);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readCoeffDiffParametersWaterFile(const unsigned int & modelIndex,
												string & waterFile) {
	string temp = "";
	ostringstream oss;

	oss << modelIndex;
	temp = "Coeff_diff.WaterInterfaceMultiFunctions.WaterInterfaceFunction[" + oss.str();
	try {
		DartSAParameters::get(temp + "].WaterLinkToPhaseFunction.waterGivenPhaseFunction", waterFile);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readCoeffDiffParametersWaterDatabases(	const unsigned int & modelIndex,
														string & databaseNameUp,
														string & databaseNameDown,
														string & databaseModelUp,
														string & databaseModelDown) {
	string temp = "";
	ostringstream oss;

	oss << modelIndex;
	temp = "Coeff_diff.WaterInterfaceMultiFunctions.WaterInterfaceFunction[" + oss.str();

	try {
		DartSAParameters::get(temp + "].WaterLinkToDbUp.databaseName", databaseNameUp);
		DartSAParameters::get(temp + "].WaterLinkToDbUp.ModelName", databaseModelUp);

		DartSAParameters::get(temp + "].WaterLinkToDbDown.databaseName", databaseNameDown);
		DartSAParameters::get(temp + "].WaterLinkToDbDown.ModelName", databaseModelDown);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions RPV dans coeff_diff (mode multispectral)
 *
 * @param int indice        			: numero de la fonction de phase en cours de traitement.

 * @return 1 si okay.
 */
bool PhaseDAO::readMultiplicativeFactorRPV(const PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readMultiplicativeFactorRPVYaML(genDTO);
	} else {
		return readMultiplicativeFactorRPVXML();
	}
}
bool PhaseDAO::readMultiplicativeFactorRPVXML() {
	string temp;
	int useMultiplicatorFactor;
	unsigned int sameFactorForAll = 0;
	double rhoZero = 1;
	double k = 1;
	double diffuseTransmittanceFactor = 1;
	double specularIntensityFactor = 1;
	double directTransmittanceFactor = 1;
	double h = 1;
	double g = 1;

	try {
		for (unsigned int indice = 0; indice < PhaseProperty::getNumberOfRPVModels(); indice++) {
			// /!\ Initialisation à chaque boucles /!\ //
			useMultiplicatorFactor = 0;
			sameFactorForAll = 0;
			rhoZero = 1;
			k = 1;
			diffuseTransmittanceFactor = 1;
			specularIntensityFactor = 1;
			directTransmittanceFactor = 1;
			h = 1;
			g = 1;

			ostringstream oss;
			oss << indice;
			temp = "Coeff_diff.Surfaces.RPVMultiFunctions.RPVMulti[" + oss.str();
			DartSAParameters::get(temp + "].useMultiplicativeFactorForLUT", useMultiplicatorFactor);
			if (useMultiplicatorFactor == 1) {
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
			}
			if (sameFactorForAll == 1) {
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.rhoZero_RPVMultiplicativeFactor", rhoZero);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.k_RPVMultiplicativeFactor", k);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.diffuseTransmittanceFactor", diffuseTransmittanceFactor);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.specularIntensityFactor", specularIntensityFactor);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.directTransmittanceFactor", directTransmittanceFactor);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.h_RPVMultiplicativeFactor", h);
				DartSAParameters::get(temp + "].RPVNodeMultiplicativeFactorForLUT.g_RPVMultiplicativeFactor", g);
			}
			for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
				if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
					ostringstream oss2;
					oss2 << i;
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].k_RPVMultiplicativeFactor", k);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str()
								+ "].rhoZero_RPVMultiplicativeFactor", rhoZero);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].g_RPVMultiplicativeFactor", g);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].h_RPVMultiplicativeFactor", h);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].directTransmittanceFactor",
						directTransmittanceFactor);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].diffuseTransmittanceFactor",
						diffuseTransmittanceFactor);
					DartSAParameters::get(
						temp + "].RPVNodeMultiplicativeFactorForLUT.RPVMultiplicativeFactorForLUT[" + oss2.str() + "].specularIntensityFactor",
						specularIntensityFactor);
				}

				PhaseProperty::setMultiplicativeFactorRho0ForRPV(indice, i, rhoZero);
				PhaseProperty::setMultiplicativeFactorKForRPV(indice, i, k);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceForRPV(indice, i, diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForRPV(indice, i, specularIntensityFactor);
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceForRPV(indice, i, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorHForRPV(indice, i, h);
				PhaseProperty::setMultiplicativeFactorGForRPV(indice, i, g);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}
bool PhaseDAO::readMultiplicativeFactorRPVYaML(const PhaseGeneralParametersDTO & genDTO) {
	double specularIntensityFactor = 1, directTransmittanceFactor = 1, diffuseTransmittanceFactor = 1, factorRhoZero = 1, factorK = 1, factorH = 1,
			factorG = 1;
	bool sameFactorForAll = false;
	bool useMultiplicatorFactor = false;

	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfRPVModels(); modelIndex++) {
			DartInputs::RPVProperty* rpvProperty = _dartInputsDAO.getDartOpticalProperties().getRPVProperties()[modelIndex];
			// /!\ Initialisation à chaque boucles /!\ //
			specularIntensityFactor = 1;
			directTransmittanceFactor = 1;
			diffuseTransmittanceFactor = 1;
			factorRhoZero = 1;
			factorK = 1;
			factorH = 1;
			factorG = 1;
			sameFactorForAll = false;
			useMultiplicatorFactor = rpvProperty->getRpvMultiplicativeFactor() ? true : false;
			if (useMultiplicatorFactor) {
				if (DartInputs::MultiplicativeFactorForAllBands* mfAllBands =
						dynamic_cast<DartInputs::MultiplicativeFactorForAllBands*>(rpvProperty->getRpvMultiplicativeFactor().get())) {
					sameFactorForAll = true;
					if (DartInputs::RpvMultiplicativeFactor* rpvMF =
							dynamic_cast<DartInputs::RpvMultiplicativeFactor*>(mfAllBands->getMultiplicativeFactor()->getMultiplicativeFactor().get())) {
						directTransmittanceFactor = rpvMF->getDirectTransmitance();
						diffuseTransmittanceFactor = rpvMF->getDiffuseTransmitance();
						specularIntensityFactor = rpvMF->getSpecularIntensity();
						factorRhoZero = rpvMF->getRho0();
						factorK = rpvMF->getK();
						factorH = rpvMF->getH();
						factorG = rpvMF->getG();
					}
				}
			}

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); ++spectralBandIndex) {
				if (useMultiplicatorFactor && !sameFactorForAll) {
					if (DartInputs::MultiplicativeFactorPerBand* mfPerBand =
							dynamic_cast<DartInputs::MultiplicativeFactorPerBand*>(rpvProperty->getRpvMultiplicativeFactor().get())) {
						for (unsigned int i = 0; i < mfPerBand->getMultiplicativeFactor().size(); ++i) {
							int idAssocBand = mfPerBand->getMultiplicativeFactor()[i]->getAssociatedSpectralBand();
							if (idAssocBand == genDTO.yamlSpectralBandIndex[spectralBandIndex]) {
								if (DartInputs::RpvMultiplicativeFactor* rpvMF =
										dynamic_cast<DartInputs::RpvMultiplicativeFactor*>(mfPerBand->getMultiplicativeFactor()[i]->getMultiplicativeFactor().get())) {
									directTransmittanceFactor = rpvMF->getDirectTransmitance();
									diffuseTransmittanceFactor = rpvMF->getDiffuseTransmitance();
									specularIntensityFactor = rpvMF->getSpecularIntensity();
									factorRhoZero = rpvMF->getRho0();
									factorK = rpvMF->getK();
									factorH = rpvMF->getH();
									factorG = rpvMF->getG();
									break;
								}
							}
						}
					}
				}
				PhaseProperty::setMultiplicativeFactorDirectTransmittanceForRPV(modelIndex, spectralBandIndex, directTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorDiffuseTransmittanceForRPV(modelIndex, spectralBandIndex, diffuseTransmittanceFactor);
				PhaseProperty::setMultiplicativeFactorSpecularIntensityForRPV(modelIndex, spectralBandIndex, specularIntensityFactor);
				PhaseProperty::setMultiplicativeFactorRho0ForRPV(modelIndex, spectralBandIndex, factorRhoZero);
				PhaseProperty::setMultiplicativeFactorKForRPV(modelIndex, spectralBandIndex, factorK);
				PhaseProperty::setMultiplicativeFactorHForRPV(modelIndex, spectralBandIndex, factorH);
				PhaseProperty::setMultiplicativeFactorGForRPV(modelIndex, spectralBandIndex, factorG);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des parametres des fonctions de phase turbide dans phase (mode multispectral).
 *
 * @param int indice 				 : numero de la fonction de phase en cours de traitement.
 * @return 1 si okay.
 */
bool PhaseDAO::readMultiplicativeFactorFoliar(	int & iterationStepPhi,
												int & iterationStepTheta,
												FoliarModel & foliarModel,
												const PhaseGeneralParametersDTO & genDTO) {
	if (_useYaML) {
		return readMultiplicativeFactorFoliarYaML(iterationStepPhi, iterationStepTheta, foliarModel, genDTO);
	} else {
		return readMultiplicativeFactorFoliarXML(iterationStepPhi, iterationStepTheta, foliarModel);
	}
}
bool PhaseDAO::readMultiplicativeFactorFoliarXML(	int & iterationStepPhi,
													int & iterationStepTheta,
													FoliarModel & foliarModel) {
	int tableDividerOfThetaMax[12] = { 1, 2, 3, 5, 6, 9, 10, 15, 18, 30, 45, 90 };
	int tableDividerOfPhiMax[24] = { 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 18, 20, 24, 30, 36, 40, 45, 60, 72, 90, 120, 180, 360 };
	string temp;
	if (DartSAParameters::exists("Coeff_diff.Volumes.UnderstoryMultiFunctions.integrationStepOnTheta")) {
		DartSAParameters::get("Coeff_diff.Volumes.UnderstoryMultiFunctions.integrationStepOnTheta", iterationStepTheta);
	} else {
		iterationStepTheta = 1;
	}
	if (DartSAParameters::exists("Coeff_diff.Volumes.UnderstoryMultiFunctions.integrationStepOnPhi")) {
		DartSAParameters::get("Coeff_diff.Volumes.UnderstoryMultiFunctions.integrationStepOnPhi", iterationStepPhi);
	} else {
		iterationStepPhi = 10;
	}
	for (int i = 0; i < 11; i++) {
		if (iterationStepTheta == tableDividerOfThetaMax[i]) {
			break;
		} else if (iterationStepTheta > tableDividerOfThetaMax[i] && iterationStepTheta <= tableDividerOfThetaMax[i + 1]) {
			if (iterationStepTheta - tableDividerOfThetaMax[i] > tableDividerOfThetaMax[i + 1] - iterationStepTheta) {
				iterationStepTheta = tableDividerOfThetaMax[i + 1];
			} else {
				iterationStepTheta = tableDividerOfThetaMax[i];
			}
			break;
		}
	}
	for (int i = 0; i < 23; i++) {
		if (iterationStepPhi == tableDividerOfPhiMax[i]) {
			break;
		} else if (iterationStepPhi > tableDividerOfPhiMax[i] && iterationStepPhi <= tableDividerOfPhiMax[i + 1]) {
			if (iterationStepPhi - tableDividerOfPhiMax[i] > tableDividerOfPhiMax[i + 1] - iterationStepPhi) {
				iterationStepPhi = tableDividerOfPhiMax[i + 1];
			} else {
				iterationStepPhi = tableDividerOfPhiMax[i];
			}
			break;
		}
	}

	bool hasDifferentBottom = false;
	try {
		for (unsigned int indice = 0; indice < PhaseProperty::getNumberOfFoliarModels(); indice++) {
			ostringstream oss;
			oss << indice;
			temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
			DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentBottom);
			if (hasDifferentBottom) {
				readFoliarMultiplicativeFactorForModelXML(temp, "UnderstoryMultiTopModel", false, indice, foliarModel);
				readFoliarMultiplicativeFactorForModelXML(temp, "UnderstoryMultiBottomModel", true, indice, foliarModel);
			} else {
				// Faire un truc plus optimisé
				readFoliarMultiplicativeFactorForModelXML(temp, "UnderstoryMultiModel", false, indice, foliarModel);
				readFoliarMultiplicativeFactorForModelXML(temp, "UnderstoryMultiModel", true, indice, foliarModel);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readFoliarMultiplicativeFactorForModelXML(	const string & temp,
															const string & modelKey,
															const bool & isBottomFace,
															const unsigned int & modelIndex,
															FoliarModel & foliarModel) {
	try {
		double LeafTransmittanceFactor = 1;
		double reflectanceFactor = 1;
		int useMultiplicatorFactor = 0;
		unsigned int sameFactorForAll = 0;
		bool hasDifferentBottom = false;

		DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentBottom);
		string modelKey = hasDifferentBottom ? "UnderstoryMultiTopModel" : "UnderstoryMultiModel";
		DartSAParameters::get(temp + "]." + modelKey + ".useMultiplicativeFactorForLUT", useMultiplicatorFactor);
		if (useMultiplicatorFactor == 1) {
			DartSAParameters::get(temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.useSameFactorForAllBands", sameFactorForAll);
		}

		double diffuseTransmittanceAcceleration = 0.;
		if (DartSAParameters::exists(temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.diffuseTransmittanceAcceleration")) {
			diffuseTransmittanceAcceleration = DartSAParameters::get<double>(
				temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.diffuseTransmittanceAcceleration");
		}
		if (isBottomFace) {
			foliarModel.setAbaxialDiffuseTransmittanceAccelerationForFoliar(modelIndex, diffuseTransmittanceAcceleration);
		} else {
			foliarModel.setAdaxialDiffuseTransmittanceAccelerationForFoliar(modelIndex, diffuseTransmittanceAcceleration);
		}

		if (sameFactorForAll == 1) {
			DartSAParameters::get(temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.reflectanceFactor", reflectanceFactor);
			DartSAParameters::get(temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.LeafTransmittanceFactor",
				LeafTransmittanceFactor);
		}
		for (unsigned int i = 0; i < PhaseProperty::getNumberOfSpectralBands(); i++) {
			ostringstream oss2;
			oss2 << i;
			if (useMultiplicatorFactor == 1 && sameFactorForAll == 0) {
				DartSAParameters::get(
					temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.understoryMultiplicativeFactorForLUT[" + oss2.str()
							+ "].reflectanceFactor", reflectanceFactor);
				DartSAParameters::get(
					temp + "]." + modelKey + ".understoryNodeMultiplicativeFactorForLUT.understoryMultiplicativeFactorForLUT[" + oss2.str()
							+ "].LeafTransmittanceFactor", LeafTransmittanceFactor);
			}
			if (isBottomFace) {
				foliarModel.setAbaxialReflectanceForFoliar(modelIndex, i, reflectanceFactor);
			} else {
				foliarModel.setAdaxialReflectanceForFoliar(modelIndex, i, reflectanceFactor);
			}
			foliarModel.setLeafTransmittanceForFoliar(modelIndex, i, LeafTransmittanceFactor, isBottomFace);
		}

	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readMultiplicativeFactorFoliarYaML(	int & iterationStepPhi,
													int & iterationStepTheta,
													FoliarModel & foliarModel,
													const PhaseGeneralParametersDTO & genDTO) {
	int tableDividerOfThetaMax[12] = { 1, 2, 3, 5, 6, 9, 10, 15, 18, 30, 45, 90 };
	int tableDividerOfPhiMax[24] = { 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 18, 20, 24, 30, 36, 40, 45, 60, 72, 90, 120, 180, 360 };
	string temp;
	int useMultiplicatorFactor;
	if (_dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()) {
		iterationStepTheta = _dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getIntegrationStepOnTetha();
		iterationStepPhi = _dartInputsDAO.getDartOpticalProperties().getOpticalGeneralProperties()->getIntegrationStepOnPhi();
	} else {
		iterationStepTheta = 1;
		iterationStepPhi = 10;
	}

	for (int i = 0; i < 11; i++) {
		if (iterationStepTheta == tableDividerOfThetaMax[i]) {
			break;
		} else if (iterationStepTheta > tableDividerOfThetaMax[i] && iterationStepTheta <= tableDividerOfThetaMax[i + 1]) {
			if (iterationStepTheta - tableDividerOfThetaMax[i] > tableDividerOfThetaMax[i + 1] - iterationStepTheta) {
				iterationStepTheta = tableDividerOfThetaMax[i + 1];
			} else {
				iterationStepTheta = tableDividerOfThetaMax[i];
			}
			break;
		}
	}

	for (int i = 0; i < 23; i++) {
		if (iterationStepPhi == tableDividerOfPhiMax[i]) {
			break;
		} else if (iterationStepPhi > tableDividerOfPhiMax[i] && iterationStepPhi <= tableDividerOfPhiMax[i + 1]) {
			if (iterationStepPhi - tableDividerOfPhiMax[i] > tableDividerOfPhiMax[i + 1] - iterationStepPhi) {
				iterationStepPhi = tableDividerOfPhiMax[i + 1];
			} else {
				iterationStepPhi = tableDividerOfPhiMax[i];
			}
			break;
		}
	}

	double leafTransmittanceFactor = 1;
	double adaxialReflectanceFactor = 1;
	double abaxialReflectanceFactor = 1;
	unsigned int sameFactorForAll = 0;
	try {
		for (unsigned int modelIndex = 0; modelIndex < PhaseProperty::getNumberOfFoliarModels(); modelIndex++) {
			shared_ptr<DartInputs::VegetationProperty> vegetationProperty =
					_dartInputsDAO.getDartOpticalProperties().getVegetationProperties()[modelIndex];

			// /!\ Initialisation à chaque boucles /!\ //
			leafTransmittanceFactor = 1;
			adaxialReflectanceFactor = 1;
			abaxialReflectanceFactor = 1;
			sameFactorForAll = false;
			useMultiplicatorFactor = vegetationProperty->getVegetationMultiplicativeFactor() ? true : false;
			if (useMultiplicatorFactor) {
				if (DartInputs::MultiplicativeFactorForAllBands* mfAllBands =
						dynamic_cast<DartInputs::MultiplicativeFactorForAllBands*>(vegetationProperty->getVegetationMultiplicativeFactor().get())) {
					sameFactorForAll = true;
					if (DartInputs::VegetationMultiplicativeFactor* vegetationMF =
							dynamic_cast<DartInputs::VegetationMultiplicativeFactor*>(mfAllBands->getMultiplicativeFactor()->getMultiplicativeFactor().get())) {
						adaxialReflectanceFactor = vegetationMF->getAdaxialReflectance();
						abaxialReflectanceFactor = vegetationMF->getAbaxialReflectance();
						leafTransmittanceFactor = vegetationMF->getLeafTransmitance();
					}
				}
			}

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); ++spectralBandIndex) {
				if (useMultiplicatorFactor && !sameFactorForAll) {
					if (DartInputs::MultiplicativeFactorPerBand* mfPerBand =
							dynamic_cast<DartInputs::MultiplicativeFactorPerBand*>(vegetationProperty->getVegetationMultiplicativeFactor().get())) {
						for (unsigned int i = 0; i < mfPerBand->getMultiplicativeFactor().size(); ++i) {
							int idAssocBand = mfPerBand->getMultiplicativeFactor()[i]->getAssociatedSpectralBand();
							if (idAssocBand == genDTO.yamlSpectralBandIndex[spectralBandIndex]) {
								if (DartInputs::VegetationMultiplicativeFactor* vegetationMF =
										dynamic_cast<DartInputs::VegetationMultiplicativeFactor*>(mfPerBand->getMultiplicativeFactor()[i]->getMultiplicativeFactor().get())) {
									adaxialReflectanceFactor = vegetationMF->getAdaxialReflectance();
									abaxialReflectanceFactor = vegetationMF->getAbaxialReflectance();
									leafTransmittanceFactor = vegetationMF->getLeafTransmitance();
									break;
								}
							}
						}
					}
				}
				foliarModel.setAdaxialReflectanceForFoliar(modelIndex, spectralBandIndex, adaxialReflectanceFactor);
				foliarModel.setAbaxialReflectanceForFoliar(modelIndex, spectralBandIndex, abaxialReflectanceFactor);
				foliarModel.setLeafTransmittanceForFoliar(modelIndex, spectralBandIndex, leafTransmittanceFactor, false);
				foliarModel.setLeafTransmittanceForFoliar(modelIndex, spectralBandIndex, leafTransmittanceFactor, true);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}

	return true;
}

bool PhaseDAO::readDirectionalDirectTransmittanceForLambertian(unsigned int modelIndex,
																bool & useDirectionalDirectTransmittance,
																std::string & filePath,
																float & angularStep) {
	return readDirectionalDirectTransmittance("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + std::to_string(modelIndex) + "]", useDirectionalDirectTransmittance, filePath, angularStep);
}
bool PhaseDAO::readDirectionalDirectTransmittanceForSpecular(unsigned int modelIndex,
																bool & useDirectionalDirectTransmittance,
																std::string & filePath,
																float & angularStep) {
	return readDirectionalDirectTransmittance("Coeff_diff.Surfaces.SpecularMultiFunctions.SpecularMulti[" + std::to_string(modelIndex) + "]", useDirectionalDirectTransmittance, filePath, angularStep);
}
bool PhaseDAO::readDirectionalDirectTransmittanceForHapke(unsigned int modelIndex,
															bool & useDirectionalDirectTransmittance,
															std::string & filePath,
															float & angularStep) {
	return readDirectionalDirectTransmittance("Coeff_diff.Surfaces.SpecularMultiFunctions.HapkeSpecularMulti[" + std::to_string(modelIndex) + "]", useDirectionalDirectTransmittance, filePath, angularStep);
}
bool PhaseDAO::readDirectionalDirectTransmittanceForHapke2(unsigned int modelIndex,
															bool & useDirectionalDirectTransmittance,
															std::string & filePath,
															float & angularStep) {
	return readDirectionalDirectTransmittance("Coeff_diff.Surfaces.SpecularMultiFunctions.HapkeSpecularMulti2[" + std::to_string(modelIndex) + "]", useDirectionalDirectTransmittance, filePath, angularStep);
}
bool PhaseDAO::readDirectionalDirectTransmittanceForRPV(unsigned int modelIndex,
														bool & useDirectionalDirectTransmittance,
														std::string & filePath,
														float & angularStep) {
	return readDirectionalDirectTransmittance("Coeff_diff.Surfaces.SpecularMultiFunctions.RPVMulti[" + std::to_string(modelIndex) + "]", useDirectionalDirectTransmittance, filePath, angularStep);
}

bool PhaseDAO::readDirectionalDirectTransmittance(const std::string & nodeXMLPath,
													bool & useDirectionalDirectTransmittance,
													std::string & filePath,
													float & angularStep) {
	try {
		if (DartSAParameters::exists(nodeXMLPath + ".useDirectionalDirectTransmittance")) {
			DartSAParameters::get(nodeXMLPath + ".useDirectionalDirectTransmittance", useDirectionalDirectTransmittance);

			if (useDirectionalDirectTransmittance) {
				DartSAParameters::get(nodeXMLPath + ".DirectionalDirectTransmittance.ddtFile", filePath);
				DartSAParameters::get(nodeXMLPath + ".DirectionalDirectTransmittance.angularStep", angularStep);
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
/**
 * Lecture des temperatures dans phase
 *
 * @param int avimode 		 			 : mode MONOSPECTRAL ou MULTISPECTRAL.
 * @param double TminSoil  			 : temperature minimal du sol.
 * @param double TmaxSoil  			 : temperature maximal du sol.
 * @param double TminWater 			 : temperature minimal de l'eau.
 * @param double TmaxWater 			 : temperature maximal de l'eau.
 * @param double TminRoad  			 : temperature minimal de la route.
 * @param double TmaxRoad  			 : temperature maximal de la route.
 * @param double TminPlot_opaque  	 : temperature minimal des plots opaques.
 * @param double TmaxPlot_opaque  	 : temperature maximal des plots opaques.
 * @param double TminRoof 			 : temperature minimal pour les toits.
 * @param double TmaxRoof 			 : temperature maximal pour les toits.
 * @param double TminWall 			 : temperature minimal pour les murs.
 * @param double TmaxWall 			 : temperature maximal pour les murs.
 * @param double TminTrunk 			 : temperature minimal pour les troncs.
 * @param double TmaxTrunk 			 : temperature maxnimal pour les troncs.
 * @param double TminMaket 			 : temperature minimal de la maquette.
 * @param vector<double> tMinLeaf : temperature minimal pour les leafs.
 * @param vector<double> tMaxLeaf : temperature minimal pour les leafs.
 * @param int avinb_leaf 				 : nombre de fonctions de phase turbide
 *
 * @return 1 si okay.
 */

bool PhaseDAO::readParametersForTemperature(vector<int> &arviIdTemperature,
											const int & avinb_leaf) {
	try {
		string tempMode;
		tempMode = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[";
		for (int i = 0; i < avinb_leaf; ++i) {
			int idTemperature = 0;
			ostringstream oss;
			oss << i;
			string temp = tempMode + oss.str();
			DartSAParameters::get(temp + "].idTemperature", idTemperature);
			/**
			 * Vincent Sac : correction du remplissage des valeurs du vecteur =>
			 * on ne peut pas faire un push_back car la taille du vecteur est deja initialisee,
			 * il faut utiliser la syntaxe normale pour les tableaux
			 */
			arviIdTemperature[i] = idTemperature;
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

void PhaseDAO::readTOABOAFromFile(	const string& preGeneratedBoaFile,
									unsigned int& numberOfSpectralBands) {
	string inputName = DartEnv::getFilePathForRead(INPUT_DIR, preGeneratedBoaFile);
	ifstream fluxEntree;
	fluxEntree.open(inputName.c_str(), ios::in);
	double value;
	double verticalIrradiance = -1. / cos(PhaseProperty::getSunZenithAngleInRadians());
	double sum;
	echappeLectureCommentaireFichier(fluxEntree, TWO_CHAR, '*');
	for (unsigned int q = 0; q < numberOfSpectralBands; q++) {
		fluxEntree >> value;
		value *= verticalIrradiance;
		PhaseProperty::setMeanIrradianceTopOfAtmosphere(q, value);
	}
	echappeLectureCommentaireFichier(fluxEntree, TWO_CHAR, '*');
	for (unsigned int q = 0; q < numberOfSpectralBands; q++) {
		fluxEntree >> value;
		value *= verticalIrradiance;
		PhaseProperty::setMeanIrradianceBottomOfAtmosphere(q, value);
	}
	echappeLectureCommentaireFichier(fluxEntree, TWO_CHAR, '*');
	for (unsigned int q = 0; q < numberOfSpectralBands; q++) {
		fluxEntree >> value;
		sum = PhaseProperty::getMeanIrradianceBottomOfAtmosphere(q) + value;
		PhaseProperty::setMeanIrradianceBottomOfAtmosphere(q, sum);
	}
	fluxEntree.close();
}

// -----------------------------------------------------------------------------
bool PhaseDAO::readIrradianceParameters(PhaseGeneralParametersDTO & genDTO,
										vector<double> & irradianceVector,
										vector<double> & skylVector) {
	if (_useYaML) {
		return readIrradianceParametersYaML(genDTO, irradianceVector, skylVector);
	} else {
		return readIrradianceParametersXML(genDTO, irradianceVector, skylVector);
	}
}
bool PhaseDAO::readIrradianceParametersXML(	PhaseGeneralParametersDTO & genDTO,
											vector<double> & irradianceVector,
											vector<double> & skylVector) {
	/*
	 * Old 0:[TOA] Irradiance spectra;1:[TOA] Irradiance per band;2:[BOA] Irradiance spectra;3:[BOA] Irradiance per band;4:[BOA] Anisotropic radiance
	 * 	irradianceMode  0:Irradiance spectra;1:Irradiance per band;2:Anisotropic radiance
	 TOAtoBOA        0:Empty atmosphere;1:Analytic model;2:Radiative transfer simulation

	 */
	int computationMethodeOfToaToBoa = 0;
	string databaseName;
	string databaseSolarTable;
	string databaseSolarColumn;
	string preGeneratedBoaFile;
	unsigned int weightingOfAtmosphereParameters;
	unsigned int weightingOfReflectanceParameters;

// Override band irradiance (if irradianceMode == userDefined && user check override band irradiance)
	vector<vector<double> > overrideBandRadianceFileContent = vector<vector<double> >();
	vector<vector<double> > overrideBandRadianceResult = vector<vector<double> >();

	try {
		int LidarMode;
		int irradianceFromDatabase = -1;
		DartSAParameters::get("Phase.calculatorMethod", LidarMode);
		DartSAParameters::get("Phase.AtmosphereRadiativeTransfer.TOAtoBOA", computationMethodeOfToaToBoa);
		if (LidarMode != 2 || PhaseProperty::getIfSimuSolarNoise()) { //si on n'est pas en lidar'
			if (PhaseProperty::getIfSimuSolarNoise()) {
				DartSAParameters::get("Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceMode",
					irradianceFromDatabase);
			} else {
				DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceMode", irradianceFromDatabase);
			}
			if (irradianceFromDatabase == 3) {
				// Mode 3 works like mode 1, except with where it gets its radiation data from (from a file instead of from the xml)
				irradianceFromDatabase = 1;
			}
			PhaseProperty::setIrradianceFromDatabaseComputationMethod(irradianceFromDatabase);
			if (irradianceFromDatabase == 0) {
				if (PhaseProperty::getIfSimuSolarNoise()) {

					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.databaseName",
						databaseName);
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.irradianceTable",
						databaseSolarTable);
					databaseSolarTable = databaseSolarTable + "_oIRR";
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.irradianceColumn",
						databaseSolarColumn);
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.weightAtmosphereParameters",
						weightingOfAtmosphereParameters);
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceDatabaseNode.weightReflectanceParameters",
						weightingOfReflectanceParameters);

				} else {
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.databaseName", databaseName);
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.irradianceTable",
						databaseSolarTable);
					databaseSolarTable = databaseSolarTable + "_oIRR";
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.irradianceColumn",
						databaseSolarColumn);

					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.weightAtmosphereParameters",
						weightingOfAtmosphereParameters);
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.irradianceDatabaseNode.weightReflectanceParameters",
						weightingOfReflectanceParameters);
				}
				if (weightingOfAtmosphereParameters == 0) {
					PhaseProperty::setWeightingOfAtmosphereParameters(false);
				} else {
					PhaseProperty::setWeightingOfAtmosphereParameters(true);
				}

				if (weightingOfReflectanceParameters == 0) {
					PhaseProperty::setWeightingOfReflectanceParameters(false);
				} else {
					PhaseProperty::setWeightingOfReflectanceParameters(true);
				}

			} else if (irradianceFromDatabase == 1) {
				unsigned int irraDefinition = 0;
				if (PhaseProperty::getIfSimuSolarNoise()) {
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.SpectralIrradiance.CommonParameters.irraDef",
						irraDefinition);
				} else {
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.SpectralIrradiance.CommonParameters.irraDef",
						irraDefinition);
				}

				if (isOverrideBandIrradiance()) {
					string overrideFileParameter = "";
					string overrideFilePathKey = "Phase.DartInputParameters.nodeIlluminationMode.BandIrradianceFileNode.bandIrradianceFile";
					//	Récupération du fichier override & du paramètre check si on override l'irradiance
					if (DartSAParameters::exists(overrideFilePathKey)) {
						DartSAParameters::get(overrideFilePathKey, overrideFileParameter);
					}
					readOverrideBandRadianceFile(overrideFileParameter, overrideBandRadianceFileContent);
					overrideBandRadianceResult = vector<vector<double> >(overrideBandRadianceFileContent.size() + 2, vector<double>(3));
				}

				for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {

					double irradianceValue = 0.0;
					if (!isOverrideBandIrradiance()) {
						ostringstream oss;
						oss << i;
						string temp;
						if (PhaseProperty::getIfSimuSolarNoise()) {
							temp =
									"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.SpectralIrradiance.SpectralIrradianceValue["
											+ oss.str();
						} else {
							temp = "Phase.DartInputParameters.nodeIlluminationMode.SpectralIrradiance.SpectralIrradianceValue[" + oss.str();
						}

						DartSAParameters::get(temp + "].irradiance", irradianceValue);
					} else {
						Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(i), PhaseProperty::getUpperBandWavelength(i),
							overrideBandRadianceFileContent.size(), 2, overrideBandRadianceFileContent, overrideBandRadianceResult);
						irradianceValue = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][1];
					}
					if (irraDefinition == 1) {
						irradianceValue = irradianceValue / fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
					}
					irradianceVector.push_back(irradianceValue);
				}
			} else if (irradianceFromDatabase == 2) {
				unsigned int numberOfSpectralBands = DartSAParameters::getNbOf(
					"Phase.DartInputParameters.SpectralIntervals.SpectralIntervalsProperties");

				if (PhaseProperty::getIfSimuSolarNoise()) {
					DartSAParameters::get(
						"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.illumination.illuminationFileName",
						preGeneratedBoaFile);
				} else {
					DartSAParameters::get("Phase.DartInputParameters.nodeIlluminationMode.illumination.illuminationFileName", preGeneratedBoaFile);
				}
				readTOABOAFromFile(preGeneratedBoaFile, numberOfSpectralBands);

				//TODO 2:Anisotropic radiance
			} else {
				cout << "Unknown type of irradiance : " << irradianceFromDatabase << endl;
			}
			if (computationMethodeOfToaToBoa != 2 && irradianceFromDatabase != 2) {
				for (unsigned int i = 0; i < genDTO.numberOfSpectralBands; i++) {
					double skylValue = 0.0;
					if (!isOverrideBandIrradiance()) {
						ostringstream oss;
						oss << i;
						string temp;
						if (PhaseProperty::getIfSimuSolarNoise()) {
							temp =
									"Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.SpectralIrradiance.SpectralIrradianceValue["
											+ oss.str();
						} else {
							temp = "Phase.DartInputParameters.nodeIlluminationMode.SpectralIrradiance.SpectralIrradianceValue[" + oss.str();
						}
						DartSAParameters::get(temp + "].Skyl", skylValue);
					} else {
						Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(i), PhaseProperty::getUpperBandWavelength(i),
							overrideBandRadianceFileContent.size(), 3, overrideBandRadianceFileContent, overrideBandRadianceResult);
						skylValue = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][2];
					}
					skylVector.push_back(skylValue);
				}
			}
		}
		PhaseProperty::setSolarConstantDatabaseParameters(databaseSolarTable, databaseSolarColumn, databaseName, computationMethodeOfToaToBoa);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readIrradianceParametersYaML(PhaseGeneralParametersDTO & genDTO,
											vector<double> & irradianceVector,
											vector<double> & skylVector) {
	/*
	 * Old 0:[TOA] Irradiance spectra;1:[TOA] Irradiance per band;2:[BOA] Irradiance spectra;3:[BOA] Irradiance per band;4:[BOA] Anisotropic radiance
	 * 	irradianceMode  0:Irradiance spectra;1:Irradiance per band;2:Anisotropic radiance
	 TOAtoBOA        0:Empty atmosphere;1:Analytic model;2:Radiative transfer simulation

	 */
	string databaseName;
	string databaseSolarTable;
	string databaseSolarColumn;

	// Override band irradiance (if irradianceMode == userDefined && user check override band irradiance)
	vector<vector<double> > overrideBandRadianceFileContent = vector<vector<double> >();
	vector<vector<double> > overrideBandRadianceResult = vector<vector<double> >();

	try {

		shared_ptr<DartInputs::IAtmosphericIlumination> solarIllumination;
		if (_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod()) {
			if (DartInputs::FluxTracking* ft =
					dynamic_cast<DartInputs::FluxTracking*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				solarIllumination = ft->getSolarIllumination();
			} else if (DartInputs::MonteCarlo* mc =
					dynamic_cast<DartInputs::MonteCarlo*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				solarIllumination = mc->getSolarIllumination();
			} else if (DartInputs::Lidar* lidar =
					dynamic_cast<DartInputs::Lidar*>(_dartInputsDAO.getDartRadiation().getRadiativeTransferMethod().get())) {
				if (lidar->getSolarNoise()) {
					solarIllumination = lidar->getSolarNoise()->getSolarNoiseParameters()->getSolarIllumination();
				}
			}
		}

		int computationMethodTOAtoBOA = 0;
		if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::NO_ATMOSPHERE) {
			computationMethodTOAtoBOA = 0;
		} else if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::ANALYTIC) {
			computationMethodTOAtoBOA = 1;
		} else if (_dartInputsDAO.getDartRadiation().getAtmosphericTransfer() == DartInputs::DartRadiation::RADIATIVE_TRANSFER) {
			computationMethodTOAtoBOA = 2;
		}

		if (solarIllumination) { //si on a de l'illumination solaire.
			if (DartInputs::SpectralIrradiance* spectralIrradiance = dynamic_cast<DartInputs::SpectralIrradiance*>(solarIllumination.get())) {
				PhaseProperty::setIrradianceFromDatabaseComputationMethod(0);
				databaseName = spectralIrradiance->getSpectralDatabase()->getDartFilePath();
				databaseSolarTable = spectralIrradiance->getSpectralDatabase()->getModelName() + "_oIRR";
				databaseSolarColumn = "irradiance";
				PhaseProperty::setWeightingOfReflectanceParameters(spectralIrradiance->getSpectralDatabase()->getWeiggtingReflectanceParameters());
				PhaseProperty::setWeightingOfAtmosphereParameters(spectralIrradiance->getSpectralDatabase()->getWeiggtingAtmosphericParameters());
			} else if (DartInputs::BandIrradiance* bandIrradiance = dynamic_cast<DartInputs::BandIrradiance*>(solarIllumination.get())) {
				PhaseProperty::setIrradianceFromDatabaseComputationMethod(1);
				for (shared_ptr<DartInputs::SpectralBand> spectralBand : _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand()) {
					if (DartInputs::SpectralRange* spectralRange = dynamic_cast<DartInputs::SpectralRange*>(spectralBand.get())) {
						for (int i = 0; i < spectralRange->getNumberOfBand(); ++i) {
							double irradianceInterval = 0.0;
							irradianceInterval = spectralBand->getSpectralSolarConstant();
							if (dynamic_cast<DartInputs::SolarIrradiance*>(bandIrradiance->getSolarRadiationDefinition().get())) {
								irradianceInterval = irradianceInterval / fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
							}
							irradianceVector.push_back(irradianceInterval);
						}
					} else {
						double irradianceValue = 0.0;
						irradianceValue = spectralBand->getSpectralSolarConstant();
						if (dynamic_cast<DartInputs::SolarIrradiance*>(bandIrradiance->getSolarRadiationDefinition().get())) {
							irradianceValue = irradianceValue / fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
						}
						irradianceVector.push_back(irradianceValue);
					}
				}
			} else if (DartInputs::BandRadiance* bandRadiance = dynamic_cast<DartInputs::BandRadiance*>(solarIllumination.get())) {
				PhaseProperty::setIrradianceFromDatabaseComputationMethod(2);
				readTOABOAFromFile(bandRadiance->getRadianceFile(), genDTO.numberOfSpectralBands);
			} else if (DartInputs::SpectralRadiation* spectralRadiation = dynamic_cast<DartInputs::SpectralRadiation*>(solarIllumination.get())) {
				readOverrideBandRadianceFile(spectralRadiation->getRadianceFile(), overrideBandRadianceFileContent);
				overrideBandRadianceResult = vector<vector<double> >(overrideBandRadianceFileContent.size() + 2, vector<double>(3));
				PhaseProperty::setIrradianceFromDatabaseComputationMethod(1);
				for (unsigned int bandNumber = 0; bandNumber < _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand().size(); ++bandNumber) {
					shared_ptr<DartInputs::SpectralBand> spectralBand = _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand()[bandNumber];
					if (DartInputs::SpectralRange* spectralRange = dynamic_cast<DartInputs::SpectralRange*>(spectralBand.get())) {
						for (int i = 0; i < spectralRange->getNumberOfBand(); ++i) {
							double irradianceInterval = 0.0;
							Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(bandNumber + i),
								PhaseProperty::getUpperBandWavelength(bandNumber + i), overrideBandRadianceFileContent.size(), 2,
								overrideBandRadianceFileContent, overrideBandRadianceResult);
							irradianceInterval = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][1];
							if (dynamic_cast<DartInputs::SolarIrradiance*>(bandIrradiance->getSolarRadiationDefinition().get())) {
								irradianceInterval = irradianceInterval / fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
							}
							irradianceVector.push_back(irradianceInterval);
						}
					} else {
						double irradianceValue = 0.0;
						Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(bandNumber), PhaseProperty::getUpperBandWavelength(bandNumber),
							overrideBandRadianceFileContent.size(), 2, overrideBandRadianceFileContent, overrideBandRadianceResult);
						irradianceValue = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][1];
						if (dynamic_cast<DartInputs::SolarIrradiance*>(bandIrradiance->getSolarRadiationDefinition().get())) {
							irradianceValue = irradianceValue / fabs(cos(PhaseProperty::getSunZenithAngleInRadians()));
						}
						irradianceVector.push_back(irradianceValue);
					}
				}
			}

			if (computationMethodTOAtoBOA != 2 && !dynamic_cast<DartInputs::BandRadiance*>(solarIllumination.get())) {
				if (dynamic_cast<DartInputs::SpectralRadiation*>(solarIllumination.get())) {
					for (unsigned int bandNumber = 0; bandNumber < _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand().size(); ++bandNumber) {
						shared_ptr<DartInputs::SpectralBand> spectralBand = _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand()[bandNumber];
						if (DartInputs::SpectralRange* spectralRange = dynamic_cast<DartInputs::SpectralRange*>(spectralBand.get())) {
							for (int j = 0; j < spectralRange->getNumberOfBand(); ++j) {
								double skylValue = 0.0;
								Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(bandNumber + j),
									PhaseProperty::getUpperBandWavelength(bandNumber + j), overrideBandRadianceFileContent.size(), 3,
									overrideBandRadianceFileContent, overrideBandRadianceResult);
								skylValue = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][2];
								skylVector.push_back(skylValue);
							}
						} else {
							double skylValue = 0.0;
							Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(bandNumber),
								PhaseProperty::getUpperBandWavelength(bandNumber), overrideBandRadianceFileContent.size(), 2,
								overrideBandRadianceFileContent, overrideBandRadianceResult);
							skylValue = overrideBandRadianceResult[overrideBandRadianceResult.size() - 1][2];
							skylVector.push_back(skylValue);
						}
					}
				} else {
					for (shared_ptr<DartInputs::SpectralBand> spectralBand : _dartInputsDAO.getDartSpectralBands().getListOfSpectralBand()) {
						if (DartInputs::SpectralRange* spectralRange = dynamic_cast<DartInputs::SpectralRange*>(spectralBand.get())) {
							for (int i = 0; i < spectralRange->getNumberOfBand(); ++i) {
								double skylValue = 0.0;
								skylValue = spectralBand->getSkyl();
								skylVector.push_back(skylValue);
							}
						} else {
							double skylValue = 0.0;
							skylValue = spectralBand->getSkyl();
							skylVector.push_back(skylValue);
						}
					}
				}
			}
		}
		PhaseProperty::setSolarConstantDatabaseParameters(databaseSolarTable, databaseSolarColumn, databaseName, computationMethodTOAtoBOA);
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readAirPhaseFunctions(	vector<AirPhaseFunction> & airPhaseFunctions,
										const int & numberOfSpectralBands) {
	if (_useYaML) {
		return readAirPhaseFunctionsYaML(airPhaseFunctions, numberOfSpectralBands);
	} else {
		return readAirPhaseFunctionsXML(airPhaseFunctions, numberOfSpectralBands);
	}
}
bool PhaseDAO::readAirPhaseFunctionsXML(vector<AirPhaseFunction> & airPhaseFunctions,
										const int & numberOfSpectralBands) {
	try {
		string airFunctionRootNode = "Coeff_diff.Volumes.AirMultiFunctions.AirFunction";
		unsigned int nbAirPhaseFunctions = DartSAParameters::getNbOf(airFunctionRootNode);
		for (unsigned int numAPF = 0; numAPF < nbAirPhaseFunctions; ++numAPF) {
			ostringstream airPFStream;
			airPFStream << airFunctionRootNode << "[" << numAPF << "].";
			string currentAPFString = airPFStream.str();
			airPhaseFunctions.push_back(AirPhaseFunction(numberOfSpectralBands));
			AirPhaseFunction & currentAPF = airPhaseFunctions.back();
			currentAPF.setParametersFilePath(DartSAParameters::get<string>(currentAPFString + "ModelName"),
				DartSAParameters::get<string>(currentAPFString + "databaseName"));
			currentAPF.setName(DartSAParameters::get<string>(currentAPFString + "ident"));
			currentAPF.setIndex(airPhaseFunctions.size() - 1);
			bool useMultiplicativeFactor = DartSAParameters::get<bool>(currentAPFString + "useMultiplicativeFactorForLUT");
			currentAPF.setUseMultiplicativeFactorForLut(useMultiplicativeFactor);
			if (useMultiplicativeFactor) {
				string multiplicativeFactorRootNode = currentAPFString
						+ "AirFunctionNodeMultiplicativeFactorForLut.AirFunctionMultiplicativeFactorForLut";
				unsigned int nbMultiplicativeFactor = DartSAParameters::getNbOf(multiplicativeFactorRootNode);
				vector<double> albedoMFPerBand(nbMultiplicativeFactor, 1.);
				vector<double> sigmaMFPerBand(nbMultiplicativeFactor, 1.);
				vector<double> p1MFPerBand(nbMultiplicativeFactor, 1.);
				vector<double> p2MFPerBand(nbMultiplicativeFactor, 1.);
				vector<double> p3MFPerBand(nbMultiplicativeFactor, 1.);
				for (unsigned int numMultiplicativeFactor = 0; numMultiplicativeFactor < nbMultiplicativeFactor; numMultiplicativeFactor++) {
					ostringstream airMFStream;
					airMFStream << multiplicativeFactorRootNode << "[" << numMultiplicativeFactor << "].";
					string currentMFFString = airMFStream.str();
					currentAPF.setAlbedoMultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "albedo"), numMultiplicativeFactor);
					currentAPF.setSigmaMultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "sigma"), numMultiplicativeFactor);
					currentAPF.setParameter1MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor1"),
						numMultiplicativeFactor);
					currentAPF.setParameter2MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor2"),
						numMultiplicativeFactor);
					currentAPF.setParameter3MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor3"),
						numMultiplicativeFactor);
				}
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}
bool PhaseDAO::readAirPhaseFunctionsYaML(	vector<AirPhaseFunction> & airPhaseFunctions,
											const int & numberOfSpectralBands) {
	try {
		for (shared_ptr<DartInputs::FluidProperty> fluidProperty : _dartInputsDAO.getDartOpticalProperties().getFluidProperties()) {
			airPhaseFunctions.push_back(AirPhaseFunction(numberOfSpectralBands));
			AirPhaseFunction & currentAPF = airPhaseFunctions.back();
			currentAPF.setParametersFilePath(fluidProperty->getOpticalDatabase()->getModelName(),
				fluidProperty->getOpticalDatabase()->getDartFilePath());
			currentAPF.setName(fluidProperty->getOpticalPropertyName());
			currentAPF.setIndex(airPhaseFunctions.size() - 1);
			// TODO MF for fluid is currently not implemented :scream:
			currentAPF.setUseMultiplicativeFactorForLut(false);

		}

//		const YAML::Node & fluidNode = getYaMLNode("fluidProperties");
//		if (fluidNode) {
//			const YAML::Node & fluidListNode = fluidNode["listWithId"];
//			if (fluidListNode) {
//				for (YAML::const_iterator it = fluidListNode.begin(); it != fluidListNode.end(); ++it) {
//					airPhaseFunctions.push_back(AirPhaseFunction(numberOfSpectralBands));
//					AirPhaseFunction & currentAPF = airPhaseFunctions.back();
//					currentAPF.setParametersFilePath((*it).second["opticalDatabase"]["modelName"].Scalar(),
//						(*it).second["opticalDatabase"]["dartFilePath"]["relativePath"].Scalar());
//					currentAPF.setName((*it).second["opticalPropertyName"].Scalar());
//					currentAPF.setIndex(airPhaseFunctions.size() - 1);
//					bool useMultiplicativeFactor = (*it).second["useSameMultiplicativeFactorForAllBands"].Scalar() == "true";
//					currentAPF.setUseMultiplicativeFactorForLut(useMultiplicativeFactor);
//					if (useMultiplicativeFactor) {
//						vector<double> albedoMFPerBand(numberOfSpectralBands, 1.);
//						vector<double> sigmaMFPerBand(numberOfSpectralBands, 1.);
//						vector<double> p1MFPerBand(numberOfSpectralBands, 1.);
//						vector<double> p2MFPerBand(numberOfSpectralBands, 1.);
//						vector<double> p3MFPerBand(numberOfSpectralBands, 1.);
//						for (unsigned int numMultiplicativeFactor = 0; numMultiplicativeFactor < (unsigned int) numberOfSpectralBands;
//								++numMultiplicativeFactor) {
//							// TODO:
////							currentAPF.setAlbedoMultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "albedo"), numMultiplicativeFactor);
////							currentAPF.setSigmaMultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "sigma"), numMultiplicativeFactor);
////							currentAPF.setParameter1MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor1"),
////								numMultiplicativeFactor);
////							currentAPF.setParameter2MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor2"),
////								numMultiplicativeFactor);
////							currentAPF.setParameter3MultiplicativeFactor(DartSAParameters::get<double>(currentMFFString + "multiplicativeFactor3"),
////								numMultiplicativeFactor);
//						}
//					}
//				}
//			}
//		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readSecondarySourcesOfLight(LightsContainer & lightsContainer) {
	if (_useYaML) {
		return readSecondarySourcesOfLightYAML(lightsContainer);
	} else {
		return readSecondarySourcesOfLightXML(lightsContainer);
	}
}

bool PhaseDAO::readSecondarySourcesOfLightXML(LightsContainer & lightsContainer) {
	try {
		std::string lightFunctionRootNode = "Directions.SecondaryIlluminationSource";
		unsigned int nbLights = DartSAParameters::getNbOf(lightFunctionRootNode);
		for (unsigned int lightIndex = 0; lightIndex < nbLights; ++lightIndex) {
			std::string currentLightString = lightFunctionRootNode + "[" + boost::lexical_cast<string>(lightIndex) + "].";
			// Light Type: 0=Unidirectional; 1=Sphere
			unsigned int lightType = DartSAParameters::get<unsigned int>(currentLightString + "type");

			if (lightType == Light::UNIDIRECTIONAL) {
				unsigned int frameDefinition = DartSAParameters::get<unsigned int>(currentLightString + "InfiniteUnidirectionalIlluminationSource.frameDefinition");
				unsigned int directionDefinition = DartSAParameters::get<unsigned int>(currentLightString + "InfiniteUnidirectionalIlluminationSource.directionDefinition");
				if (directionDefinition == 0) {
					double x = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.CarthesianDirection.directionX");
					double y = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.CarthesianDirection.directionY");
					double z = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.CarthesianDirection.directionZ");
					if (frameDefinition == 0) {
						x = -x;
						y = -y;
						z = -z;
					}
					lightsContainer.addLight(UnidirectionalLight(Vecteur<double>(x, y, z)));
				} else if (directionDefinition == 1) {
					double zenith = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.SphericalDirection.directionTheta");
					double azimuth = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.SphericalDirection.directionPhi");
					if (frameDefinition == 0) {
						zenith = 180. - zenith;
						azimuth = WrapTo360(azimuth + 180.);
					}
					lightsContainer.addLight(UnidirectionalLight(zenith, azimuth));
				} else if (directionDefinition == 2) {
					unsigned int eulerAngleDefinition = DartSAParameters::get<unsigned int>(currentLightString + "InfiniteUnidirectionalIlluminationSource.EulerAngles.angleDefinition");
					double angle1 = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.EulerAngles.angle1");
					double angle2 = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.EulerAngles.angle2");
					double angle3 = DartSAParameters::get<double>(currentLightString + "InfiniteUnidirectionalIlluminationSource.EulerAngles.angle3");

					lightsContainer.addLight(UnidirectionalLight((DirectionCartesienne::ModeTaitBryan)eulerAngleDefinition, angle1, angle2, angle3, frameDefinition));
				}
			} else if (lightType == Light::SPHERE) {
				double radius = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.sphereRadius");
				unsigned int sampling = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.sampling");
				unsigned int spherePositionDefinition = DartSAParameters::get<unsigned int>(currentLightString + "SphereIlluminationSource.positionDefinition");
				if (spherePositionDefinition == 0) {
					double zenith = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereDirection.secondarySourceZenithAngle");
					double azimuth = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereDirection.secondarySourceAzimuthAngle");
					unsigned int angleDefinition = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereDirection.directionDefinition");
					if (angleDefinition == 0) {
						zenith = 180. - zenith;
						azimuth = WrapTo360(azimuth + 180.);
					}
					double distance = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereDirection.distance");

					lightsContainer.addLight(LightSphere(radius, zenith, azimuth, distance, sampling));
				} else {
					double posX = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereIlluminationSourcePosition.secondarySourcePositionX");
					double posY = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereIlluminationSourcePosition.secondarySourcePositionY");
					double posZ = DartSAParameters::get<double>(currentLightString + "SphereIlluminationSource.SphereIlluminationSourcePosition.secondarySourcePositionZ");

					lightsContainer.addLight(LightSphere(radius, Point3<double>(posX, posY, posZ), sampling));
				}
			} else if (lightType == Light::SPOT) {
				double fov = DartSAParameters::get<unsigned int>(currentLightString + "SpotIlluminationSource.fov");

				double posX = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.SpotPosition.secondarySourcePositionX");
				double posY = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.SpotPosition.secondarySourcePositionY");
				double posZ = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.SpotPosition.secondarySourcePositionZ");

				unsigned int frameDefinition = DartSAParameters::get<unsigned int>(currentLightString + "SpotIlluminationSource.frameDefinition");
				unsigned int directionDefinition = DartSAParameters::get<unsigned int>(currentLightString + "SpotIlluminationSource.directionDefinition");
				if (directionDefinition == 0) {
					double x = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.CarthesianDirection.directionX");
					double y = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.CarthesianDirection.directionY");
					double z = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.CarthesianDirection.directionZ");
					if (frameDefinition == 0) {
						x = -x;
						y = -y;
						z = -z;
					}
					lightsContainer.addLight(SpotLight(Point3<double>(posX, posY, posZ), fov, Vecteur<double>(x, y, z)));
				} else if (directionDefinition == 1) {
					double zenith = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.SphericalDirection.directionTheta");
					double azimuth = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.SphericalDirection.directionPhi");
					if (frameDefinition == 0) {
						zenith = 180. - zenith;
						azimuth = WrapTo360(azimuth + 180.);
					}
					lightsContainer.addLight(SpotLight(Point3<double>(posX, posY, posZ), fov, zenith, azimuth));
				} else if (directionDefinition == 2) {
					unsigned int eulerAngleDefinition = DartSAParameters::get<unsigned int>(currentLightString + "SpotIlluminationSource.EulerAngles.angleDefinition");
					double angle1 = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.EulerAngles.angle1");
					double angle2 = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.EulerAngles.angle2");
					double angle3 = DartSAParameters::get<double>(currentLightString + "SpotIlluminationSource.EulerAngles.angle3");

					lightsContainer.addLight(SpotLight(Point3<double>(posX, posY, posZ), fov, (DirectionCartesienne::ModeTaitBryan)eulerAngleDefinition, angle1, angle2, angle3, frameDefinition));
				}
			}
			Light & currentLight = lightsContainer.getModifiableLight(lightsContainer.getNbLights() - 1);

			// Irradiance Mode: 0=database; 1=XML attributes; 3=CSV File. 2 is for precomputed files and unimplemented yet.
			unsigned int irradianceMode = DartSAParameters::get<unsigned int>(currentLightString + "irradianceMode");

			if (irradianceMode == 0) {
				std::string irradianceDatabaseFileName = DartSAParameters::get<std::string>(currentLightString + "irradianceDatabaseNode.databaseName");
				std::string irradianceDatabaseFilePath = DartEnv::getFilePathForRead(INPUT_DIR, irradianceDatabaseFileName);
				if (irradianceDatabaseFilePath != "") {
					std::string irradianceDatabaseTable = DartSAParameters::get<std::string>(currentLightString + "irradianceDatabaseNode.irradianceTable") + "_oIRR";
					std::string irradianceDatabaseColumn = DartSAParameters::get<std::string>(currentLightString + "irradianceDatabaseNode.irradianceColumn");

					currentLight.initIrradianceThroughDatabase(irradianceDatabaseFilePath, irradianceDatabaseTable, irradianceDatabaseColumn);
				} else {
					cerr << "Secondary light source " << lightIndex << " ignored: can't find \"" << irradianceDatabaseFileName << "\" database." << endl;
					lightsContainer.removeLight(lightsContainer.getNbLights() - 1);
				}
			} else if (irradianceMode == 1) {
				unsigned int irradianceDefinition = DartSAParameters::getNbOf(currentLightString + "SpectralIrradiances.irraDef");

				double factor = 1;
				if (irradianceDefinition == 1) {
					factor /= fabs(currentLight.getDirection().getZ()); // Cos(theta)
				}

				unsigned int nbIrradianceValues = DartSAParameters::getNbOf(currentLightString + "SpectralIrradiances.SpectralIrradianceValue");
				std::vector<double> irradianceValues(nbIrradianceValues);
				for (unsigned int irradianceValueIndex = 0; irradianceValueIndex < nbIrradianceValues; ++irradianceValueIndex) {
					irradianceValues[irradianceValueIndex] = DartSAParameters::get<double>(currentLightString + "SpectralIrradiances.SpectralIrradianceValue[" + boost::lexical_cast<string>(irradianceValueIndex) + "].irradiance") * factor;
				}
				currentLight.setIrradiances(irradianceValues);
			} else if (irradianceMode == 3) {
				unsigned int irradianceDefinition = DartSAParameters::getNbOf(currentLightString + "SpectralIrradiances.irraDef");
				std::string csvIrradianceFileName = DartSAParameters::get<std::string>(currentLightString + "BandIrradianceFileNode.bandIrradianceFile");
				std::string csvIrradianceFilePath = DartEnv::getFilePathForRead(INPUT_DIR, csvIrradianceFileName);
				if (csvIrradianceFilePath != "") {
					std::vector<std::vector<double> > fileData;
					readOverrideBandRadianceFile(csvIrradianceFilePath, fileData);

					// The interpolation method needs wavelengthd and irradiances in separate vectors
					std::vector<double> fileWavelengths(fileData.size()), fileIrradiances(fileData.size());
					for (unsigned int valueIndex = 0; valueIndex < fileWavelengths.size(); ++valueIndex) {
						fileWavelengths[valueIndex] = fileData[valueIndex][0];
						fileIrradiances[valueIndex] = fileData[valueIndex][1];
					}
					std::vector<double> irradianceValues(PhaseProperty::_userSpectralBands.size());
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::_userSpectralBands.size(); ++spectralBandIndex) {
						double irradianceValue = PhaseProperty::computeMeanIrradianceWithTrapezoidalMethod(fileWavelengths.size(), spectralBandIndex, fileWavelengths, fileIrradiances);
						irradianceValue *= (PhaseProperty::_userSpectralBands[spectralBandIndex].upperLambdaValue - PhaseProperty::_userSpectralBands[spectralBandIndex].lowerLambdaValue);
						if (irradianceDefinition == 1) {
							irradianceValue /= fabs(currentLight.getDirection().getZ());
						}
						irradianceValues[spectralBandIndex] = irradianceValue;
					}
					currentLight.setIrradiances(irradianceValues);
				} else {
					cerr << "Secondary light source " << lightIndex << " ignored: can't find \"" << csvIrradianceFileName << "\"." << endl;
					lightsContainer.removeLight(lightsContainer.getNbLights() - 1);
				}
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readSecondarySourcesOfLightYAML(LightsContainer & lightsContainer) {
	return true;
}

bool PhaseDAO::readAndStoreDirections(vector<DirectionCartesienne> & listDirections) {
	FichierProperties directionProperty(DartEnv::getFilePathForRead(OUTPUT_DIR, SIMULATIONPROPERTIESFILE));
	unsigned int nbDirections;
	directionProperty.valeurProperties<unsigned int>("direction.numberOfDirections", nbDirections);

	for (unsigned int indexDir = 0; indexDir < nbDirections; indexDir++) {
		ostringstream dirstring;
		dirstring << "direction.direction" << indexDir << ".";
		string directionKey = dirstring.str();
		DirectionCartesienne dir;

		dir.setBeginTheta(deg2rad(directionProperty.valeurProperties<double>(directionKey + "thetaBegin")));
		dir.setCenterTheta(deg2rad(directionProperty.valeurProperties<double>(directionKey + "thetaCenter")));
		dir.setEndTheta(deg2rad(directionProperty.valeurProperties<double>(directionKey + "thetaEnd")));

		dir.setBeginPhi(deg2rad(directionProperty.valeurProperties<double>(directionKey + "phiBegin")));
		dir.setCenterPhi(deg2rad(directionProperty.valeurProperties<double>(directionKey + "phiCenter")));
		dir.setEndPhi(deg2rad(directionProperty.valeurProperties<double>(directionKey + "phiEnd")));

		dir.setCosThetaDOmega(directionProperty.valeurProperties<double>(directionKey + "cosThetaDOmega"));

		dir.setSolidAngle(directionProperty.valeurProperties<double>(directionKey + "omega"));
		dir.setAngularSector(directionProperty.valeurProperties<unsigned int>(directionKey + "angularSector"));
		dir.setProperty(DirectionRoot::propertyOfString(directionProperty.valeurProperties<string>(directionKey + "property")));

		dir.calculerCoordonneesCartesiennes();

		listDirections.push_back(dir);
	}
	return true;
}


bool PhaseDAO::readTemperatureProperties(std::vector<TemperatureFunction> & temperatureFunctions) {
	if (_useYaML) {
		return readTemperaturePropertiesYAML(temperatureFunctions);
	} else {
		return readTemperaturePropertiesXML(temperatureFunctions);
	}
}

bool PhaseDAO::readTemperaturePropertiesXML(std::vector<TemperatureFunction> & temperatureFunctions) {
	try {
		std::string temperatureFunctionRootNode = "Coeff_diff.Temperatures.ThermalFunction";
		unsigned int nbTemperatureFunctions = DartSAParameters::getNbOf(temperatureFunctionRootNode);
		temperatureFunctions.resize(nbTemperatureFunctions);
		for (unsigned int tfIndex = 0; tfIndex < nbTemperatureFunctions; ++tfIndex) {
			std::string currentTemperatureString = temperatureFunctionRootNode + "[" + boost::lexical_cast<string>(tfIndex) + "].";

			temperatureFunctions[tfIndex].setIdTemperature(DartSAParameters::get<std::string>(currentTemperatureString + "idTemperature"));
			temperatureFunctions[tfIndex].setMeanTemprature(DartSAParameters::get<double>(currentTemperatureString + "meanT"));
			temperatureFunctions[tfIndex].setDeltaTemperature(DartSAParameters::get<double>(currentTemperatureString + "deltaT"));
			temperatureFunctions[tfIndex].setOverrideTemperatureProfile(DartSAParameters::get<bool>(currentTemperatureString + "override3DMatrix"));
			temperatureFunctions[tfIndex].setSingleTemperatureSurface(DartSAParameters::get<bool>(currentTemperatureString + "singleTemperatureSurface"));
			temperatureFunctions[tfIndex].setUseOpticalFactorMatrix(DartSAParameters::get<unsigned int>(currentTemperatureString + "useOpticalFactorMatrix"));
			temperatureFunctions[tfIndex].setUsePrecomputedIpaRs(DartSAParameters::get<bool>(currentTemperatureString + "usePrecomputedIPARs"));

			if (temperatureFunctions[tfIndex].isUsingOpticalFactorMatrix()) {
				temperatureFunctions[tfIndex].setDuplicateFirstOpticalMatrixLayer(DartSAParameters::get<bool>(currentTemperatureString + "opticalFactorMatrix.duplicateFirstMatrixLayer"));
				temperatureFunctions[tfIndex].setOpticalFactorMatrixFile(DartSAParameters::get<std::string>(currentTemperatureString + "opticalFactorMatrix.opticalFactorMatrixFile"));
			}
			if (temperatureFunctions[tfIndex].isUsingSunlitShadedTemperatures()) {
				temperatureFunctions[tfIndex].setSunlitTemperature(DartSAParameters::get<double>(currentTemperatureString + "SunlitAndShadedTemperature.sunlitTemperature"));
				temperatureFunctions[tfIndex].setShadedTemperature(DartSAParameters::get<double>(currentTemperatureString + "SunlitAndShadedTemperature.shadedTemperature"));
			}
			if (temperatureFunctions[tfIndex].isUsingScopeFiles()) {
				temperatureFunctions[tfIndex].setShadedScopeFile(DartSAParameters::get<std::string>(currentTemperatureString + "ScopeTemperatureFile.shadedFile"));
				temperatureFunctions[tfIndex].setSunlitScopeFile(DartSAParameters::get<std::string>(currentTemperatureString + "ScopeTemperatureFile.sunlitFile"));
			}
			if (temperatureFunctions[tfIndex].isUsingPrecomputedIpaRs()) {
				temperatureFunctions[tfIndex].setMinIpar(DartSAParameters::get<double>(currentTemperatureString + "PrecomputedIPARs.minIPAR"));
				temperatureFunctions[tfIndex].setMaxIpar(DartSAParameters::get<double>(currentTemperatureString + "PrecomputedIPARs.maxIPAR"));
			}
		}
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
		return false;
	}
	return true;
}

bool PhaseDAO::readTemperaturePropertiesYAML(std::vector<TemperatureFunction> & temperatureFunctions) {
	unsigned int nbTemperatureFunctions = _dartInputsDAO.getDartOpticalProperties().getThermalProperties().size();
	bool override3DMatrix = false;
	bool allDeltasEqual0 = true;

	temperatureFunctions.resize(nbTemperatureFunctions);

	unsigned int tfIndex = 0;
	for (const shared_ptr<DartInputs::IThermalProperty> &iThermProp : _dartInputsDAO.getDartOpticalProperties().getThermalProperties()) {
		if (DartInputs::ThermalProperty* thermalProp = dynamic_cast<DartInputs::ThermalProperty*>(iThermProp.get())) {
			tfIndex = _dartInputsDAO.getXml2DPropertyIndex(iThermProp->getItemId());
			if (temperatureFunctions.size() <= tfIndex) {
				temperatureFunctions.resize(tfIndex + 1);
			}
			temperatureFunctions[tfIndex].setIdTemperature(thermalProp->getThermalPropertyName());
			temperatureFunctions[tfIndex].setMeanTemprature(thermalProp->getMeanTemperature());
			temperatureFunctions[tfIndex].setDeltaTemperature(thermalProp->getDeltaTemperature());
			temperatureFunctions[tfIndex].setOverrideTemperatureProfile(thermalProp->getOverride3DMatrixOfTemperature());
			temperatureFunctions[tfIndex].setSingleTemperatureSurface(thermalProp->getSingleTemperatureSurface());

			if (DartInputs::PrecomputedIPARs* precompIPARs = thermalProp->getPrecomputedIPARs().get()) {
				temperatureFunctions[tfIndex].setUsePrecomputedIpaRs(true);

				temperatureFunctions[tfIndex].setMinIpar(precompIPARs->getMinIPAR());
				temperatureFunctions[tfIndex].setMaxIpar(precompIPARs->getMaxIPAR());
			} else {
				temperatureFunctions[tfIndex].setUsePrecomputedIpaRs(false);
			}

			if (auto tridimensionalNode = thermalProp->getTriDimensionalMultiplicativeFactor()) {
				temperatureFunctions[tfIndex].setUseOpticalFactorMatrix(true);
				temperatureFunctions[tfIndex].setOpticalFactorMatrixFile(tridimensionalNode->getTriDimensionalmultiplicativeFactorFile());
				temperatureFunctions[tfIndex].setDuplicateFirstOpticalMatrixLayer(tridimensionalNode->getIsDuplicateFirstVoxelLayer());
			} else {
				temperatureFunctions[tfIndex].setUseOpticalFactorMatrix(false);
			}
		}
	}

	return true;
}

bool PhaseDAO::isOverrideBandIrradiance() {
	/**
	 * Glossaire (On s'le fait à l'ancienne ~~ )
	 */
	int irradianceMode = 0;
	string irradianceModeKey = "Phase.DartInputParameters.nodeIlluminationMode.irradianceMode";
	if (DartSAParameters::exists(irradianceModeKey)) {
		DartSAParameters::get(irradianceModeKey, irradianceMode);
	} else {
		irradianceModeKey = "Phase.DartInputParameters.Lidar.FluxtrackingSolarNoise.nodeIlluminationMode.irradianceMode";
		if (DartSAParameters::exists(irradianceModeKey)) {
			DartSAParameters::get(irradianceModeKey, irradianceMode);
		}
	}
	return irradianceMode == 3;
}

/**
 * Matrice
 * Lignes : bandes spectrales (nbLignes = nbBandes spectrales)
 * Colonnes : 1ere = centralWavelength, 2nd = irradiance value, 3eme = SKYL
 */
void PhaseDAO::readOverrideBandRadianceFile(const std::string & overrideFileParameter,
											vector<vector<double> > & overrideBandRadianceFileContent) {
	/**
	 * Glossaire (On s'le fait à l'ancienne ~~ )
	 */
	double tempCentralWavelength = 0.0;
	double tempIrradiance = 0.0;
	double tempSkyl = 0.0;
	vector<double> vectorLine = vector<double>();
	string strLine = "";

	/**
	 * Corps
	 */
// Lecture du fichier
	if (!overrideFileParameter.empty()) {
		string overrideFilePath = DartEnv::getFilePathForRead(INPUT_DIR, overrideFileParameter);
		ifstream stream(overrideFilePath.c_str(), ifstream::in);
		skipBOM(stream);
		while (getline(stream, strLine)) {
// Lines beginning with '*' character are user commentaries
			if (strLine.compare("") != 0 && strLine.at(0) != '*') {
				std::replace(strLine.begin(), strLine.end(), ';', ' '); // .csv style separator
				istringstream iss_line(strLine);
				if (iss_line >> tempCentralWavelength) {
					if (iss_line >> tempIrradiance) {
						if (iss_line >> tempSkyl) {
							vectorLine.clear();
							vectorLine.push_back(tempCentralWavelength);
							vectorLine.push_back(tempIrradiance);
							vectorLine.push_back(tempSkyl);
							overrideBandRadianceFileContent.push_back(vectorLine);
						}
					}
				}
			}
		}
		stream.close(); // on ferme le fichier
	}
}

bool PhaseDAO::skipBOM(std::istream & in) {
	char test[4] = { 0 };
	in.read(test, 3);
	if (strcmp(test, "\xEF\xBB\xBF") == 0)
		return true;
	in.seekg(0);
	return false;
}

/* Read LiDAR parameters */
bool PhaseDAO::readLiDARXML() {
       string LiDARmodeKey = "Phase.DartInputParameters.Lidar.PulseDuration.pulse_energy";
       double Epulse = 1.0;
       if (DartSAParameters::exists(LiDARmodeKey)) {
               DartSAParameters::get(LiDARmodeKey, Epulse);
       }else
               return false;
       PhaseProperty::setLiDARpulseEnergy(Epulse/1000.0);
       return true;
}

