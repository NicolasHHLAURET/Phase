/*! Time-stamp: <@(#) Lib_physic.cpp   10/09/04 - 10:00:50   GPR CESBIO>
 *********************************************************************
 *  @file    : Lib_util.c
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : Dounaeva                               Date: 07/03/2005
 *
 *  Description : Librairies physiques
 *
 *********************************************************************
 * Historique :
 n
 *
 *********************************************************************
 */

#include "../include/Lib_physic.h"
#include "../include/Lib_math.h"
#include "../include/GeneralDef.hpp"
#include "../include/Library_math.hpp"
#include <math.h>
#include <stdio.h>

#include "boost/date_time/gregorian/gregorian.hpp" //include all types plus i/o
// -----------------------------------------------------------------------------------------
/*Calcul de la luminance du corps noir emise par les cellules (atmo ou terrestres) selon la loi de Planck comme une integrale sur la bande spectrale (divisee par la longueur de bande), ou bien selon la loi de Stephan.
 Constantes utilisees: Cplanck1, Cplanck2, SIGMA, DELTA_LAMBDA, M_PI.
 Variables globales: emissionLaw, lambdamin, lambdamax. Thierry 08/2001*/
// -----------------------------------------------------------------------------------------

double temperatureToLuminanceForOneWavelength(	const double & temperature,
												const int & emissionLaw,
												const double & wavelength) {

	if (temperature < 0.) {
		cerr << "Warning : in method Temperature to Radiance : null or negative temperature : " << temperature << endl;
		return 0.;
	}
	if (temperature < 1E-1) {
		return 0.;
	}
	double luminance = 0;
	switch (emissionLaw) {
		case STEPHAN:
			return SIGMA * pow(temperature, 4.0) / M_PI;
			break;

		case PLANCK:
			luminance = planckLaw(wavelength, temperature);
			// en W/µm
	}
	return luminance;
}
// -----------------------------------------------------------------------------------------


double planckLaw(	const double & wavelength,
					const double & temperature) {
	return Cplanck1 / (pow(wavelength, 5.0) * (exp(Cplanck2 / (wavelength * temperature)) - 1.0) * 1E6);
}

double planckLawMicrons(const double & wavelength,
						const double & temperature) {
	return Cplanck1 / (pow(wavelength, 5.0) * (exp(Cplanck2 / (wavelength * temperature)) - 1.0));
}

double computeOfGetPlanckLaw(	const double & temperature,
								const double & lowerLimitWavelength,
								const double & h,
								const unsigned int& numberOfIterations,
								const unsigned int& maxNumberOfIterations,
								vector<double>& functionValues,
								const unsigned int& currentIteration) {
	unsigned int vectorIndex = currentIteration * maxNumberOfIterations / numberOfIterations;
	if (functionValues[vectorIndex] == -1) {
		double wavelength = lowerLimitWavelength + currentIteration * h;
		functionValues[vectorIndex] = planckLaw(wavelength, temperature);
	}
	return functionValues[vectorIndex];

}

double simpsonIntegralOfPlanckLaw(	const double & temperature,
									const double & lowerLimitWavelength,
									const double & upperLimitWavelength,
									const unsigned int& numberOfIterations,
									const unsigned int& maxNumberOfIterations,
									vector<double>& functionValues) {
	double integral = 0;
	double h = (upperLimitWavelength - lowerLimitWavelength) / numberOfIterations;
	integral += computeOfGetPlanckLaw(temperature, lowerLimitWavelength, h, numberOfIterations, maxNumberOfIterations,
		functionValues, 0);
	integral += computeOfGetPlanckLaw(temperature, lowerLimitWavelength, h, numberOfIterations, maxNumberOfIterations,
		functionValues, numberOfIterations);
	for (unsigned int evenValue = 2; evenValue < numberOfIterations; evenValue += 2) {
		integral += 2 * computeOfGetPlanckLaw(temperature, lowerLimitWavelength, h, numberOfIterations,
			maxNumberOfIterations, functionValues, evenValue);
	}
	for (unsigned int oddValue = 1; oddValue < numberOfIterations; oddValue += 2) {
		integral += 4 * computeOfGetPlanckLaw(temperature, lowerLimitWavelength, h, numberOfIterations,
			maxNumberOfIterations, functionValues, oddValue);
	}
	return integral * h / 3.;
}

void initiateVector(vector<double>& vectorToInitiate) {
	for (unsigned int i = 0; i < vectorToInitiate.size(); i++) {
		vectorToInitiate[i] = -1;
	}
}

