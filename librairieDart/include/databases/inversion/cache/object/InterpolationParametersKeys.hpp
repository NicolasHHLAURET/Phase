/*
 * InterpolationKeys.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef INTERPOLATIONKEYS_HPP_
#define INTERPOLATIONKEYS_HPP_

#include "InterpolationParametersValues.hpp"
#include <vector>
#include <iostream>

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

class InterpolationParametersKeys {
private:
	int _idSpectralBand;
	int _idSunAngle;
	int _idParameter;
	std::vector<shared_ptr<InterpolationParametersValues> > _interpolationValues;

public:
	InterpolationParametersKeys(int idSpectralBand,
	                            int idSunAngle,
	                            int idParameter,
	                            int nbParameters) {
		_idSpectralBand = idSpectralBand;
		_idSunAngle = idSunAngle;
		_idParameter = idParameter;
		_interpolationValues.reserve(nbParameters);
	}

	void addInterpolationInterval(double min, double max, const std::vector<int> &combinaisons);

	bool getCombinaisonsForInterpolationValue(const double &interpolationValue,std::vector<int> &combinaisons) const;

	bool isParameter(const int &idSpectralBand,
	                 const int &idSunAngle,
	                 const int &idParameter) {
		return (idSpectralBand == _idSpectralBand
				&& idSunAngle == _idSunAngle
				&& idParameter == _idParameter);
	}
};

#endif /* INTERPOLATIONKEYS_HPP_ */
