/*! Time-stamp: <@(#) Lib_math.c   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : Lib_math.c
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : GPR                               Date: 07/03/2005
 *
 *  Description : Utilitaire permettant de faire divers controle et de quitter proprement.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0   07/03/2005  GPR : First Revision
 *
 *********************************************************************
 */

/**************************************************/
/*                                                */
/*    Bibliotheque des librairies "Mathematiques" */
/*    Date   : 28/10/94                           */
/*                                                */
/**************************************************/

#include "../include/Lib_math.h"

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define RNMX (1.0 - 1.2e-7)
#define MASK 123459876

#define EPSILON_ERROR 1e-5

using namespace std;

/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/

/*
 "Minimal" random number generator of Park and Miller
 with Bays-Durham shuffle and added safeguards.
 Returns a uniform random deviate between 0.0 and 1.0
 Call with idum a negative integer to initialize
 Don't alter idum between sucessive deviates in a sequence
 RNMX should approximate the largest floating value that is less 1
 */
//----------------------------------------------------
float random1(long *idum) {
	static long iy = 0;
	static long iv[NTAB];
	int j;
	long k;
	float temp;
	
	if (*idum <= 0 || !iy) // Initialize
			{
		if (-(*idum) < 1)
			*idum = 1; // Be sure to prevent idum=0
		else
			*idum = -(*idum);
		
		for (j = NTAB + 7; j >= 0; j--) // Load the shuffle table
				{ //    after 8 warm-ups
			k = (*idum) / IQ;
			*idum = IA * (*idum - k * IQ) - IR * k;
			if (*idum < 0)
				*idum += IM;
			if (j < NTAB)
				iv[j] = *idum;
		}
		iy = iv[0];
	}
	k = (*idum) / IQ; // Start here when not initializing
	*idum = IA * (*idum - k * IQ) - IR * k; // Compute idum=(IA*idum)%IM
	if (*idum < 0)
		*idum += IM;
	j = (int) (iy / NDIV); // Will be in the range 0..NTAB-1
	iy = iv[j]; // Output previously stored value
	iv[j] = *idum; //  and refill the shuffle table
	if ((temp = ((float) AM * iy)) > RNMX)
		return RNMX; // Because users don't except
	else
		return temp; // endpoint values
}
//----------------------------------------------------
/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/
/*
 "Minimal" random number generator of Park and Miller.
 -> Returns an uniform random deviate in [0;1].
 -> Set or reset 'idum' to any integer value (except  the
 unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive
 deviates in a sequence.
 */
//----------------------------------------------------
float random0(long *idum)

{
	long k;
	float ans;
	
	*idum ^= MASK;
	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0)
		*idum += IM;
	//clf
	ans = ((float) AM * (*idum));
	*idum ^= MASK;
	return ans;
}

const float randomGenerator(int & graine) {
	long k;
	float ans;
	
	graine ^= MASK;
	k = graine / IQ;
	graine = IA * (graine - k * IQ) - IR * k;
	if (graine < 0) {
		graine += IM;
	}
	//clf
	ans = ((float) AM * graine);
	graine ^= MASK;
	return ans;
}
const float randomGenerator(long int & graine) {
	long k;
	float ans;
	
	graine ^= MASK;
	k = graine / IQ;
	graine = IA * (graine - k * IQ) - IR * k;
	if (graine < 0) {
		graine += IM;
	}
	//clf
	ans = ((float) AM * graine);
	graine ^= MASK;
	return ans;
}

//----------------------------------------------------
/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/
/*
 Returns an uniform random deviate in [borne_inf..borne_sup].
 -> Set or reset 'idum' to any integer value (except  the
 unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive
 deviates in a sequence.
 */
//----------------------------------------------------
float ran0(	long *idum,
			int borne_inf,
			int borne_sup) {
	long k;
	float ans;
	
	*idum ^= MASK;
	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0)
		*idum += IM;
	ans = (float) AM * (*idum) * (borne_sup - borne_inf) + borne_inf;
	*idum ^= MASK;
	return ans;
}
//----------------------------------------------------
/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/
/*
 Returns a normally distributed deviate for
 -> mean = 0
 -> variance = 1
 -> using random1(idum) as the  source of uniform deviates.
 */
