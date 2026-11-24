/*! Time-stamp: <@(#) DART_PHASE_ReadPhaseParameters.hapkeParameterh   10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : DART_PHASE_ReadPhaseParameters.hapkeParameterh
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description : Contient les fonctions de lectures des fichiers d'entree xml.
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
 * V 1.0  07/03/2005  VBT : First Revision
 *
 *********************************************************************
 * CVS $Log : DART_PHASE_ReadPhaseParameters.hapkeParameterh,v$
 */

#ifndef _PHASE_DAO
#define _PHASE_DAO

#include <vector>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "libDartBase.h"
#include "DartEnv.h"
#include "PhaseDTOs.hpp"
#include "PhaseProperty.hpp"
#include "containers/DirectionCartesienne.hpp"
#include "FichierProperties.hpp"
#include "./containers/air/AirPhaseFunction.hpp"
#include "GeneralDef.hpp"
#include "foliar/FoliarModel.hpp"
#include "containers/lights/LightsContainer.hpp"
#include "containers/lights/UnidirectionalLight.hpp"
#include "containers/lights/SpotLight.hpp"
#include "containers/lights/LightSphere.hpp"
#include "containers/TemperatureFunction.hpp"

/**
 *  @ingroup Phase
 *
 *  Fichier de lecture de fichier xml.
 */

/**
 * Verification de l'existance de l'ancien format de fichier
 *
 * @param char *File : contient le chemin complet vers le fichier a tester.
 *
 * @return 1 si le fichier existe
 */
//bool DART_PHASE_CheckOldPhaseParametersFile (string File) ;
class PhaseDAO {
private:

public:
	/**
	 * Permet de ne faire qu'une seule initialisation des fichiers pour ceux que l'on ouvre plusieurs fois.
	 */
	static void initPhase(bool fromStream = false);
	static void initXMLs(bool fromStream = false);

	/**
	 * Permet de ne faire qu'une seule fermeture des fichiers pour ceux que l'on ouvre plusieurs fois.
	 */
	static void closePhase();

	static void readSpectralBandsXML(PhaseGeneralParametersDTO & genDTO);

	static void readMaterials(PhaseGeneralParametersDTO & genDTO);

	static bool readPhaseParameters(PhaseGeneralParametersDTO & genDTO);
	static bool readPhaseParametersXML(PhaseGeneralParametersDTO & genDTO);

	static bool readAtmosphereParameters(AtmosphereParameters& atmosphereParameters);
	static bool readAtmosphereParametersXML(AtmosphereParameters& atmosphereParameters);

	static bool readIrradianceParameters(	PhaseGeneralParametersDTO & genDTO,
											std::vector<double> & irradiance,
											std::vector<double> & skyl);
	static bool readIrradianceParametersXML(PhaseGeneralParametersDTO & genDTO,
											std::vector<double> & irradiance,
											std::vector<double> & skyl);

	static bool readNumberOfOpticalPropertiesModels();
	static bool readNumberOfOpticalPropertiesModelsXML();

	static bool readCoeffDiffParametersForLambertian(	const int & index,
														std::string & phaseName,
														std::string & modelName,
														std::string & databaseName,
														double & standardDeviation,
														bool& isSpecular,
														bool& useProspectGen,
														int& lambertianDefinition,
														double & smc, double & delta,
														bool & keepOutputFile);
	static bool readCoeffDiffParametersForLambertianXML(const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName,
														double & standardDeviation,
														bool& isSpecular,
														bool& useProspectGen,
														int& lambertianDefinition,
														double & smc, double & delta,
														bool & keepOutputFile);
	static bool readCoeffDiffParametersForIsProspectUseLambertian(	const int & index,
																	bool& useProspectGen,
																	int& lambertianDefinition);
	static bool readCoeffDiffParametersForPhaseExtern(	const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName);

	static bool readCoeffDiffParametersForPhaseExternXML(	const int & index,
															string & phaseName,
															string & modelName,
															string & databaseName);


