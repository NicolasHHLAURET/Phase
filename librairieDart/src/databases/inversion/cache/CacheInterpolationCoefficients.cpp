/*
 * CacheInterpolationCoefficients.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */
#include "../include/databases/inversion/cache/CacheInterpolationCoefficients.hpp"

using namespace std;

bool CacheInterpolationCoefficients::getInterpolationCoefficients(	const int &idSpectralBand,
																	const int &idSunAngle,
																	const int &idCombinaison,
																	vector<double> &coefficients,
																	vector<string> &nomParametres,
																	vector<string> &valeurParametres,
																	vector<int> &indexsLut) {
	vector<shared_ptr<InterpolationCoefficientsKeys> >::const_iterator itrCache, itrEndCache = _cache.end();
	for (itrCache = _cache.begin(); itrCache != itrEndCache; ++itrCache) {
		if ((*itrCache)->isKey(idSpectralBand, idSunAngle)) {
			return (*itrCache)->getInterpolationCoefficientsValues(idCombinaison, coefficients, nomParametres, valeurParametres, indexsLut);
		}
	}
	return false;
}

void CacheInterpolationCoefficients::addInterpolationCoefficients(	const int &idSpectralBand,
									const double &centralWavelength,
									const double &spectralBandwidth,
									const int &idSunAngle,
									const double &zenithAngle,
									const double &azimuthAngle,
									const int &idCombinaison,
									const int &nbCoefficientsModele,
									vector<double> &coefficients,
									vector<string> &nomParametres,
									vector<string> &valeurParametres,
									vector<int> &indexsLut) {
	vector<shared_ptr<InterpolationCoefficientsKeys> >::const_iterator itrCache, itrEndCache = _cache.end();
	for (itrCache = _cache.begin(); itrCache != itrEndCache; ++itrCache) {
		if ((*itrCache)->isKey(idSpectralBand, idSunAngle)) {
			(*itrCache)->addInterpolationCoefficientsValues(idCombinaison, coefficients, nomParametres, valeurParametres, indexsLut);
			return;
		}
	}
	shared_ptr<InterpolationCoefficientsKeys> element(
		new InterpolationCoefficientsKeys(
			InterpolationCoefficientsKeys(idSpectralBand, centralWavelength, spectralBandwidth, idSunAngle, zenithAngle, azimuthAngle,nbCoefficientsModele)));
	element->addInterpolationCoefficientsValues(idCombinaison, coefficients, nomParametres, valeurParametres, indexsLut);
	_cache.push_back(element);
}
