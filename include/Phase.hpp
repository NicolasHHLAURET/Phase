/*! Time-stamp: <@(#) Phase.hapkeParameterh   10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : Phase.hapkeParameterh
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description : Module Phase.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.4.0 21/04/2008  MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture/ecriture)
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0  07/03/2005  VBT : First Revision
 *
 *********************************************************************
 */

/************************************************************/
/*                                                          */
/*        MODELISATION D'UN MILIEU NATUREL FORESTIER        */
/*        __________________________________________        */
/*                                                          */
/*    CALCUL DES CARACTERISTIQUES OPTIQUES DES ELEMENTS     */
/*                                                          */
/*      Version  4.4.0 compatible UNIX/LINUX/WINDOWS        */
/*                                                          */
/*          Derniere Modification : 27/06/2008              */
/*                                                          */
/************************************************************/

#ifndef _PHASE
#define _PHASE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include "libDartBase.h"
#include "libDartEnv.h"
#include "../../librairieDart/include/databases/optical/LireDatabase2D.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include "../../libSOCI/src/core/soci.h"
#include "../../libSOCI/src/backends/sqlite3/soci-sqlite3.h"

#include "containers/DirectionCartesienne.hpp"
#include "containers/air/AirPhaseFunction.hpp"

#include "../../libDartGeometry/include/Vecteur.hpp"

#include "../../librairieDart/include/GestOption.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/filesystem.hpp>

// Generation an unique id for databases
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "../include/PhaseDAO.hpp"
#include "PhaseProperty.hpp"
#include "PhasePolarization.hpp"
#include "PhaseDTOs.hpp"

#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/SpecularDatabaseParameters.hpp"
#include "database/model/LambertianDatabaseParameters.hpp"
#include "database/model/PhaseTemperatureDatabaseParameters.hpp"
#include "database/model/HapkeDatabaseParameters.hpp"
#include "database/model/RpvDatabaseParameters.hpp"
#include "database/model/RefractiveIndexDatabaseParameters.hpp"
#include "database/model/TransmittanceDatabaseParameters.hpp"
#include "database/model/WaterDatabaseParameters.hpp"
#include "database/model/WaterNoDatabaseUserDefinedParameters.hpp"
#include "database/model/WaterNoDatabaseFileParameters.hpp"
#include "database/model/FoliarDatabaseParameters.hpp"
#include "database/model/SpecularFoliarDatabaseParameters.hpp"
#include "database/model/PhaseExternDatabaseParameters.hpp"

#include "database/model/PhaseTemperatureDatabaseParameters.hpp"
#include "database/model/FoliarTemperatureDatabaseParameters.hpp"

#include "foliar/FoliarModel.hpp"
#include "foliar/FoliarComputation.hpp"
#include "foliar/LADComputation.hpp"

#include "containers/lights/LightsContainer.hpp"

#include "phaseExtern/PhaseExternComputation.hpp"
#include "phaseExtern/MarmitComputation.hpp"
#include "phaseExtern/ProspectComputation.hpp"

extern unsint SpectralDomain;
extern int RAMI;

using namespace soci;

//Netcdf
#include "../../librairieDart/include/namespaces/netcdf/AtmosphereNcKeys.hpp"
#include "../../librairieDart/include/netcdf/DartNetcdf.hpp"

#include <netcdf>
using namespace netCDF;
using namespace AtmosphereNcKeys;

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

/**
 *  @defgroup Phase
 */

// Launching of the appropriate function. It's depend on the parameters of ths files.
void execute_batchPhase(PhaseGeneralParametersDTO & genDTO,
						std::string const& fileNameProperty);

// Delete all the dynamic variable.
void free_var();

// Get the value of the angle teta and phi.
int read_teta_phi_domega(	int &,
							double &,
							double &,
							std::string &,
							std::vector<int> &,
							int &,
							std::vector<Vecteur<double> > & tab_dir_vect);

// Sum of all the solid angle for all the directions except solar direction.
int Initialisations(int &,
					double &,
					double &,
					double &,
					std::string &,
					std::vector<int> &,
					int &,
					std::vector<Vecteur<double> > & tab_dir_vect);

// Allocate dynamic variable we need.
void resizePhaseParametersVectors(	ReflectanceModelsParameters& reflectanceModelsParameters,
									std::vector<MultiplicativeFactorForRpv>& parametersForRPVModels,
									std::vector<FoliarModel::ParametersForFoliarModels>& parametersForFoliarModels,
									std::vector<MultiplicativeFactorForHapke>& parametersForHapkeModels,
									std::vector<MultiplicativeFactorForHapke2>& parametersForHapke2Models,
									std::vector<MultiplicativeFactorForLambertien>& parametersForLambertianModels);

