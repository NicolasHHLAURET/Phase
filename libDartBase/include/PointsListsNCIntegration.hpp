/*
 * PointsListsNCIntegration.hpp
 *
 *  Created on: Feb 15, 2012
 *      Author: yint
 */

#ifndef POINTLIST_NC_HPP_
#define POINTLIST_NC_HPP_

#include<iostream>
#include<vector>
#include<list>
#include "math.h"

#include "DirectionRoot.hpp"
#include "Vecteur.hpp"

using namespace std;

template<typename T>
class PointsListsNCIntegration {
private:
	vector<T> _listOfDirections;
	unsigned int _numberOfDivisions;
	vector<vector<vector<Doublet<double> > > > _pointsList;
	vector<vector<vector<Vecteur<double> > > > _pointsListCartesian;
	vector<double> _deltaCosTheta;
	vector<double> _deltaPhi;

public:
	PointsListsNCIntegration() {
		_numberOfDivisions = 2;
	}
	PointsListsNCIntegration(	const vector<T> & listOfDirections,
								const unsigned int & numberOfDivisions) {
		setDirList(listOfDirections).setNumDiv(numberOfDivisions);
		generatePointsLists();
	}
	PointsListsNCIntegration(	const vector<T> & listOfDirections,
								const unsigned int & numberOfDivisions,
								const bool & halfDeltaPhi) {
		setDirList(listOfDirections).setNumDiv(numberOfDivisions);
		generatePointsLists(halfDeltaPhi);
	}
	
	const vector<T> & getDirList() const;
	unsigned int getNumDiv() const;
	PointsListsNCIntegration<T> & setDirList(vector<T> const& listOfDirections);
	PointsListsNCIntegration<T> & setNumDiv(unsigned int const& numberOfDivisions);

	const vector<vector<Doublet<double> > > & getPointsMap(unsigned int index) const;
	const vector<vector<Vecteur<double> > > & getPointsMapCartesian(unsigned int index) const;
	double getDeltaCosTheta(unsigned int index) const;
	double getDeltaPhi(unsigned int index) const;

	void generatePointsLists();
	void generatePointsLists(const bool & halfDeltaPhi);

	static void generatePointsMapFromDir(	const T & dir,
											const unsigned int & n,
											vector<vector<Doublet<double> > > & pointsMap,
											vector<vector<Vecteur<double> > > & pointsMapCartesian,
											double & unitDeltaCosTheta,
											double & unitDeltaPhi);
	static void generatePointsMapFromDir(	const T & dir,
											const unsigned int & n,
											vector<vector<Doublet<double> > > & pointsMap,
											vector<vector<Vecteur<double> > > & pointsMapCartesian,
											double & unitDeltaCosTheta,
											double & unitDeltaPhi,
											const bool & halfDeltaPhi);
};

// 30 Mar 2012 generatePointsMapSA: generate a 2D (n*n) points map of (theta,phi) from a single direction.
template<typename T>
void PointsListsNCIntegration<T>::generatePointsLists() {
	unsigned int listOfDirectionsSize = _listOfDirections.size();
	_deltaCosTheta.resize(listOfDirectionsSize);
	_deltaPhi.resize(listOfDirectionsSize);
	for (unsigned int directionIndex = 0; directionIndex < listOfDirectionsSize; directionIndex++) {
		vector<vector<Doublet<double> > > pointsMap;
		vector<vector<Vecteur<double> > > pointsMapCartesian;
		generatePointsMapFromDir(_listOfDirections[directionIndex], _numberOfDivisions, pointsMap, pointsMapCartesian, _deltaCosTheta[directionIndex],
			_deltaPhi[directionIndex]);
		_pointsList.push_back(pointsMap);
		_pointsListCartesian.push_back(pointsMapCartesian);
	}
}

template<typename T>
void PointsListsNCIntegration<T>::generatePointsLists(const bool & halfDeltaPhi) {
	unsigned int listOfDirectionsSize = _listOfDirections.size();
	_deltaCosTheta.resize(listOfDirectionsSize);
	_deltaPhi.resize(listOfDirectionsSize);
	for (unsigned int directionIndex = 0; directionIndex < listOfDirectionsSize; directionIndex++) {
		vector<vector<Doublet<double> > > pointsMap;
		vector<vector<Vecteur<double> > > pointsMapCartesian;
		generatePointsMapFromDir(_listOfDirections[directionIndex], _numberOfDivisions, pointsMap, pointsMapCartesian, _deltaCosTheta[directionIndex],
			_deltaPhi[directionIndex], halfDeltaPhi);
		_pointsList.push_back(pointsMap);
		_pointsListCartesian.push_back(pointsMapCartesian);
	}
}

