/*! Time-stamp: <@(#) GeneralDef.hpp   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : GeneralDef.hpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 30/03/2005
 *
 *  Description : Contient les variables communes aux modules DART.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 */

#ifndef _GENERALDEF
#define _GENERALDEF

#include <string>
#include <cstdint>

/**
 *  @ingroup DartBase
 * 
 *  @version 1.2
 * 
 *  Classe statique d'utilitaire et de variables communes aux differents modules 
 *  de DART.
 * 
 */

// both linux and windows accept / as path separator
static const std::string pathSeparator = "/";

// Thomas Galllineri : M_PI in math.h is not standard !
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI2
#define M_PI2 6.28318530717958647692
#endif

#ifndef M_PI4
#define M_PI4 12.56637061435917295384
#endif

#ifndef M_PI05
#define M_PI05 1.57079632679489661923
#endif

#ifndef M_SQRT1_2l
#define M_SQRT1_2l 0.707106781186547524400844362104849039L
#endif

#define DIRECTIONFILE 					"directions.txt"
#define PHASEFILE 						"phase"
#define MAKETFILE 						"maket.txt"
#define SIMULATIONPROPERTIESFILE		"simulation.properties.txt"
#define MAKETTREESRESULT				"Maket_Trees_result.txt"
#define DARTFILE 						"dart.txt"
#define DARTLUTPROPERTIESFILE			"lut.properties"
#define DARTSEQUENCEURPROPERTIESFILE	"dart.sequenceur.properties"
#define TRIANGLEFILE 					"triangles.txt"
#define DARTTYPEFILE 					"dart.typ"
#define DARTTYPEFILEPROPERTIES 			"dart.typ.properties"
#define IMAGEDIRECTORY 					"IMAGES_DART"
#define INPUT_DIR						"input"
#define OUTPUT_DIR						"output"
#define LIB_PHASE						"lib_phase"
#define NETCDF_FOLDER					"netcdf"
#define NETCDF_RB_FOLDER				"radiativeBudget"
#define NETCDF_BRF_FOLDER				"brf"
#define NETCDF_IMAGE_FOLDER				"image"
#define NETCDF_MAJOR_IMAGE_FILE			"image_dart"
#define NETCDF_MAJOR_JACOBIAN_FILE	    "jacobian"
#define NETCDF_LIDAR_IMAGE_FILE			"lidar_image"
#define NETCDF_RB_IMAGE_FILE			"rb_image"
#define NETCDF_RB1D_FILE				"radiativeBudget_1D"
#define NETCDF_RB3D_FILE				"radiativeBudget_3D"
#define NETCDF_RBTriangles_FILE			"radiativeBudget_Triangles"
#define NETCDF_RBType_FILE				"radiativeBudget_Type"
#define NETCDF_RBSunlitShaded_FILE		"radiativeBudget_SunlitShaded"
#define NETCDF_BRF_FILE					"brf"
#define ATMOSPHEREFILE_NC				"atmosphere.nc"
#define ATMOSPHEREMAKETFILE_NC			"atmosphereMaket.nc"
#define ATMOSPHEREPHASEFUNCTIONFILE		"AtmospherePhaseFunction"

#define ONE_CHAR 1
#define TWO_CHAR 2
#define THREE_CHAR 3
#define FOUR_CHAR 4
#define FIVE_CHAR 5
#define SIX_CHAR 6

#define NB_CHAR_FILE_SEARCH 7

#define REELLE  0 // - real distribution
#define STRICTE 1 // - only onr value = 1 and the other = 0

#define DESSOUS             0
#define DESSUS              1
#define AVANT               2
#define DROIT               3
#define ARRIERE             4
#define GAUCHE              5
#define CENTRE              6

#define LAMBDA_MINTH_TIR    2.5E-6

#define SZA_DAY_LIMIT       90.2

#define DISC_ILLUM          1
#define FLUO                2
#define NBLAD 10 // Nb de type de distribution foliaire

#define EPSILON             1E-5
#define EPSILON4			1E-4

#define byte uint8_t
#define FAUX 0
#define VRAI 1
#define unsint uint32_t

// Alias des valeurs pour le champ PositionDuCapteur de la table Coefficients dans le schema des LUT en base de donnees
#define POSITIONDUCAPTEUR_BOA 0
#define POSITIONDUCAPTEUR_TOA 1
#define POSITIONDUCAPTEUR_SENSOR 2

// Type d'element
#define soil         1

// Type of the Maquette
#define EXACTMAKET   1  // - Exact parameters.
#define EXACTPOSTREE 2  // - exact tree positions.
#define ALEATMAKET   3  // - average parameters.

// Etude de la Maquette
#define NORMAL 0        // - Creation pour DART

// Modelisation surfaces opaques      // Manu 08/02
// => le lambertien variable sera traite par DART donne_ro_spectral
#define NBTYPEDIFFUSION		8 	// nombre total de type de diffusion (lamb+)

#define DIFF_FEUILLE        5
#define SPEC_FEUILLE        6
#define DIFF_AIR        	7