// Initialize PDF array with typeOfLeafAngleDistribution array for a specific foliar distribution.
/*
 int initialisationLadProbabilityDensityFunctions(	const int & NumLAD,
 const std::vector<std::vector<double> > & typeOfLeafAngleDistribution,
 std::vector<std::vector<double> > & PDF);
 */
//int initialisationLadProbabilityDensityFunctions(	std::vector<double> & LAD,
//													std::vector<std::vector<double> > & PDF);
// MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture/ecriture)
/**
 *Traitement_Multispectral
 *
 * Cette fonction permet d'interpoler (methode du trapreze) sur chaque intervalle spectral [lowerBandWavelength[q];upperBandWavelength[q]] tous
 * les parametres issus des fichiers de la base de donnees eux memes tires de la base de donnees ASTER (NASA) pour
 * chaque type de modelisation specifie dans phase.txt.
 * Chaque fichier possede les valeurs des proprietes optiques (utilisables) des materiaux sur l'intervalle lambda_min_data
 * et lambda_max_data.
 * Cette fonction permet de traiter plusieurs cas particuliers :
 *	-  Plusieurs valeurs successives des lambda peuvent etre egales
 *	-  Dans un meme fichier la taille des differents intervalles spectraux peut varier
 *	-  lowerBandWavelength[q] et/ou upperBandWavelength[q] = lambda_min_data et/ou lambda_max_data
 *
 * @param string Comm			: nom de fichier de la base de donnees
 * @param int numero_intervalle	: index de l'intervalle spectral
 * @param type_modelisation		: type de fonction de phase
 * @param OpticalProperties *optique		: structure permettant de gerer les parametres des bases de donnees
 * @param double *lowerBandWavelength			: tableau des bornes inferieures de chaque intervalle
 * @param double *upperBandWavelength			: tableau des bornes superieures de chaque intervalle
 * @param int nb_dir 			: nombre de directions discretes
 * @param vecteur *tab_dir_vect 	: tableau des vecteurs directeurs des directions discretes
 * @param int typeOfLeafAngleDistribution			: type de typeOfLeafAngleDistribution
 * @param double foliarDimension		: dimension des feuilles
 *@param std::vector<int> & lambdaDomain :    tableau du mode (R,R+T ou T) de chaque intervalle
 */
void processingAtmosphere(	PhaseGeneralParametersDTO & genDTO,
							PhaseKeyMappingGenerator & pkmg,
							std::ofstream & outCoeffSpecProperties,
							std::vector<AirPhaseFunction> & airPhaseFunctions,
							int& dayOfTheYear,
							const std::vector<DirectionCartesienne> & listDirections);
void executeAtmosphereWithDatabase(	PhaseGeneralParametersDTO & genDTO,
									AtmosphereParameters& atmosphereParameters,
									AirPhaseFunction & moleculeAFP,
									vector<AirPhaseFunction> & aerosolAFP,
									const vector<DirectionCartesienne> & listDirections);

void writeAtmosphereWithDatabase(	PhaseGeneralParametersDTO & genDTO,
									AtmosphereParameters& atmosphereParameters,
									AirPhaseFunction & moleculeAFP,
									vector<AirPhaseFunction> & aerosolAFP,
									const vector<DirectionCartesienne> & listDirections);

void writeAtmosphereNcWithDatabase(	const AtmosphereParameters& atmosphereParameters,
		vector<AtmosphereComposition>& atmospherePerBandMeanValues,
		const std::vector<std::vector<HenyeyGreensteinParameters> > &henleyGreensteinMeanParameters);

void executeAtmosphereNcWithoutDatabase(	AtmosphereParameters& atmosphereParameters,
										AirPhaseFunction & moleculeAFP,
										AirPhaseFunction & aerosolAFP,
										const vector<DirectionCartesienne> & listDirections);

int main(	int argc,
			char *argv[]);
int run(int argc,
		char *argv[]);

void removePreviousPhaseKeys(FichierProperties & luxCoreRenderSceneProperties);
void reorganiseLightsId(PhaseGeneralParametersDTO & genDTO, FichierProperties & luxCoreLightProperties);
unsigned int getLightIDFromType(PhaseGeneralParametersDTO & genDTO, unsigned int lightTypeId, const std::string & lightName);
/* Databases */
std::vector<OpticalPropertyParameters> createDatabasesReadingProperties(const unsigned int & numberOfBands);

void prepareProspect(PhaseGeneralParametersDTO & genDTO);
void processSurfaceMaterials(const PhaseGeneralParametersDTO & genDTO);

/* Specular */
void computeSpecular(	SpecularData & specularProperty,
						const double & specularIntensity,
						const unsigned int & spectralBandIndex,
						shared_ptr<SpecularDatabaseParameters> & specularDatabaseParameters);
