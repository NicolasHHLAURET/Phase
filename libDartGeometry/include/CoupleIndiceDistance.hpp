/*
 * CoupleIndiceDistance.hpp
 *
 *  Created on: 9 mai 2011
 *      Author: lauretn
 */

#ifndef COUPLEINDICEDISTANCE_HPP_
#define COUPLEINDICEDISTANCE_HPP_

class CoupleIndiceDistance {
private:
	double _Distance;
	int _Indice;

public:
	CoupleIndiceDistance(	const double & distance = 0,
							const int & indice = 0) :
			_Distance(distance), _Indice(indice) {}
	
	const double & getDistance() const {
		return _Distance;
	}
	const int & getIndice() const {
		return _Indice;
	}
	
	CoupleIndiceDistance & setDistance(const double & distance) {
		_Distance = distance;
		return *this;
	}
	CoupleIndiceDistance & setIndice(const int & indice) {
		_Indice = indice;
		return *this;
	}
	
	const bool operator<(const CoupleIndiceDistance & couple2) const {
		return _Distance < couple2.getDistance();
	}
};

#endif /* COUPLEINDICEDISTANCE_HPP_ */
