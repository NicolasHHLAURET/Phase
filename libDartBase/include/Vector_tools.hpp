/*! 
 *********************************************************************
 *  @file    : Veector_tools.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 17/02/2006
 *
 *  Description : Contient diverses fonctions de vecteurs communes aux modules DART.
 *
 *********************************************************************
 * Historique :
 * 
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison: creation fichier independant
 *
 *********************************************************************
 */

/*****************************************************/
/*                                                   */
/*     Bibliotheque des librairies utilitaires 		*/
/*     pour les vecteurs    						*/
/*     Date : 14/05/93                               */
/*     Dernieres modifications 05/04/2006             */
/*****************************************************/

#ifndef VECTOR_TOOLS_H_
#define VECTOR_TOOLS_H_

#include <cmath>

typedef struct vecteur {
	float x;
	float y;
	float z;
} vecteur;

//--------------- OPs sur VECTEUR

vecteur normalise_vecteur(vecteur);

// Calcul the norm of a vector.
float norme(const vecteur &v);

// Calcul the vectoriel product.
void Vectoriel(	const vecteur &v1,
				const vecteur &v2,
				vecteur *tmp);

// Calcul the scalar result of the two vector in parameter.
double prod_scal(	const vecteur &v1,
					const vecteur &v2);

// Fonction de calcul du produit d'un scalaire par un vecteur
//vecteur prod_scalvect (float, vecteur);

// Fonction de calcul de la difference de deux vecteurs 
vecteur diff_vect(	const vecteur &v1,
					const vecteur &v2);

//CESB-TT-208-clf rempl. par distance_c
//// Determine the distance between two vector
//float Calcul_distance (const vecteur &v1, 
//					   const vecteur &v2) ;

vecteur Calcul_vect_dir_norm(	double,
								double);

vecteur mult_vect(	const vecteur &,
					float);

// Fonction de calcul du determinant de 2 vecteurs 2D (plan xy).
float det1(	const vecteur &v1,
			const vecteur &v2);

// Fonction de calcul du determinant de 2 vecteurs 2D (plan yz).
float det2(	const vecteur &v1,
			const vecteur &v2);

// Fonction de calcul du determinant de 2 vecteurs 2D (plan zx).
float det3(	const vecteur &v1,
			const vecteur &v2);

// Calcul de la distance entre 2 points.
//CESB-TT-208-clf modif signature
float distance_c(	const vecteur&,
					const vecteur&);

float min(	float,
			float);

// calcul du signe d'une expression: retourne 1 ou -1
int signe(float);

//Nouvelle fonction qui rearrange un tableau dans l'ordre geometrique direct 
// * et enleve les valeurs redondantes
int nouveau_epure_tableau(	vecteur *,
							int *);

double Calcul_aire(	vecteur *,
					int); // Attention : Taille est la taille-1 du tableau.
					
#endif /*VECTOR_TOOLS_H_*/
