/*
 * CoefficientRayleigh.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */

#ifndef COEFFICIENTRAYLEIGH_HPP_
#define COEFFICIENTRAYLEIGH_HPP_

class CoefficientRayleigh {
private:
	double _a;
	double _c;

public:
	CoefficientRayleigh(const double & a = 0,
						const double & c = 0) :
			_a(a), _c(c) {}
	
	const double & getCoefficientA() const {
		return _a;
	}
	const double & getCoefficientC() const {
		return _c;
	}
	
	void setCoefficientA(const double & a) {
		_a = a;
	}
	void setCoefficientC(const double & c) {
		_c = c;
	}
};

#endif /* COEFFICIENTRAYLEIGH_HPP_ */
