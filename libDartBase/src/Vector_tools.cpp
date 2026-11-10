/*!
 *********************************************************************
 *  @file    : Vector_tools.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : GPR                               Date: 17/02/2006
 *
 *  Description : Contient diverses fonctions de vecteurs communes aux modules DART.
 *
 *********************************************************************
 * Historique :
 * 
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.1 22/04/2008  Thomas Gallineri : Redefinition de la macro M_PI
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 045/04/2006  CLF : Livraison : fichier independant
 *
 *********************************************************************
 */
#include "../include/Vector_tools.hpp"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vecteur normalise_vecteur(vecteur v) {
	float norm = norme(v);
	if (norm != 0) {
		v.x /= norm;
		v.y /= norm;
		v.z /= norm;
	}
	if (norm == 0) {
		v.x = 0;
		v.y = 0;
		v.z = 0;
	}
	return v;
}

// -----------------------------------------------------------------------------
// Fonction de calcul de la norme d'un vecteur
// -----------------------------------------------------------------------------
float norme(const vecteur &v) {
	return ((float) sqrt(prod_scal(v, v)));
}

// -----------------------------------------------------------------------------
// Fonction de calcul du produit scalaire de 2 vecteurs
// -----------------------------------------------------------------------------
double prod_scal(	const vecteur &v1,
					const vecteur &v2) {
	return (double) v1.x * (double) v2.x + (double) v1.y * (double) v2.y + (double) v1.z * (double) v2.z;
}

// -----------------------------------------------------------------------------
// Fonction de calcul du produit vectoriel de 2 vecteurs.
// ----------------------------------------------------------------------------- 
void Vectoriel(	const vecteur &v1,
				const vecteur &v2,
				vecteur *tmp) {
	tmp->x = v1.y * v2.z - v1.z * v2.y;
	tmp->y = v1.z * v2.x - v1.x * v2.z;
	tmp->z = v1.x * v2.y - v1.y * v2.x;
}

// -------------------------------------------------------------------------------- 
// Fonction de calcul du produit vecteur * scalaire.
// -------------------------------------------------------------------------------- 
vecteur mult_vect(	const vecteur &v1,
					float scalaire) {
	vecteur res;
	
	res.x = scalaire * v1.x;
	res.y = scalaire * v1.y;
	res.z = scalaire * v1.z;
	return res;
}

// -----------------------------------------------------------------------------
// Fonction de calcul de la difference de deux vecteurs 
// -----------------------------------------------------------------------------
vecteur diff_vect(	const vecteur &v1,
					const vecteur &v2) {
	vecteur res;
	res.x = v2.x - v1.x;
	res.y = v2.y - v1.y;
	res.z = v2.z - v1.z;
	return res;
}

//// -----------------------------------------------------------------------------
////CESB-TT-208-clf rempl par distance_c
//// -----------------------------------------------------------------------------
//float Calcul_distance (const vecteur &v1, 
//					   const vecteur &v2)
//{
//	double d1 = pow ((v2.x - v1.x), 2);
//	double d2 = pow ((v2.y - v1.y), 2);
//	double d3 = pow ((v2.z - v1.z), 2);
//	return sqrt(d1+d2+d3);
//}

