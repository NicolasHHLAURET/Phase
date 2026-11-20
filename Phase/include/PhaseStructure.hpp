/*
 * PhaseStructure.hpp
 *
 *  Created on: Apr 16, 2014
 *      Author: cajgfingert
 */

#ifndef PhaseStructure_HPP_
#define PhaseStructure_HPP_

typedef struct elem_tabspec // Tableau de gestion des directions speculaires
{
	int dir;
	double domegavp;
	double critere;
} elem_tabspec;

struct SpecularData {
	bool isSpecular;

	bool _useRefraction;

	int _specularRadianceDistribution;

	double refraction_index_down;
	double intensity_factor_up_to_down;
	double angular_divergence_up_to_down;
	double k_power_up_to_down;
	double refraction_index_up;
	double intensity_factor_down_to_up;
	double angular_divergence_down_to_up;
	double k_power_down_to_up;

	double albedo_down_to_up;
	double albedo_up_to_down;

	double albedo_refraction_down_to_up;
	double albedo_refraction_up_to_down;
};

struct TemperatureData {
	double directTransmittanceTemperature;
};

struct TemperatureFoliarData {
	double transmittanceTemperature;
};

struct SpecularFoliarData {
	bool _isSpecular;

	double roughnessTop;
	double roughnessBottom;
	double refractionIndexTop;
	double refractionIndexBottom;
	double roughnessCoefficientTop;
	double roughnessCoefficientBottom;
};

struct PropertyLambertian {
	double reflectance;
	double directTransmittance;
	double diffuseTransmittance;
};

struct PropertySpecular {
	double ni;
	double nt;
	double roughness;
	double tau; // optical depth
	double intensityR;
	double intensityT;
	int type; // 0 = interface; 1 = thin layer; 2 = glossy coating + diffuse base
	int baseBSDF;
	int baseBSDFIndex;

	SpecularData specularData;
	TemperatureData temperatureData;
};

struct PhaseExternProperty {

	double thetaView;
	double phiView;

	double thetaIncident;
	double phiIncident;

	double reflectance;
	double interpolatedReflectance;
};


struct PropertyHapke {
	double directTransmittance;
	double diffuseTransmittance;

	double hapkeParameterW;
	double hapkeParameterH1;
	double hapkeParameterH2;
	double hapkeParameterC1;
	double hapkeParameterC3;
	double hapkeParameterC2;
	double hapkeParameterC4;

	SpecularData specularData;
	TemperatureData temperatureData;
};

struct PropertyHapke2 {
	double directTransmittance;
	double diffuseTransmittance;

	double hapkeParameterW;
	double hapkeParameterC;
	double hapkeParameterB;
	double hapkeParameterH;
	double hapkeParameterB0;
	double hapkeParameterTh;

	SpecularData specularData;
	TemperatureData temperatureData;
};

struct PropertyRpv {
	double reflectance;

	double directTransmittance;
	double diffuseTransmittance;

//	double rpvParameterRho0;
	double rpvParameterK;
	double rpvParameterG;
	double rpvParameterH;
	double theta_max; // maximum theta larger than which the reflectance is saturated.

	SpecularData specularData;
	TemperatureData temperatureData;
};

struct FoliarProperty {
	double reflectanceTop;
	double reflectanceBottom;
	double transmittance;
	double bottomTransmittance;

	std::vector<double> topPSIBackward;
	std::vector<double> topPSIForward;
	std::vector<double> topPSIIBackward;
	std::vector<double> topPSIIForward;

	std::vector<double> bottomPSIBackward;
	std::vector<double> bottomPSIForward;
	std::vector<double> bottomPSIIBackward;
	std::vector<double> bottomPSIIForward;

	double diffuseTransmittanceAccelerationFactor;
	double bottomDiffuseTransmittanceAccelerationFactor;

	int typeOfLeafAngleDistribution;
	double foliarDimension;
	double averageLeafAngle;
	double ellipsePrincipalAngle;
	double ellipseEccentricity;
	double directTransmittance;
	double diffuseTransmittance;