//----------------------------------------------------
float gasdev(long *idum) {
	static int iset = 0;
	static float gset;
	
	if (iset == 0) {
		float fac, rsq, v1, v2;
		do {
			//clf
			v1 = 2.0f * random1(idum) - 1.0f;
			v2 = 2.0f * random1(idum) - 1.0f;
			rsq = v1 * v1 + v2 * v2;
		}
		//clf fabs
		while (rsq >= 1.0 || fabs(rsq) < EPSILON_ERROR);
		fac = sqrtf(-2.0f * logf(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	} else {
		iset = 0;
		return gset;
	}
}
//----------------------------------------------------
/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/

/*
 Returns a normally distributed deviate for
 -> mean = m
 -> ecart-type = sigma
 -> using random1(idum) as the  source of uniform deviates
 RQ: normal=gasdev*sigma+m
 */
//----------------------------------------------------
float normal(	long *idum,
				float m,
				float sigma) {
	static int iset = 0;
	static float gset;
	
	if (iset == 0) {
		float fac, rsq, v1, v2;
		do {
			//clf
			v1 = 2.0f * random1(idum) - 1.0f;
			v2 = 2.0f * random1(idum) - 1.0f;
			rsq = v1 * v1 + v2 * v2;
		}
		//clf
		while (rsq >= 1.0 || fabs(rsq) < EPSILON_ERROR);
		fac = sqrtf(-2.0f * logf(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac * sigma + m;
	}
	iset = 0;
	return gset * sigma + m;
}

//----------------------------------------------------
/****************************/
/* RANDOM NUMBER GENERATORS */
/****************************/

/*
 Returns a normally distributed deviate for
 -> mean = m
 -> ecart-type = sigma
 -> using random1(idum) as the  source of uniform deviates
 RQ: normal=gasdev*sigma+m
 */
//----------------------------------------------------
float normalPercent(long *idum,
					float m,
					float sigmaPercent) {
	static int iset = 0;
	static float gset;
	
	if (iset == 0) {
		float fac, rsq, v1, v2;
		do {
			//clf
			v1 = 2.0f * random1(idum) - 1.0f;
			v2 = 2.0f * random1(idum) - 1.0f;
			rsq = v1 * v1 + v2 * v2;
		}
		//clf
		while (rsq >= 1.0 || fabs(rsq) < EPSILON_ERROR);
		fac = sqrtf(-2.0f * logf(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return (1.f + v2 * fac * sigmaPercent) * m;
	}
	iset = 0;
	return (1. + gset * sigmaPercent) * m;
}

//----------------------------------------------------
/************************************/
/* METHODES D'INTERPOLATION lineaire*/
/************************************/

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
double Interpolation_lineaire(	double nu_ou_lambda,
								double **Tab_spectral_tmp,
								unsigned int indice,
								unsigned int NumParam) {
	if (fabs(Tab_spectral_tmp[indice + 1][0] - Tab_spectral_tmp[indice][0]) == 0) {
		printf("\nWARNING : Division by 0 in Interpolation_lineaire() !!!\n");
	}
	
	return ((Tab_spectral_tmp[indice][NumParam] * (Tab_spectral_tmp[indice + 1][0] - nu_ou_lambda)
			+ Tab_spectral_tmp[indice + 1][NumParam] * (nu_ou_lambda - Tab_spectral_tmp[indice][0]))
			/ (Tab_spectral_tmp[indice + 1][0] - Tab_spectral_tmp[indice][0]));
}

//----------------------------------------------------
/************************************/
/* METHODES D'INTERPOLATION lineaire*/
/* 20100303 : prototype pour        */
/* la nouvelle version de Dart      */
/************************************/

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
double Interpolation_lineaire(	const double & nu_ou_lambda,
								const vector<vector<double> > & Tab_spectral_tmp,
								const unsigned int & indice,
								const unsigned int & NumParam) {
	if (fabs(Tab_spectral_tmp[indice + 1][0] - Tab_spectral_tmp[indice][0]) == 0) {
		cout << endl << "WARNING : Division by 0 in Interpolation_lineaire() !!!" << endl;
	}
	
	return ((Tab_spectral_tmp[indice][NumParam] * (Tab_spectral_tmp[indice + 1][0] - nu_ou_lambda)
			+ Tab_spectral_tmp[indice + 1][NumParam] * (nu_ou_lambda - Tab_spectral_tmp[indice][0]))
			/ (Tab_spectral_tmp[indice + 1][0] - Tab_spectral_tmp[indice][0]));
}


double Interpolation_lineaire(	const double & borneMin,
								const double & borneMax,
								const double & borneinterpolated,
								const double & valMin,
								const double & valMax) {

	if (fabs(borneMax - borneMin) == 0) {
		return valMin;
	}
	return ((valMin * (borneMax - borneinterpolated) + valMax * (borneinterpolated -borneMin))
			/ (borneMax - borneMin));
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// FA-65 : ELG : correction indices pour copier dans tab_spectral les seules valeurs de tab_spectral_data qui nous interessent pour l'interpolation
// FA-49 : ELG : correction bug interpolation trapeze
/**
 * @param borne_min : bornemin de l'interpolation (correspond à lambdamin de la simu)
 * @param borne_max : bornemax de l'interpolation (correspond à lambdamax de la simu)
 * @param nbli_spectral : nombre de ligne maximum de la database
 * @param nb_param : nombre de parametyres sur lesquels on va interpoler
 * @param Tab_spectral_data : database
 * @param Tab_spectral : tableau contenant les valeurs interpolees
 * 
 * renvoie dans le tableau pointe par **Tab_spectral, a la ligne nbli_spectral+1 et pour les colonnes de 1 à nb_param,
 * la valeur interpolee pour la valeur (mornemin+bornemax)/2
 * 
 * */
void Interpolation_Trapeze(	double borne_min,
							double borne_max,
							unsigned int nbli_spectral,
							unsigned int nb_param,
							double **Tab_spectral_data,
							double **Tab_spectral) {
	unsigned int i = 0, j = 0, k = 0;
	double a = 0, b = 0;
	unsigned int c = 0, m = 0;
	//	for (i = 0; i < nbli_spectral;i++){
	//		for (k = 0; k < nb_param; k++){
	//			printf("i:%d  k:%d  data:%f",i,k, Tab_spectral_data[i][k]);
	//		}
	//		printf("\n");
	//	}
	// on se place sur la valeur de lambmin ou la valeur precedente si borne_min ne fait pas partie de la base de donnees (indice m)
	for (i = 0;
			i < (nbli_spectral - 1) && Tab_spectral_data[i + 1][0] <= borne_min /*ELG : pas besoin && fabs(Tab_spectral_data[i + 1][0]) > EPSILON*/;
			i++, m++) {
		;
	}
	
	// stockage valeurs 
	for (j = 0; i < nbli_spectral && Tab_spectral_data[i][0] <= borne_max; i++, j++) {
		for (k = 0; k < nb_param; k++)
			Tab_spectral[j][k] = Tab_spectral_data[i][k];
		c++;
	}
	
	//stockage derniere valeur
	for (k = 0; k < nb_param; k++) {
		Tab_spectral[j][k] = Tab_spectral_data[i][k];
	}
	
	//stockage  borne min/valeur associee ds tab_spectral[0] 
	Tab_spectral[0][0] = borne_min;
	bool borneMinEstDatabase = false;
	//	printf("test1:%d \n",(fabs(borne_min-Tab_spectral_data[m][0]))< EPSILON);
	if ((fabs(borne_min - Tab_spectral_data[m][0])) < EPSILON_ERROR) //si borne min correspond a un point de la database
	{
		borneMinEstDatabase = true;
		for (k = 1; k < nb_param; k++) {
			Tab_spectral[0][k] = Tab_spectral_data[m][k];
		}
	} else //si borne min != valeur database, alors interpolation
	{
		if (m > 0 && fabs(borne_min - Tab_spectral_data[m - 1][0]) < EPSILON_ERROR) {
			
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[0][k] = Tab_spectral_data[m - 1][k];
			}
		} else {
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[0][k] = Interpolation_lineaire(borne_min, Tab_spectral_data, m, k);
			}
		}
	}
	// stockage borne max/valeur associee ds tab_spectral[nbli_spectral-1] 
	Tab_spectral[c][0] = borne_max;
	bool borneMaxEstDatabase = false;
	if (i > 0) {
		if ((fabs(borne_max - Tab_spectral_data[i - 1][0])) < EPSILON_ERROR) //si borne max correspond a un point de la database
		{
			borneMaxEstDatabase = true;
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[c][k] = Tab_spectral_data[i - 1][k];
			}
			//			printf("borne_max:%f  Tab_spectral_data[i-1][k]:%f\n",borne_max,Tab_spectral_data[i-1][0]);
		} else //si borne max != valeur database, alors interpolation
		{
			//			printf("AAAborne_max:%f  Tab_spectral_data[i-1][k]:%f\n",borne_max,Tab_spectral_data[i-1][0]);
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[c][k] = Interpolation_lineaire(borne_max, Tab_spectral_data, i - 1, k);
			}
		}
	} else {
		
		for (k = 1; k < nb_param; k++) {
			Tab_spectral[c][k] = Interpolation_lineaire(borne_max, Tab_spectral_data, i, k);
		}
	}
	
	// Calcul moyenne Par interpolation lineaire directe sur un point (si borne min ~= borne max)
	// Eloi Grau : correction indice d'interpolation
	if (fabs(borne_min - borne_max) < EPSILON_ERROR) {
		//		printf("i:%d  Tab_spectral_data-1: %f\t %f  Tab_spectral_data[m][0]:%f  bool:%d  %d\n",i,Tab_spectral_data[i-1][1],Tab_spectral_data[i][1],Tab_spectral_data[m][0],borneMinEstDatabase,borneMaxEstDatabase);
		if (borneMinEstDatabase || borneMaxEstDatabase) {
			for (k = 1; k < nb_param; k++)
				Tab_spectral[nbli_spectral + 1][k] = Tab_spectral_data[i - 1][k];
		} else {
			for (k = 1; k < nb_param; k++)
				Tab_spectral[nbli_spectral + 1][k] = Interpolation_lineaire(borne_min, Tab_spectral_data, i - 1, k);
		}
	}
	// Sinon : Calcul de moyenne par Methode des trapezes 
	else {
		double *tampon = dvector(0, nb_param);
		for (j = 0; j < c - 1; j++) {
			a = Tab_spectral[j][0];
			b = Tab_spectral[j + 1][0];
			for (k = 1; k < nb_param; k++) {
				tampon[k] += 0.5 * (Tab_spectral[j][k] + Tab_spectral[j + 1][k]) * (b - a);
			}
		}
		
		for (k = 1; k < nb_param; k++) {
			tampon[k] /= (borne_max - borne_min);
			Tab_spectral[nbli_spectral + 1][k] = tampon[k];
		}
		
		free_dvector(tampon, 0);
	}
}

void Interpolation_Trapeze(	double borne_min,
							double borne_max,
							unsigned int nbli_spectral,
							unsigned int nb_param,
							double **Tab_spectral) {
	
	double *tampon = dvector(0, nb_param);
	for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
		//a = Tab_spectral[j][0];
		//b = Tab_spectral[j + 1][0];
		for (unsigned int k = 1; k < nb_param; k++) {
			tampon[k] += 0.5 * (Tab_spectral[j][k] + Tab_spectral[j + 1][k]) * (Tab_spectral[j + 1][0] - Tab_spectral[j][0]);
		}
	}
	for (unsigned int k = 1; k < nb_param; k++) {
		tampon[k] /= (Tab_spectral[nbli_spectral - 1][0] - Tab_spectral[0][0]);
		Tab_spectral[nbli_spectral + 1][k] = tampon[k];
	}
	free_dvector(tampon, 0);
}

/**
 * METHODE D'INTERPOLATION des trapezes (20100303 prototype pour la nouvelle version de Dart)
 * 
 * @param borne_min : bornemin de l'interpolation (correspond à lambdamin de la simu)
 * @param borne_max : bornemax de l'interpolation (correspond à lambdamax de la simu)
 * @param nbli_spectral : nombre de ligne maximum de la database
 * @param nb_param : nombre de parametyres sur lesquels on va interpoler
 * @param Tab_spectral_data : database
 * @param Tab_spectral : tableau contenant les valeurs interpolees
 * 
 * renvoie dans le tableau pointe par **Tab_spectral, a la ligne nbli_spectral+1 et pour les colonnes de 1 à nb_param,
 * la valeur interpolee pour la valeur (mornemin+bornemax)/2
 * 
 * Modifications apportees :
 * FA-49 : ELG : correction bug interpolation trapeze
 * FA-65 : ELG : correction indices pour copier dans tab_spectral les seules valeurs de tab_spectral_data qui nous interessent pour l'interpolation
 * */
void Interpolation_Trapeze(	const double & borne_min,
							const double & borne_max,
							const unsigned int & nbli_spectral,
							const unsigned int & nb_param,
							const vector<vector<double> > & Tab_spectral_data,
							vector<vector<double> > & Tab_spectral) {
	if (Tab_spectral_data[nbli_spectral - 1][0] <= borne_min) {
		for (unsigned int k = 1; k < nb_param; k++) {
			Tab_spectral[nbli_spectral + 1][k] = Tab_spectral_data[nbli_spectral - 1][k];
		}
		return;
	}
	if (Tab_spectral_data[0][0] >= borne_max) {
		for (unsigned int k = 1; k < nb_param; k++) {
			Tab_spectral[nbli_spectral + 1][k] = Tab_spectral_data[0][k];
		}
		return;
	}
	unsigned int i = 0, j = 0, k = 0;
	double a = 0, b = 0;
	unsigned int c = 0, m = 0;
	//		for (i = 0; i < nbli_spectral;i++){
	//			for (k = 0; k < nb_param; k++){
	//				printf("i:%d  k:%d  data:%f",i,k, Tab_spectral_data[i][k]);
	//			}
	//			printf("\n");
	//		}
	// on se place sur la valeur de lambmin ou la valeur precedente si borne_min ne fait pas partie de la base de donnees (indice m)
	for (i = 0;
			i < (nbli_spectral - 1) && Tab_spectral_data[i + 1][0] <= borne_min /*ELG : pas besoin && fabs(Tab_spectral_data[i + 1][0]) > EPSILON*/;
			i++, m++) {}
	
	// stockage valeurs 
	for (j = 0; i < (nbli_spectral - 1) && Tab_spectral_data[i][0] <= borne_max; i++, j++) {
		for (k = 0; k < nb_param; k++) {
			Tab_spectral[j][k] = Tab_spectral_data[i][k];
		}
		c++;
	}
	//stockage derniere valeur
	for (k = 0; k < nb_param; k++) {
		Tab_spectral[j][k] = Tab_spectral_data[i][k];
	}
	
	//stockage  borne min/valeur associee ds tab_spectral[0] 
	Tab_spectral[0][0] = borne_min;
	bool borneMinEstDatabase = false;
	if ((fabs(borne_min - Tab_spectral_data[m][0])) < EPSILON_ERROR) //si borne min correspond a un point de la database
	{
		borneMinEstDatabase = true;
		for (k = 1; k < nb_param; k++) {
			Tab_spectral[0][k] = Tab_spectral_data[m][k];
		}
	} else //si borne min != valeur database, alors interpolation
	{
		if (m > 0 && fabs(borne_min - Tab_spectral_data[m - 1][0]) < EPSILON_ERROR) {
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[0][k] = Tab_spectral_data[m - 1][k];
			}
		} else {
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[0][k] = Interpolation_lineaire(borne_min, Tab_spectral_data, m, k);
			}
		}
	}
	
	// stockage borne max/valeur associee ds tab_spectral[nbli_spectral-1] 
	Tab_spectral[c][0] = borne_max;
	bool borneMaxEstDatabase = false;
	if (i > 0) {
		if ((fabs(borne_max - Tab_spectral_data[i - 1][0])) < EPSILON_ERROR) //si borne max correspond a un point de la database
		{
			borneMaxEstDatabase = true;
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[c][k] = Tab_spectral_data[i - 1][k];
			}
		} else //si borne max != valeur database, alors interpolation
		{
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[c][k] = Interpolation_lineaire(borne_max, Tab_spectral_data, i - 1, k);
			}
		}
	} else {
		for (k = 1; k < nb_param; k++) {
			Tab_spectral[c][k] = Interpolation_lineaire(borne_max, Tab_spectral_data, i, k);
		}
	}
	
	// Calcul moyenne Par interpolation lineaire directe sur un point (si borne min ~= borne max)
	// Eloi Grau : correction indice d'interpolation
	if (fabs(borne_min - borne_max) < EPSILON_ERROR) {
		if (borneMinEstDatabase || borneMaxEstDatabase) {
			for (k = 1; k < nb_param; k++)
				Tab_spectral[nbli_spectral + 1][k] = Tab_spectral_data[i - 1][k];
		} else {
			//printf("Tab_spectral_data-1: %f\t %f\n",Tab_spectral_data[i-1][1],Tab_spectral_data[i][1]);
			for (k = 1; k < nb_param; k++) {
				Tab_spectral[nbli_spectral + 1][k] = Interpolation_lineaire(borne_min, Tab_spectral_data, i - 1, k);
			}
		}
	}
	// Sinon : Calcul de moyenne par Methode des trapezes 
	else {
		vector<double> tampon(nb_param + 1);
		for (j = 0; j < c; j++) {
			a = Tab_spectral[j][0];
			b = Tab_spectral[j + 1][0];
			for (k = 1; k < nb_param; k++) {
				tampon[k] += 0.5 * (Tab_spectral[j][k] + Tab_spectral[j + 1][k]) * (b - a);
			}
		}
		
		for (k = 1; k < nb_param; k++) {
			tampon[k] /= (borne_max - borne_min);
			Tab_spectral[nbli_spectral + 1][k] = tampon[k];
		}
	}
}
void Interpolation_Trapeze(	const double & borne_min,
							const double & borne_max,
							const unsigned int & nbli_spectral,
							const unsigned int & nb_param,
							vector<vector<double> > & Tab_spectral) {
	vector<double> tampon(nb_param + 1);
	for (unsigned int k = 1; k < nb_param; k++)
		tampon[k] = 0;
	if (nbli_spectral != 1) {
		for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
			for (unsigned int k = 1; k < nb_param; k++) {
				tampon[k] += 0.5 * (Tab_spectral[j][k] + Tab_spectral[j + 1][k]) * (Tab_spectral[j + 1][0] - Tab_spectral[j][0]);
			}
		}
	} else {
		for (unsigned int k = 1; k < nb_param; k++)
			tampon[k] = Tab_spectral[0][k];
	}
	for (unsigned int k = 1; k < nb_param; k++) {
		if (nbli_spectral != 1) {
			tampon[k] /= (Tab_spectral[nbli_spectral - 1][0] - Tab_spectral[0][0]);
		}
		Tab_spectral[nbli_spectral + 1][k] = tampon[k];
	}
}
double Interpolation_Trapeze(	const double & borne_min,
								const double & borne_max,
								const unsigned int & nbli_spectral,
								vector<double> const& Tab_spectral,
								vector<double> & Lambda) {
	double tampon = 0;
	if (nbli_spectral != 1) {
		
		for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
			tampon += 0.5 * (Tab_spectral[j] + Tab_spectral[j + 1]) * (Lambda[j + 1] - Lambda[j]);
		}
	} else {
		tampon = Tab_spectral[0];
	}
	if (nbli_spectral != 1)
		tampon /= (Lambda[nbli_spectral - 1] - Lambda[0]);
	return tampon;
}
double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								vector<double> const& Tab_spectral,
								vector<double> & Lambda) {
	double tampon = 0;
	if (nbli_spectral != 1) {
		
		for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
			tampon += 0.5 * (Tab_spectral[j] + Tab_spectral[j + 1]) * (fabs(Lambda[j + 1] - Lambda[j]));
		}
	} else {
		tampon = Tab_spectral[0];
	}
	if (nbli_spectral != 1)
		tampon /= fabs((Lambda[nbli_spectral - 1] - Lambda[0]));
	return tampon;
}
double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								vector<double> const& Tab_spectral,
								vector<double> const& Lambda) {
	double tampon = 0;
	if (nbli_spectral != 1) {
		
		for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
			tampon += 0.5 * (Tab_spectral[j] + Tab_spectral[j + 1]) * (fabs(Lambda[j + 1] - Lambda[j]));
		}
	} else {
		tampon = Tab_spectral[0];
	}
	if (nbli_spectral != 1)
		tampon /= fabs((Lambda[nbli_spectral - 1] - Lambda[0]));
	return tampon;
}
double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								vector<double> & Tab_spectral,
								vector<double> & Lambda) {
	double tampon = 0;
	if (nbli_spectral != 1) {
		
		for (unsigned int j = 0; j < nbli_spectral - 1; j++) {
			tampon += 0.5 * (Tab_spectral[j] + Tab_spectral[j + 1]) * (fabs(Lambda[j + 1] - Lambda[j]));
		}
	} else {
		tampon = Tab_spectral[0];
	}
	if (nbli_spectral != 1)
		tampon /= fabs((Lambda[nbli_spectral - 1] - Lambda[0]));
	return tampon;
}