// Type de diffusion
#define HAPKE      1      //  - hapke

// Topographie
#define PLAN   0        // - sans relief
#define RELIEF 1        // - avec relief

// Zones d'Eaux
#define NOWATER    0    // - Pas de zones d'eaux
#define LAKE_RIVER 3    // - lac & riviere
#define LAKE       1    // - lac
#define RIVER      2    // - riviere, fleuve, ...

// Zone Urbaine
#define NOURBAN        0  // - Pas de zone urbaine
#define BUILDINGS_ROADS_WALLS  4 // - batiments & routes
#define ONLY_BUILDINGS   1    // - batiment
#define SMALL_WALL       3    // - muret

#define NB_GASES_ABS_TYPE  14 // - Number of absorption gas type
#define NB_ATMOS_TYPE  NB_GASES_ABS_TYPE + 2 // - Number of atoms type: Addition of scattering gas and aerosol.

// Type de composants
namespace globalReferences {
	enum CellType {
		EMPTY = 0,
		AIR = 1,
		SOIL = 2,
		WATER = 3,
		ROAD = 4,
		PLOT = 5,
		CANOPY = 6,
		SOIL_MNT = 7,
		WATER_MNT = 8,
		ROAD_MNT = 9,
		WALL = 10,
		TRUNC = 11,
		ROOF = 12,
		PLOT_OPAQUE = 13,
		PLOT_MNT = 14,
		TWIG = 15,
		OPAQUE_NT = 16,
		N_TURBID = 17,
		BRANCH = 18,
		MIXED = 19,
		ATMOSPHERE = 20,
		SUN = 21,
		SKY = 22,
		LIDAR = 23,
		OTHER_LIGHT = 24,
		TURBID_TRIANGLE = 99,
		OBJECT3D = 100,
		NBTYPECELLS = 27
	};

	enum PlotType {
		MILIEU_PLOT_OPAQUE = 0, MILIEU_PLOT_TURBIDE = 1, MILIEU_PLOT_OPAQUE_TURBIDE = 2, MILIEU_PLOT_AIR = 3, MILIEU_PLOT_WATER = 4
	};

	// Type de LAD
	enum LADType {
		UNIFORM = 0, // - uniforme (densite cste/degre)
		SPHERIC = 1, // - spherique (densite cste/str)
		ERECTO = 2, // - erectophile (surtout vertical)
		PLANO = 3, // - planophile (surtout horizontal)
		EXTREMO = 4, // - extremophile (surtout non oblique)
		PLAGIO = 5, // - plagiophile  (surtout oblique)
		HORIZ = 6, // - horizontale
		VERTI = 7, // - verticale
		ELLIPS = 8, /* - ellipsoidale   */
		ELLIPT = 9, /* - elliptique     */
		USERDEFINED = 10, /* - User Defined   */
		MANUAL = 11, /* - Manual - De Wit   */
		BOUNDEDUNI = 12, /*  - bounded uniform         */
	};

	// Type of gases

	enum GasType {
		H2O = 0, O3 = 1, N2 = 2, CO2 = 3, CO = 4, CH4 = 5, N2O = 6, O2 = 7, OtherGas = 8,
	};

	enum PhaseFunctionType {
		LAMBERTIEN = 0, // - lambertien
		LAMBERTIEN_SPEC = 1, // - lambertien + speculaire
		HAPKE_SPEC = 2,   // - HAPKE + speculaire
		HAPKE2_SPEC = 23,   // - HAPKE + speculaire
		FCT_PHASE_EXT = 3,	// - Fct de phase externe
		RPV_PHASE = 4,	// - RPV
		FOLIAIRE = 5,	// - FOLIAIRE
		MONTE_CARLO = 6,	// - MONTE_CARLO
		AIR_FP = 7,	// - AIR phase function
		WATER_INTERFACE = 8,	// - WATER INTERFACE phase function
		WATER_VOLUME = 9,	// - WATER VOLUME phase function
		SPECULAR = 10, // - SPECULAR phase function
		PHASE_EXTERN = 11, // - Phase extern
		MIX_VOLUME = 12, // - Mixed volume in DART-Lux
		SPECULAR_LUX = 13, // for specular meterial in DART-Lux
		NULL_MAT = 14, // Null material (fully transparent)
		NULL_COLLISION_MATERIAL = 25,
		MIX_MATERIAL = 22, // Mixed materials in DART-Lux
		NB_FONCTION_PHASE = 16,	// - nombre total de fonction de phase differente
	};

	enum ProspectPhaseFunction {
		PROSPECT_LAMBERTIEN = 0, // - LAMBERTIEN
		PROSPECT_FOLIAIRE = 1,	// - FOLIAIRE
		PROSPECT_FOLIAIRE_TOP = 2,	// - FOLIAIRE TOP
		PROSPECT_FOLIAIRE_BOTTOM = 3	// - FOLIAIRE BOTTOM
	};
}