double temperatureToLuminanceMeanOnSpectralBand(const double & temperature,
												const int & emissionLaw,
												const double & lowerLimitWavelength,
												const double & upperLimitWavelength,
												const double & precision,
												const unsigned int& maxNumberOfIterations) {

	if (temperature < 0.) {
		cerr << "Warning : in method Temperature to Radiance : null or negative temperature : " << temperature << endl;
		return 0.;
	}
	if (temperature < 1E-1) {
		return 0.;
	}
	if (lowerLimitWavelength == upperLimitWavelength) {
		return temperatureToLuminanceForOneWavelength(temperature, emissionLaw, lowerLimitWavelength);
	}
	double luminance = 0;
	switch (emissionLaw) {
		case STEPHAN:
			return SIGMA * pow(temperature, 4.0) / M_PI;
			break;
		case PLANCK:
			vector<double> functionValues;
			functionValues.resize(maxNumberOfIterations + 1);
			initiateVector(functionValues);
			unsigned int numberOfIterations = 2;
			double errorOnIntegrals = 1;
			double currentIntegralValue = 0;
			double lastIntegralValue = 0;
			while (numberOfIterations == 4 || (errorOnIntegrals > precision && numberOfIterations
					< maxNumberOfIterations)) {
				currentIntegralValue = simpsonIntegralOfPlanckLaw(temperature, lowerLimitWavelength,
					upperLimitWavelength, numberOfIterations, maxNumberOfIterations, functionValues);
				errorOnIntegrals = abs(currentIntegralValue - lastIntegralValue) / currentIntegralValue;
				lastIntegralValue = currentIntegralValue;
				numberOfIterations *= 2;
			}
			luminance = currentIntegralValue / (upperLimitWavelength - lowerLimitWavelength);
			break;
	}
	return luminance;
}

// -----------------------------------------------------------------------------------------
/*Calcul de la temperature de brillance Tapp par inversion de la loi de Stephan ou de Planck. Dans ce dernier cas, on recherche pour une bande spectrale le lambda0 equivalent pour inverser la loi, solution de f(x) = Cplanck1/x**5/(exp(Cplanck2/x/Temperature)-1) - Lapp = 0.
 Resolution avec la methode de Newton: x(n+1) = x(n) - f(x(n))/f'(x(n));
 Constantes utilisees: Cplanck1, Cplanck2, SIGMA, M_PI, EPSILON.
 Variables globales: emissionLaw, lambdamin, lambdamax, TempMin, TempMax*/
// -----------------------------------------------------------------------------------------
double Luminance_to_Temperature(double Lapp,
								int emissionLaw,
								const double& lambdaMoy) // calcul de Tapp.
{
	if (emissionLaw == PLANCK) {
		//Lapp *= 1e6; pour passage en W/m
		return (1 / lambdaMoy) * Cplanck2 / log(Cplanck1 / (pow(lambdaMoy, 5.0) * Lapp * 1E6) + 1.0);
	} else if (emissionLaw == STEPHAN) {
		return pow((Lapp * M_PI) / (double) (SIGMA), 0.25);
	} else {
		cout << endl;
		cout << "Error in function Luminance_to_Temperature () not STEPHAN and PLANCK (emissionLaw = " << emissionLaw
				<< " ). Exiting ..." << endl;
		exit(1);
	}
}

double Jacobian_Luminance_to_Temperature(double Lapp,
											double dLapp,
											int emissionLaw,
											const double& lambdaMoy){
	if (emissionLaw == PLANCK) {
		double temp = log(Cplanck2 / (pow(lambdaMoy, 5.0)) * 1/(Lapp*1E6) +1.0);
		return (lambdaMoy / Cplanck1 * 1/temp * 1/temp * 1/(Lapp * 1E6 * ( 1.0 + Lapp * 1E6)) * dLapp * 1E6);
	}else if (emissionLaw == STEPHAN) {
		return pow((dLapp * M_PI) / (double) (SIGMA), 0.25);
	}else {
		cout << endl;
		cout << "Error in function Luminance_to_Temperature () not STEPHAN and PLANCK (emissionLaw = " << emissionLaw
				<< " ). Exiting ..." << endl;
		exit(1);
	}
}

