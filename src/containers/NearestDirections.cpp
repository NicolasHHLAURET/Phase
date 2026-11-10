/*
 * NearestDirections.cpp
 *
 *  Created on: 13 avr. 2015
 *      Author: lauretn
 */

#include "../../include/containers/NearestDirections.hpp"

void NearestDirections::cleanDoubles() {
	NearestCorners temp = _fourCorners;
	_fourCorners.clear();
	bool modified = false;
	for (unsigned int i = 0; i < temp.size(); ++i) {
		bool hasDouble = false;
		for (unsigned int j = i +1; j <  temp.size(); ++j) {
			if (temp[i].first == temp[j].first) {
				hasDouble = true;
			}
		}
		if (!hasDouble) {
			_fourCorners.push_back(temp[i]);
		} else {
			modified = true;
		}
	}
	if (modified) {
		_nearestCorner = (Corners)0;
		for (unsigned int i = 1; i < _fourCorners.size(); ++i) {
			if (_fourCorners[i].second > _fourCorners[_nearestCorner].second) {
				_nearestCorner = (Corners)i;
			}
		}
	}
}

void NearestDirections::setNearestCorner(const unsigned int & directionIndex, const double & cosDistance, const Corners & corner) {
	if (_fourCorners[corner].second < cosDistance) {
		_fourCorners[corner].first = directionIndex;
		_fourCorners[corner].second = cosDistance;
		// Note: Distance here is defined as the cosinus of the angle between the 2 directions
		// i.e.: 1 is close, 0 is far.
		if (corner != _nearestCorner && cosDistance > _fourCorners[_nearestCorner].second) {
			_nearestCorner = corner;
		}
	}
}