void Interpolation_Trapeze(	int& start,
							int& end,
							vector<double> & Tab,
							vector<double> & Lambda,
							double& result) {
	result = 0;
	for (int j = start; j <= end - 1; j++) {
		result += 0.5 * (Tab[j] + Tab[j + 1]) * (fabs(Lambda[j + 1] - Lambda[j]));
	}
	result /= fabs((Lambda[end] - Lambda[start]));
}

void Interpolation_Trapeze(	int& start,
							int& end,
							vector<double> & Tab,
							vector<double> & Lambda,
							double& result,
							double& lambdaMin,
							double& lambdaMax) {
	result = 0;
	double valueMin = 0;
	double valueMax = 0;
	double lMin = lambdaMin;
	double lMax = lambdaMax;
	if (lambdaMin > Lambda[start]) {
		valueMin = (Tab[start] * (fabs(Lambda[start + 1] - lambdaMin)) + Tab[start + 1] * (fabs(Lambda[start] - lambdaMin)))
				/ (fabs(Lambda[start + 1] - Lambda[start]));
		result += 0.5 * (valueMin + Tab[start + 1]) * (fabs(Lambda[start + 1] - lambdaMin));
	} else {
		result += 0.5 * (Tab[start] + Tab[start + 1]) * (fabs(Lambda[start + 1] - Lambda[start]));
		lMin = Lambda[start];
	}
	if (lambdaMax < Lambda[end]) {
		valueMax = (Tab[end - 1] * (fabs(Lambda[end] - lambdaMax)) + Tab[end] * (fabs(Lambda[end - 1] - lambdaMax)))
				/ (fabs(Lambda[end] - Lambda[end - 1]));
		result += 0.5 * (Tab[end - 1] + valueMax) * (fabs(Lambda[end - 1] - lambdaMax));
	} else {
		result += 0.5 * (Tab[end - 1] + Tab[end]) * (fabs(Lambda[end - 1] - Lambda[end]));
		lMax = Lambda[end];
	}
	for (int j = start + 1; j <= end - 2; j++) {
		result += 0.5 * (Tab[j] + Tab[j + 1]) * (fabs(Lambda[j + 1] - Lambda[j]));
	}
	result /= fabs((lMax - lMin));
}

