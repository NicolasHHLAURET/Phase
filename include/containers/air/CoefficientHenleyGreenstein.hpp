/*
 * CoefficientHenleyGreenstein.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */

#ifndef COEFFICIENTHENLEYGREENSTEIN_HPP_
#define COEFFICIENTHENLEYGREENSTEIN_HPP_

class CoefficientHenleyGreenstein {
private:
	double _a;
	double _g1;
	double _g2;

public:
	CoefficientHenleyGreenstein(const double & a = 0,
								const double & g1 = 0,
								const double & g2 = 0) :
			_a(a), _g1(g1), _g2(g2) {}
	
	const double & getCoefficientA() const {
		return _a;
	}
	const double & getCoefficientG1() const {
		return _g1;
	}
	const double & getCoefficientG2() const {
		return _g2;
	}
	
	void setCoefficientA(const double & a) {
		_a = a;
	}
	void setCoefficientG1(const double & g1) {
		_g1 = g1;
	}
	void setCoefficientG2(const double & g2) {
		_g2 = g2;
	}
};

#endif /* COEFFICIENTHENLEYGREENSTEIN_HPP_ */
