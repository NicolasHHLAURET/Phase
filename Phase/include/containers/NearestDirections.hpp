/*
 * NearestDirections.hpp
 *
 *  Created on: 13 avr. 2015
 *      Author: lauretn
 *
 * Copié/collé de la même classe dans DART
 */

#ifndef NEARESTDIRECTIONS_HPP_
#define NEARESTDIRECTIONS_HPP_

#include <vector>
#include <map>
using namespace std;

class NearestDirections {
public:
	typedef vector<pair<unsigned int, double> > NearestCorners;

	enum Corners {
		UPPER_LEFT = 0,
		UPPER_RIGHT = 1,
		LOWER_LEFT = 2,
		LOWER_RIGHT = 3
	};
private:
	NearestCorners _fourCorners;
	Corners _nearestCorner;

public:
	NearestDirections() : _fourCorners(4, pair<unsigned int, double>(0, 0.)), _nearestCorner(UPPER_LEFT) {
		// TODO Auto-generated constructor stub
		
	}
	virtual ~NearestDirections() {
		// TODO Auto-generated destructor stub
	}

	const NearestCorners & getNearestCorners() const {
		return _fourCorners;
	}
	const unsigned int & getNearestDirection() const {
		return _fourCorners[_nearestCorner].first;
	}

	void setNearestCorner(const unsigned int & directionIndex, const double & cosDistance, const Corners & corner);

	void setNearestUpperLeftCorner(const unsigned int & directionIndex, const double & distance) {
		setNearestCorner(directionIndex, distance, UPPER_LEFT);
	}
	void setNearestUpperRightCorner(const unsigned int & directionIndex, const double & distance) {
		setNearestCorner(directionIndex, distance, UPPER_RIGHT);
	}
	void setNearestLowerLeftCorner(const unsigned int & directionIndex, const double & distance) {
		setNearestCorner(directionIndex, distance, LOWER_LEFT);
	}
	void setNearestLowerRightCorner(const unsigned int & directionIndex, const double & distance) {
		setNearestCorner(directionIndex, distance, LOWER_RIGHT);
	}
	void cleanDoubles();
};

#endif /* NEARESTDIRECTIONS_HPP_ */
