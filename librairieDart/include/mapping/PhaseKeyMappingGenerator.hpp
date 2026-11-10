/*
 * PhaseKeyMappingGenerator.hpp
 *
 *  Created on: 5 juil. 2010
 *      Author: Vincent Sac
 */

#ifndef PHASEKEYMAPPINGGENERATOR_HPP_
#define PHASEKEYMAPPINGGENERATOR_HPP_

#include "GeneralDef.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "namespaces/phasesProperties.hpp"

using namespace phasesProperties;
class PhaseKeyMappingGenerator {
private:
	/**
	 * Attributs de la classe
	 * @param bandeSpectrale : le numero de la bande spectral courante
	 * @param numeroFonctionDePhase : le numero de la fonction de phase etudiee
	 * @param typeFonctionDePhase : le type de la fonction de phase (Lambertien (avec ou sans speculaire), hapke,
	 * 								phase externe, RPV, Foliaire)
	 * @param typeElement : les diffrents objets autres que les fonctions de phases (eau, route, toit, sol, tronc,
	 * 						feuille)
	 * @param separateurCleValeur : si on utilise la genereteur de cle pour ecrire dans un fichier, il faut
	 * 								l'initialiser avec ":"
	 * 								sinon on est en mode de lecture et les deux points ":" sont inutiles
	 * If we use the key generator to write in the file, you have to initialize with ":", if not, it means we are in read only mode and the 'double dots' are useless
	 */
	unsigned int bandeSpectrale;
	unsigned int numeroFonctionDePhase;
	unsigned int typeFonctionPhase;
	unsigned int typeElement;
	std::string separateurCleValeur;

	static std::string nodePhaseSolarNoiseSimu;

	/**
	 * Methode de conversion d'un type numerique en std::string
	 */
	template<typename T>
	const std::string convertirEnString(const T & typeAConvertir) const;

	/**
	 * Methode determinant la fonction de phase traitee et renvoyant son intitule pour la cle de recherche
	 * Valeurs possibles : lambertien, lambertien speculaire, hapke, phase externe, RPV, foliaire
	 */
	const std::string getIntitulePhase() const;

	/**
	 * Methode faisant l'acquisition de la premiere composante commune de la cle de recherche
	 */
	const std::string getPremiereComposante() const;
	const std::string getPremiereComposanteSansPoint() const;

	/**
	 * Methode faisant la meme chose que getPremiereComposante() mais ajoutant le numero de spectrale
	 */
	const std::string getPremiereComposanteSpectrale() const;

	/**
	 * Methode faisant l'acquisition de la seconde composante de la cle de recherche
	 * /!\ Cette composante n'est pas commune a toute les cles de recherche
	 */
	const std::string getComposantePartielleTypePhase() const;

	/**
	 * Methode obtenant le type de l'element autre qu'une fonction de phase
	 */
	const std::string determinerElement();

	/**
	 * Methode fusionnant les composantes communes entre les cles
	 */
	const std::string getComposantePhase() const;
	const std::string getComposanteNonSpectralePhase() const;

	/**
	 * Methode fusionnant la premiere composante de la cle avec la composante constituant les objets (choix entre eau, tronc, feuille, toit, mur)
	 */
	const std::string getComposanteElement();

public:
	
	static void useNodeSolarNoiseSimuSN() {
		nodePhaseSolarNoiseSimu = "sn" + _point_;
	}
	
	static void useNodeSolarNoiseSimuLIDAR() {
		nodePhaseSolarNoiseSimu = "lidar" + _point_;
	}
	
	static void resetNodeSolarNoiseSimu() {
		nodePhaseSolarNoiseSimu = "";
	}
	
	/**
	 * Constructeur par defaut ou parametre
	 */
	PhaseKeyMappingGenerator(	const bool & estEnModeLecture = true,
								const unsigned int & numeroSpectrale = 0,
								const unsigned int & numeroPhase = 0,
								const unsigned int & typePhase = 0,
								const unsigned int & element = 0) :
			bandeSpectrale(numeroSpectrale), numeroFonctionDePhase(numeroPhase), typeFonctionPhase(typePhase), typeElement(element) {
		if (estEnModeLecture) {
			this->separateurCleValeur = "";
		} else {
			this->separateurCleValeur = _deuxPoints_;
		}
	}
	