double** temperatureToLuminance(double** p_matrix,
								const int p_height,
								const int p_width,
								double p_lambda) {
	double** tmpMatrix1;
	double** tmpMatrix2;

	tmpMatrix1 = matrixProduct(p_matrix, p_lambda * 1e-6, p_height, p_width); // = temperature.Lambda
	tmpMatrix2 = matrixDivision(Cplanck2, tmpMatrix1, p_height, p_width); // = (h.c) / (k.temperature.wavelength)
	deleteMatrix(tmpMatrix1, p_height);
	tmpMatrix1 = matrixExp(tmpMatrix2, p_height, p_width); // = exp((h.c) / (k.temperature.wavelength))
	deleteMatrix(tmpMatrix2, p_height);
	tmpMatrix2 = matrixSum(tmpMatrix1, -1, p_height, p_width); // = exp((h.c) / (k.temperature.wavelength)) - 1
	deleteMatrix(tmpMatrix1, p_height);
	tmpMatrix1 = matrixDivision(Cplanck1 / pow(p_lambda * 1e-6, 5), tmpMatrix2, p_height, p_width); // = (2.h.c^2) / wavelength^5 / (exp((h.c) / (k.temperature.wavelength)) - 1)
	deleteMatrix(tmpMatrix2, p_height);
	tmpMatrix2 = matrixDivision(tmpMatrix1, 1E6, p_height, p_width); // = (2.h.c^2) / wavelength^5 / (exp((h.c) / (k.temperature.wavelength)) - 1)

	return tmpMatrix2;
}

vector<double> temperatureToLuminance(	const vector<double> &p_matrix,
										const double &p_lambda) {
	vector<double> tmpMatrix1;
	vector<double> tmpMatrix2;

	tmpMatrix1 = Library_math::matrixProduct(p_matrix, p_lambda * 1e-6);
	tmpMatrix2 = Library_math::matrixDivision(Cplanck2, tmpMatrix1);
	// = (h.c) / (k.temperature.wavelength)
	tmpMatrix1 = Library_math::matrixExp(tmpMatrix2); // = exp((h.c) / (k.temperature.wavelength))
	tmpMatrix2 = Library_math::matrixSum(tmpMatrix1, -1);
	tmpMatrix1 = Library_math::matrixDivision(Cplanck1 / pow(p_lambda * 1e-6, 5), tmpMatrix2); // = (2.h.c^2) / wavelength^5 / (exp((h.c) / (k.temperature.wavelength)) - 1)
	tmpMatrix2 = Library_math::matrixDivision(tmpMatrix1, 1E6); // = (2.h.c^2) / wavelength^5 / (exp((h.c) / (k.temperature.wavelength)) - 1)

	return tmpMatrix2;
}

double** luminanceToTemperature(double** p_matrix,
								const int p_height,
								const int p_width,
								double p_lambda) {
	double** tmpMatrix1;
	double** tmpMatrix2;
	double** tmpMatrix3;
	tmpMatrix3 = matrixProduct(p_matrix, 1E6, p_height, p_width);
	tmpMatrix1 = matrixProduct(tmpMatrix3, pow(p_lambda, 5), p_height, p_width); // = luminance en W/m
	deleteMatrix(tmpMatrix3, p_height); // = temperature.wavelength^5
	tmpMatrix2 = matrixDivision(Cplanck1, tmpMatrix1, p_height, p_width); // = (2.h.c^2) / (temperature.wavelength^5)
	deleteMatrix(tmpMatrix1, p_height);
	tmpMatrix1 = matrixSum(tmpMatrix2, 1, p_height, p_width); // = 1 + (2.h.c^2) / (temperature.wavelength^5)
	deleteMatrix(tmpMatrix2, p_height);
	tmpMatrix2 = matrixLog(tmpMatrix1, p_height, p_width); // = log(1 + (2.h.c^2) / (temperature.wavelength^5))
	deleteMatrix(tmpMatrix1, p_height);
	tmpMatrix1 = matrixDivision(Cplanck2 / p_lambda, tmpMatrix2, p_height, p_width); // = (h.c) / ((k.wavelength) * log(1 + (2.h.c^2) / (temperature.wavelength^5)))
	deleteMatrix(tmpMatrix2, p_height);
	return tmpMatrix1;
}

