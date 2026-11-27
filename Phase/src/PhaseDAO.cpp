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

/**!
 * Permet de ne faire qu'une seule initialisation des fichiers pour ceux que l'on ouvre plusieurs fois.
 */
void PhaseDAO::initPhase(bool fromStream) {
	DartSAParameters::initialize(DartEnv::getCurrentSimulationInputPath());
	// ouverture des fichiers
	initXMLs(fromStream);
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

// -----------------------------------------------------------------------------
/**!
 * Permet de ne faire qu'une seule fermeture des fichiers pour ceux que l'on ouvre plusieurs fois.
 */
void PhaseDAO::closePhase() {
	DartSAParameters::terminate();
}

bool PhaseDAO::readPhaseParameters(PhaseGeneralParametersDTO & genDTO) {
	readSpectralBandsXML(genDTO);
	readMaterials(genDTO);


	if (genDTO.isThermalEmissionEnabled) {
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

	return readPhaseParametersXML(genDTO);
}

void PhaseDAO::readMaterials(PhaseGeneralParametersDTO & genDTO) {
	DartSAParameters::openFile("materials.xml");
	parseOpticalMaterials(genDTO);
	parseEmissionMaterials(genDTO);
	DartSAParameters::terminate();
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
		if(DartSAParameters::exists("Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer"))
			genDTO.atmosphereRTmode = DartSAParameters::get<unsigned int>(
					"Phase.EngineParameter.LuxCoreRenderEngineParameters.atmosphericRadiativeTransfer");
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
	} catch (DartException &E) {
		cerr << E.getOriginMessage() << endl;
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

bool PhaseDAO::readAtmosphereParameters(AtmosphereParameters& atmosphereParameters) {
		return readAtmosphereParametersXML(atmosphereParameters);
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
	return readNumberOfOpticalPropertiesModelsXML();
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

bool PhaseDAO::readCoeffDiffParametersForIsProspectUseLambertian(	const int & index,
																	bool& useProspectGen,
																	int& lambertianDefinition) {
	useProspectGen = false;
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
	return true;
}

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
		return readCoeffDiffParametersForLambertianXML(index, phaseName, modelName, databaseName, standardDeviation, isSpecular, useProspectGen, lambertianDefinition, smc, delta, keepOutputFile);
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

/*
 *    LAMBERTIEN
 */
bool PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(const int & index,
																  OpticalFactorParameters & opticalFactorParameters) {
	ostringstream oss;
	oss << index;
	return readCoeffDiffParametersOpticalFactorsXML("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str() + "].lambertianNodeMultiplicativeFactorForLUT.", "lambertianMultiplicativeFactorForLUT", opticalFactorParameters);
}

/*
 * Foliar
 */

bool PhaseDAO::readCoeffDiffParametersForFluidOpticalFactors(const int & index,
																  OpticalFactorParameters & opticalFactorParameters) {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersOpticalFactorsXML("Coeff_diff.Volumes.AirMultiFunctions.AirFunction[" + oss.str() + "].AirFunctionNodeMultiplicativeFactorForLut.", "AirFunctionMultiplicativeFactorForLut", opticalFactorParameters);
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

bool PhaseDAO::readCoeffDiffParametersForLambertianFluspect(const int & index,
															FluorescenceParameters & fluorescenceParameters) {
		ostringstream oss;
		oss << index;
		return readCoeffDiffParametersForFluspect("Coeff_diff.Surfaces.LambertianMultiFunctions.LambertianMulti[" + oss.str() + "].Lambertian.ProspectExternalModule.",
			fluorescenceParameters);
}

bool PhaseDAO::readCoeffDiffParametersForFoliarFluspect(const int & index,
														FluorescenceParameters & fluorescenceParameters,
														bool & hasDifferentBottom,
														bool bottom) {
		ostringstream oss;
		oss << index;

		hasDifferentBottom = false;
		string temp = "Coeff_diff.Volumes.UnderstoryMultiFunctions.UnderstoryMulti[" + oss.str();
		DartSAParameters::get(temp + "].hasDifferentModelForBottom", hasDifferentBottom);
		string modelKey = hasDifferentBottom ? (bottom ? "UnderstoryMultiBottomModel" : "UnderstoryMultiTopModel") : "UnderstoryMultiModel";
		return readCoeffDiffParametersForFluspect(temp + "]." + modelKey + ".ProspectExternalModule.", fluorescenceParameters);
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
					DartSAParameters::get(key + "WindProfileEta.useBioClimaticWeighting", fluorescenceParameters.useBioClimaticProfileEta);
					if (fluorescenceParameters.useBioClimaticProfileEta) {
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
					fluorescenceParameters.useBioClimaticProfileEta = false;
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
		return readCoeffDiffParametersForSpecularLambertianXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
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

bool PhaseDAO::readCoeffDiffParametersForRPVThetaMax(	const int & index,
														double & theta_max) {
		return readCoeffDiffParametersForRPVThetaMaxXML(index, theta_max);
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

bool PhaseDAO::readCoeffDiffParametersForRPVSpecular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution) {
		return readCoeffDiffParametersForRPVSpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution);
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

bool PhaseDAO::readCoeffDiffParametersForHapke(	const int & index,
												string & phaseName,
												string & modelName,
												string & databaseName,
												string & transmittanceModelName,
												string & transmittanceDatabaseName,
												bool& isSpecular,
												int &indexOrder) {
		return readCoeffDiffParametersForHapkeXML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular, indexOrder);
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

bool PhaseDAO::readCoeffDiffParametersForHapkeSpecular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int &indexOrder) {

		return readCoeffDiffParametersForHapkeSpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution, indexOrder);
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
		return readCoeffDiffParametersForHapke2XML(index, phaseName, modelName, databaseName, transmittanceModelName, transmittanceDatabaseName,
			isSpecular, indexOrder);
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

bool PhaseDAO::readCoeffDiffParametersForHapke2Specular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int &indexOrder) {
	return readCoeffDiffParametersForHapke2SpecularXML(index, modelName, databaseName, useRefraction, specularRadianceDistribution, indexOrder);

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

/* read parameters of specular scatteirng function
 *
 * */
bool PhaseDAO::readCoeffDiffParametersForSpecular(	const int & index,
											std::string & phaseName,
											std::string & databaseName,
											std::string & niModelName,
											std::string & ntModelName){
		return readCoeffDiffParametersForSpecularXML(index, phaseName, databaseName, niModelName, ntModelName);
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

bool PhaseDAO::readCoeffDiffParametersForSpecularMaterial(const int & index, int & type, int & baseBSDF, int & baseBSDFindex){
		return readCoeffDiffParametersForSpecularMaterialXML(index, type, baseBSDF, baseBSDFindex);
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

bool PhaseDAO::readCoeffDiffParametersForSpecularRoughness(const int & index, double & roughness){
		return readCoeffDiffParametersForSpecularRoughnessXML(index, roughness);
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

bool PhaseDAO::readCoeffDiffParametersForSpecularTau(const int & index, double & tau){
		return readCoeffDiffParametersForSpecularTauXML(index, tau);
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

bool PhaseDAO::readCoeffDiffParametersForRPV(	const int & indice,
												string & phaseName,
												string & modelName,
												string & databaseName,
												bool& isSpecular,
												string & transmittanceModelName,
												string & transmittanceDatabaseName) {

		return readCoeffDiffParametersForRPVXML(indice, phaseName, modelName, databaseName, isSpecular, transmittanceModelName,
			transmittanceDatabaseName);
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


bool PhaseDAO::readMultiplicativeFactorLambertien(const PhaseGeneralParametersDTO & genDTO) {
	return readMultiplicativeFactorLambertienXML();
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

// read Specular multiplicative factor
bool PhaseDAO::readMultiplicativeFactorSpecular(const PhaseGeneralParametersDTO & genDTO){
		return readMultiplicativeFactorSpecularXML();
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

bool PhaseDAO::readMultiplicativeFactorHapke(const PhaseGeneralParametersDTO & genDTO) {
		return readMultiplicativeFactorHapkeXML();
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

bool PhaseDAO::readMultiplicativeFactorHapke2(const PhaseGeneralParametersDTO & genDTO) {
		return readMultiplicativeFactorHapke2XML();
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

bool PhaseDAO::readMultiplicativeFactorRPV(const PhaseGeneralParametersDTO & genDTO) {
		return readMultiplicativeFactorRPVXML();
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
		return readIrradianceParametersXML(genDTO, irradianceVector, skylVector);
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

bool PhaseDAO::readAirPhaseFunctions(	vector<AirPhaseFunction> & airPhaseFunctions,
										const int & numberOfSpectralBands) {
		return readAirPhaseFunctionsXML(airPhaseFunctions, numberOfSpectralBands);
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

bool PhaseDAO::readSecondarySourcesOfLight(LightsContainer & lightsContainer) {
		return readSecondarySourcesOfLightXML(lightsContainer);
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