void writeSpecular(	SpecularData & specularProperty,
					std::ofstream & outCoeffSpecProperties,
					PhaseKeyMappingGenerator & pkmg);
void treatSpecular(	OpticalPropertyParameters & specularPropertyParameters,
					unsigned int opIndex,
						std::vector<int> & useSpectralIrradianceForReflectance,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties,
						PhaseGeneralParametersDTO & genDTO,
						const std::vector<DirectionCartesienne> & listDirections);
void computeOpticalPropertiesSpecular(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & specularPropertyParameters,
										const std::vector<DirectionCartesienne> & listDirections,
										PropertySpecular & specularProperty,
										double & albedo,
										double & dTauMoy);

void processSurfaceMaterials(const PhaseGeneralParametersDTO & genDTO,  PhaseKeyMappingGenerator & pkmg, ofstream & outCoeffSpecProperties);


void writeFluorescenceToSimulationProperties(	const FluorescenceResults & fluorescenceResults,
												PhaseKeyMappingGenerator & pkmg,
												ofstream & outCoeffSpecProperties);
void computeFluorescenceMatrices(	FluorescenceResults & fluorescenceResults,
									vector<vector<vector<double> > > & interpolatedMatrices,
									const int & typeOfReflectance,
									unsigned int modelIndex,
									const FluorescenceParameters & fluorescence,
									PhaseGeneralParametersDTO & genDTO,
									unsigned int face);

void interpolateAndWriteInFluorescenceMatrixFile(	fstream & ofstreamFluorescenceMatrices,
													const string & fluspectOutputFile,
													vector<vector<vector<double> > > & fluspectMatrices,
													vector<vector<vector<double> > > & interpolatedMatrices,
													PhaseGeneralParametersDTO & genDTO,
													unsigned int wlEMin,
													unsigned int wlEMax,
													unsigned int wlFMin,
													unsigned int wlFMax,
													const bool & useCombinedYields);
void computeOpticalPropertiesLambertian(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & lambertianPropertyParameters,
										const std::vector<DirectionCartesienne> & listDirections,
										PropertyLambertian & lambertianProperty,
										double & dTauMoy,
										double & albedo);

void writeLambertian(	const unsigned int& modelIndex,
						const std::string & modelName,
						const unsigned int& spectralBandIndex,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties,
						PropertyLambertian & lambertianProperty,
						const double & dTauMoy,
						const double & albedo,
						const std::vector<DirectionCartesienne> & listDirections);

/* Phase Extern */
void treatPhaseExtern(OpticalPropertyParameters & PhaseExternPropertyParameters,
						vector<int> & useSpectralIrradianceForReflectance,
						PhaseKeyMappingGenerator & pkmg,
						ofstream & outCoeffSpecProperties,
						PhaseGeneralParametersDTO & genDTO,
						const vector<DirectionCartesienne> & listDirections);

void computeOpticalPropertiesPhaseExtern(const unsigned int& spectralBandIndex,
										OpticalPropertyParameters & PhaseExternPropertyParameters,
										const vector<DirectionCartesienne> & listDirections,
										PhaseExternProperty & PhaseExternProperty) ;

void writeTreatPhaseExtern(	ofstream & outCoeffSpecProperties,
					PhaseKeyMappingGenerator & pkmg,
					PhaseExternProperty & PhaseExternProperty,
					const string & modelName,
					const unsigned int & spectralBandIndex,
					const unsigned int & modelIndex);

/* LiDAR*/
void readLiDAR();

/* Hapke */
void treatHapke(OpticalPropertyParameters & hapkePropertyParameters,
				unsigned int opIndex,
				std::vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<DirectionCartesienne> & listDirections);
void computeAlbedodTauMoyHapke(	const PropertyHapke & opticalProperties,
								double & albedo,
								double & dTauMoy,
								const std::vector<DirectionCartesienne> & listDirections);
void computeOpticalPropertiesHapke(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & hapkePropertyParameters,
									const std::vector<DirectionCartesienne> & listDirections,
									PropertyHapke & hapkeProperty);
void testHapkeAlbedo(	const unsigned int& modelIndex,
						PropertyHapke & opticalProperties);
void writeTreatHapke(	std::ofstream & outCoeffSpecProperties,
						PhaseKeyMappingGenerator & pkmg,
						PropertyHapke & hapkeProperty,
						const std::string & modelName,
						const unsigned int & spectralBandIndex,
						const unsigned int & modelIndex,
						const double & albedo,
						const double & dTauMoy);

/* Hapke2 */
void treatHapke2(OpticalPropertyParameters & hapke2PropertyParameters,
					unsigned int opIndex,
				std::vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<DirectionCartesienne> & listDirections);
