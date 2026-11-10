/*
 * CacheInterpolationParameters.cpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */
#include "../include/databases/inversion/cache/CacheInterpolationParameters.hpp"

using namespace std;

bool CacheInterpolationParameters::getInterpolationParameterCombinaison(	const int &idSpectralBand,
											const int &idSunAngle,
											const int &idParameter,
											const double &interpolationValue,
											vector<int> &combinaisons) const {
	vector<shared_ptr<InterpolationParametersKeys> >::const_iterator itrCache, itrEndCache = _cache.end();
	for (itrCache = _cache.begin(); itrCache != itrEndCache; ++itrCache) {
		if ((*itrCache)->isParameter(idSpectralBand, idSunAngle, idParameter)) {
			return (*itrCache)->getCombinaisonsForInterpolationValue(interpolationValue, combinaisons);
		}
	}
	return false;
}

void CacheInterpolationParameters::addInterpolationParameter(	const int &idSpectralBand,
								const int &idSunAngle,
								const int &idParameter,
								const int &nbParameters,
								const double &interpolationValueMin,
								const double &interpolationValueMax,
								const vector<int> &combinaisons) {
	vector<shared_ptr<InterpolationParametersKeys> >::const_iterator itrCache, itrEndCache = _cache.end();
	for (itrCache = _cache.begin(); itrCache != itrEndCache; ++itrCache) {
		if ((*itrCache)->isParameter(idSpectralBand, idSunAngle, idParameter)) {
			(*itrCache)->addInterpolationInterval(interpolationValueMin, interpolationValueMax, combinaisons);
			return;
		}
	}
	shared_ptr<InterpolationParametersKeys> element(
		new InterpolationParametersKeys(InterpolationParametersKeys(idSpectralBand, idSunAngle, idParameter, nbParameters)));
	element->addInterpolationInterval(interpolationValueMin, interpolationValueMax, combinaisons);
	_cache.push_back(element);
}

