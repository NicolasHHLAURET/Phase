/*
 * PhasesProperties.hpp
 *
 * Vincent Sac  declaration de chaines de caracteres constantes constituant les informations
 * fixes du fichier de phase => ex  phase.spectralDomain.
 * Cela permettra a un utilisateur de changer ces infos pour les faires evoluer
 *
 * Definition des variables statiques necessaire pour fabriquer les clefs de recherches
 * Separez les elements terminaux de ceux qui ne le sont pas
 * exemple d'une cle : element1.element2.element3:valeur
 * Les elements terminaux sont :
 * 			element1 et element2 => ils doivent etre suivi d'un point "." qui est le separateur interne d'une cle
 *
 * L'element non terminal est :
 * 			element3 => il doit toujours etre suivi de deux points ":" qui separe la cle de recherche de la valeur a trouver
 */

#ifndef PHASESPROPERTIES_HPP_
#define PHASESPROPERTIES_HPP_

#include <string>

namespace phasesProperties {
	/**
	 * Elements non finaux qui ne seront jamais en fin de la chaine de caracteres formant la cle
	 * Pour desactiver certains element de la cle : remplacer la partie a droite du signe = par ""
	 * On peut egalement supprimer la variable globale mais cela est franchement deconseille : la raison est que
	 *  vu qu'il s'agit d'un fichier de mapping cela signifie qu'il se base sur un fichier de donnees aussi cela
	 *  sous-entend que la variable globale avait une existence justifier et que sa suppression resulte d'une modification
	 *  de structure du fichier de donnees. Ce genre de modification pourrait se reitierer des annees plus tard et la necessite
	 *  de cette variable globale supprimee pour a nouveau se faire sentir. Donc prudence avant la suppression definitive des
	 *  variables globales du mapping
	 *
	 * exemple de desactivation d'une variable globale :
	 *	const static std::string PRODUIT = "produit" + _point_ => element de la cle de recherche actif
	 *	const static std::string PRODUIT = ""; => element de la cle de recherche inactif
	 */
	const std::string _point_ = ".";
	const std::string _zero_ = "0" + _point_;
	const std::string sPHASE = "phase" + _point_;
	const std::string sTYPEPHASE = "phaseType" + _point_;
	const std::string sPHASESPECDOMAIN = "band" + _point_;
	const std::string sPHASESPECDOMAIN_NOPOINT = "band";
	const std::string sSOIL = "soil" + _point_;
	const std::string sWATER = "water" + _point_;
	const std::string sROADS = "roads" + _point_;
	const std::string sPLOTSOPAQUE = "plotsOpaque" + _point_;
	const std::string sROOFS = "roofs" + _point_;
	const std::string sWALLS = "walls" + _point_;
	const std::string sTRUNKS = "trunks" + _point_;
	const std::string sTYPELAMBERTIEN = "lambertien" + _point_;
	const std::string sTYPEHAPKE = "hapke" + _point_;
	const std::string sTYPEPHASEEXTERNE = "phaseExterne" + _point_;
	const std::string sTYPERPV = "RPV" + _point_;
	const std::string sTYPELEAF = "leaf" + _point_;
	const std::string sTYPEAIR = "air" + _point_;
	const std::string sLEAF = "leaf" + _point_;
	const std::string sSPECTRAL = "spectral" + _point_;
	const std::string sTYPEWATERINTERFACE = "waterInterface" + _point_;
	const std::string sTYPEWATERVOLUME = "waterVolume" + _point_;
	const std::string sTYPEPHASEEXTERN = "phaseExtern" + _point_;
	const std::string sYAML = "yaml" + _point_;