// Figures solMNT
#define CARRE 0           // - carre
#define TRIANGLE1 1       // - triangle de type 1
#define TRIANGLE2 2       // - triangle de type 2
#define TRIANGLE12 3      // - triangles type 1 et type 2

// Positionnement Arbres
//#define GRID            0 // - en rang�e
//#define FCTDISTRIBUTION 1 // - selon fct de distribution
//#define ZONE            2 // - dans un polygone (3/4 cotes)

// Taille Max Tableau
#define MAXPOINTSNB        10 // TabPoints[Point]=Value

#define CONSTANT 0            // - Rugosite constante

// --- Definition des phases -
#define EXISTE 1              // - deja definie
#define NBMAX_TETAF     91 // Nb max de valeurs d'inclinaison du fichier LIDF

// Type de modelisation dans phase.txt

#define ECL_ATM_ISO         1
#define ECL_ATM_ANISO       2
#define SIMUL_ATMOS         3
#define CONV_DEG_RADIAN     (M_PI / 180.0)

#define RAYON_TERRESTRE		6360
#define RAYON_TERRESTRE_METRE	6360000

#define SIGMA               5.6704E-8  // Cste de Stephan Boltzmann (W/m2/T4)
#define Cplanck1            1.19104402394075E-016  // =2.h.c2 (W/m2) (Formule de Planck)
#define Cplanck2            0.0143876876890208   // =hc/k (m.K) (formule de Planck)
#define STEPHAN             0
#define PLANCK              1
#define DELTA_LAMBDA        1E-8   // Pas de discretisation de la loi d'emission de Planck (unite m) Thierry

#define NBSPECTRALPARAMTOREAD	10 // Nombre de parametres spectraux a lire dans les databases

// ---------------  TYPES ------------------------
typedef enum PeriodiciteDeLaScene {
	SCENE_ISOLEE = 0, SCENE_INFINIE_REPETITIVE = 1, PENTE_INFINIE = 2
} PeriodiciteDeLaScene;

typedef struct plan {
	float a;
	float b;
	float c;
	float d;
} plan;

typedef struct droite {
	float a1;
	float a2;
	float b1;
	float b2;
	float a3;
	float b3;
} droite;

#define BAND_PREFIX				"BAND"					// Prefix of the BAND directory
#define ITERATION_PREFIX		"ITER"					// Prefix of the ITER directories
#define BRF_DIR					"BRF"					// Name of the BRF directory
#define TAPP_DIR				"Tapp"					// Name of the Tapp directory
#define COUPL_DIR				"COUPL"					// Name of the COUPL directory
#define SENSOR_DIR				"SENSOR"				// Name of the SENSOR directory
#define TOA_DIR					"TOA"					// Name of the TOA directory
#define BRF_FILE_PREFIX			"brf"					// Prefix of the brf files
#define ATMOSPHERE_BRF_FILE		"Atmosphere"	// Name of the atmosphere BRF file
#define ATMOSPHERE_BRF_ORDER1_FILE		"AtmosphereOrder1"	// Name of the atmosphere BRF file
#define TAPP_FILE_PREFIX		"tapp"					// Prefix of the tapp files
#define RADIATIVE_BUDGET_DIR 	"RADIATIVE_BUDGET"		// Name of the view directions output file
#define RadiativeBudget_3D 	 	"RadiativeBudget_3D"
#define RadiativeBudget_profile "RadiativeBudget_profile"
#define MPR_EXT				    ".mpr"					// mpr extension
#define IMAGE_SIZE_FLAG		    "Size"					// Flag to retrieve the image size in the mpr image file
#define IMAGE_PREFIX			""					// Prefix of the image files
#define GRF_EXT				    ".grf" 					// grf extension
#define GRSHARP_EXT			    ".gr#" 					// gr# extension
#define REF_PREFIX				"ref"					// ref prefix
#define SENSOR_PREFIX			"sensor" 				// Prefix of the sensor files
#define GEOREF_FILE			    "georef" 				// Name of the georef file
#define MPSHARP_EXT			    ".mp#"					// mp# extension
#define IMAGE_MIN_MAX_FLAG		"MinMax"				// Flag to retrieve the line containing the minimum and maximum values in the mpr image file
#define INFOIMA_FILE			"infoima"				// Name of the infoima file
#define MEAN_TEMPERATURE_FLAG	"* Mean temperature (K) ____________________________ *"		// Flag to retrieve the mean temperature of the scene elements from the DARTFILE file
#define TEMPERATURE_THRESHOLD	0.01					// The minimum temperature value for which the radiance will be computed
#define EXPONENT_MAX			709.0					// The maximum exponent value : double variables are 8 bytes length, that means maximum value is 1.7e308 => exp(EXPONENT_MAX must be less than 1.7e308)
#define RADIANCE_EXT			".txt"  				// Extension of the radiance files
#define RADIANCE_PREFIX		    "radiance"				// Prefix of the radiance files
#define BAND_EXT				".out"					// Extension of the band output files

#endif //_GENERALDEF