	/**
	 * Getters et setters des attributs
	 */
	const unsigned int & getBandeSpectrale() const;
	const unsigned int & getNumeroFonctionDePhase() const;
	const unsigned int & getTypeFonctionDePhase() const;
	const unsigned int & getTypeElement() const;

	void setBandeSpectrale(const unsigned int & numeroSpectrale);
	void setNumeroFonctionDePhase(const unsigned int & numeroPhase);
	void setTypeFonctionDePhase(const unsigned int & typePhase);
	void setTypeElement(const unsigned int & typeElement);

	/**
	 * Methodes de generations des cles de recherche communes a toutes les fonctions de phases
	 */
	const std::string cleNombreBandesSpectrales() const;
	const std::string cleNombreFonctionDePhase() const;
	const std::string cleLambdaMin() const;
	const std::string cleLambdaMax() const;
	const std::string cleSpectralDomain() const;
	const std::string cleLoiEmission() const;
	const std::string cleTOA() const;
	const std::string cleBOA() const;
	const std::string cleSKYL() const;
	const std::string cleAtmosphereTemperatureAbsorption() const;
	const std::string clePhaseUtilisee() const;
	const std::string cleNomFichier() const;
	const std::string cleTauDiffus() const;
	const std::string cleTauDirect() const;
	const std::string cleTauDirectPourTemperature() const;
	const std::string cleTauMoy() const;

	/**
	 * Methodes communes a certaine fonction de phase
	 */
	//Lambertien, lambertien speculaire, phase externe
	const std::string cleRO() const;
	const std::string cleThetaView() const;
	const std::string clePhiView() const;
	const std::string cleThetaIncident() const;
	const std::string clePhiIncident() const;
	const std::string cleEcartType() const;

	//Phase externe et hapke
	const std::string cleW() const;
	const std::string cleCoeffH1() const;
	const std::string cleCoeffC1() const;
	const std::string cleCoeffC3() const;
	const std::string cleCoeffC2() const;
	const std::string cleCoeffC4() const;
	const std::string cleCoeffH2() const;
	const std::string cleAmplitude() const;
	const std::string isSpecularKey() const;
	const std::string cleConeAngulaire() const;
	const std::string cleIndiceRefraction() const;
	const std::string cleAlbedo() const;

	//... et hapke2
	const std::string cleCoeffC() const;
	const std::string cleCoeffB() const;
	const std::string cleCoeffH() const;
	const std::string cleCoeffB0() const;
	const std::string cleCoeffTh() const;

	// Clé speculaires
	const std::string cleUseRefraction() const;
	const std::string cleSpecularRadianceDistribution() const;
	const std::string cleRefractionIndexUp() const;
	const std::string cleRefractionIndexDown() const;
	const std::string cleIntensityFactorDownToUp() const;
	const std::string cleIntensityFactorUpToDown() const;
	const std::string cleAngularDivergenceDownToUp() const;
	const std::string cleAngularDivergenceUpToDown() const;
	const std::string cleKPowerDownToUp() const;
	const std::string cleKPowerUpToDown() const;
	const std::string cleAlbedoSpecularDownToUp() const;
	const std::string cleAlbedoSpecularUpToDown() const;
	const std::string cleAlbedoRefractionSpecularDownToUp() const;
	const std::string cleAlbedoRefractionSpecularUpToDown() const;

	/**
	 * Methodes specifiques a chaque fonction de phase
	 */
	//RPV
	const std::string cleRho0_RPV() const;
	const std::string cleK_RPV() const;
	const std::string cleG_RPV() const;
	const std::string cleH_RPV() const;
	const std::string cleThetaMax_RPV() const;

	//Water Interface
	const std::string isDatabaseInWater() const;
	const std::string downwardRefractionIndexInWater() const;
	const std::string upwardRefractionIndexInWater() const;

	const std::string databaseNameInWater() const;
	//Lambertien speculaire
	inline const std::string cleAlbedoSpec() const;

