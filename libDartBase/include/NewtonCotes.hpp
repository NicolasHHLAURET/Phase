/*
 * NewtonCotes.hpp
 *
 *  Created on: Feb 15, 2012
 *      Author: yint
 */

#ifndef NEWTONCOTES_HPP_
#define NEWTONCOTES_HPP_

#include<iostream>
#include<vector>
#include<list>
#include "math.h"

#include "DirectionRoot.hpp"

using namespace std;

template<typename T>
class NewtonCotesIntegrationCalculator {
private:
	vector<vector<T> > integrant;
	vector<T> integrant1D;
	vector<double> coefficientsX;
	vector<double> coefficientsY;
	double deltaX;
	double deltaY;
	double output;
	unsigned int numberOfPointsX; //Number of points used to estimate the integration.
	unsigned int numberOfPointsY;

public:
	NewtonCotesIntegrationCalculator() {
		deltaX = 0;
		deltaY = 0;
		output = 0;
		numberOfPointsX = 0;
		numberOfPointsY = 0;
	}
	NewtonCotesIntegrationCalculator(	const vector<vector<T> > & input,
										const double & deltaX,
										const double & deltaY) {
		setInput(input);
		setDeltaX(deltaX);
		setDeltaY(deltaY);
		calculateIntegration2D();
	}
	
	NewtonCotesIntegrationCalculator(	const vector<T> & input,
										const double & deltaX) {
		setInput1D(input);

		setDeltaX(deltaX);
		calculateIntegration1D();
	}
	
	void setNumberOfPointsX(const unsigned int & numOfPointsX);
	void setNumberOfPointsY(const unsigned int & numOfPointsY);
	void setInput(const vector<vector<T> > & input);
	void setInput1D(const vector<T> & input);
	void calculateIntegration1D();
	void calculateIntegration2D();
	void setDeltaX(const double & deltaX);
	void setDeltaY(const double & deltaY);
	double getDeltaX() const;
	double getDeltaY() const;
	double getOutput() const;

	static void generatePointsMapFromDir(	DirectionRoot const& dir,
											unsigned int const& n,
											vector<vector<Doublet<double> > > & pointsMap,
											double & unitDeltaCosTheta,
											double & unitDeltaPhi);

	//    static
	
	static const double coefMap[11][11];
	
};

// 29 Mar 2012 generatePointsMapSA: generate a 2D (n*n) points map of (theta,phi) from a single direction.
template<typename T>
void NewtonCotesIntegrationCalculator<T>::generatePointsMapFromDir(	DirectionRoot const& dir,
																	unsigned int const& n,
																	vector<vector<Doublet<double> > > & pointsMap,
																	double & unitDeltaCosTheta,
																	double & unitDeltaPhi) {
	double beginCosTheta = cos(dir.getBeginTheta());
	double beginPhi = dir.getBeginPhi();
	unitDeltaCosTheta = dir.getDeltaCosineTheta() / (n - 1);
	unitDeltaPhi = dir.getDeltaPhi() / (n - 1);
	pointsMap.resize(n, vector<Doublet<double> >(n));
	
	for (unsigned int i = 0; i < n; i++) {
		for (unsigned int j = 0; j < n; j++) {
			pointsMap[i][j] = Doublet<double>(acos(beginCosTheta - i * unitDeltaCosTheta), beginPhi + j * unitDeltaPhi);
		}
	}
}

//template <typename T>
//vector<vector<vector<Doublet<double> > > > NewtonCotesIntegrationCalculator2D<T>::wholePointsMapFromDirList(DirectionRoot const& dir, unsigned int const& n, vector<vector<Doublet<double> > > & pointsMap,
//		double & unitDeltaCosTheta, double & unitDeltaPhi) {
//	double beginCosTheta = cos(dir.getBeginTheta());
//	double beginPhi = dir.getBeginPhi();
//	unitDeltaCosTheta = dir.getDeltaCosineTheta() / (n - 1);
//	unitDeltaPhi = dir.getDeltaPhi() / (n - 1);
//	pointsMap.resize(n, vector<Doublet<double> > (n));
//
//	for (unsigned int i = 0; i < n; i++) {
//		for (unsigned int j = 0; j < n; j++) {
//			pointsMap[i][j] = Doublet<double> (acos(beginCosTheta - i * unitDeltaCosTheta), beginPhi + j * unitDeltaPhi);
//		}
//	}
//}

template<typename T>
double const NewtonCotesIntegrationCalculator<T>::coefMap[11][11] =
		{ { 1 }, { 1.0 / 2, 1.0 / 2 }, { 1.0 / 3, 4.0 / 3, 1.0 / 3 }, { 3.0 / 8, 9.0 / 8, 9.0 / 8, 3.0 / 8 }, { 2.0 / 45 * 7, 2.0 / 45 * 32, 2.0 / 45
				* 12, 2.0 / 45 * 32, 2.0 / 45 * 7 },
//{5.0/288*19, 5.0/288*75, 5.0/288*50, 5.0/288*50, 5.0/288*75, 5.0/288*19},
		//{1.0/140*41, 1.0/140*216, 1.0/140*27, 1.0/140*272, 1.0/140*27, 1.0/140*216, 1.0/140*41},
		//{7.0/17280*751, 7.0/17280*3577, 7.0/17280*1323, 7.0/17280*2989, 7.0/17280*2989, 7.0/17280*1323, 7.0/17280*3577, 7.0/17280*751}
		};

template<typename T>
inline double NewtonCotesIntegrationCalculator<T>::getDeltaX() const {
	return deltaX;
}

