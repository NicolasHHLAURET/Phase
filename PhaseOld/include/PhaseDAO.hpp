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
#include "./containers/YaMLContainer.hpp"
#include "yaml-cpp/yaml.h"
#include "GeneralDef.hpp"
#include "foliar/FoliarModel.hpp"
#include "containers/lights/LightsContainer.hpp"
#include "containers/lights/UnidirectionalLight.hpp"
#include "containers/lights/SpotLight.hpp"
#include "containers/lights/LightSphere.hpp"
#include "containers/TemperatureFunction.hpp"


#include "../../SimulationEditorInputsDAO/include/DartInputsDAO.hpp"

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
	static bool _useYaML;
	static DartInputs::DartInputsDAO _dartInputsDAO;

public:
	/**
	 * Permet de ne faire qu'une seule initialisation des fichiers pour ceux que l'on ouvre plusieurs fois.
	 */
	static void initPhase(bool fromStream = false);
	static void initXMLs(bool fromStream = false);
	static void initYaMLs();

	/**
	 * Permet de ne faire qu'une seule fermeture des fichiers pour ceux que l'on ouvre plusieurs fois.
	 */
	static void closePhase();

	/**
	 * Lecture des parametres de Phase
	 *
	 * @param int arbrecomputeAll 			: booleen a 1 si phase doit tout recalculer
	 * @param int numberOfSpectralBands 			: nombre de bandes spectrales.
	 * @param std::vector<double> lowerBandWavelength 	: vecteur contenant les valeurs des bandes spectrales minimum.
	 * @param std::vector<double> upperBandWavelength 	: vecteur contenant les valeurs des bandes spectrales maximales.
	 * @param double dAS 					: angle azimuthal solaire
	 * @param double dZS 					: angle zenithal solaire
	 * @param string sfich_angle 		: fichier contenant toutes directions.
	 * @param int MonteCarlo 				: 1 si la simulation tourne en mode Monte Carlo.
	 * @param int Specul 					: 1 si effet speculaire demande.
	 *
	 * @return 1 si okay
	 */
	static bool readPhaseParameters(PhaseGeneralParametersDTO & genDTO);
	static bool readPhaseParametersXML(PhaseGeneralParametersDTO & genDTO);
	static bool readPhaseParametersYaML(PhaseGeneralParametersDTO & genDTO);

	/**
	 * Lecture des param�tres de l'atmosphere dans Phase
	 *
	 * @param unsigned int aruflag 			: 1 si l'atmosphere a deja ete calcule ; 0 si on doit la calculer ; -1 s'il n'y a pas d'atmosphere.
	 * @param unsigned int arudata 			: 1 si fichier de la database, 0 sinon.
	 * @param int 	&aruMode,			: si Rt dans BA : 1 si sans database, 0 si avec database
	 * @param int   &aruRTinBA,			: 1 si RT dans BA
	 * unsigned int &arusSpectralDomain		: domaine spectral: mode T ou R ou T+R en mono
	 * @param string arsFichData 	: nom du fichier de la database.
	 * @param string arsFichData 	: nom du fichier de profil de temperature.
	 * @param double ardlambda_max 		: borne superieure de l'intervalle spectral.
	 * @param double ardlambda_min 		: borne inferieure de l'intervalle spectral.
	 * @param unsigned int aruiatmosphericModel 	: type de modele atmospherique.
	 * @param unsigned int aruiH_model 		: modele des facteurs d'echelle
	 * @param unsigned int aruiozonModel 	: modele pour l'ozone
	 * @param unsigned int aruitemperatureModel 	: modele pour la distribution des temperatures.
	 * @param unsigned int aruivaporModel 		: modele pour la vapeur d'eau
	 * @param double ardd 				: quantite de vapeur d'eau precipitable
	 * @param unsigned int aruiaerosolModel 		: modele pour les aerosols.
	 * @param double ardEs 				: constante solaire.
	 * @param double ardH_p 			: facteur d'echelle pour les gazs
	 * @param double ardtau_diff_m 		: epaisseur optique pour les gazs
	 * @param double ardtau_diff_p 		: epaisseur optique pour les aerosols
	 * @param double ardomega_p 		: albedo pour les aerosols
	 * @param double ardg1_p 			: Parametre de la fonction de phase de Henyey Greenstein (entre 0 et 1). Diffusion vers l'avant
	 * @param double ardg2_p 			: Parametre de la fonction de phase de Henyey Greenstein (entre 0 et 1). Retrodiffusion
	 * @param double arda_p 			: facteur multiplicatif de la fonction de phase de Henyey Greenstein
	 * @param double ardH_m 			: facteur d'echelle pour les aerosols
	 * @param double ardTg_total 		: transmittance totale pour les gazs
	 * @param double ardhSensor 		: altitude du capteur
	 *
	 * @return 1 si okay
	 */
	static bool readAtmosphereParameters(AtmosphereParameters& atmosphereParameters);
	static bool readAtmosphereParametersXML(AtmosphereParameters& atmosphereParameters);
	static bool readAtmosphereParametersYaML(AtmosphereParameters& atmosphereParameters);

	static bool readIrradianceParameters(	PhaseGeneralParametersDTO & genDTO,
											std::vector<double> & irradiance,
											std::vector<double> & skyl);
	static bool readIrradianceParametersXML(PhaseGeneralParametersDTO & genDTO,
											std::vector<double> & irradiance,
											std::vector<double> & skyl);
	static bool readIrradianceParametersYaML(	PhaseGeneralParametersDTO & genDTO,
												std::vector<double> & irradiance,
												std::vector<double> & skyl);

	/**
	 * Lecture du nombre de fonctions de phase dans coeff_diff (mode multispectral)
	 *
	 * @param int nb_lambertien 			: nombre de fonctions lambertiennes.
	 * @param int nb_lambertien_specular : nombre de fonctions lambertienne-speculaires.
	 * @param int nb_hapke_specular		: nombre de fonctions hapke-speculaire.
	 * @param int nb_phase 				: nombre de fonctions de phase.
	 * @param int nb_leaf 				: nombre de fonctions pour les leafs.
	 *
	 * @return 1 si okay
	 */
	static bool readNumberOfOpticalPropertiesModels();
	static bool readNumberOfOpticalPropertiesModelsXML();
	static bool readNumberOfOpticalPropertiesModelsYaML();

	/**
	 * Lecture des parametres des fonctions de phase lambertiennes dans coeff_diff (mode multispectral)
	 *
	 * @param int index 					: numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_lamb 	: nom de la fonction de phase lambertienne.
	 * @param double ardecart_type 		: valeur de l'ecart type.
	 *
	 * @return 1 si okay
	 */
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
	static bool readCoeffDiffParametersForLambertianYaML(	const int & index,
															string & phaseName,
															string & modelName,
															string & databaseName,
															double & standardDeviation,
															bool& isSpecular);
	static bool readCoeffDiffParametersForIsProspectUseLambertian(	const int & index,
																	bool& useProspectGen,
																	int& lambertianDefinition);
	static bool readCoeffDiffParametersForIsProspectUseFoliar(	const int & index,
																bool& useProspectGen,
																bool& bottomProspectGen,
																bool& hasDifferentModelForBottom);

	static bool readCoeffDiffParametersForPhaseExtern(	const int & index,
														string & phaseName,
														string & modelName,
														string & databaseName);

	static bool readCoeffDiffParametersForPhaseExternXML(	const int & index,
															string & phaseName,
															string & modelName,
															string & databaseName);

	static bool readCoeffDiffParametersForPhaseExternYaML(	const int & index,
															string & phaseName,
															string & modelName,
															string & databaseName);

	static bool readCoeffDiffParametersForLambertianOpticalFactors(const int & index,
																OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersForFoliarOpticalFactors(const int & index,
																OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersForFluidOpticalFactors(const int & index,
																OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersOpticalFactorsXML(	const string & key,
															const string & subNodeKey,
															OpticalFactorParameters & opticalFactorParameters);
	static bool readCoeffDiffParametersOpticalFactorsYaML(const int & index,
																		const int & TYPE,
																OpticalFactorParameters & opticalFactorParameters);

	static bool readCoeffDiffParametersForProspect(		const int & modelIndex,
															const int & prospectFunction,
															ProspectParameters & prospectParameters);
	static bool readCoeffDiffParametersForProspectXML(	const std::string & prefixKey,
														ProspectParameters & prospectParameters);

	static bool readCoeffDiffParametersForFluspect(	const string & key,
													FluorescenceParameters & fluorescenceParameters);

	static bool readCoeffDiffParameterForFluspectYaML(	const int & index,
														const int & TYPE,
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
	static bool readCoeffDiffParametersForSpecularLambertianYaML(	const int & index,
																	string & modelName,
																	string & databaseName,
																	bool & useRefraction,
																	int & specularRadianceDistribution);

	static bool readCoeffDiffParametersForRPVThetaMax(const int & index,
													  double & theta_max);

	static bool readCoeffDiffParametersForRPVThetaMaxXML(const int & index,
													     double & theta_max);

	static bool readCoeffDiffParametersForRPVThetaMaxYaML(const int & index,
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
	static bool readCoeffDiffParametersForRPVSpecularYaML(	const int & index,
															string & modelName,
															string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution);
	/**
	 * Lecture des parametres des fonctions de phase "Lambertien + Speculaire" dans coeff_diff (mode multispectral).
	 *
	 * @param int index 						: numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_lamb_spec 	: nom de la fonction de phase lambertienne-speculaire.
	 *
	 * @return 1 si okay.
	 */
	static bool readCoeffDiffParametersForLambertianSpecular(	const int & index,
																std::string & databaseName,
																std::string & lambertianSpecularName,
																bool & useRefraction,
																int & specularRadianceDistribution);

	/**
	 * Lecture des parametres des fonctions de phase de type "Hapke + Speculaire" dans coeff_diff (mode multispectral).
	 *
	 * @param int index        			: numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_hapke 	: nom de la fonction de phase hapke.
	 *
	 * @return 1 si okay.
	 */
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
	static bool readCoeffDiffParametersForHapkeYaML(const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName,
													bool& isSpecular);

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
	static bool readCoeffDiffParametersForHapkeSpecularYaML(const int & index,
															std::string & modelName,
															std::string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution);


	/*
	 * Foir hapke2
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
	static bool readCoeffDiffParametersForHapke2YaML(const int & index,
													std::string & phaseName,
													std::string & modelName,
													std::string & databaseName,
													std::string & transmittanceModelName,
													std::string & transmittanceDatabaseName,
													bool& isSpecular);

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
	static bool readCoeffDiffParametersForHapke2SpecularYaML(const int & index,
															std::string & modelName,
															std::string & databaseName,
															bool & useRefraction,
															int & specularRadianceDistribution);
	/* read parameters of specular scatteirng function
	 *
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
	static bool readCoeffDiffParametersForSpecularYaML(	const int & index,
												std::string & phaseName,
												std::string & databaseName,
												std::string & niModelName,
												std::string & ntModelName);

	static bool readCoeffDiffParametersForSpecularMaterial(const int & index, int & type, int & baseBSDF, int & baseBSDFindex);
	static bool readCoeffDiffParametersForSpecularMaterialXML(const int & index, int & type, int & baseBSDF, int & baseBSDFindex);
	static bool readCoeffDiffParametersForSpecularMaterialYaML(const int & index, int & type, int & baseBSDF, int & baseBSDFindex);

	static bool readCoeffDiffParametersForSpecularRoughness(const int & index, double & roughness);
	static bool readCoeffDiffParametersForSpecularRoughnessXML(const int & index, double & roughness);
	static bool readCoeffDiffParametersForSpecularRoughnessYaML(const int & index, double & roughness);

	static bool readCoeffDiffParametersForSpecularTau(const int & index, double & tau);
	static bool readCoeffDiffParametersForSpecularTauXML(const int & index, double & tau);
	static bool readCoeffDiffParametersForSpecularTauYaML(const int & index, double & tau);

	/**
	 * Lecture des parametres des fonctions phase externe dans coeff_diff (mode multispectral)
	 *
	 * @param int index        			: numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_phase 	: nom de la fonction de phase.
	 * @param int aritype_diff_phase 	: type de diffusion ("Lambertien + Speculaire" ou "Hapke + speculaire")
	 *
	 * @return 1 si okay.
	 */
	//bool DART_PHASE_ReadCoeffDiffParametersPhaseMulti (const int & index, string & namephase, int &aritype_diff_phase) ;
	/**
	 * Lecture des parametres des fonctions phase RPV dans coeff_diff (mode multispectral)
	 *
	 * @param int index        			: numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_phase 	: nom de la fonction de phase.
	 * @return 1 si okay.
	 */
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
	static bool readCoeffDiffParametersForRPVYaML(	const int & index,
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

	static bool readCoeffDiffParametersForMixedMaterialYaML(unsigned int modelIndex,
														std::string & phaseName,
														unsigned int & material1Type,
														unsigned int & material1Index,
														std::string & material1Name,
														unsigned int & material2Type,
														unsigned int & material2Index,
														std::string & material2Name,
														double & percentOfMaterial1);

	/**
	 * Lecture des parametres des fonctions de phase turbide dans coeff_diff (mode multispectral).
	 *
	 * @param int index 				 : numero de la fonction de phase en cours de traitement.
	 * @param string arsnom_leaf : nom de la fonction de phase pour les leafs.
	 * @param int arinum_LAD 		 : valeur du typeOfLeafAngleDistribution.
	 * @param double &ardomegaMin 	 : parametre du clumping index
	 * @param double &ardomegaMax 	 : parametre du clumping index
	 * @param double &ardclumpingA 	 : parametre du clumping index
	 * @param double &ardclumpingB 	 : parametre du clumping index
	 * @param double ardALA           : Angle foliaire moyen pour la distribution angulaire moyenne ellipsoidale (�)
	 * @param double ardThetam        : angle de l'axe principal de l'ellipse (� typeOfLeafAngleDistribution elliptique)
	 * @param double ardEccentricity  : excentricite de la distribution angulaire foliaire elliptique
	 *
	 * @return 1 si okay.
	 */
	static bool readCoeffDiffParametersForFoliar(	const unsigned int & foliarModelIndice,
													double &m,
													double &b0,
													double &b1,
													double &b2,
													std::string& nameDatabaseLAD,
													FoliarModel & foliarModel);
	static bool readCoeffDiffParametersForFoliarXML(const unsigned int & foliarModelIndice,
													double &m,
													double &b0,
													double &b1,
													double &b2,
													std::string& nameDatabaseLAD,
													FoliarModel & foliarModel);
	static bool readFoliarMultiplicativeFactorForModelXML(	const string & temp,
															const string & modelKey,
															const bool & isBottomFace,
															const unsigned int & modelIndex,
															FoliarModel & foliarModel);
	static bool readCoeffDiffParametersForFoliarYaML(	const unsigned int & foliarModelIndice,
														double &m,
														double &b0,
														double &b1,
														double &b2,
														std::string& nameDatabaseLAD,
														FoliarModel & foliarModel);

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

	static bool readCoeffDiffParametersForFoliarDatabases(	const unsigned int & modelIndex,
															std::string & phaseName,
															std::string & modelName,
															std::string & databaseName,
															bool & isSpecular,
															bool & useProspectGen,
															bool & hasDifferentBottom,
															std::string & specularDatabaseName,
															std::string & specularDatabaseModel,
															std::string & bottomDatabase,
															std::string & bottomModel,
															bool & bottomProspectGen);
	static bool readCoeffDiffParametersForFoliarDatabasesXML(	const unsigned int & modelIndex,
																std::string & phaseName,
																std::string & modelName,
																std::string & databaseName,
																bool & isSpecular,
																bool & useProspectGen,
																bool & hasDifferentBottom,
																std::string & specularDatabaseName,
																std::string & specularDatabaseModel,
																std::string & bottomDatabase,
																std::string & bottomModel,
																bool & bottomProspectGen);
	static bool readCoeffDiffParametersForFoliarDatabasesYaML(	const unsigned int & modelIndex,
																std::string & phaseName,
																std::string & modelName,
																std::string & databaseName,
																bool & isSpecular,
																std::string & specularDatabaseName,
																std::string & specularDatabaseModel);

	/**
	 * Lecture des temperatures dans coeff_diff
	 *
	 * @param int avimode 		 			 : mode MONOSPECTRAL ou MULTISPECTRAL.
	 * @param double ardTminSoil  			 : temperature minimal du sol.
	 * @param double ardTmaxSoil  			 : temperature maximal du sol.
	 * @param double ardTminWater 			 : temperature minimal de l'eau.
	 * @param double ardTmaxWater 			 : temperature maximal de l'eau.
	 * @param double ardTminRoad  			 : temperature minimal de la route.
	 * @param double ardTmaxRoad  			 : temperature maximal de la route.
	 * @param double ardTminPlot_opaque  	 : temperature minimal des plots opaques.
	 * @param double ardTmaxPlot_opaque  	 : temperature maximal des plots opaques.
	 * @param double ardTminRoof 			 : temperature minimal pour les toits.
	 * @param double ardTmaxRoof 			 : temperature maximal pour les toits.
	 * @param double ardTminWall 			 : temperature minimal pour les murs.
	 * @param double ardTmaxWall 			 : temperature maximal pour les murs.
	 * @param double ardTminTrunk 			 : temperature minimal pour les troncs.
	 * @param double ardTmaxTrunk 			 : temperature maxnimal pour les troncs.
	 * @param double ardTminMaket 			 : temperature minimal de la maquette.
	 * @param std::vector<double> ardTminLeaf : temperature minimal pour les leafs.
	 * @param std::vector<double> ardTmaxLeaf : temperature minimal pour les leafs.
	 * @param int avinb_leaf 				 : nombre de fonctions de phase turbide
	 *
	 * @return 1 si okay.
	 */

	static bool readParametersForTemperature(	std::vector<int> &arviIdTemperature,
												const int & avinb_leaf);

	static bool readMultiplicativeFactorLambertien(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorLambertienXML();
	static bool readMultiplicativeFactorLambertienYaML(const PhaseGeneralParametersDTO & genDTO);

	static bool readMultiplicativeFactorHapke(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorHapkeXML();
	static bool readMultiplicativeFactorHapkeYaML(const PhaseGeneralParametersDTO & genDTO);

	static bool readMultiplicativeFactorHapke2(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorHapke2XML();
	static bool readMultiplicativeFactorHapke2YaML(const PhaseGeneralParametersDTO & genDTO);

	static bool readMultiplicativeFactorRPV(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorRPVXML();
	static bool readMultiplicativeFactorRPVYaML(const PhaseGeneralParametersDTO & genDTO);

	static bool readMultiplicativeFactorSpecular(const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorSpecularXML();
	static bool readMultiplicativeFactorSpecularYaML(const PhaseGeneralParametersDTO & genDTO);

	static bool readMultiplicativeFactorFoliar(	int & iterationStepPhi,
												int & iterationStepTheta,
												FoliarModel & foliarModel,
												const PhaseGeneralParametersDTO & genDTO);
	static bool readMultiplicativeFactorFoliarXML(	int & iterationStepPhi,
													int & iterationStepTheta,
													FoliarModel & foliarModel);
	static bool readMultiplicativeFactorFoliarYaML(	int & iterationStepPhi,
													int & iterationStepTheta,
													FoliarModel & foliarModel,
													const PhaseGeneralParametersDTO & genDTO);

	static bool readAndStoreDirections(std::vector<DirectionCartesienne> & listDirections);

	static bool readAirPhaseFunctions(	std::vector<AirPhaseFunction> & airPhaseFunctions,
										const int & nb_spectral_bands);
	static bool readAirPhaseFunctionsXML(	vector<AirPhaseFunction> & airPhaseFunctions,
											const int & numberOfSpectralBands);
	static bool readAirPhaseFunctionsYaML(	vector<AirPhaseFunction> & airPhaseFunctions,
											const int & numberOfSpectralBands);

	static bool readSecondarySourcesOfLight(LightsContainer & lightsContainer);
	static bool readSecondarySourcesOfLightXML(LightsContainer & lightsContainer);
	static bool readSecondarySourcesOfLightYAML(LightsContainer & lightsContainer);

	static bool readTemperatureProperties(std::vector<TemperatureFunction> & temperatureFunctions);
	static bool readTemperaturePropertiesXML(std::vector<TemperatureFunction> & temperatureFunctions);
	static bool readTemperaturePropertiesYAML(std::vector<TemperatureFunction> & temperatureFunctions);

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
	static bool useYaMl() {
		return _useYaML;
	}
	
	static void setYaMlUsage(bool useYaMl) {
		_useYaML = useYaMl;
	}

	static const DartInputs::DartInputsDAO & getInputsDAO() {
		return _dartInputsDAO;
	}
};

#endif