vector<double> luminanceToTemperature(	const vector<double> &p_matrix,
										const double &p_lambda) {
	vector<double> tmpMatrix1;
	vector<double> tmpMatrix2;
	vector<double> tmpMatrix3;
	tmpMatrix3 = Library_math::matrixProduct(p_matrix, 1E6); // = luminance en W/m
	tmpMatrix1 = Library_math::matrixProduct(tmpMatrix3, pow(p_lambda, 5)); // = temperature.wavelength^5
	tmpMatrix2 = Library_math::matrixDivision(Cplanck1, tmpMatrix1); // = (2.h.c^2) / (temperature.wavelength^5)
	tmpMatrix1 = Library_math::matrixSum(tmpMatrix2, 1); // = 1 + (2.h.c^2) / (temperature.wavelength^5)
	tmpMatrix2 = Library_math::matrixLog(tmpMatrix1); // = log(1 + (2.h.c^2) / (temperature.wavelength^5))
	tmpMatrix1 = Library_math::matrixDivision(Cplanck2 / p_lambda, tmpMatrix2); // = (h.c) / ((k.wavelength) * log(1 + (2.h.c^2) / (temperature.wavelength^5)))
	return tmpMatrix1;
}
//----------------------------------------------------------------------------------------
/// ELG passage aux jours juliens
void SolarAngle(int jour,/* int mois, int an,*/
				float heure,
				float DecalageHeure,
				float latitude,
				float angleNorthMinus0x,
				double *zenithAngles,
				double *azimuthAngles) {
	float declinaison; // � declinaison (angle entre la dir du soleil et le plan equatorial. Varie entre -23.45� et 23.45� et est nul aux equinoxes)
	float omega; // � Angle solaire
	float hauteur; // � Hauteur du Soleil
	float azimuth; // � Azimuth du Soleil
	//int j; // jour de l'annee
	float TS; // heures Heure Solaire = Heure avec soleil a la verticale a 12h
	//static int  year[2][13] = {{0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },{0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }}; // jour de l'ann�e du premier jour de chaque mois pour une annee normale ou bissextile

	while (heure >= 24) {
		heure -= 24;
		jour++;
	}

	while (heure < 0) {
		heure += 24;
		jour--;
	}

	/*	int bissextile = 0;
	 // Annee bissextile = annee divisible par 4 mais pas par 100, ou divisible par 400.
	 if (((fmod(an,4)==0) && (fmod(an,100)!=0)) || (fmod(an,400)==0))
	 bissextile = 1;

	 if ((mois==1) && (jour>31)) jour -= 31, mois++;
	 if ((mois==2) && (!bissextile) && (jour>28)) jour -= 28, mois++;
	 if ((mois==2) && (bissextile) && (jour>29)) jour -= 29, mois++;
	 if ((mois==3) && (jour>31)) jour -= 31, mois++;
	 if ((mois==4) && (jour>30)) jour -= 30, mois++;
	 if ((mois==5) && (jour>31)) jour -= 31, mois++;
	 if ((mois==6) && (jour>30)) jour -= 30, mois++;
	 if ((mois==7) && (jour>31)) jour -= 31, mois++;
	 if ((mois==8) && (jour>31)) jour -= 31, mois++;
	 if ((mois==9) && (jour>30)) jour -= 30, mois++;
	 if ((mois==10) && (jour>31)) jour -= 31, mois++;
	 if ((mois==11) && (jour>30)) jour -= 30, mois++;
	 if ((mois==12) && (jour>31)) jour -= 31, mois=1, an++;

	 j = year[bissextile][mois] + jour;
	 */

	TS = heure - DecalageHeure; // D�calage horaire par rapport au Temps Solaire

	//		eTRN = E0 * (1 + 0.033 * cos((0.984 * j)*2*M_PI/360));
	declinaison = 23.45 * sin((0.980 * (jour + 284)) * M_PI / 180);

	omega = 15 * (TS - 12);
	hauteur = (asin(sin(latitude * M_PI / 180) * sin(declinaison * M_PI / 180) + cos(latitude * M_PI / 180) * cos(
		declinaison * M_PI / 180) * cos(omega * M_PI / 180))) * 180 / M_PI;
	azimuth = (asin(cos(declinaison * M_PI / 180) * sin(omega * M_PI / 180) / cos(hauteur * M_PI / 180))) * 180 / M_PI;


	// Module de Correction de l'angle car asin dans [-Pi/2,Pi/2]
	//			omega2 = 15 * (TS + 0.01 - 12); // decalage de 36 secondes (choisit au hasard)
	//			hauteur2 = (asin(sin(latitude*2*M_PI/360) * sin(declinaison*2*M_PI/360) + cos(latitude*2*M_PI/360) * cos(declinaison*2*M_PI/360) * cos(omega2*2*M_PI/360)))*360/(2*M_PI);
	//			azimuth2 = (asin(cos(declinaison*2*M_PI/360) * sin(omega2*2*M_PI/360) / cos(hauteur2*2*M_PI/360)))*360/(2*M_PI);

	*zenithAngles = 90 + hauteur;
	*azimuthAngles = azimuth - angleNorthMinus0x;


	// Arrondi � 2 decimales apres la virgule
	*zenithAngles = floor(100 * *zenithAngles) / 100;
	*azimuthAngles = floor(100 * *azimuthAngles) / 100;


	// Nettoyage des r�sultats
	if (*zenithAngles > 180)
		*zenithAngles = 180;
	if (*zenithAngles < 0)
		*zenithAngles = 0;
	while (*azimuthAngles >= 360)
		*azimuthAngles -= 360;
	while (*azimuthAngles < 0)
		*azimuthAngles += 360;
}