template<typename T>
inline double NewtonCotesIntegrationCalculator<T>::getDeltaY() const {
	return deltaY;
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setDeltaX(double const& deltaX) {
	this->deltaX = deltaX;
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setDeltaY(double const& deltaY) {
	this->deltaY = deltaY;
}

template<typename T>
inline double NewtonCotesIntegrationCalculator<T>::getOutput() const {
	return output;
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setInput(vector<vector<T> > const& input) {
	this->integrant = input;
	if (integrant.size() == 0 || integrant[0].size() == 0) {
		cerr << "Error: The input is not in 2D." << endl;
		//		abort();
	}
	setNumberOfPointsX(integrant.size());
	setNumberOfPointsY(integrant[0].size());
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setInput1D(vector<T> const& input) {
	this->integrant1D = input;
	setNumberOfPointsX(integrant1D.size());
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::calculateIntegration2D() {
	output = 0;
	for (unsigned int i = 0; i < numberOfPointsX; i++) {
		double segTheta = 0;
		for (unsigned int j = 0; j < numberOfPointsY; j++) {
			segTheta += integrant[i][j] * coefficientsY[j] * deltaY;
		}
		output += segTheta * coefficientsX[i];
	}
	output *= deltaX;
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::calculateIntegration1D() {
	output = 0;
	for (unsigned int i = 0; i < numberOfPointsX; i++) {
		double segTheta = integrant1D[i];
		output += segTheta * coefficientsX[i];
	}
	output *= deltaX;
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setNumberOfPointsX(unsigned int const& numX) {
	if (numX < 2) {
		cerr << "Error: At least 2 points are needed for Newton-Cotes Integration" << endl;
		//		abort();
	}
	
	coefficientsX.clear();
	numberOfPointsX = numX;
	unsigned int numSet, numSet_Temp;
	numSet = ceil(double(numberOfPointsX) / 5);
	do {
		numSet_Temp = numSet;
		numSet = ceil(double(numberOfPointsX + numSet_Temp - 1) / 5);
	} while (numSet != numSet_Temp);
	//Iterative way to adaptively find the suitable number of points used.
	unsigned int expectNumberOfPoints = numberOfPointsX + numSet - 1;
	
	unsigned int low = floor(expectNumberOfPoints / numSet);
	unsigned int high = low + 1;
	unsigned int numHigh = (expectNumberOfPoints) % numSet;
	unsigned int numLow = numSet - numHigh;
	
	double tempX = 0;
	for (unsigned int i = 0; i < numSet; i++) {
		unsigned int numPoints;
		if (i < numLow)
			numPoints = low;
		else
			numPoints = high;
		
		for (unsigned int j = 0; j < numPoints; j++) {
			if (j == 0) {
				coefficientsX.push_back(coefMap[numPoints - 1][j] + tempX);
			} else {
				if ((i == numSet - 1) && (j == numPoints - 1))
					coefficientsX.push_back(coefMap[numPoints - 1][j]);
				else if (j != numPoints - 1)
					coefficientsX.push_back(coefMap[numPoints - 1][j]);
			}
			if (j == numPoints - 1)
				tempX = coefMap[numPoints - 1][j];
		}
	}
	tempX = 0;
	vector<double>::iterator i;
	double sum = 0;
	for (i = coefficientsX.begin(); i < coefficientsX.end(); i++) {
		sum += (*i);
	}
	//	cout<<"\nSummation of Coefficient X of Newton-Cotes Integration: "<<sum<<endl;
	
}

template<typename T>
inline void NewtonCotesIntegrationCalculator<T>::setNumberOfPointsY(unsigned int const& numY) {
	if (numY < 2) {
		cerr << "Error: At least 2 points are needed for Newton-Cotes Close Integration" << endl;
		throw(0);
		//		abort();
	}
	
	coefficientsY.clear();
	numberOfPointsY = numY;
	unsigned int numSet, numSet_Temp;
	numSet = ceil(double(numberOfPointsY) / 5);
	do {
		numSet_Temp = numSet;
		numSet = ceil(double(numberOfPointsY + numSet_Temp - 1) / 5);
	} while (numSet != numSet_Temp);
	//Iterative way to adaptively find the suitable number of points used.
	unsigned int expectNumberOfPoints = numberOfPointsY + numSet - 1;
	
	unsigned int low = floor(expectNumberOfPoints / numSet);
	unsigned int high = low + 1;
	unsigned int numHigh = (expectNumberOfPoints) % numSet;
	unsigned int numLow = numSet - numHigh;
	double tempY = 0;
	for (unsigned int i = 0; i < numSet; i++) {
		unsigned int numPoints;
		if (i < numLow)
			numPoints = low;
		else
			numPoints = high;
		
		for (unsigned int j = 0; j < numPoints; j++) {
			if (j == 0) {
				coefficientsY.push_back(coefMap[numPoints - 1][j] + tempY);
			} else {
				if ((i == numSet - 1) && (j == numPoints - 1))
					coefficientsY.push_back(coefMap[numPoints - 1][j]);
				else if (j != numPoints - 1)
					coefficientsY.push_back(coefMap[numPoints - 1][j]);
			}
			if (j == numPoints - 1)
				tempY = coefMap[numPoints - 1][j];
		}
	}
	tempY = 0;
	vector<double>::iterator i;
	double sum = 0;
	for (i = coefficientsY.begin(); i < coefficientsY.end(); i++) {
		sum += (*i);
	}
	//	cout<<"Summation of Coefficient Y of Newton-Cotes Integration: "<<sum<<endl;
	
}

#endif /* NEWTONCOTES_HPP_ */