	/**
	 * Elements finaux
	 */
	const std::string _deuxPoints_ = ":";
	const std::string sNBSPECTRALBANDS = "nbSpectralBands";
	const std::string sNBPHASES = "nombres";
	const std::string sLAMBDAMIN = "lambdamin";
	const std::string sLAMBDAMAX = "lambdamax";
	const std::string sAFPMOLECULES = "airPhaseFunctionMoleculesId";
	const std::string sAFPAEROSOLS = "airPhaseFunctionAerosolsId";
	const std::string sSPECTRALDOMAIN = "spectralDartMode";
	const std::string EMISSIONLAW = "emissionLaw";
	const std::string sUSE = "use";
	const std::string sFILENAME = "fileName";
	const std::string sINDREFRACINF = "indiceRefractionInferieur";
	const std::string sINDREFRACSUP = "indiceRefractionSuperieur";
	const std::string sTYPERUGOSITEINF = "typeRugositeInferieur";
	const std::string sTYPERUGOSITESUP = "typeRugositeInferieur";
	const std::string sINDRUGINF = "indiceRugositeInferieur";
	const std::string sINDRUGSUP = "indiceRugositeSuperieur";
	const std::string sSIGMA = "sigma";
	const std::string sTMIN = "Tmin";
	const std::string sTMAX = "Tmax";
	const std::string sIDTEMPERATURE = "IdTemperature";
	const std::string sBOA = "BOA";
	const std::string sTOA = "TOA";
	const std::string sSKYL = "SKYL";
	const std::string sATM_TEMP_ABSO = "AtmosphereTemperatureAbsoption";
	const std::string sRO = "RO";
	const std::string sThetaView = "ThetaView";
	const std::string sPhiView = "PhiView";
	const std::string sThetaIncident = "ThetaIncident";
	const std::string sPhiIncident = "PhiIncident";
	const std::string sECARTTYPE = "EcartType";
	const std::string sTDIRECT = "TauDirect";
	const std::string sTDIRECTPOURTEMPERATURE = "TauDirectPourTemperature";
	const std::string sTDIFFUS = "TauDiffus";
	const std::string sTMOY = "TauMoy";
	const std::string sAMPLITUDE = "amplitude";


	const std::string isSpecular = "isSpecular";
	const std::string sUSE_REFRACTION = "useRefraction";
	const std::string sSPECULAR_RADIANCE_DISTRIBUTION = "specularRadianceDistribution";
	const std::string sREFRACTION_INDEX_UP = "refractionIndexUp";
	const std::string sREFRACTION_INDEX_DOWN = "refractionIndexDown";
	const std::string sINTENSITY_FACTOR_UP_TO_DOWN = "intensityFactorUpToDown";
	const std::string sINTENSITY_FACTOR_DOWN_TO_UP = "intensityFactorDownToUp";
	const std::string sANGULAR_DIVERGENCE_UP_TO_DOWN = "angularDivergenceUpToDown";
	const std::string sANGULAR_DIVERGENCE_DOWN_TO_UP = "angularDivergenceDownToUp";
	const std::string sK_POWER_UP_TO_DOWN = "kPowerUpToDown";
	const std::string sK_POWER_DOWN_TO_UP = "kPowerDownToUp";
	const std::string sALBEDO_REFRACTION_SPECULAR_UP_TO_DOWN = "albedoRefractionSpecularUpToDown";
	const std::string sALBEDO_REFRACTION_SPECULAR_DOWN_TO_UP = "albedoRefractionSpecularDownToUp";
	const std::string sALBEDO_SPECULAR_UP_TO_DOWN = "albedoSpecularUpToDown";
	const std::string sALBEDO_SPECULAR_DOWN_TO_UP = "albedoSpecularDownToUp";