void Interpolation_TrapezeWithSun(	int& start,
									int& end,
									vector<double> & Tab,
									vector<double> & Lambda,
									double& result,
									vector<double> & MeanSolarFactor) {
	result = 0;
	for (int j = start; j <= end; j++) {
		result += Tab[j] * MeanSolarFactor[j];
	}
	result /= fabs(end - start + 1);
}

void Interpolation_1Value(	int& start,
							vector<double> & Tab,
							vector<double> & Lambda,
							double& result,
							double& lambdaMin,
							double& lambdaMax) {
	double lambdaMean = 0.5 * (lambdaMin + lambdaMax);
	int sup = start;
	int inf = start - 1;
	if (lambdaMean > Lambda[start]) {
		sup = start + 1;
		inf = start;
	}
	result = (Tab[sup] * (fabs(Lambda[inf] - lambdaMean)) + Tab[inf] * (fabs(Lambda[sup] - lambdaMean))) / (fabs(Lambda[sup] - Lambda[inf]));
}

//----------------------------------------------------
// Arrondis
//----------------------------------------------------
/**
 * Fonction permettant d'arrondir à tronc près une value
 */
double computeArroundDouble(double tronc,
							double value) {
	int valueTmp = (int) (value / tronc);
	return (double) valueTmp * tronc;
}

