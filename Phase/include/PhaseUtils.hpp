/*! Time-stamp: <@(#) PhaseUtils.hapkeParameterh   10/09/04 - 10:00:50   VBT MAGELLIUM>
 *********************************************************************
 *  @file    : PhaseUtils.hapkeParameterh
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 07/03/2005
 *
 *  Description : Contient les structures et quelques fonctions necessaires au module Phase.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.5.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.5.0 21/04/2008  MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture)
 * V 4.4.0 22/04/2008  Thomas Gallineri : Redefinition de la macro M_PI
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

/**
 *  @ingroup Phase
 *
 *  Fichier contenant des outils divers pour phase.
 */

#ifndef _PHASEUTILS
#define _PHASEUTILS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

#include "libDartBase.h"
#include "libDartEnv.h"
#include "../../libDartGeometry/include/Vecteur.hpp"

// Pour une direction'n',le tableau donne le triplet Teta,Phi,angle solide
/**
 * TODO remplacer par DirectionSpherique
 */
//typedef struct direction
//{
//  double teta;
//  double phi;
//  double angle_solide;
//} struct_dir;
// -------------------------------------------------------------------------
//extern   struct_dir *tab_dir;
// -------------------------------------------------------------------------
class PhaseUtils {
private:
	static std::string _outputFolder;

	static double _deg2Rad;
	static std::vector<double> _cosineVector;
	static std::vector<double> _sineVector;
	static std::vector<double> _degreesToRadiansVector;

public:
	static void initStaticVectors() {
		_deg2Rad = M_PI / 180.;

		_degreesToRadiansVector.resize(721);
		_sineVector.resize(721);
		_cosineVector.resize(721);

		for (unsigned int i = 0; i <= 720; i++) {
			_degreesToRadiansVector[i] = _deg2Rad * (0.5 * i);
			_sineVector[i] = sin(_degreesToRadiansVector[i]);
			_cosineVector[i] = cos(_degreesToRadiansVector[i]);
		}
	}

	static double getStaticCosine(const int & pos) {
		return _cosineVector[pos];
	}

	static double getStaticSine(const int & pos) {
		return _sineVector[pos];
	}

	static double getStaticDeg2Rad() {
		return _deg2Rad;
	}

	static const Vecteur<double> conv_direction(const double & theta,
												const double & phi);

	// MAG-EMN : Surfaces translucides (enrichissement des fonctions de lecture)
	/**
	 * Fonction permettant de recuperer des donnees sur les proprietes optiques pour la mise a jour du fichier
	 * coeff_diff.xml.
	 *
	 * @param string phasePath : contient le path pour mettre la jour la variable au bon endroi dans le fichier xml.
	 * @param int j 				: numero de la fonction de phase en cours de traitement.
	 * @param double albedo 			: La valeur calculee de l'albedo doit etre mise a jour dans le fichier xml.
	 * @param double albedoSpecular 	: La valeur calculee de l'albedoSpecular doit etre mise a jour dans le fichier xml.
	 * @param double dTauMoy 		: La valeur calculee de la transmittance moyenne (direct + diffus) doit etre mise a jour dans le fichier xml.
	 *
	 */
	static void updatePropertyForOpticalPropertyInCoeffDiffXmlFile(	const string & phasePath,
																	const int & j,
																	const double & albedo,
																	const double & albedo_spec,
																	const double & dTauMoy);

	/**
	 * Fonction permettant de recuperer des donnees sur l'atmosphere pour la mise a jour du fichier
	 * Atmosphere.xml.
	 *
	 * @param string phasePath : contient le path pour mettre la jour la variable au bon endroi dans le fichier xml.
	 * @param int j : valeur a mettre a jour.
	 *
	 */
	static void updatePropertyForAtmosphereXmlFile(	const string & phasePath,
													const int & j);

	static double computeRoughness(	double & PSIfs,
									double & roughness,
									double & roughnessIndex);

	static const std::string & getOutputFolder() {
		return _outputFolder;
	}

	static void setOutputFolder(const std::string & folder) {
		_outputFolder = folder;
	}
};

#endif // _PHASEUTILS