	//Foliaire
	const std::string cleOmegaMin() const;
	const std::string cleOmegaMax() const;
	const std::string cleClumpingA() const;
	const std::string cleClumpingB() const;
	const std::string cleLAD() const;
	const std::string cleDimensionFoliaire() const;
	const std::string cleRoSuperieur() const;
	const std::string cleRoInferieur() const;
	const std::string cleTo() const;
	const std::string cleToInf() const;
	const std::string cleNinf() const;
	const std::string cleNsup() const;
	const std::string cleTypeRI() const;
	const std::string cleTypeRS() const;
	const std::string cleKoInf() const;
	const std::string cleKoSup() const;
	const std::string cleALA() const;
	const std::string cleThetaM() const;
	const std::string cleEccentricity() const;
	const std::string cleIndiceRugositeInf() const;
	const std::string cleIndiceRugositeSup() const;
	const std::string cleEquivalentFrontLambertianIndex() const;
	const std::string cleEquivalentBackLambertianIndex() const;

	// Air
	const std::string cleSigma() const;

	/**
	 * Cles de recherches pour des objets specifique d'une scene
	 */
	//Elements de type sol, eau, route, plots (parcelle de vegetation), toit, mur, tronc
	const std::string cleTemperatureMin();
	const std::string cleTemperatureMax();

	//Methode speciale pour les feuilles
	const std::string cleLeafTemperatureMin();
	const std::string cleLeafTemperatureMax();
	const std::string cleLeafIdTemperature();

	//Elements specifique a l'element mur
	const std::string cleRoofBoundingWallsTemperature();

	//Indices des fonctions de phase d'air extrapole de l'Atmosphere dans la couche BOA
	const std::string cleFonctionPhaseAirMoleculesIndice() const;
	const std::string cleFonctionPhaseAirAerosolsIndice() const;

	// Cles lies a la fluorescence
	const std::string cleUseFluorescence() const;
	const std::string cleForceYields() const;
	const std::string cleUseCombinedYields() const;
	const std::string getComposanteFluorescence() const;
	const std::string cleFluorescenceFileName() const;
	const std::string cleFluorescenceNbCases() const;
	const std::string cleFluorescenceYieldPSI(unsigned int i) const;
	const std::string cleFluorescenceYieldPSII(unsigned int i) const;

	const std::string cleFluorescenceBioClimaticUsage() const;
	const std::string cleTopFluorescenceBioClimaticUsage() const;
	const std::string cleBottomFluorescenceBioClimaticUsage() const;
	const std::string getComposanteBioClimaticProfile() const;
	const std::string getTopComposanteBioClimaticProfile() const;
	const std::string getBottomComposanteBioClimaticProfile() const;
	const std::string cleFluorescenceBioClimaticFile() const;
	const std::string cleTopFluorescenceBioClimaticFile() const;
	const std::string cleBottomFluorescenceBioClimaticFile() const;
	const std::string cleFluorescenceBioClimaticLinkedPropertyType() const;
	const std::string cleFluorescenceBioClimaticLinkedPropertyIndex() const;
	const std::string cleFluorescenceTopBioClimaticLinkedPropertyType() const;
	const std::string cleFluorescenceTopBioClimaticLinkedPropertyIndex() const;
	const std::string cleFluorescenceBottomBioClimaticLinkedPropertyType() const;
	const std::string cleFluorescenceBottomBioClimaticLinkedPropertyIndex() const;
	const std::string cleFluorescenceBioClimaticMinLayer() const;
	const std::string cleFluorescenceBioClimaticMaxLayer() const;

	// Cles de transition au YAML
	const std::string cleEquivalentYAMLIndice() const;

	const std::string cleThermalHotSpotFactor() const;
	const std::string cleDiffuseTransmittanceAccelerationFactor() const;

	virtual ~PhaseKeyMappingGenerator() {}
};

/**
 * Methodes privees
 */
template<typename T>
inline const std::string PhaseKeyMappingGenerator::convertirEnString(const T & typeAConvertir) const {
	std::ostringstream variableConvertie;
	variableConvertie << typeAConvertir;
	return (variableConvertie.str() + _point_);
}

inline const std::string PhaseKeyMappingGenerator::getPremiereComposante() const {
	return (sPHASE + nodePhaseSolarNoiseSimu/* + sPHASESPECDOMAIN*/);
}
inline const std::string PhaseKeyMappingGenerator::getPremiereComposanteSansPoint() const {
	return (sPHASE + nodePhaseSolarNoiseSimu + sPHASESPECDOMAIN_NOPOINT);
}