/**
 * Fonction permettant d'arrondir à tronc près une value
 */
float computeArroundFloat(	float tronc,
							float value) {
	int valueTmp = (int) (value / tronc);
	return (float) valueTmp * tronc;
}

/**
 * fonction de dichotomie
 * @param	double (*f)(double) : fonction à 1 paramètre à laquelle on doit trouver 0
 * @param	a	: bornemin
 * @param	b	: bornemax
 * @param	*c	: borne du resultat
 * */
double dichotomie(	double (*f)(double d,
								double e,
								double f),
					double a,
					double b,
					double gamma,
					double tau0) {
	double fa, fb, fc, c = 0;
	int stop = 0, compt = 0, compt_max = 1000;
	fa = f(a, gamma, tau0);
	fb = f(b, gamma, tau0);
	if (std::isnan(fa) || std::isfinite(fa)) {
		c = a;
		stop = 1;
		printf("Error in dichotomie function");
	}
	if (std::isnan(fb) || std::isfinite(fb)) {
		c = b;
		stop = 1;
		printf("Error in dichotomie function");
	}
	if ((fa < 0 && fb < 0) || (fa > 0 && fb > 0)) {
		stop = 1;
	}
	while (fabs(a - b) > 0 && (stop == 0) && (compt <= compt_max)) {
		c = (a + b) / 2;
		fc = f(c, gamma, tau0);
		compt++;
		
		if (std::isnan(fc) || std::isfinite(fc)) {
			stop = 1;
			printf("Error in dichotomie function");
		} else {
			if ((a < 0 && fc > 0) || (fa > 0 && fc < 0)) {
				b = c;
				fb = fc;
			} else {
				a = c;
				fa = fc;
			}
		}
	}
	return c;
	//printf("\n %d itérations.  Racine = %lf.  f(racine) = %lf",compt,*resu,f(*resu));
}
//----------------------------------------------------
// Matrix operations
//----------------------------------------------------

