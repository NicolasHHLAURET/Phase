/*! Time-stamp: <@(#) PhaseUtils.c   10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : PhaseUtils.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description :  Implémentation des Nouvelles methodes d'acces aux parametres Phase
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.4.0 21/04/2008  MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture) + suppression anciennes fonctions de lecture
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.2 17/02/2006  CLF : Revision
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.1 07/07/2005  VBT : Agrandissement du logo
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0   07/03/2005  VBT : MAG-TT-004 : Implementation nouvelles fonctions de lecture des param�tres Phase
 *
 *********************************************************************
 */

#include "../include/PhaseUtils.hpp"

std::string PhaseUtils::_outputFolder = "";

double PhaseUtils::_deg2Rad;
std::vector<double> PhaseUtils::_cosineVector;
std::vector<double> PhaseUtils::_sineVector;
std::vector<double> PhaseUtils::_degreesToRadiansVector;

//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------------
const Vecteur<double> PhaseUtils::conv_direction(	const double & zenith,
													const double & azimuth) {
	double zenithInRadians = CONV_DEG_RADIAN * zenith;
	double azimuthInRadians = CONV_DEG_RADIAN * azimuth;

	// Problemes des angles remarquables
	double cosineZenith = cos(zenithInRadians);
	double cosineAzimuth = cos(azimuthInRadians);
	double sineZenith = sin(zenithInRadians);
	double sineAzimuth = sin(azimuthInRadians);

	// Passage au repere global (x,y,z)
	Vecteur<double> result(sineZenith * cosineAzimuth, sineZenith * sineAzimuth, cosineZenith);
	//= normalise_vecteur (res);
	return result.normaliser();
}

//------------------------------------------------------------------------------
// MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture)
/**
 * Fonction permettant de recuperer des donnees sur les proprietes optiques pour la mise a jour du fichier
 * phase.xml.
 *
 * @param string phasePath	: contient le path pour mettre la jour la variable au bon endroi dans le fichier xml.
 * @param int j 				: numero de la fonction de phase en cours de traitement.
 * @param double albedo 		: La valeur calculee de l'albedo doit etre mise a jour dans le fichier xml.
 * @param double albedoSpecular : La valeur calculee de l'albedoSpecular doit etre mise a jour dans le fichier xml.
 * @param double dTauMoy 		: La valeur calculee de la transmittance moyenne (direct + diffus) doit etre mise a jour dans le fichier xml.
 *
 */
void PhaseUtils::updatePropertyForOpticalPropertyInCoeffDiffXmlFile(const string & phasePath,
																	const int & modelIndex,
																	const double & albedo,
																	const double & specularAlbedo,
																	const double & dTauMoy) {
	ostringstream oss;
	oss << modelIndex;
	string pathToUpdate = "Coeff_diff." + phasePath;
	pathToUpdate += oss.str();
	string pathToUpdateProperty;
	if (fabs(albedo - (-1.0)) > EPSILON) {
		pathToUpdateProperty = pathToUpdate + "].rhoMoy"; // chemin a suivre a l'interieur du xml.
		string temp = "";
		oss.str(temp);
		oss << albedo;
		temp = oss.str();
		(DartEnvParamPropertiesToUpdate::addParamPropertyToUpdate(pathToUpdateProperty, temp));
	}
	if (fabs(specularAlbedo - (-1.0)) > EPSILON) {
		pathToUpdateProperty = pathToUpdate + "].rhoMoySpec";
		string temp = "";
		oss.str(temp);
		oss << specularAlbedo;
		temp = oss.str();
		DartEnvParamPropertiesToUpdate::addParamPropertyToUpdate(pathToUpdateProperty, temp);
	}
	if (fabs(dTauMoy - (-1.0)) > EPSILON) {
		pathToUpdateProperty = pathToUpdate + "].tauMoy";
		string temp = "";
		oss.str(temp);
		oss << dTauMoy;
		temp = oss.str();
		DartEnvParamPropertiesToUpdate::addParamPropertyToUpdate(pathToUpdateProperty, temp);
	}
}

double PhaseUtils::computeRoughness(double & PSIfs,
									double & roughness,
									double & roughnessIndex) {
	if (roughnessIndex <= 0) {
		return roughness;
	} else {
		double epsilon = 0.001;

		if (PSIfs > M_PI - epsilon) {
			PSIfs = M_PI - epsilon;
		} else if (PSIfs < M_PI + epsilon) {
			PSIfs = M_PI + epsilon;
		}
		return roughness * (exp(-roughnessIndex * tan(fabs(PSIfs))));
	}
}

// 17 Feb 2012:
// Since Occupied direction is occupied by embedded direction, so the integration of this direction needs a discretization
// around the embedded direction, windmill method used in direction discretization is used.

//------------------------------------------------------------------------------
