/*
 * InterpolationCoefficientsKeys.cpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */
#include "../include/databases/inversion/cache/object/InterpolationCoefficientsKeys.hpp"

using namespace std;

void InterpolationCoefficientsKeys::addInterpolationCoefficientsValues(	const int &idCombinaison,
																		const vector<double> &coefficients,
																		const vector<string> &nomParametres,
																		const vector<string> &valeurParametres,
																		vector<int> &indexsLut) {
	vector<shared_ptr<InterpolationCoefficientsValues> >::const_iterator itrCoefficients, itrEndCoefficients = _coefficients.end();
	for (itrCoefficients = _coefficients.begin(); itrCoefficients != itrEndCoefficients; ++itrCoefficients) {
		if ((*itrCoefficients)->getIdCombiaison() == idCombinaison) {
			return;
		}
	}
	shared_ptr<InterpolationCoefficientsValues> element(
		new InterpolationCoefficientsValues(
			InterpolationCoefficientsValues(idCombinaison, coefficients, nomParametres, valeurParametres, indexsLut)));
	_coefficients.push_back(element);
}

bool InterpolationCoefficientsKeys::getInterpolationCoefficientsValues(	const int &idCombinaison,
																		vector<double> &coefficients,
																		vector<string> &nomParametres,
																		vector<string> &valeurParametres,
																		vector<int> &indexsLut) {
	vector<shared_ptr<InterpolationCoefficientsValues> >::const_iterator itrCoefficients, itrEndCoefficients = _coefficients.end();
	for (itrCoefficients = _coefficients.begin(); itrCoefficients != itrEndCoefficients; ++itrCoefficients) {
		if ((*itrCoefficients)->getIdCombiaison() == idCombinaison) {
			coefficients = (*itrCoefficients)->getCoefficients();
			(*itrCoefficients)->getParameters(nomParametres, valeurParametres, indexsLut);
			return true;
		}
	}
	return false;
}