const double radian(const double & degre) {
	return degre * M_PI / 180;
}
const double degre(const double & radian) {
	return radian * 180 / M_PI;
}

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
						const double & sensorLongitude) {
	int updatedHour = hour;
	int updatedMinute = minute;
	//	int supplementaryhour = floor(sensorLongitude - targetLongitude) * 24. / 360.;
	//	updatedHour += supplementaryhour;
	//	updatedMinute += (sensorLongitude - targetLongitude) / 360. * 24. * 60. - supplementaryhour * 60 ;
	SolarAngle(solarZenith, solarAzimuth, year, month, day, updatedHour, updatedMinute, second, sensorLatitude,
		sensorLongitude, timeZone, daylightSavingTime);
}

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
				const bool & daylightSavingTime) {
	/*
	 * Recadrage de la date fournie en entree :
	 * [0 <= secondes < 60]; [0 <= minutes < 60]; [0 <= heures < 24]
	 */
	double secondesRecadrees = second;
	double minutesRecadrees = minute;
	// Tant que le nombre de secondes est superieur ou egal a 60, on lui retranche 60, et on ajoute 1 aux minutes.
	while (secondesRecadrees >= 60) {
		secondesRecadrees -= 60;
		++minutesRecadrees;
	}
	// Tant que le nombre de secondes est inferieur a 0, on lui ajoute 60, et on retire 1 aux minutes.
	while (secondesRecadrees < 0) {
		secondesRecadrees += 60;
		--minutesRecadrees;
	}

	double heuresRecadrees = hour;
	// Tant que le nombre de minutes est superieur ou egal a 60, on lui retranche 60, et on ajoute 1 aux heures.
	while (minutesRecadrees >= 60) {
		minutesRecadrees -= 60;
		++heuresRecadrees;
	}
	// Tant que le nombre de minutes est inferieur a 0, on lui ajoute 60, et on retire 1 aux heures.
	while (minutesRecadrees < 0) {
		minutesRecadrees += 60;
		--heuresRecadrees;
	}

	// Si l'heure d'ete est active, on ajoute 1 heure a l'heure fournie.
	if (daylightSavingTime) {
		++heuresRecadrees;
	}

	double joursRecadrees = day;
	// Tant que le nombre d'heures est superieur ou egal a 24, on lui retranche 24, et on ajoute 1 aux jours.
	while (heuresRecadrees >= 24) {
		heuresRecadrees -= 24;
		++joursRecadrees;
	}
	// Tant que le nombre d'heures est inferieur a 0, on lui ajoute 24, et on retire 1 aux jours.
	while (heuresRecadrees < 0) {
		heuresRecadrees += 24;
		--joursRecadrees;
	}

	/*
	 * Utilisation de la librairie date_time de Boost pour gerer la date JJ/MM/AA, avec les annees bissextiles, mois a 28,29,30,31 jours etc.	 *
	 * [1 <= jours <= (29,30,31)]; [1 <= Mois <= 12]; [1400 <= Annee <= 10 000]
	 */
	using namespace boost::gregorian;
	// On se place au 1er janvier de l'annee fournit en parametre (date qui existe toujours (tant que >= 1400...), et est insensible aux annees bissextiles et autres)
	date dateRecadree(year, Jan, 1);
	// On traite les mois en premier, afin d'eviter les problemes de "snap-to-end-of-month"/"collage-a-la-fin-du-mois" :
	// http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/gregorian.html#snap_to_details
	// On lui ajoute (ou retire si le nombre est negatif) le nombre de mois fournit en entree -1 (les mois vont de 1 a 12, et la date est deja a 1)
	if (month - 1 > 0) { // Si le mois en entree est fevrier (2), on ajoutera 2-1=1 mois a partir de janvier : ce qui donnera bien fevrier.
		dateRecadree += months(month - 1);
	} else if (month <= 0) { // 0 correspond a -1 mois par rapport a janvier, -1 a -2 mois etc. Il s'agit (tres certainement) du resultat d'un passage par le sequenceur.
		dateRecadree -= months(1 - month);
	}

	// On ajoute (ou retire si le nombre est negatif) le nombre de jours recadres -1, (les jours vont de 1 a (potentiellement) 31, et la date est deja a 1)
	if (joursRecadrees - 1 > 0) {
		dateRecadree += days(joursRecadrees - 1);
	} else if (joursRecadrees <= 0) { // 0 correspond a -1 jour par rapport au 1er du mois, -1 a -2 jours etc. Il s'agit (tres certainement) du resultat d'un passage par le sequenceur.
		dateRecadree -= days(1 - joursRecadrees);
	}

	/*
	 * Conversion en jour Julien (JD : Julian Date)
	 * Notre calendrier est le calendrier Gregorien, qui fut introduit en 1582 en remplacement du calendrier Julien (qui comme son nom l'indique fut introduit par Jules Cesar vers -46 av. J.C.)
	 * En dehors du point de vue culture general, la date que nous recuperons est une date Gregorienne decomposee (en annee, mois, jours etc.),
	 * tandis que les algorithme qui vont suivre utiliseront une date Julienne (JD : Julian Date)
	 * qui est un nombre decimal, ou la partie entiere est le nombre de jours (JDN : Julian Date Number), et la partie decimale decrit les heures, minutes, secondes.
	 * Le nombre de jours est compte a partir de midi, 24 novembre 4714 av. J.-C. (dans le calendrier Gregorien).
	 */

	long jdn = dateRecadree.julian_day();


	// Calcul de l'heure solaire (fraction dans la journee), par rapport a midi (d'ou le -0,5)
	double meanDayTime = heuresRecadrees / 24 + minutesRecadrees / 1440 + secondesRecadrees / 86400;
	double solarTime = meanDayTime - 0.5;


	// Sommation de la partie entiere et fractionnel, ainsi que de la time zone
	double jd = solarTime + ((double) jdn) - timeZone / 24;


	// Calcul du Julian Century
	double jc = (jd - 2451545) / 36525;


	/*
	 * Formule provenant de :
	 * NOAA Sun calculation
	 * http://www.esrl.noaa.gov/gmd/grad/solcalc/
	 */
	double geomMeanLongSun = fmod(280.46646 + jc * (36000.76983 + jc * 0.0003032), 360); // Degre
	double geomMeanAnomSun = 357.52911 + jc * (35999.05029 - 0.0001537 * jc); // Degre
	double eccentEarthOrbit = 0.016708634 - jc * (0.000042037 + 0.0000001267 * jc);
	double sunEqOfCtr = sin(radian(geomMeanAnomSun)) * (1.914602 - jc * (0.004817 + 0.000014 * jc)) + sin(radian(2
			* geomMeanAnomSun)) * (0.019993 - 0.000101 * jc) + sin(radian(3 * geomMeanAnomSun)) * 0.000289;

	double sunTrueLong = geomMeanLongSun + sunEqOfCtr; //deg
	double sunAppLong = sunTrueLong - 0.00569 - 0.00478 * sin(radian(125.04 - 1934.136 * jc)); // deg
	double meanObliqEcliptic = 23 + (26 + ((21.448 - jc * (46.815 + jc * (0.00059 - jc * 0.001813)))) / 60) / 60; // deg
	double obliqCorrection = meanObliqEcliptic + 0.00256 * cos(radian(125.04 - 1934.136 * jc)); // deg

	double sunDeclin = degre(asin(sin(radian(obliqCorrection)) * sin(radian(sunAppLong)))); // deg
	double varY = tan(radian(obliqCorrection / 2)) * tan(radian(obliqCorrection / 2));

	double eqOfTime = 4 * degre(varY * sin(2 * radian(geomMeanLongSun)) - 2 * eccentEarthOrbit * sin(radian(
		geomMeanAnomSun)) + 4 * eccentEarthOrbit * varY * sin(radian(geomMeanAnomSun)) * cos(2
			* radian(geomMeanLongSun)) - 0.5 * varY * varY * sin(4 * radian(geomMeanLongSun)) - 1.25 * eccentEarthOrbit
			* eccentEarthOrbit * sin(2 * radian(geomMeanAnomSun))); // minutes

	double trueSolarTime = fmod(meanDayTime * 1440 + eqOfTime + 4 * longitude - 60 * timeZone, 1440); // minutes

	double hourAngle = trueSolarTime / 4; // deg
	if (hourAngle < 0) {
		hourAngle += 180;
	} else {
		hourAngle -= 180;
	}

	double solarZenithAngle = degre(acos(sin(radian(latitude)) * sin(radian(sunDeclin)) + cos(radian(latitude)) * cos(
		radian(sunDeclin)) * cos(radian(hourAngle))));


	// Calcul final (et enregistrement) des angles solaires azimuthaux et zenithaux
	if (hourAngle > 0) {
		solarAzimuth = fmod(degre(acos(((sin(radian(latitude)) * cos(radian(solarZenithAngle)))
				- sin(radian(sunDeclin))) / (cos(radian(latitude)) * sin(radian(solarZenithAngle))))) + 180, 360);
	} else {
		solarAzimuth = fmod(540 - degre(acos(((sin(radian(latitude)) * cos(radian(solarZenithAngle))) - sin(radian(
			sunDeclin))) / (cos(radian(latitude)) * sin(radian(solarZenithAngle))))), 360);
	}
	solarAzimuth = 360 - solarAzimuth;
	solarZenith = 180 - solarZenithAngle;
}