	SpecularFoliarData specularFoliarData;
	TemperatureFoliarData temperatureData;
};

struct FTFoliar {
	int casPDF;
	bool ifSpecIntegration;
	bool ifDiffIntegration;
	int monteCarloCheck;
	unsigned int listDirectionsSize;
	unsigned int numDivDiff;
	unsigned int numDivSpecPol;
	double solidAnglesSum;
	int iterationStepPhi;
	int iterationStepTheta;
};

struct SolarIrradianceValues {
	double meanIrradianceBottomOfAtmosphere;
	double meanIrradianceTopOfAtmosphere;
	double meanSKYL;
	double temperatureAtmosphereAbsorption;
	std::vector<double> irradianceBottomOfAtmosphereVector;
	std::vector<double> irradianceTopOfAtmosphereVector;
	std::vector<double> irradianceForPonderationVector;
	std::vector<double> irradianceSpectralWavelengthVector;
	std::vector<double> temperatureAtmosphereAbsorptionVector;
	std::vector<double> skylVector;
	unsigned int numberOfIrradianceSpectralValues;
	unsigned int numberOfGasSpectralValue;
};

struct HenyeyGreensteinParameters {
	double g1;
	double g2;
	double a;
};

struct MultiplicativeFactorForWaterInterface {
	std::vector<double> upwardRefractionIndexVector;
	std::vector<double> downwardRefractionIndexVector;

};
struct MultiplicativeFactorForRpv {
	std::vector<double> rpvParameterRho0Vector;
	std::vector<double> rpvParameterKVector;
	std::vector<double> rpvParameterGVector;
	std::vector<double> rpvParameterHVector;
	std::vector<double> directTransmittanceVector;
	std::vector<double> diffuseTransmittanceVector;
	std::vector<double> specularIntensityVector;
};

struct MultiplicativeFactorForSpecular {
	std::vector<double> niVector;
	std::vector<double> ntVector;
	std::vector<double> opticalDepthVector;
	std::vector<double> reflectionIntensityVector;
	std::vector<double> transmissionIntensityVector;
};

struct MultiplicativeFactorForLambertien {
	std::vector<double> reflectanceVector;
	std::vector<double> diffuseTransmittanceVector;
	std::vector<double> directTransmittanceVector;
	std::vector<double> specularIntensityVector;
	double diffuseTransmittanceAccelerationFactor;
};

struct MultiplicativeFactorForHapke {
	std::vector<double> directTransmittanceVector;
	std::vector<double> diffuseTransmittanceVector;
	std::vector<double> specularIntensityVector;
	std::vector<double> W;
	std::vector<double> C1;
	std::vector<double> C2;
	std::vector<double> C3;
	std::vector<double> C4;
	std::vector<double> H1;
	std::vector<double> H2;
};

struct MultiplicativeFactorForHapke2 {
	std::vector<double> directTransmittanceVector;
	std::vector<double> diffuseTransmittanceVector;
	std::vector<double> specularIntensityVector;
	std::vector<double> W;
	std::vector<double> C;
	std::vector<double> B;
	std::vector<double> H;
	std::vector<double> B0;
	std::vector<double> Th;
};

struct ReflectanceModelsParameters {
	unsigned int numberOfHapkeModels;
	unsigned int numberOfHapke2Models;
	unsigned int numberOfFoliarModels;
	unsigned int numberOfLambertianModels;
	unsigned int numberOfEquivalentLambertianModels;
	unsigned int numberOfRPVModels;
	unsigned int numberOfSpecularModels;
	unsigned int numberOfWaterInterfaceModels;
	unsigned int numberOfPhaseExternModels;
	unsigned int numberOfMixedMaterialModels;
};

struct UserSpectralBand {
	double lowerLambdaValue;
	double upperLambdaValue;
//	std::string lowerLambdaString;
//	std::string upperLambdaString;
};