	const std::string sCONEANGULAIRE = "coneAngulaire";
	const std::string sINDICEREFRACTION = "indiceRefraction";
	const std::string sALBEDO = "albedo";
	const std::string sALBEDOSPEC = "albedoSpec";
	const std::string sWHAPKE = "wHapke";
	const std::string sCOEFFH1 = "coeffH1";
	const std::string sCOEFFC1 = "coeffC1";
	const std::string sCOEFFC3 = "coeffC3";
	const std::string sCOEFFC2 = "coeffC2";
	const std::string sCOEFFC4 = "coeffC4";
	const std::string sCOEFFH2 = "coeffH2";
	const std::string sCOEFFC = "coeffC";
	const std::string sCOEFFB = "coeffB";
	const std::string sCOEFFH = "coeffH";
	const std::string sCOEFFB0 = "coeffB0";
	const std::string sCOEFFTh = "coeffTh";
	const std::string sRHO0_RPV = "rho0";
	const std::string sK_RPV = "k";
	const std::string sG_RPV = "g";
	const std::string sH_RPV = "h";
	const std::string sTheta_max_RPV = "theta_max";
	const std::string sOMEGAMIN = "omegaMin";
	const std::string sOMEGAMAX = "omegaMax";
	const std::string sCLUMPINGA = "clumpingA";
	const std::string sCLUMPINGB = "clumpingB";
	const std::string sLAD = "LAD";
	const std::string sALA = "ALA";
	const std::string sTHETAM = "THETAM";
	const std::string sEQUIVALENTFRONTLAMBERTIAN = "equivalentFrontLambertianIndex";
	const std::string sEQUIVALENTBACKLAMBERTIAN = "equivalentBackLambertianIndex";
	const std::string sECCENTRICITY = "eccentricity";
	const std::string sDIMFOL = "dimensionFoliaire";
	const std::string sROSUP = "RoSuperieur";
	const std::string sROINF = "RoInferieur";
	const std::string sTO = "to";
	const std::string sTOINF = "toInferieur";
	const std::string sNINF = "ninf";
	const std::string sNSUP = "nsup";
	const std::string sTYPERI = "typeRI";
	const std::string sTYPERS = "typeRS";
	const std::string sKOINF = "koInf";
	const std::string sKOSUP = "KoSup";
	const std::string sENERGIE = "energieW";
	const std::string sTYPEDIFF = "typeDiffusion";
	const std::string sBOUNDINGWALLS = "boundingWalls.Temperature";
	const std::string sIsDatabaseInWater = "isDb";
	const std::string sUpwardRefractionIndexInWater = "upwardRefractionIndex";
	const std::string sDownwardRefractionIndexInWater = "downwardRefractionIndex";

	const std::string sDatabaseNameInWater = "dbName";

	const std::string sYaMLEquivalence = "equivalentIndex";

	const std::string sThermalHotSpotFactor = "thermalHotSpotFactor";
	const std::string sDiffuseTransmittanceAccelerationFactor = "diffuseTransmittanceAccelerationFactor";

	// Fluorescence
	const std::string sIsFluorescent = "isFluorescent";
	const std::string sIsForcedYields = "isForcedYields";
	const std::string sUseCombinedYields = "useCombinedYields";
	const std::string sFluorescence = "fluorescence" + _point_;
	const std::string sFluorescenceFileName = "fileName";
	const std::string sNbYieldCases = "nbYieldCases";
	const std::string sYieldPSI = "yieldPSI";
	const std::string sYieldPSII = "yieldPSII";
	const std::string sUseBioClimaticProfile = "useBioClimaticProfile";
	const std::string sTopUseBioClimaticProfile = "topUseBioClimaticProfile";
	const std::string sBottomUseBioClimaticProfile = "bottomUseBioClimaticProfile";
	const std::string sBioClimatic = "bioClimaticProfile" + _point_;
	const std::string sTopBioClimatic = "topBioClimaticProfile" + _point_;
	const std::string sBottomBioClimatic = "bottomBioClimaticProfile" + _point_;
	const std::string sBioClimaticFileName = "fileName";
	const std::string sLinkedOpticalPropertyType = "linkedOpticalPropertyType";
	const std::string sLinkedOpticalPropertyIndex = "linkedOpticalPropertyIndex";
	const std::string sTopLinkedOpticalPropertyType = "topLinkedOpticalPropertyType";
	const std::string sTopLinkedOpticalPropertyIndex = "topLinkedOpticalPropertyIndex";
	const std::string sBottomLinkedOpticalPropertyType = "bottomLinkedOpticalPropertyType";
	const std::string sBottomLinkedOpticalPropertyIndex = "bottomLinkedOpticalPropertyIndex";
	const std::string sMinLayerAltitude = "minLayerAltitude";
	const std::string sMaxLayerAltitude = "maxLayerAltitude";

}

#endif /* PHASESPROPERTIES_HPP_ */