int calculJourDeLAnnee(	int & year,
						int & month,
						int & joursRecadrees,
						int & heuresRecadrees,
						int & minutesRecadrees,
						int & secondesRecadrees,
						double & timeZone,
						const bool & daylightSavingTime) {
	/*
	 * Recadrage de la date fournie en entree :
	 * [0 <= secondes < 60]; [0 <= minutes < 60]; [0 <= heures < 24]
	 */
	// Tant que le nombre de secondes est superieur ou egal a 60, on lui retranche 60, et on ajoute 1 aux minutes.
	while (secondesRecadrees >= 60) {
		secondesRecadrees -= 60;
		++minutesRecadrees;
	}
	// Tant que le nombre de secondes est inferieur a 0, on lui ajoute 60, et on retire 1 aux minutes.
	while (secondesRecadrees < 0) {
		secondesRecadrees += 60;
		--minutesRecadrees;
	}

	// Tant que le nombre de minutes est superieur ou egal a 60, on lui retranche 60, et on ajoute 1 aux heures.
	while (minutesRecadrees >= 60) {
		minutesRecadrees -= 60;
		++heuresRecadrees;
	}
	// Tant que le nombre de minutes est inferieur a 0, on lui ajoute 60, et on retire 1 aux heures.
	while (minutesRecadrees < 0) {
		minutesRecadrees += 60;
		--heuresRecadrees;
	}
	// Si l'heure d'ete est active, on ajoute 1 heure a l'heure fournie.
	if (daylightSavingTime) {
		++heuresRecadrees;
	}
	//On se place a l'heure UTC (Heure UTC = Heure Locale - TimeZone) ex: 6h a NYC alors 11h a LON (UTC), ici timeZone = -5
	heuresRecadrees -= timeZone;
	timeZone = 0;


	// Tant que le nombre d'heures est superieur ou egal a 24, on lui retranche 24, et on ajoute 1 aux jours.
	while (heuresRecadrees >= 24) {
		heuresRecadrees -= 24;
		++joursRecadrees;
	}
	// Tant que le nombre d'heures est inferieur a 0, on lui ajoute 24, et on retire 1 aux jours.
	while (heuresRecadrees < 0) {
		heuresRecadrees += 24;
		--joursRecadrees;
	}

	/*
	 * Utilisation de la librairie date_time de Boost pour gerer la date JJ/MM/AA, avec les annees bissextiles, mois a 28,29,30,31 jours etc.	 *
	 * [1 <= jours <= (29,30,31)]; [1 <= Mois <= 12]; [1400 <= Annee <= 10 000]
	 */
	using namespace boost::gregorian;
	// On se place au 1er janvier de l'annee fournit en parametre (date qui existe toujours (tant que >= 1400...), et est insensible aux annees bissextiles et autres)
	date dateRecadree(year, Jan, 1);
	// On traite les mois en premier, afin d'eviter les problemes de "snap-to-end-of-month"/"collage-a-la-fin-du-mois" :
	// http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/gregorian.html#snap_to_details
	// On lui ajoute (ou retire si le nombre est negatif) le nombre de mois fournit en entree -1 (les mois vont de 1 a 12, et la date est deja a 1)
	if (month - 1 > 0) { // Si le mois en entree est fevrier (2), on ajoutera 2-1=1 mois a partir de janvier : ce qui donnera bien fevrier.
		dateRecadree += months(month - 1);
	} else if (month <= 0) { // 0 correspond a -1 mois par rapport a janvier, -1 a -2 mois etc. Il s'agit (tres certainement) du resultat d'un passage par le sequenceur.
		dateRecadree -= months(1 - month);
	}

	// On ajoute (ou retire si le nombre est negatif) le nombre de jours recadres -1, (les jours vont de 1 a (potentiellement) 31, et la date est deja a 1)
	if (joursRecadrees - 1 > 0) {
		dateRecadree += days(joursRecadrees - 1);
	} else if (joursRecadrees <= 0) { // 0 correspond a -1 jour par rapport au 1er du mois, -1 a -2 jours etc. Il s'agit (tres certainement) du resultat d'un passage par le sequenceur.
		dateRecadree -= days(1 - joursRecadrees);
	}
	month = dateRecadree.month();
	year = dateRecadree.year();
	return dateRecadree.day_of_year();
}

