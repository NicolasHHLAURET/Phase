/*
 * CacheInterpolationParameters.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef CACHEINTERPOLATIONPARAMETERS_HPP_
#define CACHEINTERPOLATIONPARAMETERS_HPP_

#include <vector>
#include "object/InterpolationParametersKeys.hpp"

class CacheInterpolationParameters {
private:
	std::vector<shared_ptr<InterpolationParametersKeys> > _cache;

public:
	CacheInterpolationParameters() {}

	CacheInterpolationParameters(int maxCacheLength) {
		_cache.reserve(maxCacheLength);
	}

	void reserve(const int &maxCacheLength) {
		_cache.reserve(maxCacheLength);
	}

	void addInterpolationParameter(	const int &idSpectralBand,
									const int &idSunAngle,
									const int &idParameter,
									const int &nbParameters,
									const double &interpolationValueMin,
									const double &interpolationValueMax,
									const std::vector<int> &combinaisons);

	bool getInterpolationParameterCombinaison(	const int &idSpectralBand,
												const int &idSunAngle,
												const int &idParameter,
												const double &interpolationValue,
												std::vector<int> &combinaisons) const;
};

#endif /* CACHEINTERPOLATIONPARAMETERS_HPP_ */
