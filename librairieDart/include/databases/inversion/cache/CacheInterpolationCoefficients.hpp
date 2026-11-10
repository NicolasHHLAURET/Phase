/*
 * CacheInterpolationCoefficients.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef CACHEINTERPOLATIONCOEFFICIENTS_HPP_
#define CACHEINTERPOLATIONCOEFFICIENTS_HPP_

#include <vector>
#include <string>
#include "object/InterpolationCoefficientsKeys.hpp"

class CacheInterpolationCoefficients {
private:
	std::vector<shared_ptr<InterpolationCoefficientsKeys> > _cache;

public:
	CacheInterpolationCoefficients() {}

	void reserve(const int maxLength) {
		_cache.reserve(maxLength);
	}

	bool getInterpolationCoefficients(	const int &idSpectralBand,
										const int &idSunAngle,
										const int &idCombinaison,
										std::vector<double> &coefficients,
										std::vector<std::string> &nomParametres,
										std::vector<std::string> &valeurParametres,
										std::vector<int> &indexsLut);

	void addInterpolationCoefficients(	const int &idSpectralBand,
										const double &centralWavelength,
										const double &spectralBandwidth,
										const int &idSunAngle,
										const double &zenithAngle,
										const double &azimuthAngle,
										const int &idCombinaison,
										const int &nbCoefficientsModele,
										std::vector<double> &coefficients,
										std::vector<std::string> &nomParametres,
										std::vector<std::string> &valeurParametres,
										std::vector<int> &indexsLut);
};

#endif /* CACHEINTERPOLATIONCOEFFICIENTS_HPP_ */
