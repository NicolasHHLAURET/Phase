/*! Time-stamp: <@(#) Lib_malloc.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : Lib_malloc.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 30/03/2005
 *
 *  Description : Contient les fonctions d'allocations communes aux modules DART.
 *
 *********************************************************************
 * Historique :
 *
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.1 29/04/2008  Pierre Chalier : Suppression des warning "deprecated"
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

/**************************************************/
/*                                                */
/*    Bibliotheque des librairies "Allocation"    */
/*    Veronique PINEL (C.E.S.B.I.O.)              */
/*    Vincent Mathiaud : Correction bug f4tensor  */
/*                                                */
/*    Date : 09 Octobre 2000                      */
/*    Derniere modification : 27/06/08            */
/*                                                */
/**************************************************/

#ifndef _LIB_MALLOC
#define _LIB_MALLOC

#include <stdio.h>
#include <stdlib.h>

/**
 *  @ingroup DartBase
 * 
 *  Contient les fonctions d'allocations communes aux modules DART.
 */

#define NR_END 0
#define FREE_ARG char*

// ------------------ 1 dimension -----------------------------------
// Allocates memory for an array with a size specified by an int.
// If the request fails, the program exit with an adapt message. 
void *my_malloc(int);
void *my_calloc(int,
				int);
void *my_realloc(	void *ptr,
					int len);

// Allocates an unsigned char array (extreme indices are input parameters).
unsigned char *cvector(	long,
						long);

// Allocates an unsint array.
unsigned int *uvector(	long nl,
						long nh);

// Allocates an int array.
int *ivector(	long,
				long);

// Allocates an unsigned long array.
unsigned long *lvector(	long,
						long);

// Allocates a float array.
float *fvector(	int,
				int);

// Allocates a double array.
double *dvector(long,
				long);

// ------------------ 2 dimensions -----------------------------------
// Allocates an unsigned char matrix.
char **cmatrix(	long,
				long,
				long,
				long);

// Allocates an int matrix.
int **imatrix(	long,
				long,
				long,
				long);

// Allocates an unsigned matrix with long dimensions.
unsigned **mat_2D(	long,
					long,
					long,
					long);

// Allocates an unsigned char matrix with unsigned dimensions.
unsigned **umatrix(	unsigned,
					unsigned,
					unsigned,
					unsigned);

//CLF Allocates a float matrix.
float **matrix2(long nrl,
				long nrh,
				long ncl,
				long nch);

// Allocates a float matrix.
float **matrix(	long,
				long,
				long,
				long);

// Allocates a double matrix.
double **dmatrix(	long,
					long,
					long,
					long);

// ------------------ 3 dimensions -----------------------------------
// Allocates an unsigned  matrix in three dimensions.
unsigned ***mat_3D(	unsigned,
					unsigned,
					unsigned,
					unsigned,
					unsigned,
					unsigned);

// Allocates an unsigned  matrix in three dimensions (extrem indices are long parameters).
unsigned ***mat_num(long,
					long,
					long,
					long,
					long,
					long);

// Allocates an double  matrix in three dimensions.
double ***mat_dble(	long,
					long,
					long,
					long,
					long,
					long);

// Allocates an float  matrix in three dimensions.
float ***f3tensor(	int,
					int,
					int,
					int,
					int,
					int);

double ***d3tensor(	int,
					int,
					int,
					int,
					int,
					int);

int ***i3tensor(int,
				int,
				int,
				int,
				int,
				int);
// ------------------ 4 dimensions -----------------------------------
// Allocates a float  matrix in four dimensions.
int ****i4tensor(	long,
					long,
					long,
					long,
					long,
					long,
					long,
					long);

float ****f4tensor(	long,
					long,
					long,
					long,
					long,
					long,
					long,
					long);

double ****d4tensor(long,
					long,
					long,
					long,
					long,
					long,
					long,
					long);
// ------------------ others allocations -----------------------------------
float *****f5tensor(long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh,
					long nel,
					long neh,
					long nvl,
					long nvh);
// point a submatrix [newrl..][newcl..] to a[oldrl..oldrh][oldcl..oldch] .
float **submatrix(	float **,
					long,
					long,
					long,
					long,
					long);

// Numerical Recipes run-time error.
// Pierre Chalier : Suppression des warning "deprecated"
void nrerror(const char[]);

/* Allocates a float matrix m[nrl..nrh][ncl..nch] that points to the matrix declared in the standard C manner as a[nrow][ncol], 
 where nrow=nrh-nrl+1 and ncol=nch-ncl+1. The routine should be called with the address &a[0][0] as the first argument.*/
float **convert_matrix(	float *,
						long,
						long,
						long,
						long);

// ------------------ Frees functions -----------------------------------
void free_uvector(	unsigned *v,
					long nl);

void free_vector(	float *,
					int);

// Frees an int vector allocated with ivector().
void free_ivector(	int *,
					long);

// Frees an unsigned char vector allocated with cvector().
void free_cvector(	unsigned char *,
					long);

// Frees an unsigned long vector allocated with lvector().
void free_lvector(	unsigned long *,
					long);

// Frees a double vector allocated with dvector().
void free_dvector(	double *,
					long);

// Frees a float vector allocated with fvector().
void free_fvector(	float *v,
					long nl);

// Frees a float matrix allocated by matrix().
void free_umatrix(	unsigned **,
					unsigned,
					unsigned);

// Frees a float matrix allocated by mat_2D().
void free_mat2D(unsigned **,
				long,
				long);

// Frees a double matrix allocated by dmatrix().
void free_dmatrix(	double **,
					long,
					long);

// Frees a float matrix allocated by matrix() 
void free_matrix(	float **,
					long,
					long);

// Frees a float matrix allocated by matrix2() 
void free_matrix2(	float **m,
					long diml,
					long dimc);

// Frees an unsigned char matrix allocated by cmatrix() 
void free_cmatrix(	char **,
					long,
					long);

// Frees an int matrix allocated by imatrix() 
void free_imatrix(	int **,
					long,
					long);

// Frees a submatrix allocated by submatrix().
void free_submatrix(float **,
					long);

// Frees a matrix allocated by convert_matrix().
void free_convert_matrix(	float **,
							long);

void free_i3tensor(	int ***,
					long,
					long,
					long);

// Frees a float f3tensor allocated by f3tensor().
void free_d3tensor(	double ***,
					long,
					long,
					long);

// Frees a float f3tensor allocated by f3tensor().
void free_f3tensor(	float ***,
					long,
					long,
					long);

// Frees a float matrix allocated by mat3D().
void free_mat3D(unsigned ***,
				unsigned,
				unsigned,
				unsigned);

// Frees an unsigned matrix allocated by mat_num(). 
void free_mat_num(	unsigned ***,
					long,
					long,
					long);

// Frees an float matrix allocated by f4tensor(). 
void free_f4tensor(	float ****,
					long,
					long,
					long,
					long);

// Frees an double matrix allocated by f4tensor(). 
void free_d4tensor(	double ****,
					long,
					long,
					long,
					long);

// Frees an int matrix allocated by f4tensor(). 
void free_i4tensor(	int ****,
					long,
					long,
					long,
					long);

void free_f5tensor(	float *****m,
					long deb_lig,
					long deb_col,
					long deb_face,
					long deb_clm,
					long deb_dept);

#endif
