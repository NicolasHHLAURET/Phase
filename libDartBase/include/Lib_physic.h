/*
 * Lib_physic.h
 *
 *  Created on: 24 août 2009
 *      Author: dounaevae
 */

#ifndef LIB_PHYSIC_H_
#define LIB_PHYSIC_H_
#include <vector>

// -----------------------------------------------------------------------------------------
/*Calcul de la luminance du corps noir emise par les cellules (atmo ou terrestres) selon la loi de Planck comme une integrale sur la bande spectrale (divisee par la longueur de bande), ou bien selon la loi de Stephan.
 Constantes utilisees: Cplanck1, Cplanck2, SIGMA, DELTA_LAMBDA, M_PI.
 Variables globales: EmisLaw, lambdamin, lambdamax. Thierry 08/2001*/
// -----------------------------------------------------------------------------------------
double temperatureToLuminanceForOneWavelength(	const double & temperature,
												const int & emissionLaw,
												const double & wavelength);

double planckLaw(	const double & wavelength,
					const double & temperature);

double planckLawMicrons(const double & wavelength,
						const double & temperature);
double temperatureToLuminanceMeanOnSpectralBand(const double & temperature,
												const int & emissionLaw,
												const double & lowerLimitWavelength,
												const double &upperLimitWavelength,
												const double & precision,
												const unsigned int& maxNumberOfIterations);
// -----------------------------------------------------------------------------------------
/*Calcul de la temperature de brillance Tapp par inversion de la loi de Stephan ou de Planck. Dans ce dernier cas, on recherche pour une bande spectrale le lambda0 equivalent pour inverser la loi, solution de f(x) = Cplanck1/x**5/(exp(Cplanck2/x/Temperature)-1) - Lapp = 0.
 Resolution avec la methode de Newton: x(n+1) = x(n) - f(x(n))/f'(x(n));
 Constantes utilisees: Cplanck1, Cplanck2, SIGMA, M_PI, EPSILON.
 Variables globales: EmisLaw, lambdamin, lambdamax, TempMin, TempMax*/
// -----------------------------------------------------------------------------------------
double Luminance_to_Temperature(double Lapp,
								int EmisLaw,
								const double& lambdaMoy); // calcul de Tapp.


double Jacobian_Luminance_to_Temperature(double Lapp,
											double dLapp,
											int emissionLaw,
											const double& lambdaMoy); // Computes dTapp / dpi from DLapp / dpi and Lapp

/**
 * Converts the given temperature matrix into a spectral radiance matrix by applying the Planck's law.
 * @param p_matrix the matrix to be converted
 * @param p_lambda the lambda value
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the converted matrix
 */
double** temperatureToLuminance(double** p_matrix,
								const int p_height,
								const int p_width,
								double p_lambda);

/**
 * Converts the given temperature matrix into a spectral radiance matrix by applying the Planck's law.
 * @param p_matrix the matrix to be converted
 * @param p_lambda the lambda value
 * @return the converted matrix
 */
std::vector<double> temperatureToLuminance(	const std::vector<double> &p_matrix,
											const double &p_lambda);

/**
 * Converts the given spectral radiance matrix into a temperature matrix by applying the Planck's law.
 * @param p_matrix the matrix to be converted
 * @param p_lambda the lambda value
 * @param p_height the matrix height
 * @param p_width the matrix width
 * @return the converted matrix
 */
double** luminanceToTemperature(double** p_matrix,
								const int p_height,
								const int p_width,
								double p_lambda);

/**
 * Converts the given spectral radiance matrix into a temperature matrix by applying the Planck's law.
 * @param p_matrix the matrix to be converted
 * @param p_lambda the lambda value
 * @return the converted matrix
 */
std::vector<double> luminanceToTemperature(	const std::vector<double> &p_matrix,
											const double &p_lambda);
// -----------------------------------------------------------------------------------------
// Gives Sun zenith and Sun azimuth angles from date.
// cf http://www.srrb.noaa.gov/highlights/sunrise/azel.html to confirm
//----------------------------------------------------------------------------------------

const double radian(const double & degre);
const double degre(const double & radian);

void SolarAngle(int,
				float,
				float,
				float,
				float,
				double *,
				double *);

void sensorSolarAngle(	/* Parametres de sortie */double & solarZenith,
						double & solarAzimuth,
						/* Parametres d'entrees temporels : date de la cible */const int & year,
						const int & month,
						const int & day,
						const int & hour,
						const int & minute,
						const int & second,
						/* Parametres d'entrees geographiques de la cible */const double & targetLatitude,
						const double & targetLongitude,
						const double & timeZone,
						const bool & daylightSavingTime,
						/* Parametres d'entrees geographiques du capteur */const double & sensorLatitude,
						const double & sensorLongitude);

void SolarAngle(/* Parametres de sortie */double & solarZenith,
				double & solarAzimuth,
				/* Parametres d'entrees temporels : date */const int & year,
				const int & month,
				const int & day,
				const int & hour,
				const int & minute,
				const int & second,
				/* Parametres d'entrees geographiques */const double & latitude,
				const double & longitude,
				const double & timeZone,
				const bool & daylightSavingTime);

int calculJourDeLAnnee(	int & year,
						int & month,
						int & day,
						int & hour,
						int & minute,
						int & second,
						double & timeZone,
						const bool & daylightSavingTime);

void sensorAngle(	/* Parametres de sortie */double & sensorZenith,
					double & sensorAzimuth,
					/* Parametres d'entrees geographiques du capteur */const double & sensorLatitude,
					const double & sensorLongitude,
					const double & sensorAltitude,
					/* Parametres d'entrees geographiques de la cible */const double & targetLatitude,
					const double & targetLongitude,
					const double & targetAltitude);

#endif /* LIB_PHYSIC_H_ */