void computeAlbedodTauMoyHapke2(	const PropertyHapke2 & opticalProperties,
								double & albedo,
								double & dTauMoy,
								const std::vector<DirectionCartesienne> & listDirections);
void computeOpticalPropertiesHapke2(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & hapke2PropertyParameters,
									const std::vector<DirectionCartesienne> & listDirections,
									PropertyHapke2 & hapke2Property);
void testHapke2Albedo(	const unsigned int& modelIndex,
						PropertyHapke2 & opticalProperties);
void writeTreatHapke2(	std::ofstream & outCoeffSpecProperties,
						PhaseKeyMappingGenerator & pkmg,
						PropertyHapke2 & hapkeProperty2,
						const std::string & modelName,
						const unsigned int & spectralBandIndex,
						const unsigned int & modelIndex,
						const double & albedo,
						const double & dTauMoy);

/* RPV */
void treatRPV(	OpticalPropertyParameters & rpvPropertyParameters,
				unsigned int opIndex,
				std::vector<int> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<DirectionCartesienne> & listDirections);
void computeOpticalPropertiesRPV(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & rpvPropertyParameters,
									const std::vector<DirectionCartesienne> & listDirections,
									PropertyRpv & rpvProperty,
									double & dTauMoy);
void testRPVAlbedo(	const unsigned int& modelIndex,
					PropertyRpv & opticalProperties);
void writeRPV(	PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties,
				PropertyRpv & rpvProperty,
				const std::string & modelName,
				const double & dTauMoy,
				const unsigned int& modelIndex,
				const unsigned int& spectralBandIndex);

void treatMixedMaterial(PhaseGeneralParametersDTO & genDTO,	unsigned int opIndex, unsigned int pfIndex);
std::string getMaterialName(unsigned int modelType, unsigned int modelIndex);

/* Foliar */
void processVegetationVolumeMaterials(PhaseGeneralParametersDTO & genDTO);
void computeLadData(const PhaseGeneralParametersDTO & genDTO,
					LADComputation & foliarComputer,
					FoliarModel & foliarModel);

void treatFoliar(	OpticalPropertyParameters & foliarPropertyParameters,
					std::vector<int> & useSpectralIrradianceForReflectance,
					PhaseKeyMappingGenerator & pkmg,
					std::ofstream & outCoeffSpecProperties,
					PhaseGeneralParametersDTO & genDTO,
					const std::vector<DirectionCartesienne> & listDirections,
					FTFoliar & fTFoliar,
					PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
					PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
					const SphereDiscretizationMap & sphereMap,
					FoliarComputation & foliarComputer);

void computeInverceMatricesForFoliar(	const unsigned int & modelIndex,
								     	const unsigned short int & face,
										const unsigned int & nbSpectralBands,
										const std::ostringstream & valueKr,
										const std::ostringstream & valueKt,
										const double & averageEta);

void readBioClimaticFile( 	string bcFilePath,
							vector<double> & bioClimaticThetaValues,
							vector<double> & bioClimaticPhiValues,
							vector<vector<vector<double> > > & bioClimaticProfileEta);

void computeAverageEta(	const vector<double> & bioClimaticThetaValues, const vector<double> & bioClimaticPhiValues,
					  	const vector<vector<vector<double>>> & bioClimaticProfileEta, double & average);

void computeAndWriteAverageBioClimaticProfile(	const unsigned int & modelIndex,
												const FoliarComputation & foliarCompute,
												const vector<double> & bioClimaticThetaValues,
												const vector<double> & bioClimaticPhiValues,
												const vector<vector<vector<double> > > & bioClimaticProfileEta,
												ofstream & ofstreamAverageEta);


void computeFoliarSpecular(	SpecularFoliarData & specularProperty,
							const unsigned int & spectralBandIndex,
							shared_ptr<SpecularFoliarDatabaseParameters> & foliarSpecularDatabaseParameters);

void writeFoliar(	const unsigned int& modelIndex,
					const unsigned int& spectralBandIndex,
					const std::string & modelName,
					PhaseKeyMappingGenerator & pkmg,
					std::ofstream & outCoeffSpecProperties,
					std::vector<FoliarProperty> & opticalPropertiesVector,
					FoliarComputation & foliarComputer);

void writeSecondarySourcesOfLight(PhaseGeneralParametersDTO & genDTO, ofstream & outCoeffSpecProperties, const LightsContainer & lightsContainer);

//void writeTemperatureProperties(PhaseGeneralParametersDTO & genDTO, ofstream & outCoeffSpecProperties, const std::vector<TemperatureFunction> & temperatureFunctions);

void processEmissionMaterials(PhaseGeneralParametersDTO & genDTO);

double computeLambdaEquivalent(const double &lambdaMin, const double &lambdaMax, const double &meanTemperature);
#endif // _PHASE