// 29 Mar 2012 generatePointsMapSA: generate a 2D (n*n) points map of (theta,phi) from a single direction.
template<typename T>
void PointsListsNCIntegration<T>::generatePointsMapFromDir(	T const& direction,
															unsigned int const& numberOfDivisions,
															vector<vector<Doublet<double> > > & pointsMap,
															vector<vector<Vecteur<double> > > & pointsMapCartesian,
															double & unitDeltaCosTheta,
															double & unitDeltaPhi) {
	double cosMinZenithAngle = cos(direction.getBeginTheta());
	double minAzimuthAngle = direction.getBeginPhi();
	unsigned int numberOfDivisionsForNadir = numberOfDivisions;

	//the azimuthal integral steps is larger for nadir direction since the azimuthal angle of which is larger.
	if(direction.getDeltaPhi() == 2*M_PI and numberOfDivisions <= 12){
		numberOfDivisionsForNadir = numberOfDivisions * 4;
	}

	unitDeltaCosTheta = direction.getDeltaCosineTheta() / (numberOfDivisions - 1);
	unitDeltaPhi = direction.getDeltaPhi() / (numberOfDivisionsForNadir - 1);

	pointsMap.resize(numberOfDivisions, vector<Doublet<double> >(numberOfDivisionsForNadir));
	pointsMapCartesian.resize(numberOfDivisions, vector<Vecteur<double> >(numberOfDivisionsForNadir));

	//unitDeltaCosTheta = direction.getDeltaCosineTheta() / (numberOfDivisions - 1);
	//unitDeltaPhi = direction.getDeltaPhi() / (numberOfDivisions - 1);
	//pointsMap.resize(numberOfDivisions, vector<Doublet<double> >(numberOfDivisions));
	//pointsMapCartesian.resize(numberOfDivisions, vector<Vecteur<double> >(numberOfDivisions));
	
	double cosZenithAngle;
	double cosAzimuthAngle;
	double sinZenithAngle;
	double sinAzimuthAngle;
	double zenithAngle;
	double azimuthAngle;
	for (unsigned int i = 0; i < numberOfDivisions; i++) {
		for (unsigned int j = 0; j < numberOfDivisionsForNadir; j++) {
			zenithAngle = acos(cosMinZenithAngle - i * unitDeltaCosTheta);
			azimuthAngle = minAzimuthAngle + j * unitDeltaPhi;
			pointsMap[i][j] = Doublet<double>(zenithAngle, azimuthAngle);
			cosZenithAngle = cos(zenithAngle);
			cosAzimuthAngle = cos(azimuthAngle);
			sinZenithAngle = sin(zenithAngle);
			sinAzimuthAngle = sin(azimuthAngle);
			// Passage au repere global (x,y,z)
			Vecteur<double> result(sinZenithAngle * cosAzimuthAngle, sinZenithAngle * sinAzimuthAngle, cosZenithAngle);
			//= normalise_vecteur (res);
			pointsMapCartesian[i][j] = result;
			
		}
	}
}

