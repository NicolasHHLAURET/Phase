/*
 * InterpolationParametersValues.cpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */
#include "../include/databases/inversion/cache/object/InterpolationParametersKeys.hpp"

using namespace std;

void InterpolationParametersKeys::addInterpolationInterval(	double min,
															double max,
															const vector<int> &combinaisons) {
	shared_ptr<InterpolationParametersValues> element(
		new InterpolationParametersValues(InterpolationParametersValues(min, max, combinaisons)));
	_interpolationValues.push_back(element);
}

bool InterpolationParametersKeys::getCombinaisonsForInterpolationValue(	const double &interpolationValue,
																		vector<int> &combinaisons) const {
	vector<shared_ptr<InterpolationParametersValues> >::const_iterator itrParameterValues, itrEndParameterValues = _interpolationValues.end();
	for (itrParameterValues = _interpolationValues.begin(); itrParameterValues != itrEndParameterValues; ++itrParameterValues) {
		if ((*itrParameterValues)->estDansIntervale(interpolationValue)) {
			combinaisons = (*itrParameterValues)->getCombinaisons();
			return true;
		}
	}
	return false;
}