inline const std::string PhaseKeyMappingGenerator::getPremiereComposanteSpectrale() const {
	return (this->getPremiereComposanteSansPoint() + this->convertirEnString(this->bandeSpectrale));
}

inline const std::string PhaseKeyMappingGenerator::getComposantePartielleTypePhase() const {
	return (sTYPEPHASE);
}

inline const std::string PhaseKeyMappingGenerator::getComposantePhase() const {
	return (this->getPremiereComposanteSpectrale() + this->getComposantePartielleTypePhase() + this->getIntitulePhase()
			+ this->convertirEnString(this->numeroFonctionDePhase));
}

inline const std::string PhaseKeyMappingGenerator::getComposanteNonSpectralePhase() const {
	return (this->getPremiereComposante() + this->getComposantePartielleTypePhase() + this->getIntitulePhase()
			+ this->convertirEnString(this->numeroFonctionDePhase));
}

inline const std::string PhaseKeyMappingGenerator::getComposanteElement() {
	return (this->getPremiereComposanteSpectrale() + this->determinerElement());
}

/**
 * Methodes publiques
 */
//Getters et setters
inline const unsigned int & PhaseKeyMappingGenerator::getBandeSpectrale() const {
	return (this->bandeSpectrale);
}

inline const unsigned int & PhaseKeyMappingGenerator::getNumeroFonctionDePhase() const {
	return (this->numeroFonctionDePhase);
}

inline const unsigned int & PhaseKeyMappingGenerator::getTypeFonctionDePhase() const {
	return (this->typeFonctionPhase);
}

inline const unsigned int & PhaseKeyMappingGenerator::getTypeElement() const {
	return (this->typeElement);
}

inline void PhaseKeyMappingGenerator::setBandeSpectrale(const unsigned int & numeroSpectrale) {
	this->bandeSpectrale = numeroSpectrale;
}

inline void PhaseKeyMappingGenerator::setNumeroFonctionDePhase(const unsigned int & numeroPhase) {
	this->numeroFonctionDePhase = numeroPhase;
}
inline void PhaseKeyMappingGenerator::setTypeFonctionDePhase(const unsigned int & typePhase) {
	this->typeFonctionPhase = typePhase;
}

inline void PhaseKeyMappingGenerator::setTypeElement(const unsigned int & typeElement) {
	this->typeElement = typeElement;
}