double** newMatrix(	const int p_height,
					const int p_width) {
	double** matrix; // The returned matrix
	
	matrix = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		matrix[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			matrix[i][j] = 0.0;
		}
	}
	
	return matrix;
}

double*** newMatrix(const int p_height,
					const int p_width,
					const int p_width2) {
	double*** matrix; // The returned matrix
	
	matrix = new double**[p_height];
	for (int i = 0; i < p_height; i++) {
		matrix[i] = new double*[p_width];
		for (int j = 0; j < p_width; j++) {
			matrix[i][j] = new double[p_width2];
			for (int k = 0; k < p_width2; k++) {
				matrix[i][j][k] = 0.0;
			}
		}
	}
	
	return matrix;
}

void deleteMatrix(	double** p_matrix,
					const int p_height) {
	for (int i = 0; i < p_height; i++) {
		delete[] p_matrix[i];
	}
	delete[] p_matrix;
}

double** matrixSum(	double** p_matrix1,
					double** p_matrix2,
					const int p_height,
					const int p_width) {
	double** sum; // The returned matrix
	
	sum = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		sum[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			sum[i][j] = p_matrix1[i][j] + p_matrix2[i][j];
		}
	}
	
	return sum;
}

double** matrixSum(	double** p_matrix,
					double p_constant,
					const int p_height,
					const int p_width) {
	double** sum; // The returned matrix
	
	sum = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		sum[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			sum[i][j] = p_matrix[i][j] + p_constant;
		}
	}
	
	return sum;
}