// -------------------------------------------------------------------------------- 
// Calcul de la distance entre 2 points.
//CESB-TT-208-clf modif signature
// -------------------------------------------------------------------------------- 
float distance_c(	const vecteur &point1,
					const vecteur &point2) {
	return norme(diff_vect(point1, point2));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vecteur Calcul_vect_dir_norm(	double theta,
								double phi) {
	vecteur res;
	theta = M_PI / 180.0 * theta;
	phi = M_PI / 180.0 * phi;
	res.z = cos(theta);
	double d1 = sin(theta);
	res.y = d1 * (sin(phi));
	d1 = sin(theta);
	res.x = d1 * cos(phi);
	return res;
}

// -------------------------------------------------------------------------------- 
// Fonction de calcul du d?�terminant de 2 vecteurs 2D (plan xy).
// -------------------------------------------------------------------------------- 
float det1(	const vecteur &v1,
			const vecteur &v2) {
	return v1.x * v2.y - v1.y * v2.x;
}
// -------------------------------------------------------------------------------- 

// Fonction de calcul du d?�terminant de 2 vecteurs 2D (plan yz).
// -------------------------------------------------------------------------------- 
float det2(	const vecteur &v1,
			const vecteur &v2) {
	return v1.y * v2.z - v1.z * v2.y;
}
// -------------------------------------------------------------------------------- 

// Fonction de calcul du d?�terminant de 2 vecteurs 2D (plan zx).
// -------------------------------------------------------------------------------- 
float det3(	const vecteur &v1,
			const vecteur &v2) {
	return v1.z * v2.x - v1.x * v2.z;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float min(	float a,
			float b) {
	float c;
	c = (a < b) ? a : b;
	return c;
}

// --------------------------------------------------------------------------------
// calcul du signe d'une expression: retourne 1 ou -1
// --------------------------------------------------------------------------------
int signe(float T) {
	if (T > 0)
		return 1;
	return -1;
}

// --------------------------------------------------------------------------------
/* Nouvelle fonction qui rearrange un tableau dans l'ordre geometrique direct 
 * et enl?�ve les valeurs redondantes*/
// --------------------------------------------------------------------------------
int nouveau_epure_tableau(	vecteur *Tableau,
							int *Taille) {
	vecteur V_prov, t, V_norm, Tableau_temp[20];
	int io, jo, a, test;
	float s;
	
	// Definition du vecteur normal a l'ecran.
	V_norm.x = V_norm.y = 0;
	V_norm.z = 1;
	test = 1;
	if ((*Taille) > 1) {
		for (io = 0; io <= (*Taille - 2); io++) {
			for (jo = io + 1; jo <= (*Taille - 1); jo++) {
				if (((Tableau[jo].x - Tableau[io].x) <= 0.001) && ((Tableau[jo].x - Tableau[io].x) >= -0.001)
						&& ((Tableau[jo].y - Tableau[io].y) <= 0.001) && ((Tableau[jo].y - Tableau[io].y) >= -0.001)) {
					if (jo != (*Taille - 1)) {
						for (a = jo; a <= (*Taille - 2); a++) {
							// On decale les indices quand il y a 2 valeurs qui sont quasiment identiques.
							Tableau[a] = Tableau[a + 1];
						}
						
						(*Taille)--;
						jo--;
					} else {
						(*Taille)--;
						jo--;
					}
				}
			}
		}
		for (io = 0; io < (*Taille - 1); io++) {
			Tableau_temp[io].x = Tableau[io + 1].x - Tableau[0].x;
			Tableau_temp[io].y = Tableau[io + 1].y - Tableau[0].y;
			Tableau_temp[io].z = Tableau[io + 1].z - Tableau[0].z;
		}
		for (io = 0; io <= (*Taille - 3); io++) {
			for (jo = (io + 1); jo <= (*Taille - 2); jo++) {
				Vectoriel(Tableau_temp[io], Tableau_temp[jo], &t);
				//clf remark #1572 *3
				/*if(fabs(t.x) < EPSILON && fabs(t.y) < EPSILON && fabs(t.z) < EPSILON)*/
				if (t.x == 0 && t.y == 0 && t.z == 0) {
					test = 0;
					/* On place un test pour voir si on n'est pas parallele*/
				}
				s = prod_scal(t, V_norm);
				if (s < 0) {
					/* si le produit vectoriel des 2 vecteurs n'est pas dans le meme sens que V_norm, alors on ?�change l'ordre des 2 vecteurs*/
					V_prov = Tableau_temp[io];
					Tableau_temp[io] = Tableau_temp[jo];
					Tableau_temp[jo] = V_prov;
					
				}
			}
		}
		for (io = 0; io <= (*Taille - 2); io++) {
			Tableau[io + 1].x = Tableau_temp[io].x + Tableau[0].x;
			Tableau[io + 1].y = Tableau_temp[io].y + Tableau[0].y;
			Tableau[io + 1].z = Tableau_temp[io].z + Tableau[0].z;
		}
		
	}
	return test;
}

/* -------------------------------------------- */
/* --- fonction qui calcule l'aire d'un ------- */
/* --- polygone dont les points sont donnes --- */
/* -------- dans un certain sens -------------- */
/* -------------------------------------------- */

double Calcul_aire(	vecteur Tabl[],
					int Taille) {
	double R, A;
	vecteur Prod, Vec[16]/* Modif a cause probleme allocation dynamique*/;
	int g, m;
	R = 0;
	
	Taille++;/*attention Taille est la taille-1 du tablea : c.f.modif Pierre BOYAT*/
	
	if (Taille < 3)
		return 0;
	else {
		for (m = 0; m <= (Taille - 2); m++) {
			Vec[m].x = Tabl[m + 1].x - Tabl[0].x;
			Vec[m].y = Tabl[m + 1].y - Tabl[0].y;
			Vec[m].z = Tabl[m + 1].z - Tabl[0].z;
		}
		
		for (g = 0; g <= (Taille - 3); g++) {
			Vectoriel(Vec[g], Vec[g + 1], &Prod);
			A = 0.5 * sqrt(prod_scal(Prod, Prod));
			R = R + A;
		}
		return (R);
	}
}
// --------------------------------------------------------------------------------