//Generateurs de cles de recherche
inline const std::string PhaseKeyMappingGenerator::cleNombreBandesSpectrales() const {
	return (sPHASE + nodePhaseSolarNoiseSimu + sNBSPECTRALBANDS + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleNombreFonctionDePhase() const {
	return (this->getPremiereComposante() + this->getComposantePartielleTypePhase() + this->getIntitulePhase() + sNBPHASES + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLambdaMin() const {
	return (this->getPremiereComposanteSpectrale() + sLAMBDAMIN + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLambdaMax() const {
	return (this->getPremiereComposanteSpectrale() + sLAMBDAMAX + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleSpectralDomain() const {
	return (this->getPremiereComposanteSpectrale() + sSPECTRALDOMAIN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleLoiEmission() const {
	return (this->getPremiereComposanteSpectrale() + EMISSIONLAW + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTOA() const {
	return (this->getPremiereComposanteSpectrale() + sSPECTRAL + sTOA + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleBOA() const {
	return (this->getPremiereComposanteSpectrale() + sSPECTRAL + sBOA + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleAtmosphereTemperatureAbsorption() const {
	return (this->getPremiereComposanteSpectrale() + sSPECTRAL + sATM_TEMP_ABSO + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleSKYL() const {
	return (this->getPremiereComposanteSpectrale() + sSPECTRAL + sSKYL + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::clePhaseUtilisee() const {
	return (this->getComposantePhase() + sUSE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleNomFichier() const {
	return (this->getComposantePhase() + sFILENAME + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTauDiffus() const {
	return (this->getComposantePhase() + sTDIFFUS + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTauDirect() const {
	return (this->getComposantePhase() + sTDIRECT + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTauDirectPourTemperature() const {
	return (this->getComposantePhase() + sTDIRECTPOURTEMPERATURE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTauMoy() const {
	return (this->getComposantePhase() + sTMOY + this->separateurCleValeur);
}

//Lambertien, lambertien speculaire, phase externe
inline const std::string PhaseKeyMappingGenerator::cleRO() const {
	return (this->getComposantePhase() + sRO + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleThetaView() const {
	return (this->getComposantePhase() + sThetaView + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::clePhiView() const {
	return (this->getComposantePhase() + sPhiView + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleThetaIncident() const {
	return (this->getComposantePhase() + sThetaIncident + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::clePhiIncident() const {
	return (this->getComposantePhase() + sPhiIncident + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleEcartType() const {
	return (this->getComposantePhase() + sECARTTYPE + this->separateurCleValeur);
}

//Phase externe et hapke
inline const std::string PhaseKeyMappingGenerator::cleW() const {
	return (this->getComposantePhase() + sWHAPKE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffH1() const {
	return (this->getComposantePhase() + sCOEFFH1 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffC1() const {
	return (this->getComposantePhase() + sCOEFFC1 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffC3() const {
	return (this->getComposantePhase() + sCOEFFC3 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffC2() const {
	return (this->getComposantePhase() + sCOEFFC2 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffC4() const {
	return (this->getComposantePhase() + sCOEFFC4 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffH2() const {
	return (this->getComposantePhase() + sCOEFFH2 + this->separateurCleValeur);
}


//hapke2
inline const std::string PhaseKeyMappingGenerator::cleCoeffC() const {
	return (this->getComposantePhase() + sCOEFFC + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffB() const {
	return (this->getComposantePhase() + sCOEFFB + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffH() const {
	return (this->getComposantePhase() + sCOEFFH + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffB0() const {
	return (this->getComposantePhase() + sCOEFFB0 + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleCoeffTh() const {
	return (this->getComposantePhase() + sCOEFFTh + this->separateurCleValeur);
}


//Cles communes a phase externe, hapke et lambertien speculaire
inline const std::string PhaseKeyMappingGenerator::cleAmplitude() const {
	return (this->getComposantePhase() + sAMPLITUDE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::isSpecularKey() const {
	return (this->getComposantePhase() + isSpecular + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleConeAngulaire() const {
	return (this->getComposantePhase() + sCONEANGULAIRE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleIndiceRefraction() const {
	return (this->getComposantePhase() + sINDICEREFRACTION + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleAlbedo() const {
	return (this->getComposantePhase() + sALBEDO + this->separateurCleValeur);
}

// Clé speculaires
inline const std::string PhaseKeyMappingGenerator::cleUseRefraction() const {
	return (this->getComposantePhase() + sUSE_REFRACTION + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleSpecularRadianceDistribution() const {
	return (this->getComposantePhase() + sSPECULAR_RADIANCE_DISTRIBUTION + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleRefractionIndexUp() const {
	return (this->getComposantePhase() + sREFRACTION_INDEX_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleRefractionIndexDown() const {
	return (this->getComposantePhase() + sREFRACTION_INDEX_DOWN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleIntensityFactorDownToUp() const {
	return (this->getComposantePhase() + sINTENSITY_FACTOR_DOWN_TO_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleIntensityFactorUpToDown() const {
	return (this->getComposantePhase() + sINTENSITY_FACTOR_UP_TO_DOWN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAngularDivergenceDownToUp() const {
	return (this->getComposantePhase() + sANGULAR_DIVERGENCE_DOWN_TO_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAngularDivergenceUpToDown() const {
	return (this->getComposantePhase() + sANGULAR_DIVERGENCE_UP_TO_DOWN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleKPowerDownToUp() const {
	return (this->getComposantePhase() + sK_POWER_DOWN_TO_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleKPowerUpToDown() const {
	return (this->getComposantePhase() + sK_POWER_UP_TO_DOWN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAlbedoRefractionSpecularDownToUp() const {
	return (this->getComposantePhase() + sALBEDO_REFRACTION_SPECULAR_DOWN_TO_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAlbedoRefractionSpecularUpToDown() const {
	return (this->getComposantePhase() + sALBEDO_REFRACTION_SPECULAR_UP_TO_DOWN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAlbedoSpecularDownToUp() const {
	return (this->getComposantePhase() + sALBEDO_SPECULAR_DOWN_TO_UP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleAlbedoSpecularUpToDown() const {
	return (this->getComposantePhase() + sALBEDO_SPECULAR_UP_TO_DOWN + this->separateurCleValeur);
}

/**
 * Methodes specifiques a chaque fonction de phase
 */
//Lambertien speculaire
inline const std::string PhaseKeyMappingGenerator::cleAlbedoSpec() const {
	return (this->getComposantePhase() + sALBEDOSPEC + this->separateurCleValeur);
}

//RPV
inline const std::string PhaseKeyMappingGenerator::cleRho0_RPV() const {
	return (this->getComposantePhase() + sRHO0_RPV + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleK_RPV() const {
	return (this->getComposantePhase() + sK_RPV + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleG_RPV() const {
	return (this->getComposantePhase() + sG_RPV + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleH_RPV() const {
	return (this->getComposantePhase() + sH_RPV + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleThetaMax_RPV()  const {
	return (this->getComposantePhase() + sTheta_max_RPV + this->separateurCleValeur);
}

//WaterInterface
inline const std::string PhaseKeyMappingGenerator::isDatabaseInWater() const {
	return (this->getComposantePhase() + sIsDatabaseInWater + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::downwardRefractionIndexInWater() const {
	return (this->getComposantePhase() + sDownwardRefractionIndexInWater + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::upwardRefractionIndexInWater() const {
	return (this->getComposantePhase() + sUpwardRefractionIndexInWater + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::databaseNameInWater() const {
	return (this->getComposantePhase() + sDatabaseNameInWater + this->separateurCleValeur);
}

//Foliaire
inline const std::string PhaseKeyMappingGenerator::cleOmegaMin() const {
	return (this->getComposantePhase() + sOMEGAMIN + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleOmegaMax() const {
	return (this->getComposantePhase() + sOMEGAMAX + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleClumpingA() const {
	return (this->getComposantePhase() + sCLUMPINGA + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleClumpingB() const {
	return (this->getComposantePhase() + sCLUMPINGB + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLAD() const {
	return (this->getComposantePhase() + sLAD + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleDimensionFoliaire() const {
	return (this->getComposantePhase() + sDIMFOL + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleRoSuperieur() const {
	return (this->getComposantePhase() + sROSUP + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleRoInferieur() const {
	return (this->getComposantePhase() + sROINF + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTo() const {
	return (this->getComposantePhase() + sTO + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleToInf() const {
	return (this->getComposantePhase() + sTOINF + this->separateurCleValeur);
}


inline const std::string PhaseKeyMappingGenerator::cleNinf() const {
	return (this->getComposantePhase() + sNINF + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleNsup() const {
	return (this->getComposantePhase() + sNSUP + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTypeRI() const {
	return (this->getComposantePhase() + sTYPERI + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleKoInf() const {
	return (this->getComposantePhase() + sKOINF + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTypeRS() const {
	return (this->getComposantePhase() + sTYPERS + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleKoSup() const {
	return (this->getComposantePhase() + sKOSUP + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleALA() const {
	return (this->getComposantePhase() + sALA + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleThetaM() const {
	return (this->getComposantePhase() + sTHETAM + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleEccentricity() const {
	return (this->getComposantePhase() + sECCENTRICITY + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleIndiceRugositeInf() const {
	return (this->getComposantePhase() + sINDRUGINF + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleIndiceRugositeSup() const {
	return (this->getComposantePhase() + sINDRUGSUP + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleSigma() const {
	return (this->getComposantePhase() + sSIGMA + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleEquivalentFrontLambertianIndex() const {
	return (this->getComposanteNonSpectralePhase() + sEQUIVALENTFRONTLAMBERTIAN + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleEquivalentBackLambertianIndex() const {
	return (this->getComposanteNonSpectralePhase() + sEQUIVALENTBACKLAMBERTIAN + this->separateurCleValeur);
}

/**
 * Cles de recherches pour des objets specifique d'une scene
 */
//Elements de type sol, eau, route, plots (parcelle de vegetation), toit, mur, tronc, feuille
inline const std::string PhaseKeyMappingGenerator::cleTemperatureMin() {
	return (this->getComposanteElement() + sTMIN + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleTemperatureMax() {
	return (this->getComposanteElement() + sTMAX + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLeafTemperatureMin() {
	return (this->getComposanteElement() + this->convertirEnString(this->numeroFonctionDePhase) + sTMIN + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLeafTemperatureMax() {
	return (this->getComposanteElement() + this->convertirEnString(this->numeroFonctionDePhase) + sTMAX + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleLeafIdTemperature() {
	return (this->getComposanteElement() + this->convertirEnString(this->numeroFonctionDePhase) + sIDTEMPERATURE + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleFonctionPhaseAirMoleculesIndice() const {
	return (sPHASE + PhaseKeyMappingGenerator::nodePhaseSolarNoiseSimu + sAFPMOLECULES + this->separateurCleValeur);
}

inline const std::string PhaseKeyMappingGenerator::cleFonctionPhaseAirAerosolsIndice() const {
	return (sPHASE + PhaseKeyMappingGenerator::nodePhaseSolarNoiseSimu + sAFPAEROSOLS + this->separateurCleValeur);
}

//Elements specifique a l'element toit
inline const std::string PhaseKeyMappingGenerator::cleRoofBoundingWallsTemperature() {
	return (this->getComposanteElement() + sBOUNDINGWALLS + this->separateurCleValeur);
}

// Fluorescence
inline const std::string PhaseKeyMappingGenerator::cleUseFluorescence() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sIsFluorescent + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleForceYields() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sIsForcedYields + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleUseCombinedYields() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sUseCombinedYields + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::getComposanteFluorescence() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sFluorescence);
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceFileName() const {
	return (this->getComposanteFluorescence() + sFluorescenceFileName + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceNbCases() const {
	return (this->getComposanteFluorescence() + sNbYieldCases + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceYieldPSI(unsigned int i) const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sYieldPSI << i << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceYieldPSII(unsigned int i) const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sYieldPSII << i << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticUsage() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sUseBioClimaticProfile << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleTopFluorescenceBioClimaticUsage() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sTopUseBioClimaticProfile << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleBottomFluorescenceBioClimaticUsage() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sBottomUseBioClimaticProfile << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::getComposanteBioClimaticProfile() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sBioClimatic;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::getTopComposanteBioClimaticProfile() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sTopBioClimatic;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::getBottomComposanteBioClimaticProfile() const {
	std::ostringstream oss;
	oss << this->getComposanteFluorescence() << sBottomBioClimatic;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticFile() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sBioClimaticFileName << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleTopFluorescenceBioClimaticFile() const {
	std::ostringstream oss;
	oss << this->getTopComposanteBioClimaticProfile() << sBioClimaticFileName << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleBottomFluorescenceBioClimaticFile() const {
	std::ostringstream oss;
	oss << this->getBottomComposanteBioClimaticProfile() << sBioClimaticFileName << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticLinkedPropertyType() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sLinkedOpticalPropertyType << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticLinkedPropertyIndex() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sLinkedOpticalPropertyIndex << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceTopBioClimaticLinkedPropertyType() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sTopLinkedOpticalPropertyType << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceTopBioClimaticLinkedPropertyIndex() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sTopLinkedOpticalPropertyIndex << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBottomBioClimaticLinkedPropertyType() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sBottomLinkedOpticalPropertyType << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBottomBioClimaticLinkedPropertyIndex() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sBottomLinkedOpticalPropertyIndex << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticMinLayer() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sMinLayerAltitude << this->separateurCleValeur;
	return oss.str();
}
inline const std::string PhaseKeyMappingGenerator::cleFluorescenceBioClimaticMaxLayer() const {
	std::ostringstream oss;
	oss << this->getComposanteBioClimaticProfile() << sMaxLayerAltitude << this->separateurCleValeur;
	return oss.str();
}

inline const std::string PhaseKeyMappingGenerator::cleThermalHotSpotFactor() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sThermalHotSpotFactor + this->separateurCleValeur);
}
inline const std::string PhaseKeyMappingGenerator::cleDiffuseTransmittanceAccelerationFactor() const {
	return (sPHASE + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sDiffuseTransmittanceAccelerationFactor + this->separateurCleValeur);
}

// YAML indices equivalent table
inline const std::string PhaseKeyMappingGenerator::cleEquivalentYAMLIndice() const {
	return (sPHASE + sYAML + getIntitulePhase() + this->convertirEnString(this->numeroFonctionDePhase) + sYaMLEquivalence + this->separateurCleValeur);
}

#endif /* PHASEKEYMAPPINGGENERATOR_HPP_ */