double** matrixProduct(	double** p_matrix1,
						double** p_matrix2,
						const int p_height,
						const int p_width) {
	double** product; // The returned matrix
	
	product = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		product[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			product[i][j] = p_matrix1[i][j] * p_matrix2[i][j];
		}
	}
	
	return product;
}

double** matrixProduct(	double** p_matrix,
						double p_constant,
						const int p_height,
						const int p_width) {
	double** product; // The returned matrix
	
	product = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		product[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			product[i][j] = p_matrix[i][j] * p_constant;
		}
	}
	
	return product;
}

double** matrixDivision(double** p_matrix1,
						double** p_matrix2,
						const int p_height,
						const int p_width) {
	double** division; // The returned matrix
	
	division = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		division[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			// Be careful to the division by zero !
			if (p_matrix2[i][j] == 0) {
				division[i][j] = 0;
			} else {
				division[i][j] = p_matrix1[i][j] / p_matrix2[i][j];
			}
		}
	}
	
	return division;
}

double** matrixDivision(double p_constant,
						double** p_matrix,
						const int p_height,
						const int p_width) {
	double** division; // The returned matrix
	
	division = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		division[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			// Be careful to the division by zero !
			if (p_matrix[i][j] == 0) {
				division[i][j] = 0;
			} else {
				division[i][j] = p_constant / p_matrix[i][j];
			}
		}
	}
	
	return division;
}

double** matrixDivision(double** p_matrix,
						double p_constant,
						const int p_height,
						const int p_width) {
	double** division; // The returned matrix
	
	division = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		division[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			// Be careful to the division by zero !
			if (p_constant == 0) {
				division[i][j] = 0;
			} else {
				division[i][j] = p_matrix[i][j] / p_constant;
			}
		}
	}
	
	return division;
}

double** matrixExp(	double** p_matrix,
					const int p_height,
					const int p_width) {
	double** expMatrix; // The returned matrix
	
	expMatrix = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		expMatrix[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			expMatrix[i][j] = exp(p_matrix[i][j]);
		}
	}
	
	return expMatrix;
}

double** matrixLog(	double** p_matrix,
					const int p_height,
					const int p_width) {
	double** logMatrix; // The returned matrix
	
	logMatrix = new double*[p_height];
	for (int i = 0; i < p_height; i++) {
		logMatrix[i] = new double[p_width];
		for (int j = 0; j < p_width; j++) {
			logMatrix[i][j] = log(p_matrix[i][j]);
		}
	}
	
	return logMatrix;
}

void copyMatrix(double** p_source,
				double** p_dest,
				const int p_height,
				const int p_width) {
	for (int i = 0; i < p_height; i++) {
		for (int j = 0; j < p_width; j++) {
			p_dest[i][j] = p_source[i][j];
		}
	}
}

double getMatrixMin(double** p_matrix,
					const int p_height,
					const int p_width) {
	double min = p_matrix[0][0]; // The returned minimum value
	
	for (int i = 0; i < p_height; i++) {
		for (int j = 0; j < p_width; j++) {
			if (p_matrix[i][j] < min) {
				min = p_matrix[i][j];
			}
		}
	}
	
	return min;
}

double getMatrixMax(double** p_matrix,
					const int p_height,
					const int p_width) {
	double max = p_matrix[0][0]; // The returned maximum value
	
	for (int i = 0; i < p_height; i++) {
		for (int j = 0; j < p_width; j++) {
			if (p_matrix[i][j] > max) {
				max = p_matrix[i][j];
			}
		}
	}
	
	return max;
}

double matrixSumOfElements(	double** p_matrix,
							const int p_height,
							const int p_width) {
	double sum; // The sum result
	
	sum = 0.0;
	for (int i = 0; i < p_height; i++) {
		for (int j = 0; j < p_width; j++) {
			sum += p_matrix[i][j];
		}
	}
	
	return sum;
}

//------------------
// Matrix operations
//------------------

vector<vector<double> > matrixCos(vector<vector<double> > p_matrix) {
	vector<vector<double> > cosMatrix; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			tmp.push_back(cos(p_matrix[i][j]));
		}
		cosMatrix.push_back(tmp);
	}
	
	return cosMatrix;
}

