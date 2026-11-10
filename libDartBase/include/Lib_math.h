/*! Time-stamp: <@(#) Lib_math.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : Lib_math.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : VBT                               Date: 30/03/2005
 *
 *  Description : Contient les fonctions mathematiques communes aux modules DART.
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
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 * CVS $Log : Lib_math.h,v$
 */

/**************************************************/
/*                                                */
/*    Bibliotheque des librairies "Mathematiques" */
/*    Date   : 28/10/94                           */
/*    Dernieres modifications 27/08/06            */
/**************************************************/

#ifndef _LIB_MATH
#define _LIB_MATH

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Exit_if.h"
#include "Lib_malloc.h"
#include "GeneralDef.hpp"

/**
 *  @ingroup DartBase
 * 
 *  Contient les fonctions mathematiques communes aux modules DART.
 */

/*
 "Minimal" random number generator of Park and Miller.
 -> Returns an uniform random number within [0;1].
 -> Sets or resets 'idum' to any integer value (except  the unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive random numbers in a sequence.
 */
float random0(long *);

/*
 "Minimal" random number generator of Park and Miller.
 -> Returns an uniform random number within [0;1].
 -> Sets or resets 'idum' to any integer value (except  the unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive random numbers in a sequence.
 */
const float randomGenerator(int & graine);

/*
 "Minimal" random number generator of Park and Miller.
 -> Returns an uniform random number within [0;1].
 -> Sets or resets 'idum' to any integer value (except  the unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive random numbers in a sequence.
 */
const float randomGenerator(long int & graine);

/*
 Returns an uniform random number in [borne_inf..borne_sup].
 -> Sets or resets 'idum' to any integer value (except  the unlikely value 'MASK' ) to  initialize  the  sequence.
 -> idum must not be altered between calls for successive deviates in a sequence.
 */
float ran0(	long *,
			int,
			int);

/*
 "Minimal" random number generator of Park and Miller with Bays-Durham shuffle and added safeguards.
 Returns a uniform random number between 0.0 and 1.0.
 Called with idum (negative integer for initializing).
 Don't alter idum between sucessive deviates in a sequence
 RNMX should approximate the largest floating value that is less 1
 */
float random1(long *);

/*
 Returns a normally distributed deviate for
 -> mean = 0
 -> variance = 1
 -> using random1(idum) as the  source of uniform deviates.
 */
float gasdev(long *);

/*
 Returns a normally distributed deviate for
 -> mean = m
 -> ecart-type = sigma
 -> using random1(idum) as the  source of uniform deviates
 RQ: normal=gasdev*sigma+m
 */
float normal(	long *,
				float,
				float);

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
					float sigmaPercent);

//----------------------------------------------------
/****************************/
/* METHODES D'INTERPOLATION */
/****************************/
//**! Calcul the average of the parameter. 
double Interpolation_lineaire(	double,
								double **,
								unsigned int,
								unsigned int);

//20100303 modification du prototype de cette methode pour la nouvelle version de Dart utilisant les flux
double Interpolation_lineaire(	const double &,
								const std::vector<std::vector<double> > &,
								const unsigned int &,
								const unsigned int &);

/** Interpolation lineaire generique
 * @precondition borneMin =< borneinterpolated && borneMax >= borneinterpolated
 */
double Interpolation_lineaire(	const double & borneMin,
								const double & borneMax,
								const double & borneinterpolated,
								const double & valMin,
								const double & valMax);

//**! Interpolation with the Trapeze method for a spectral band.
void Interpolation_Trapeze(	double,
							double,
							unsigned int,
							unsigned int,
							double **,
							double **);

//20100303 modification du prototype de cette methode pour la nouvelle ver
void Interpolation_Trapeze(	const double & borne_min,
							const double & borne_max,
							const unsigned int & nbli_spectral,
							const unsigned int & nb_param,
							const std::vector<std::vector<double> > & Tab_spectral_data,
							std::vector<std::vector<double> > & Tab_spectral);