struct IrradianceSpectralBand {
	int lowerSpectralBandIdValue;
	int upperSpectralBandIdValue;

	std::string lowerSpectralBandIdString;
	std::string upperSpectralBandIdString;
};

struct AtmosphereAerosolParameters {
	int idLowerSpectralBand;
	int idUpperSpectralBand;
	std::string tableName;
	std::string columnsChoice;
	int nbColumnsChoice;
};

struct OpticalFactorParameters {
	std::vector<bool> duplicateFirstLayer;
	std::vector<std::string> fileNames;
	bool sameOpticalFactorForAllBands;
};

struct FluorescenceParameters {
	bool isFluorescent;
	std::string databaseName;
	std::string pythonScript; //"Fluspect_B_CX_P6.py" or "Fluspect_B_CX_PRO.py"
	double structureCoeff;
	double chlorophyll;
	double carotenoid;
	double brownPigment;
	double waterThickness;
	bool useNewCm;
	double leafMassPerArea;
	double Cbc;
	double Cp;
	double anthocyanin;
	double v2z;
	bool forceYield;
	bool useCombinedYields;
	double yieldPSI;
	double yieldPSII;

	bool useBioClimaticProfileEta;
	std::string bioClimaticFileRelativePath; // Currently generated by the SCOPE model
	int linkedOpticalPropertyType; // Phase function sharing the same profile/group
	int linkedOpticalPropertyIndex; // Phase function sharing the same profile/group
};

struct ProspectParameters {
	std::string inputProspectFile;
	std::string pythonScript; //"Fluspect_B_CX_P6.py" or "Fluspect_B_CX_PRO.py"
	double structureCoeff;
	double chlorophyll;
	double carotenoid;
	double brownPigment;
	double waterThickness;
	bool   useNewCm;
	double leafMassPerArea;
	double Cbc;
	double Cp;
	double anthocyanin;
	double v2z;
};

struct AtmosphereParameters {

	int computationMethodTOAtoBOA;
	bool isAtmosphereDatabase;
	bool ifDefAmountOtherGases;
	int airInEarthSceneDefinedForRadiativeTransfer;
	int isAtmosphereDefined;
	int numberOfAerosol;
	int numberOfAbsorbingGas;
	std::string databaseName;
	std::vector<HenyeyGreensteinParameters> henleyGreensteinParameters;

	unsigned int _directionSetIndex = 0;

	std::string atmosphereTemperatureFileName;
	std::vector<std::string> aerosolsModelName;
	std::vector<std::string> aerosolCumulativeModelName;
	std::vector<std::string> hgParametersModelName;
	std::string gasModelName;
	std::string gasCumulativeModelName;
	std::string gasParametersModelName;
	std::string temperatureModelName;
	bool h2oAmountCkeckbox;
	bool redefTemperature;
	bool fullyPrecomputed;
	double sceneAltitude;

	double redef_temp;
	double redef_altitude;

	int h2oAmountDef;
	//0: define by multiplicative factor
	//1: define by amount in g/cm2
	//2: define by amount in atm-cm
	//3: define by density in atm-cm/km
	//4: define by relative humidity

	double gas_u;
	double aerosols_u;

	double mulFactorH2O;
	double def_temp, def_altitude, def_amount_H2O_g_per_cm2, def_amount_H2O_atm_cm, def_density_atm_cm_per_km, def_rh;
	double ref_amount_H2O_atm_cm, ref_amount_H2O_g_per_cm2, ref_density_H2O_atm_cm_per_km_0, ref_density_H2O_atm_cm_per_km_1, ref_temp_K_0,
			ref_temp_K_1;

