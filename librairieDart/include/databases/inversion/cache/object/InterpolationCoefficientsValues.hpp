/*
 * InterpolationCoefficientsValues.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef INTERPOLATIONCOEFFICIENTSVALUES_HPP_
#define INTERPOLATIONCOEFFICIENTSVALUES_HPP_

#include <vector>
#include <string>

using namespace std;

class InterpolationCoefficientsValues {
private:
	int _idCombinaison;
	vector<double> _coefficients;

	// Paramètres associés
	vector<string> _nomParametres;
	vector<string> _valeurParametres;
	vector<int> _indexsLut;

public:
	InterpolationCoefficientsValues(const int &idCombinaison,
									const vector<double> &coefficients,
									const vector<string> &nomParametres,
									const vector<string> &valeurParametres,
									vector<int> &indexsLut) {
		_idCombinaison = idCombinaison;

		_coefficients .assign(coefficients.begin(),coefficients.end());
		_indexsLut.assign(indexsLut.begin(),indexsLut.end());
		_nomParametres.assign(nomParametres.begin(),nomParametres.end());
		_valeurParametres.assign(valeurParametres.begin(),valeurParametres.end());
	}

	const int& getIdCombiaison() const {
		return _idCombinaison;
	}

	vector<double>& getCoefficients() {
		return _coefficients;
	}

	void getParameters(	vector<string> &nomParametres,
						vector<string> &valeurParametres,
						vector<int> &indexsLut) {
		nomParametres.assign(_nomParametres.begin(),_nomParametres.end());
		valeurParametres.assign(_valeurParametres.begin(),_valeurParametres.end());
		indexsLut.assign(_indexsLut.begin(),_indexsLut.end());
	}
};

#endif /* INTERPOLATIONCOEFFICIENTSVALUES_HPP_ */
