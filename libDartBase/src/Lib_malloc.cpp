/*! Time-stamp: <@(#) Lib_malloc.c   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : Lib_malloc.c
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
 * V 4.4.0 27/06/2008  MAG-EMN : Surfaces translucides (livraison)
 * V 4.3.1 29/04/2008  Pierre Chalier : Suppression des warning "deprecated"
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0   07/03/2005  GPR : First Revision
 *
 *********************************************************************
 * CVS $Log : Lib_malloc.c,v$
 */

/**************************************************/
/*                                                */
/*    Bibliotheque des librairies "Allocation"    */
/*    Veronique PINEL (C.E.S.B.I.O.)              */
/*    Vincent Mathiaud : Correction bug f4tensor  */
/*                                                */
/*    Dernier Modification : 27/06/08             */
/*                                                */
/**************************************************/

#include "../include/Lib_malloc.h"

//----------------------------------------------------------------------

void *my_malloc(int len) {
	void *copie = malloc(len);
	if (copie == NULL) {
		fprintf(stderr, "\n\n%s:%d condition d'arret: Error in allocating memory\n\n",
		__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	return copie;
}
//----------------------------------------------------------------------

void *my_calloc(int len,
				int taille) {
	void *copie = calloc(len, taille);
	if (copie == NULL) {
		fprintf(stderr, "\n\n%s:%d condition d'arret: Error in allocating memory\n\n",
		__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	return copie;
}
//----------------------------------------------------------------------

void *my_realloc(	void *ptr,
					int len) {
	void *copie = realloc(ptr, len);
	if (copie == NULL) {
		fprintf(stderr, "\n\n%s:%d condition d'arret: Error in allocating memory\n\n",
		__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	return copie;
}

//----------------------------------------------------------------------
// Pierre Chalier : Suppression des warning "deprecated"
void nrerror(const char error_text[]) {
	fprintf( stderr, "Numerical Recipes run-time error...\n");
	fprintf( stderr, "%s\n", error_text);
	fprintf( stderr, "...now exiting to system...\n");
	exit(1);
}
//----------------------------------------------------------------------

unsigned char *cvector(	long nl,
						long nh)
// allocate an unsigned char vector with subscript range v[nl..nh] 
{
	unsigned char *v;
	
	v = (unsigned char *) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(unsigned char));
	
	return v - nl + NR_END;
}
//----------------------------------------------------------------------

int *ivector(	long nl,
				long nh)
// allocate an int vector with subscript range v[nl..nh] 
{
	int *v;
	
	v = (int *) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(int));
	
	return v - nl + NR_END;
}
//-------------------------------------------------------------------------
unsigned int *uvector(	long nl,
						long nh)
// allocate an unsigned long vector with subscript range v[nl..nh] 
{
	unsigned int*v;
	
	v = (unsigned int*) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(unsigned int));
	
	return v - nl + NR_END;
}

//-------------------------------------------------------------------------
unsigned long *lvector(	long nl,
						long nh)
// allocate an unsigned long vector with subscript range v[nl..nh] 
{
	unsigned long *v;
	
	v = (unsigned long *) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(long));
	
	return v - nl + NR_END;
}
//-------------------------------------------------------------------------

float *fvector(	int nl,
				int nh)
// allocate a float vector with subscript range v[nl..nh] 
{
	float *v;
	
	v = (float *) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(float));
	
	return v - nl + NR_END;
}
//----------------------------------------------------------------------

double *dvector(long nl,
				long nh)
// allocate a double vector with subscript range v[nl..nh] 
{
	double *v;
	
	v = (double *) my_calloc((size_t) (nh - nl + 1 + NR_END), sizeof(double));
	
	return v - nl + NR_END;
}

//----------------------------------------------------------------------

char **cmatrix(	long nrl,
				long nrh,
				long ncl,
				long nch)
// allocate a unsigned char matrix with subscript range m[nrl..nrh][ncl..nch] 
{
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	char **m;
	
	// allocate pointers to rows 
	m = (char **) my_malloc((size_t) ((nrow + NR_END) * sizeof(char*)));
	
	m += NR_END;
	m -= nrl;
	
	// allocate rows and set pointers to them 
	m[nrl] = (char *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(char));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	// return pointer to array of pointers to rows 
	return m;
}
//----------------------------------------------------------------------

int **imatrix(	long nrl,
				long nrh,
				long ncl,
				long nch)
// allocate a int matrix with subscript range m[nrl..nrh][ncl..nch] 
{
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	int **m;
	
	// allocate pointers to rows 
	m = (int **) my_malloc((size_t) ((nrow + NR_END) * sizeof(int*)));
	
	m += NR_END;
	m -= nrl;
	
	// allocate rows and set pointers to them 
	m[nrl] = (int *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(int));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	// return pointer to array of pointers to rows 
	return m;
}
//-------------------------------------------------------------------------

unsigned **mat_2D(	long nrl,
					long nrh,
					long ncl,
					long nch)
// allocate a int matrix with subscript range m[nrl..nrh][ncl..nch] 
{
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	unsigned **m;
	
	// allocate pointers to rows  
	m = (unsigned **) my_malloc((size_t) ((nrow + NR_END) * sizeof(unsigned *)));
	
	m += NR_END;
	m -= nrl;
	
	// allocate rows and set pointers to them .Calloc innitialise la matrice a zero 
	m[nrl] = (unsigned *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(unsigned));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	
	// return pointer to array of pointers to rows  
	return m;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
unsigned **umatrix(	unsigned nrl,
					unsigned nrh,
					unsigned ncl,
					unsigned nch)
// allocate a int matrix with subscript range m[nrl..nrh][ncl..nch] 
{
	unsigned i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, **m;
	
	// allocate pointers to rows 
	m = (unsigned **) my_malloc((size_t) ((nrow + NR_END) * sizeof(unsigned*)));
	
	m += NR_END;
	m -= nrl;
	
	// allocate rows and set pointers to them .Calloc innitialise la matrice a zero
	m[nrl] = (unsigned *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(unsigned));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	
	// return pointer to array of pointers to rows 
	return m;
}
//-------------------------------------------------------------------------

float **matrix2(long nrl,
				long nrh,
				long ncl,
				long nch) {
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	float **m;
	
	m = (float **) my_malloc((size_t) ((nrow + NR_END) * sizeof(float*)));
	
	m += NR_END;
	m -= nrl;
	
	for (i = nrl; i <= nrh; i++)
		m[i] = (float *) my_calloc((size_t) (ncol + NR_END), sizeof(float));
	
	return m;
}

//-------------------------------------------------------------------------
float **matrix(	long nrl,
				long nrh,
				long ncl,
				long nch) {
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	float **m;
	
	m = (float **) my_malloc((size_t) ((nrow + NR_END) * sizeof(float*)));
	
	m += NR_END;
	m -= nrl;
	
	m[nrl] = (float *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(float));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	
	return m;
}

//-------------------------------------------------------------------------

void free_matrix(	float **m,
					long nrl,
					long ncl) {
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}

//-------------------------------------------------------------------------
void free_matrix2(	float **m,
					long diml,
					long dimc) {
	long i;
	for (i = 0; i <= diml; i++)
		free(m[i] - NR_END);
	
	//free ((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m - NR_END));
	
}

//-------------------------------------------------------------------------

double **dmatrix(	long nrl,
					long nrh,
					long ncl,
					long nch)
// allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] 
{
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	double **m;
	
	// allocate pointers to rows 
	m = (double **) my_malloc((size_t) ((nrow + NR_END) * sizeof(double*)));
	
	m += NR_END;
	m -= nrl;
	
	// allocate rows and set pointers to them 
	m[nrl] = (double *) my_calloc((size_t) (nrow * ncol + NR_END), sizeof(double));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;
	
	// return pointer to array of pointers to rows 
	return m;
}
//-------------------------------------------------------------------------

float **submatrix(	float **a,
					long oldrl,
					long oldrh,
					long oldcl,
					long newrl,
					long newcl)
// point a submatrix [newrl..][newcl..] to a[oldrl..oldrh][oldcl..oldch] 
{
	long i, j, nrow = oldrh - oldrl + 1, ncol = oldcl - newcl;
	float **m;
	
	// allocate array of pointers to rows 
	m = (float **) my_malloc((size_t) ((nrow + NR_END) * sizeof(float*)));
	
	m += NR_END;
	m -= newrl;
	
	// set pointers to rows 
	for (i = oldrl, j = newrl; i <= oldrh; i++, j++)
		m[j] = a[i] + ncol;
	
	// return pointer to array of pointers to rows 
	return m;
}
//-------------------------------------------------------------------------

float **convert_matrix(	float *a,
						long nrl,
						long nrh,
						long ncl,
						long nch)
/* allocate a float matrix m[nrl..nrh][ncl..nch] that points to the matrix
 declared in the standard C manner as a[nrow][ncol], where nrow=nrh-nrl+1
 and ncol=nch-ncl+1. The routine should be called with the address
 &a[0][0] as the first argument.*/
{
	long i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	float **m;
	
	// allocate pointers to rows 
	m = (float **) my_malloc((size_t) ((nrow + NR_END) * sizeof(float*)));
	
	m += NR_END;
	m -= nrl;
	
	// set pointers to rows 
	m[nrl] = a - ncl;
	for (i = 1, j = nrl + 1; i < nrow; i++, j++)
		m[j] = m[j - 1] + ncol;
	// return pointer to array of pointers to rows 
	return m;
}
//-------------------------------------------------------------------------

unsigned ***mat_3D(	unsigned nrl,
					unsigned nrh,
					unsigned ncl,
					unsigned nch,
					unsigned ndl,
					unsigned ndh)
// allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	unsigned i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1, ***t;
	
	// allocate pointers to pointers to rows 
	t = (unsigned ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(unsigned**)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (unsigned **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(unsigned*)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them. Calloc innitialise la matrice a zero
	t[nrl][ncl] = (unsigned *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(unsigned));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
	
}   // fin mat 
//-------------------------------------------------------------------------

unsigned ***mat_num(long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh)
// allocate a unsigned 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	long i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1;
	unsigned ***t;
	
	// allocate pointers to pointers to rows 
	t = (unsigned ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(unsigned**)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (unsigned **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(unsigned*)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them. Calloc innitialise la matrice a zero
	t[nrl][ncl] = (unsigned *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(unsigned));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
	
}   // fin mat 
//-------------------------------------------------------------------------

double ***mat_dble(	long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh)
// allocate a unsigned 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	long i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1;
	double ***t;
	
	// allocate pointers to pointers to rows 
	t = (double ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(double **)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (double **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(double *)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them. Calloc innitialise la matrice a zero
	t[nrl][ncl] = (double *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(double));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
	
}   // fin mat 
//-------------------------------------------------------------------------

float ***f3tensor(	int nrl,
					int nrh,
					int ncl,
					int nch,
					int ndl,
					int ndh)
// allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	int i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1;
	float ***t;
	
	// allocate pointers to pointers to rows 
	t = (float ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(float**)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (float **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(float*)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them.Calloc innitialise la matrice a zero 
	t[nrl][ncl] = (float *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(float));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
}
//-------------------------------------------------------------------------

double ***d3tensor(	int nrl,
					int nrh,
					int ncl,
					int nch,
					int ndl,
					int ndh)
// allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	int i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1;
	double ***t;
	
	// allocate pointers to pointers to rows 
	t = (double ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(double**)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (double **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(double*)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them.Calloc innitialise la matrice a zero 
	t[nrl][ncl] = (double *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(double));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
}
//-------------------------------------------------------------------------
int ***i3tensor(int nrl,
				int nrh,
				int ncl,
				int nch,
				int ndl,
				int ndh)
// allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] 
{
	int i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1;
	int ***t;
	
	// allocate pointers to pointers to rows 
	t = (int ***) my_malloc((size_t) ((nrow + NR_END) * sizeof(int**)));
	
	t += NR_END;
	t -= nrl;
	
	// allocate pointers to rows and set pointers to them 
	t[nrl] = (int **) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(int*)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	// allocate rows and set pointers to them.Calloc innitialise la matrice a zero 
	t[nrl][ncl] = (int *) my_calloc((size_t) (nrow * ncol * ndep + NR_END), sizeof(int));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}
	
	// return pointer to array of pointers to rows 
	return t;
}
//-------------------------------------------------------------------------
int ****i4tensor(	long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh,
					long nvl,
					long nvh) {
	long k, i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1, nclm = nvh - nvl + 1;
	int ****t;
	
	t = (int ****) my_malloc((size_t) ((nrow + NR_END) * sizeof(int ***)));
	
	t += NR_END;
	t -= nrl;
	
	t[nrl] = (int ***) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(int **)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	t[nrl][ncl] = (int **) my_malloc((size_t) ((nrow * ncol * ndep + NR_END) * sizeof(int*)));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	t[nrl][ncl][ndl] = (int *) my_calloc((size_t) (nrow * ncol * ndep * nclm + NR_END), sizeof(int));
	
	t[nrl][ncl][ndl] += NR_END;
	t[nrl][ncl][ndl] -= nvl;
	
	for (k = ndl + 1; k <= ndh; k++) {
		t[nrl][ncl][k] = t[nrl][ncl][k - 1] + nclm;
	}
	
	for (j = ncl + 1; j <= nch; j++) {
		t[nrl][j] = t[nrl][j - 1] + ndep;
		t[nrl][j][ndl] = t[nrl][j - 1][ndl] + ndep * nclm;
		for (k = ndl + 1; k <= ndh; k++) {
			t[nrl][j][k] = t[nrl][j][k - 1] + nclm;
		}
	}
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		t[i][ncl][ndl] = t[i - 1][ncl][ndl] + (ncol * ndep * nclm);
		for (j = ncl + 1; j <= nch; j++) {
			t[i][j] = t[i][j - 1] + ndep;
			t[i][j][ndl] = t[i][j - 1][ndl] + (ndep * nclm);
			for (k = ndl + 1; k <= ndh; k++) {
				t[i][ncl][k] = t[i][ncl][k - 1] + nclm;
				t[i][j][k] = t[i][j][k - 1] + nclm;
			}
		}
		
	}
	return t;
}
//-------------------------------------------------------------------------

float ****f4tensor(	long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh,
					long nvl,
					long nvh) {
	long k, i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1, nclm = nvh - nvl + 1;
	float ****t;
	
	t = (float ****) my_malloc((size_t) ((nrow + NR_END) * sizeof(float ***)));
	
	t += NR_END;
	t -= nrl;
	
	t[nrl] = (float ***) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(float **)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	t[nrl][ncl] = (float **) my_malloc((size_t) ((nrow * ncol * ndep + NR_END) * sizeof(float*)));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	t[nrl][ncl][ndl] = (float *) my_calloc((size_t) (nrow * ncol * ndep * nclm + NR_END), sizeof(float));
	
	t[nrl][ncl][ndl] += NR_END;
	t[nrl][ncl][ndl] -= nvl;
	
	for (k = ndl + 1; k <= ndh; k++) {
		t[nrl][ncl][k] = t[nrl][ncl][k - 1] + nclm;
	}
	
	for (j = ncl + 1; j <= nch; j++) {
		t[nrl][j] = t[nrl][j - 1] + ndep;
		t[nrl][j][ndl] = t[nrl][j - 1][ndl] + ndep * nclm;
		for (k = ndl + 1; k <= ndh; k++) {
			t[nrl][j][k] = t[nrl][j][k - 1] + nclm;
		}
	}
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		t[i][ncl][ndl] = t[i - 1][ncl][ndl] + (ncol * ndep * nclm);
		for (j = ncl + 1; j <= nch; j++) {
			t[i][j] = t[i][j - 1] + ndep;
			t[i][j][ndl] = t[i][j - 1][ndl] + (ndep * nclm);
			for (k = ndl + 1; k <= ndh; k++) {
				t[i][ncl][k] = t[i][ncl][k - 1] + nclm;
				t[i][j][k] = t[i][j][k - 1] + nclm;
			}
		}
		
	}
	return t;
}

//------------------------------------------------------------
double ****d4tensor(long nrl,
					long nrh,
					long ncl,
					long nch,
					long ndl,
					long ndh,
					long nvl,
					long nvh) {
	long k, i, j, nrow = nrh - nrl + 1, ncol = nch - ncl + 1, ndep = ndh - ndl + 1, nclm = nvh - nvl + 1;
	double ****t;
	
	t = (double ****) my_malloc((size_t) ((nrow + NR_END) * sizeof(double ***)));
	
	t += NR_END;
	t -= nrl;
	
	t[nrl] = (double ***) my_malloc((size_t) ((nrow * ncol + NR_END) * sizeof(double **)));
	
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	
	t[nrl][ncl] = (double **) my_malloc((size_t) ((nrow * ncol * ndep + NR_END) * sizeof(double*)));
	
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	
	t[nrl][ncl][ndl] = (double *) my_calloc((size_t) (nrow * ncol * ndep * nclm + NR_END), sizeof(double));
	
	t[nrl][ncl][ndl] += NR_END;
	t[nrl][ncl][ndl] -= nvl;
	
	for (k = ndl + 1; k <= ndh; k++) {
		t[nrl][ncl][k] = t[nrl][ncl][k - 1] + nclm;
	}
	
	for (j = ncl + 1; j <= nch; j++) {
		t[nrl][j] = t[nrl][j - 1] + ndep;
		t[nrl][j][ndl] = t[nrl][j - 1][ndl] + ndep * nclm;
		for (k = ndl + 1; k <= ndh; k++) {
			t[nrl][j][k] = t[nrl][j][k - 1] + nclm;
		}
	}
	for (i = nrl + 1; i <= nrh; i++) {
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		t[i][ncl][ndl] = t[i - 1][ncl][ndl] + (ncol * ndep * nclm);
		for (j = ncl + 1; j <= nch; j++) {
			t[i][j] = t[i][j - 1] + ndep;
			t[i][j][ndl] = t[i][j - 1][ndl] + (ndep * nclm);
			for (k = ndl + 1; k <= ndh; k++) {
				t[i][ncl][k] = t[i][ncl][k - 1] + nclm;
				t[i][j][k] = t[i][j][k - 1] + nclm;
			}
		}
	}
	return t;
}
// -----------------------------------------------------------  

void free_vector(	float *v,
					int nl)
// free a float vector allocated with vector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}

//-------------------------------------------------------------------------

void free_ivector(	int *v,
					long nl)
// free an int vector allocated with ivector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//-------------------------------------------------------------------------

void free_cvector(	unsigned char *v,
					long nl)
// free an unsigned char vector allocated with cvector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//-------------------------------------------------------------------------

void free_lvector(	unsigned long *v,
					long nl)
// free an unsigned long vector allocated with lvector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//-------------------------------------------------------------------------

void free_uvector(	unsigned *v,
					long nl)
// free an unsigned long vector allocated with lvector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//-------------------------------------------------------------------------
void free_dvector(	double *v,
					long nl)
// free a double vector allocated with dvector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//------------------------------------------------------------------------
void free_fvector(	float *v,
					long nl)
// free a double vector allocated with dvector() 
{
	free((FREE_ARG) (v + nl - NR_END));
}
//-------------------------------------------------------------------------

void free_umatrix(	unsigned **m,
					unsigned nrl,
					unsigned ncl)
// free a float matrix allocated by matrix() 
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_mat2D(unsigned **m,
				long nrl,
				long ncl)
// free a float matrix allocated by matrix() 
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_dmatrix(	double **m,
					long nrl,
					long ncl)
// free a double matrix allocated by dmatrix() 
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_cmatrix(	char **m,
					long nrl,
					long ncl)
// free an int matrix allocated by imatrix() 
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_imatrix(	int **m,
					long nrl,
					long ncl)
// free an int matrix allocated by imatrix() 
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}

//-------------------------------------------------------------------------

void free_submatrix(float **b,
					long nrl)
// free a submatrix allocated by submatrix() 
{
	free((FREE_ARG) (b + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_convert_matrix(	float **b,
							long nrl)
// free a matrix allocated by convert_matrix() 
{
	free((FREE_ARG) (b + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_d3tensor(	double ***t,
					long nrl,
					long ncl,
					long ndl)
// free a float f3tensor allocated by f3tensor() 
{
	free((FREE_ARG) (t[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (t[nrl] + ncl - NR_END));
	free((FREE_ARG) (t + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_i3tensor(	int ***t,
					long nrl,
					long ncl,
					long ndl)
// free a float f3tensor allocated by f3tensor() 
{
	free((FREE_ARG) (t[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (t[nrl] + ncl - NR_END));
	free((FREE_ARG) (t + nrl - NR_END));
}
//-------------------------------------------------------------------------
void free_f3tensor(	float ***t,
					long nrl,
					long ncl,
					long ndl)
// free a float f3tensor allocated by f3tensor() 
{
	free((FREE_ARG) (t[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (t[nrl] + ncl - NR_END));
	free((FREE_ARG) (t + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_mat3D(unsigned ***m,
				unsigned nrl,
				unsigned ncl,
				unsigned ndl) {
	//libere l'emplacement memoire alloue par matrix() 
	free((FREE_ARG) (m[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_mat_num(	unsigned ***m,
					long nrl,
					long ncl,
					long ndl) {
	//libere l'emplacement memoire alloue par matrix() 
	free((FREE_ARG) (m[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}
//-------------------------------------------------------------------------

void free_f4tensor(	float ****m,
					long deb_lig,
					long deb_col,
					long deb_face,
					long deb_clm) {
	free((FREE_ARG) (m[deb_lig][deb_col][deb_face] + deb_clm - NR_END));
	free((FREE_ARG) (m[deb_lig][deb_col] + deb_face - NR_END));
	free((FREE_ARG) (m[deb_lig] + deb_col - NR_END));
	free((FREE_ARG) (m + deb_lig - NR_END));
}
//-------------------------------------------------------------------------

void free_f5tensor(	float *****m,
					long deb_lig,
					long deb_col,
					long deb_face,
					long deb_clm,
					long deb_dept) {
	free((FREE_ARG) (m[deb_lig][deb_col][deb_face][deb_clm] + deb_dept - NR_END));
	free((FREE_ARG) (m[deb_lig][deb_col][deb_face] + deb_clm - NR_END));
	free((FREE_ARG) (m[deb_lig][deb_col] + deb_face - NR_END));
	free((FREE_ARG) (m[deb_lig] + deb_col - NR_END));
	free((FREE_ARG) (m + deb_lig - NR_END));
}
// -----------------------------------------------------------------------
void free_d4tensor(	double ****m,
					long deb_lig,
					long deb_col,
					long deb_face,
					long deb_clm) {
	free((FREE_ARG) (m[deb_lig][deb_col][deb_face] + deb_clm - NR_END));
	free((FREE_ARG) (m[deb_lig][deb_col] + deb_face - NR_END));
	free((FREE_ARG) (m[deb_lig] + deb_col - NR_END));
	free((FREE_ARG) (m + deb_lig - NR_END));
}
// -----------------------------------------------------------------------
void free_i4tensor(	int ****m,
					long deb_lig,
					long deb_col,
					long deb_face,
					long deb_clm) {
	free((FREE_ARG) (m[deb_lig][deb_col][deb_face] + deb_clm - NR_END));
	free((FREE_ARG) (m[deb_lig][deb_col] + deb_face - NR_END));
	free((FREE_ARG) (m[deb_lig] + deb_col - NR_END));
	free((FREE_ARG) (m + deb_lig - NR_END));
}
// -----------------------------------------------------------------------