void Interpolation_Trapeze(	const double & borne_min,
							const double & borne_max,
							const unsigned int & nbli_spectral,
							const unsigned int & nb_param,
							std::vector<std::vector<double> > & Tab_spectral);
void Interpolation_Trapeze(	double,
							double,
							unsigned int,
							unsigned int,
							double **);
double Interpolation_Trapeze(	const double & borne_min,
								const double & borne_max,
								const unsigned int & nbli_spectral,
								std::vector<double> const& Tab_spectral,
								std::vector<double> & Lambda);

double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								std::vector<double> & Tab_spectral,
								std::vector<double> & Lambda);

double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								std::vector<double> const & Tab_spectral,
								std::vector<double> & Lambda);

double Interpolation_Trapeze(	const unsigned int & nbli_spectral,
								std::vector<double> const & Tab_spectral,
								std::vector<double> const & Lambda);

void Interpolation_Trapeze(	int& start,
							int& end,
							std::vector<double> & Tab,
							std::vector<double> & Lambda,
							double& result);

void Interpolation_TrapezeWithSun(	int& start,
									int& end,
									std::vector<double> & Tab,
									std::vector<double> & Lambda,
									double& result,
									std::vector<double> & MeanSolarFactor);

void Interpolation_1Value(	int& start,
							std::vector<double> & Tab,
							std::vector<double> & Lambda,
							double& result,
							double& lambdaMin,
							double& lambdaMax);
void Interpolation_Trapeze(	int& start,
							int& end,
							std::vector<double> & Tab,
							std::vector<double> & Lambda,
							double& result,
							double& lambdaMin,
							double& lambdaMax);
//----------------------------------------------------
// Arrondis
//----------------------------------------------------
/**
 * Fonction permettant d'arrondir à tronc près une value double
 */
double computeArroundDouble(double tronc,
							double value);

/**
 * Fonction permettant d'arrondir à tronc près une value float
 */
float computeArroundFloat(	float tronc,
							float value);

/**
 * fonction de dichotomie
 * @param	double (*f)(double) : fonction à 1 paramètre à laquelle on doit trouver 0
 * @param	a	: bornemin
 * @param	b	: bornemax
 * @param	*c	: borne du resultat
 * */
double dichotomie(	double (*f)(double,
								double,
								double),
					double a,
					double b,
					double gamma,
					double tau0);

//------------------
// Matrix operations
//------------------

/**
 * Creates a matrix and initializes all values to 0.0.
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return an empty matrix
 */
double** newMatrix(	const int p_height,
					const int p_width);

/**
 * Creates a matrix and initializes all values to 0.0.
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return an empty matrix
 */
double*** newMatrix(const int p_height,
					const int p_width,
					const int p_width2);

/**
 * Frees the memory allocated for the given matrix.
 * @param p_matrix the matrix to delete
 * @param p_height the matrix height
 */
void deleteMatrix(	double** p_matrix,
					const int p_height);

/**
 * Frees the memory allocated for the given matrix.
 * @param p_matrix the matrix to delete
 * @param p_height the matrix height
 */
void deleteMatrix(	double*** p_matrix,
					const int p_height,
					const int p_width);

/**
 * Computes the given matrix sum. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixSum(	double** p_matrix1,
					double** p_matrix2,
					const int p_height,
					const int p_width);

/**
 * Computes the sum of the given matrix and the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixSum(	double** p_matrix,
					double p_constant,
					const int p_height,
					const int p_width);

/**
 * Computes the given matrix product. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixProduct(	double** p_matrix1,
						double** p_matrix2,
						const int p_height,
						const int p_width);

/**
 * Computes the product of the given matrix by the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixProduct(	double** p_matrix,
						double p_constant,
						const int p_height,
						const int p_width);

/**
 * Computes the given matrix division. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixDivision(double** p_matrix1,
						double** p_matrix2,
						const int p_height,
						const int p_width);

/**
 * Computes the division of the given constant by the given matrix.
 * @param p_constant the first operand
 * @param p_matrix the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixDivision(double p_constant,
						double** p_matrix,
						const int p_height,
						const int p_width);

/**
 * Computes the division of the given matrix by the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixDivision(double** p_matrix,
						double p_constant,
						const int p_height,
						const int p_width);

/**
 * Computes the given matrix exponential.
 * @param p_matrix the matrix operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixExp(	double** p_matrix,
					const int p_height,
					const int p_width);

/**
 * Computes the given matrix logarithm.
 * @param p_matrix the matrix operand
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the result matrix
 */
