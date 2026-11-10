/*
 * DirectionsMetier.h
 *
 *  Created on: 25 sept. 2009
 *      Author: crosj
 */

#ifndef VALEURSNULLES_H_
#define VALEURSNULLES_H_
#include <string>
#include <iostream>

template<typename T> struct ValeurNulle;

template<> struct ValeurNulle<int> {
	static int Zero() {
		return 0;
	}
};
template<> struct ValeurNulle<double> {
	static double Zero() {
		return 0.0;
	}
};
template<> struct ValeurNulle<std::string> {
	static std::string Zero() {
		return "";
	}
};

#endif /* ACCESFICHIERS_H_ */