	void processMulFactorForH2O() {

		switch (h2oAmountDef) {
			case 0: //define by multiplicative factor
				return;
				break;
			case 1: //define by amount in g/cm2
				mulFactorH2O = def_amount_H2O_g_per_cm2 / ref_amount_H2O_g_per_cm2;
				break;
			case 2: //2: define by amount in atm-cm
				mulFactorH2O = def_amount_H2O_atm_cm / ref_amount_H2O_atm_cm;
				break;
			case 3: //3: define by density in atm-cm/km
			{
				double ref_now = linearInterpolateDensity_Atm_cm_per_km(def_altitude);
				mulFactorH2O = def_density_atm_cm_per_km / ref_now;
			}
				break;
			case 4: //4: define by relative humidity
			{
				double pressure_saturation_H2O = 6.112 * exp((17.62 * (def_temp - 273.15)) / (243.12 + def_temp - 273.15));
				double pressure_H2O = def_rh / 100 * pressure_saturation_H2O;
				double density_km_per_m3 = pressure_H2O * 216.67 / def_temp / 1000;
				double density_mol_per_cm3 = density_km_per_m3 / 18000;
				double density_mol_per_cm2_per_km = density_mol_per_cm3 * 100 * 1000;
				double cal_density_atm_cm_per_km = density_mol_per_cm2_per_km / 2.69e19;

				double ref_now2 = linearInterpolateDensity_Atm_cm_per_km(def_altitude);
				mulFactorH2O = cal_density_atm_cm_per_km / ref_now2;
				//!!!!To be implemented.

			}
				break;
			default: {
				mulFactorH2O = 1;
				std::cout << "Option water vapour amount not defined..." << std::endl;
//			abort();
				break;
			}
		}

		std::cout << "H2O scale factor: " << mulFactorH2O << std::endl;

	}

	double linearInterpolateDensity_Atm_cm_per_km(double const& altitude) const {
		return (ref_density_H2O_atm_cm_per_km_1 - ref_density_H2O_atm_cm_per_km_0) / 1000 * altitude + ref_density_H2O_atm_cm_per_km_0;
	}

	double linearInterpolateTemperature(double const& altitude) const {
		return (ref_temp_K_1 - ref_temp_K_0) / 1000 * altitude + ref_temp_K_0;
	}

	double calculateOffSetDefTemperature() const {
		if (redefTemperature) {
			double tempAtDefAltitude = linearInterpolateTemperature(redef_altitude);
			return redef_temp - tempAtDefAltitude;
		}
		return 0;
	}

	/**
	 * Water multiplicative factor definition
	 */
	std::string userDefinedWaterTable;
	std::string userDefinedWaterTableFollowing;
	double userDefinedWaterFactor;
	double userDefinedWaterFactorFollowing;
	/* Marqueur booléen pour indiquer que le facteur multiplicatif est < à la 1ere valeur de la DB,
	 donc appliquer une interpolation différente (entre 1 et la 1ere valeur de la DB) */
	bool userDefinedWaterIsLowerThanFirstDBValue;

	std::vector<double> aerosolOpticalDepthFactor;
	double co2MixRate;

	double mulScatMulFactor, coMulFactor, ch4MulFactor, n2oMulFactor, o2MulFactor, n2MulFactor, o3MulFactor, nh3MulFactor, noMulFactor, no2MulFactor,
			so2MulFactor, hno3MulFactor, othersMulFactor;

	std::vector<double> otherGasesTransMultiFactor; // Not used ???

	double aerosolScaleFactor;
	double gasOpticalDepthForScattering;
	double aerosolOpticalDepthForScattering;
	double aerosolSingleScatteringAlbedo;
	double gasScaleFactor;
	double gasTotalTransmittanceForAbsoption;
	double sensorHeight;
//	double h2oScaleFactor;
//	double co2ScaleFactor;
//	double coScaleFactor;
//	double ch4ScaleFactor;
//	double n2oScaleFactor;
//	double o2ScaleFactor;
//	double n2ScaleFactor;
	bool aerosolProfil;
	double mus;
	double muPrimeGaz;
	double muPrimeAerosols;
	double mPrimeGaz;
	double mPrimeAerosols;

	std::vector<double> mPrimeGazPerDirection;
	std::vector<double> mPrimeAerosolsPerDirection;

	unsigned int minimumNumberOfDivisions;

};

#endif