/**
 * Formule du fichier excel/doc Courbure_Terre_Capteur
 */
void sensorAngle(	/* Parametres de sortie */double & sensorZenith,
					double & sensorAzimuth,
					/* Parametres d'entrees geographiques du capteur */const double & sensorLatitude,
					const double & sensorLongitude,
					const double & sensorAltitude,
					/* Parametres d'entrees geographiques de la cible */const double & targetLatitude,
					const double & targetLongitude,
					const double & targetAltitude) {
	static const double EARTH_VERTICAL_RADIUS = 6356.751; // KM
	static const double EARTH_HORIZONTAL_RADIUS = 6378.136; // KM

	double earthDistance_O_Focal = sqrt(pow(EARTH_HORIZONTAL_RADIUS, 2) - pow(EARTH_VERTICAL_RADIUS, 2));
	double earthExcentricity = earthDistance_O_Focal / EARTH_HORIZONTAL_RADIUS;

	double targetLocalRadius = EARTH_VERTICAL_RADIUS
			/ sqrt(1 - pow(earthExcentricity * cos(radian(targetLatitude)), 2));
	double sensorLocalRadius = EARTH_VERTICAL_RADIUS
			/ sqrt(1 - pow(earthExcentricity * cos(radian(sensorLatitude)), 2));

	double targetTheta = 90. - targetLatitude;
	double sensorTheta = 90. - sensorLatitude;


	// Ellipse Level
	double otx = targetLocalRadius * sin(radian(targetTheta));
	double osx = sensorLocalRadius * sin(radian(sensorTheta)) * cos(radian(sensorLongitude - targetLongitude));

	double oty = 0;
	double osy = sensorLocalRadius * sin(radian(sensorTheta)) * sin(radian(sensorLongitude - targetLongitude));

	double otz = targetLocalRadius * cos(radian(targetTheta));
	double osz = sensorLocalRadius * cos(radian(sensorTheta));

	// Local Vertical
	double omegaTx = 1;
	double omegaSx = 1;

	double omegaTy = 0;
	double omegaSy = 0;

	double omegaTz = pow(EARTH_HORIZONTAL_RADIUS, 2) / pow(EARTH_VERTICAL_RADIUS, 2) * otz / otx * omegaTx;
	double omegaSz = pow(EARTH_HORIZONTAL_RADIUS, 2) / pow(EARTH_VERTICAL_RADIUS, 2) * osz / osx * omegaSx;

	double omegaTLongueur = sqrt(pow(omegaTx, 2) + pow(omegaTy, 2) + pow(omegaTz, 2));
	double omegaSLongueur = sqrt(pow(omegaSx, 2) + pow(omegaSy, 2) + pow(omegaSz, 2));

	double targetLocalVerticalYStar = acos(omegaTz / omegaTLongueur);
	double sensorLocalVerticalYStar = acos(omegaSz / omegaSLongueur);

	// Target/Sensor Level
	double oTx = otx + targetAltitude * sin(targetLocalVerticalYStar);
	double oSx = osx + sensorAltitude * sin(sensorLocalVerticalYStar) * cos(radian(sensorLongitude - targetLongitude));

	double oTy = oty;
	double oSy = osy + sensorAltitude * sin(sensorLocalVerticalYStar) * sin(radian(sensorLongitude - targetLongitude));

	double oTz = otz + targetAltitude * cos(targetLocalVerticalYStar);
	double oSz = osz + sensorAltitude * cos(sensorLocalVerticalYStar);

	// Earth TS
	double tsx = oSx - oTx;
	double tsy = oSy - oTy;
	double tsz = oSz - oTz;
	double tsLongueur = sqrt(pow(tsx, 2) + pow(tsy, 2) + pow(tsz, 2));


	// Target Tangent
	double utx = 1;
	double uty = 0;
	double utz = -utx * otx * pow(EARTH_VERTICAL_RADIUS, 2) / pow(EARTH_HORIZONTAL_RADIUS, 2) / otz;
	if (targetLatitude < 0) {
		utx = -1;
		utz = -utz;
	} else if (targetLatitude == 0) {
		utx = 0;
		utz = 1;
	}
	double utLongueur = sqrt(pow(utx, 2) + pow(uty, 2) + pow(utz, 2));

	// Alpha
	double facteur = (tsx * omegaTx + tsy * omegaTy + tsz * omegaTz) / (tsLongueur * omegaTLongueur);
	double alphaLocal = 0;
	if (facteur <= -1) {
		alphaLocal = M_PI;
	} else if (facteur >= 1) {
		alphaLocal = 0;
	} else {
		alphaLocal = acos(facteur); // RAD
	}

	// Phi
	facteur = (utx * tsx + uty * tsy + utz * tsz) / (utLongueur * tsLongueur * sin(alphaLocal));
	double sensorPhi = 0; // RAD
	if (abs(facteur) >= 1) {
		if (targetLatitude < sensorLatitude) {
			sensorPhi = M_PI;
		} // else {sensorPhi = 0.;}
	} else {
		sensorPhi = acos(facteur); // RAD
	}
	if (targetLongitude >= sensorLongitude) {
		sensorPhi = 2 * M_PI - sensorPhi;
	}

	// Enregistrement des valeurs en degre
	sensorZenith = degre(alphaLocal);
	sensorAzimuth = degre(sensorPhi);
}