vector<vector<double> > matrixDivision(	vector<vector<double> > p_matrix1,
										vector<vector<double> > p_matrix2) {
	vector<vector<double> > division; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix1.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix1[i].size(); j++) {
			// Be careful to the division by zero !
			if (p_matrix2[i][j] == 0) {
				tmp.push_back(0);
			} else {
				tmp.push_back(p_matrix1[i][j] / p_matrix2[i][j]);
			}
		}
		division.push_back(tmp);
	}
	
	return division;
}

vector<vector<double> > matrixDivision(	double p_constant,
										vector<vector<double> > p_matrix) {
	vector<vector<double> > division; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			// Be careful to the division by zero !
			if (p_matrix[i][j] == 0) {
				tmp.push_back(0);
			} else {
				tmp.push_back(p_constant / p_matrix[i][j]);
			}
		}
		division.push_back(tmp);
	}
	
	return division;
}

vector<vector<double> > matrixDivision(	vector<vector<double> > p_matrix,
										double p_constant) {
	vector<vector<double> > division; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			// Be careful to the division by zero !
			if (p_constant == 0) {
				tmp.push_back(0);
			} else {
				tmp.push_back(p_matrix[i][j] / p_constant);
			}
		}
		division.push_back(tmp);
	}
	
	return division;
}

vector<vector<double> > matrixExp(vector<vector<double> > p_matrix) {
	vector<vector<double> > expMatrix; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			tmp.push_back(exp(p_matrix[i][j]));
		}
		expMatrix.push_back(tmp);
	}
	
	return expMatrix;
}

vector<vector<double> > matrixProduct(	vector<vector<double> > p_matrix1,
										vector<vector<double> > p_matrix2) {
	vector<vector<double> > product; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix1.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix1[i].size(); j++) {
			tmp.push_back(p_matrix1[i][j] * p_matrix2[i][j]);
		}
		product.push_back(tmp);
	}
	
	return product;
}

vector<double> matrixProduct(	vector<vector<double> > p_matrix1,
								vector<double> p_matrix2) {
	vector<double> product; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix1.size(); i++) {
		if (p_matrix1[i].size() != p_matrix2.size()) {
			cerr << "Error in Matrix product : matrix size are different" << endl;
		}
		double tmp = 0;
		for (unsigned int j = 0; j < p_matrix1[i].size(); j++) {
			tmp += p_matrix1[i][j] * p_matrix2[j];
			//cout << i<<" "<<j<<" "<<"p_matrix1[i][j] : " <<p_matrix1[i][j]<<"  p_matrix2[j]:"<<p_matrix2[j]<<" total:"<<p_matrix1[i][j] * p_matrix2[j]<<" "<<"  somme="<<tmp<<endl;
		}
		product.push_back(tmp);
	}
	
	return product;
}

vector<vector<double> > matrixProduct(	vector<vector<double> > p_matrix,
										double p_constant) {
	vector<vector<double> > product; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			tmp.push_back(p_matrix[i][j] * p_constant);
		}
		product.push_back(tmp);
	}
	
	return product;
}

vector<vector<double> > matrixSum(	vector<vector<double> > p_matrix1,
									vector<vector<double> > p_matrix2) {
	vector<vector<double> > sum; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix1.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix1[i].size(); j++) {
			tmp.push_back(p_matrix1[i][j] + p_matrix2[i][j]);
		}
		sum.push_back(tmp);
	}
	
	return sum;
}

vector<vector<double> > matrixSum(	vector<vector<double> > p_matrix,
									const double p_constant) {
	vector<vector<double> > sum; // The returned matrix
	
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		vector<double> tmp;
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			tmp.push_back(p_matrix[i][j] + p_constant);
		}
		sum.push_back(tmp);
	}
	
	return sum;
}

double matrixSumOfElements(vector<vector<double> > p_matrix) {
	double sum; // The sum result
	
	sum = 0.0;
	for (unsigned int i = 0; i < p_matrix.size(); i++) {
		for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
			sum += p_matrix[i][j];
		}
	}
	
	return sum;
}

double deg2rad(const double& degree) {
	return (degree / 180. * M_PI);
}

double rad2deg(const double& rad) {
	return (rad * 180. / M_PI);
}

double WrapTo2PI(const double& radians) {
	if (radians >= 0. && radians <= M_PI2)
		return radians;
	else {
		double result = fmod(radians, M_PI2);
		return (result < 0) ? (result + M_PI2) : result;
	}
}

double WrapTo360(const double& deg) {
	if (deg >= 0. && deg <= 360.)
		return deg;
	else {
		double result = fmod(deg, 360.);
		return (result < 0) ? (result + 360.) : result;
	}
}

double WrapToDouble(const double& numer,
					const double& denom) {
	if (denom < 0) {
		cerr << "WrapTo is not implemented for demon < 0";
		abort();
	}
	double result = fmod(numer, denom);
	if (result < 0)
		return (result + denom);
	
	return result;
}

int WrapToInt(	int const& numer,
				int const& denom) {
	if (denom < 0) {
		cerr << "WrapTo is not implemented for demon < 0";
		abort();
	}
	int result = numer % denom;
	if (result < 0)
		return (result + denom);
	
	return result;
}

//----------------------------------------------------