double** matrixLog(	double** p_matrix,
					const int p_height,
					const int p_width);

/**
 * Copies the first given matrix to the second one. The matrix need to have the same size.
 * @param p_source the source matrix
 * @param p_dest the destination matrix
 * @param p_height the matrix height
 * @param p_width the matrix width
 */
void copyMatrix(double** p_source,
				double** p_dest,
				const int p_height,
				const int p_width);

/**
 * @return the minimum value of the given matrix
 * @param p_matrix the matrix
 * @param p_height the matrix height
 * @param p_width the matrix width
 */
double getMatrixMin(double** p_matrix,
					const int p_height,
					const int p_width);

/**
 * @return the maximum value of the given matrix
 * @param p_matrix the matrix
 * @param p_height the matrix height
 * @param p_width the matrix width
 */
double getMatrixMax(double** p_matrix,
					const int p_height,
					const int p_width);

/**
 * Computes the sum of the matrix values.
 * @param p_matrix the matrix to be treated
 * @return the sum of the matrix values
 */
double matrixSumOfElements(	double** p_matrix,
							const int p_height,
							const int p_width);

//------------------
// Matrix operations
//------------------

/**
 * Computes the given matrix cosinus.
 * @param p_matrix the matrix operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixCos(std::vector<std::vector<double> > p_matrix);

/**
 * Computes the given matrix division. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixDivision(	std::vector<std::vector<double> > p_matrix1,
										std::vector<std::vector<double> > p_matrix2);

/**
 * Computes the division of the given constant by the given matrix.
 * @param p_constant the first operand
 * @param p_matrix the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixDivision(	double p_constant,
										std::vector<std::vector<double> > p_matrix);

/**
 * Computes the division of the given matrix by the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixDivision(	std::vector<std::vector<double> > p_matrix,
										double p_constant);

/**
 * Computes the given matrix exponential.
 * @param p_matrix the matrix operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixExp(std::vector<std::vector<double> > p_matrix);

/**
 * Computes the given matrix product. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixProduct(	std::vector<std::vector<double> > p_matrix1,
										std::vector<std::vector<double> > p_matrix2);

/**
 * Computes the product of the given matrix by the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixProduct(	std::vector<std::vector<double> > p_matrix,
										double p_constant);

/**
 * Computes the product of the given matrix by the given vector (must be same length).
 * @param p_matrix1 the first matrix
 * @param p_matrix2 the vector
 * @return the result vector
 */
std::vector<double> matrixProduct(	std::vector<std::vector<double> > p_matrix1,
								std::vector<double> p_matrix2);

/**
 * Computes the given matrix sum. The matrix need to have the same size.
 * @param p_matrix1 the first operand
 * @param p_matrix2 the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixSum(	std::vector<std::vector<double> > p_matrix1,
									std::vector<std::vector<double> > p_matrix2);

/**
 * Computes the sum of the given matrix and the given constant.
 * @param p_matrix the first operand
 * @param p_constant the second operand
 * @return the result matrix
 */
std::vector<std::vector<double> > matrixSum(	std::vector<std::vector<double> > p_matrix,
									double p_constant);

/**
 * Computes the sum of the matrix values.
 * @param p_matrix the matrix to be treated
 * @return the sum of the matrix values
 */
double matrixSumOfElements(std::vector<std::vector<double> > p_matrix);

double deg2rad(const double& degree);

double rad2deg(const double& rad);

double WrapTo2PI(const double& radians);

double WrapTo360(const double& deg);

double WrapToDouble(const double& numer,
					const double& denom);

int WrapToInt(	int const& numer,
				int const& denom);

#endif //_LIB_MATH
