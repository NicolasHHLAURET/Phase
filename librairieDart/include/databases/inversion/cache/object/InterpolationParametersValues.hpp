/*
 * InterpolationValues.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef INTERPOLATIONVALUES_HPP_
#define INTERPOLATIONVALUES_HPP_

#include <vector>

class InterpolationParametersValues {
private:
	double _min;
	double _max;
	std::vector<int> _combinaisons;
public:
	InterpolationParametersValues(double &min, double &max,const std::vector<int> &combinaisons) {
		_min = min;
		_max = max;
		_combinaisons.assign(combinaisons.begin(),combinaisons.end());
	}

	bool estDansIntervale(const double &interpolationValue) const {
		return (interpolationValue >= _min && interpolationValue <= _max);
	}

	std::vector<int>& getCombinaisons() {
		return _combinaisons;
	}
};

#endif /* INTERPOLATIONVALUES_HPP_ */