template<typename T>
void PointsListsNCIntegration<T>::generatePointsMapFromDir(	T const& direction,
															unsigned int const& numberOfDivisions,
															vector<vector<Doublet<double> > > & pointsMap,
															vector<vector<Vecteur<double> > > & pointsMapCartesian,
															double & unitDeltaCosTheta,
															double & unitDeltaPhi,
															const bool & halfDeltaPhi) {
	double cosMinZenithAngle = cos(direction.getBeginTheta());
	double minAzimuthAngle = direction.getBeginPhi();
	double cosZenithAngle;
	double cosAzimuthAngle;
	double sinZenithAngle;
	double sinAzimuthAngle;
	double zenithAngle;
	double azimuthAngle;
	unitDeltaCosTheta = direction.getDeltaCosineTheta() / (numberOfDivisions - 1);
	unitDeltaPhi = direction.getDeltaPhi() * 1.0 / (numberOfDivisions - 1);
	if (halfDeltaPhi) {
		unitDeltaPhi = direction.getDeltaPhi() * 0.5 / (numberOfDivisions - 1);
	}
	pointsMap.resize(numberOfDivisions, vector<Doublet<double> >(numberOfDivisions));
	pointsMapCartesian.resize(numberOfDivisions, vector<Vecteur<double> >(numberOfDivisions));
	for (unsigned int i = 0; i < numberOfDivisions; i++) {
		for (unsigned int j = 0; j < numberOfDivisions; j++) {
			zenithAngle = acos(cosMinZenithAngle - i * unitDeltaCosTheta);
			azimuthAngle = minAzimuthAngle + j * unitDeltaPhi;
			pointsMap[i][j] = Doublet<double>(zenithAngle, azimuthAngle);
			cosZenithAngle = cos(zenithAngle);
			cosAzimuthAngle = cos(azimuthAngle);
			sinZenithAngle = sin(zenithAngle);
			sinAzimuthAngle = sin(azimuthAngle);
			// Passage au repere global (x,y,z)
			Vecteur<double> result(sinZenithAngle * cosAzimuthAngle, sinZenithAngle * sinAzimuthAngle, cosZenithAngle);
			//= normalise_vecteur (res);
			pointsMapCartesian[i][j] = result;
		}
	}
}
/*
template<typename T>
void PointsListsNCIntegration<T>::generatePointsMapFromDir(	T const& direction,
															unsigned int const& numberOfDivisions,
															vector<vector<Doublet<double> > > & pointsMap,
															vector<vector<Vecteur<double> > > & pointsMapCartesian,
															double & unitDeltaCosTheta,
															double & unitDeltaPhi,
															const double & SamplingFactor) {
	double cosMinZenithAngle = cos(direction.getBeginTheta());
	double minAzimuthAngle = direction.getBeginPhi();
	double cosZenithAngle;
	double cosAzimuthAngle;
	double sinZenithAngle;
	double sinAzimuthAngle;
	double zenithAngle;
	double azimuthAngle;
	unsigned int numberOfDivisionsForNadir = numberOfDivisions;

	if(direction.getDeltaPhi() == 2*M_PI){
		numberOfDivisionsForNadir = numberOfDivisions * 6;
	}

	unitDeltaCosTheta = direction.getDeltaCosineTheta() / (numberOfDivisions - 1 + 2 * SamplingFactor);
	unitDeltaPhi = direction.getDeltaPhi() / (numberOfDivisionsForNadir - 1 + 2 * SamplingFactor);

	minAzimuthAngle = minAzimuthAngle + SamplingFactor * unitDeltaPhi;
	cosMinZenithAngle = cosMinZenithAngle - SamplingFactor * unitDeltaCosTheta;

	pointsMap.resize(numberOfDivisions, vector<Doublet<double> >(numberOfDivisionsForNadir));
	pointsMapCartesian.resize(numberOfDivisions, vector<Vecteur<double> >(numberOfDivisionsForNadir));
	for (unsigned int i = 0; i < numberOfDivisions; i++) {
		for (unsigned int j = 0; j < numberOfDivisionsForNadir; j++) {
			zenithAngle = acos(cosMinZenithAngle - i * unitDeltaCosTheta);
			azimuthAngle = minAzimuthAngle + j * unitDeltaPhi;
			pointsMap[i][j] = Doublet<double>(zenithAngle, azimuthAngle);
			cosZenithAngle = cos(zenithAngle);
			cosAzimuthAngle = cos(azimuthAngle);
			sinZenithAngle = sin(zenithAngle);
			sinAzimuthAngle = sin(azimuthAngle);
			// Passage au repere global (x,y,z)
			Vecteur<double> result(sinZenithAngle * cosAzimuthAngle, sinZenithAngle * sinAzimuthAngle, cosZenithAngle);
			//= normalise_vecteur (res);
			pointsMapCartesian[i][j] = result;
		}
	}
}
*/
template<typename T>
const vector<vector<Doublet<double> > > & PointsListsNCIntegration<T>::getPointsMap(unsigned int index) const {
	return _pointsList[index];
}

template<typename T>
const vector<vector<Vecteur<double> > > & PointsListsNCIntegration<T>::getPointsMapCartesian(unsigned int index) const {
	return _pointsListCartesian[index];
}

template<typename T>
double PointsListsNCIntegration<T>::getDeltaCosTheta(unsigned int index) const {
	return _deltaCosTheta[index];
}

template<typename T>
double PointsListsNCIntegration<T>::getDeltaPhi(unsigned int index) const {
	return _deltaPhi[index];
}

template<typename T>
const vector<T> & PointsListsNCIntegration<T>::getDirList() const {
	return _listOfDirections;
}

template<typename T>
unsigned int PointsListsNCIntegration<T>::getNumDiv() const {
	return _numberOfDivisions;
}

template<typename T>
PointsListsNCIntegration<T> & PointsListsNCIntegration<T>::setDirList(vector<T> const& listOfDirections) {
	this->_listOfDirections = listOfDirections;
	return *this;
}

template<typename T>
PointsListsNCIntegration<T> & PointsListsNCIntegration<T>::setNumDiv(unsigned int const& numberOfDivisions) {
	this->_numberOfDivisions = numberOfDivisions;
	return *this;
}

#endif /* NEWTONCOTES_HPP_ */