	static bool readCoeffDiffParametersForLambertianOpticalFactors(const int & index,
																OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersForFluidOpticalFactors(const int & index,
																OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersOpticalFactorsXML(	const string & key,
															const string & subNodeKey,
															OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersForFluspect(	const string & key,
													FluorescenceParameters & fluorescenceParameters);

	static bool readCoeffDiffParametersForLambertianFluspect(	const int & index,
																FluorescenceParameters & fluorescenceParameters);

	static bool readCoeffDiffParametersForSpecularLambertian(	const int & index,
																std::string & modelName,
																std::string & databaseName,
																bool & useRefraction,
																int & specularRadianceDistribution);
	static bool readCoeffDiffParametersForSpecularLambertianXML(const int & index,
																string & modelName,
																string & databaseName,
																bool & useRefraction,
																int & specularRadianceDistribution);

	static bool readCoeffDiffParametersForRPVThetaMax(const int & index,
													  double & theta_max);

	static bool readCoeffDiffParametersForRPVThetaMaxXML(const int & index,
													     double & theta_max);


	static bool readCoeffDiffParametersForRPVSpecular(	const int & index,
														string & modelName,
														string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution);
	static bool readCoeffDiffParametersForRPVSpecularXML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution);

	static bool readCoeffDiffParametersForLambertianSpecular(	const int & index,
																std::string & databaseName,
																std::string & lambertianSpecularName,
																bool & useRefraction,
																int & specularRadianceDistribution);

	static bool readCoeffDiffParametersForHapke(const int & index,
												std::string & phaseName,
												std::string & modelName,
												std::string & databaseName,
												std::string & transmittanceModelName,
												std::string & transmittanceDatabaseName,
												bool& isSpecular,
												int & indexOrder);
	static bool readCoeffDiffParametersForHapkeXML(	const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName,
													bool& isSpecular,
													int & indexOrder);

	static bool readCoeffDiffParametersForHapkeSpecular(const int & index,
														std::string & modelName,
														std::string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int & indexOrder);
	static bool readCoeffDiffParametersForHapkeSpecularXML(	const int & index,
															std::string & modelName,
															std::string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution,
															int & indexOrder);


	/*
	 * For hapke2
	 */
	static bool readCoeffDiffParametersForHapke2(const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName,
													bool& isSpecular,
													int &indexOrder);
	static bool readCoeffDiffParametersForHapke2XML(	const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName,
													bool& isSpecular,
													int & indexOrder);

	static bool readCoeffDiffParametersForHapke2Specular(const int & index,
														std::string & modelName,
														std::string & databaseName,
														bool & useRefraction,
														int & specularRadianceDistribution,
														int & indexOrder);
	static bool readCoeffDiffParametersForHapke2SpecularXML(	const int & index,
															std::string & modelName,
															std::string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution,
															int & indexOrder);
	/*
	 * read parameters of specular scatteirng function
	 * */

	static bool readCoeffDiffParametersForSpecular(	const int & index,
												std::string & phaseName,
												std::string & databaseName,
												std::string & niModelName,
												std::string & ntModelName);
	static bool readCoeffDiffParametersForSpecularXML(	const int & index,
												std::string & phaseName,
												std::string & databaseName,
												std::string & niModelName,
												std::string & ntModelName);

	static bool readCoeffDiffParametersForSpecularMaterial(const int & index, int & type, int & baseBSDF, int & baseBSDFindex);
	static bool readCoeffDiffParametersForSpecularMaterialXML(const int & index, int & type, int & baseBSDF, int & baseBSDFindex);

	static bool readCoeffDiffParametersForSpecularRoughness(const int & index, double & roughness);
	static bool readCoeffDiffParametersForSpecularRoughnessXML(const int & index, double & roughness);

	static bool readCoeffDiffParametersForSpecularTau(const int & index, double & tau);
	static bool readCoeffDiffParametersForSpecularTauXML(const int & index, double & tau);

	static bool readCoeffDiffParametersForRPV(	const int & index,
												std::string & phaseName,
												std::string & modelName,
												std::string & databaseName,
												bool& isSpecular,
												std::string & transmittanceModelName,
												std::string & transmittanceDatabaseName);
	static bool readCoeffDiffParametersForRPVXML(	const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													bool& isSpecular,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName);

	// Mixed optical properties
	static bool readCoeffDiffParametersForMixedMaterial(unsigned int modelIndex,
														std::string & phaseName,
														unsigned int & material1Type,
														unsigned int & material1Index,
														std::string & material1Name,
														unsigned int & material2Type,
														unsigned int & material2Index,
														std::string & material2Name,
														double & percentOfMaterial1);

	static bool readCoeffDiffParametersForMixedMaterialXML(unsigned int modelIndex,
														std::string & phaseName,
														unsigned int & material1Type,
														unsigned int & material1Index,
														std::string & material1Name,
														unsigned int & material2Type,
														unsigned int & material2Index,
														std::string & material2Name,
														double & percentOfMaterial1);

	static bool readCoeffDiffParametersForFoliarFluspect(	const int & index,
															FluorescenceParameters & fluorescenceParameters,
															bool & hasDifferentBottom,
															bool bottom);

	/* OUATEUR */
	static bool readCoeffDiffWaterMode(	const unsigned int & modelIndex,
										unsigned int & waterMode,
										std::string & phaseName);
	static bool readCoeffDiffParametersWaterUserDefinedForBand(	const unsigned int & modelIndex,
																const unsigned int & numBand,
																double & upwardRefractionIndex,
																double & downwardRefractionIndex);
	static bool readCoeffDiffParametersWaterFile(	const unsigned int & modelIndex,
													std::string & waterFile);
	static bool readCoeffDiffParametersWaterDatabases(	const unsigned int & modelIndex,
														std::string & databaseNameUp,
														std::string & databaseNameDown,
														std::string & databaseModelUp,
														std::string & databaseModelDown);

	static bool readParametersForTemperature(	std::vector<int> &arviIdTemperature,
												const int & avinb_leaf);

	static bool readMultiplicativeFactorLambertien(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorLambertienXML();

	static bool readMultiplicativeFactorHapke(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorHapkeXML();

	static bool readMultiplicativeFactorHapke2(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorHapke2XML();

	static bool readMultiplicativeFactorRPV(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorRPVXML();

	static bool readMultiplicativeFactorSpecular(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorSpecularXML();

	static bool readAndStoreDirections(PhaseGeneralParametersDTO & genDTO);

	static bool readAirPhaseFunctions(	std::vector<AirPhaseFunction> & airPhaseFunctions,
										const int & nb_spectral_bands);
	static bool readAirPhaseFunctionsXML(	vector<AirPhaseFunction> & airPhaseFunctions,
											const int & numberOfSpectralBands);

	static bool readSecondarySourcesOfLight(LightsContainer & lightsContainer);
	static bool readSecondarySourcesOfLightXML(LightsContainer & lightsContainer);


	static bool readDirectionalDirectTransmittanceForLambertian(unsigned int modelIndex,
																bool & useDirectionalDirectTransmittance,
																std::string & filePath,
																float & angularStep);
	static bool readDirectionalDirectTransmittanceForSpecular(	unsigned int modelIndex,
																bool & useDirectionalDirectTransmittance,
																std::string & filePath,
																float & angularStep);
	static bool readDirectionalDirectTransmittanceForHapke(	unsigned int modelIndex,
															bool & useDirectionalDirectTransmittance,
															std::string & filePath,
															float & angularStep);
	static bool readDirectionalDirectTransmittanceForHapke2(unsigned int modelIndex,
															bool & useDirectionalDirectTransmittance,
															std::string & filePath,
															float & angularStep);
	static bool readDirectionalDirectTransmittanceForRPV(unsigned int modelIndex,
															bool & useDirectionalDirectTransmittance,
															std::string & filePath,
															float & angularStep);
	static bool readDirectionalDirectTransmittance(	const std::string & nodeXMLPath,
													bool & useDirectionalDirectTransmittance,
													std::string & filePath,
													float & angularStep);

	static void readTOABOAFromFile(	const std::string & preGeneratedBoaFile, unsigned int & numberOfSpectralBands);

	static bool isOverrideBandIrradiance();
	static void readOverrideBandRadianceFile(const std::string & overrideFileParameter, vector<vector<double> > & overrideBandRadianceFileContent);

	static bool skipBOM(std::istream & in);
	static bool readPlotTxt(const std::string & fileName,
							PhaseGeneralParametersDTO & genDTO);
	static bool readLiDARXML();

private:
	static void parseOpticalMaterials(PhaseGeneralParametersDTO & genDTO);
	static void parseEmissionMaterials(PhaseGeneralParametersDTO & genDTO);
	static phsdata::DirectEmission parseDirectEmission(const string & header, int idx);
	static phsdata::IlluminationRatio parseIlluminationRatio(const string & header, int idx);
	static phsdata::EmissionMatrix parseEmissionMatrix(const string & header, int idx);
	static phsdata::EmissionPerTriangle parseEmissionPerTriangle(const string & header, int idx);
};

#endif
