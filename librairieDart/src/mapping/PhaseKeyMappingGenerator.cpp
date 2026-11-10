/*
 * PhaseKeyMappingGenerator.cpp
 *
 *  Created on: 5 juil. 2010
 *      Author: Administrateur
 */

#include "../../include/mapping/PhaseKeyMappingGenerator.hpp"

using namespace std;

string PhaseKeyMappingGenerator::nodePhaseSolarNoiseSimu = "";

const string PhaseKeyMappingGenerator::determinerElement() {
	string elementExtrait;
	//On fait l'acquisition du type d'element de la cle en fonction du type d'element qui a ete enregistre dans la classe
	if (this->typeElement == globalReferences::WALL) {
		elementExtrait = sWALLS;
	} else if (this->typeElement == globalReferences::SOIL) {
		elementExtrait = sSOIL;
	} else if (this->typeElement == globalReferences::ROOF) {
		elementExtrait = sROOFS;
	} else if (this->typeElement == globalReferences::WATER) {
		elementExtrait = sWATER;
	} else if (this->typeElement == globalReferences::ROAD) {
		elementExtrait = sROADS;
	} else if (this->typeElement == globalReferences::PLOT_OPAQUE) {
		elementExtrait = sPLOTSOPAQUE;
	} else if (this->typeElement == globalReferences::TRUNC) {
		elementExtrait = sTRUNKS;
	}

	else if (this->typeElement == globalReferences::CANOPY) {
		elementExtrait = sLEAF;
	} else {
		elementExtrait = "";
	}
	return (elementExtrait);
}

const string PhaseKeyMappingGenerator::getIntitulePhase() const {
	string intitulePhase;
	//Voir le fichier GeneralDef.h pour le numero de chaque fonction de phase
	switch (this->typeFonctionPhase) {
		//Fonction de phase Lambertien
		case 0:
			intitulePhase = sTYPELAMBERTIEN;
			break;
			
			//Fonction de phase Hapke speculaire
		case 2:
			intitulePhase = sTYPEHAPKE;
			break;
			
			//Fonction de phase Phase Externe
		case 3:
			intitulePhase = sTYPEPHASEEXTERNE;
			break;
			
			//Fonction de phase RPV
		case 4:
			intitulePhase = sTYPERPV;
			break;
			
			//Fonction de Foliaire
		case 5:
			intitulePhase = sTYPELEAF;
			break;
			
			//Fonction de Air
		case 7:
			intitulePhase = sTYPEAIR;
			break;
		case 8:
			intitulePhase = sTYPEWATERINTERFACE;
			break;
		case 9:
			intitulePhase = sTYPEWATERVOLUME;
			break;

		case 11: // PHASE EXTERN
			intitulePhase = sTYPEPHASEEXTERN;
			break;
			
//Aucune precision sur la fonction de phase utilisee
		default:
			intitulePhase = "";
			break;
	}
	return (intitulePhase);
}
