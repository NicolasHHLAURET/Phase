/*
 * InterpolationCoefficientsKeys.hpp
 *
 *  Created on: 13 nov. 2014
 *      Author: guilleuxj
 */

#ifndef INTERPOLATIONCOEFFICIENTSKEYS_HPP_
#define INTERPOLATIONCOEFFICIENTSKEYS_HPP_

#include <vector>
#include <string>
#include "InterpolationCoefficientsValues.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

class InterpolationCoefficientsKeys {
private:
	int _idSpectralBand;
	double _centralWavelength;
	double _spectralBandwidth;
	int _idSunAngle;
	double _zenithAngle;
	double _azimuthAngle;
	std::vector<shared_ptr<InterpolationCoefficientsValues> > _coefficients;

public:

	InterpolationCoefficientsKeys(	const int &idSpectralBand,
									const double &centralWavelength,
									const double &spectralBandwidth,
									const int &idSunAngle,
									const double &zenithAngle,
									const double &azimuthAngle,
									const double &nbCoefficientsModele) {
		_idSpectralBand = idSpectralBand;
		_idSunAngle = idSunAngle;
		_centralWavelength = centralWavelength;
		_spectralBandwidth = spectralBandwidth;
		_zenithAngle = zenithAngle;
		_azimuthAngle = azimuthAngle;
		_coefficients.reserve(nbCoefficientsModele);
	}

	void addInterpolationCoefficientsValues(const int &idCombinaison,
											const std::vector<double> &coefficients,
											const std::vector<std::string> &nomParametres,
											const std::vector<std::string> &valeurParametres,
											vector<int> &indexsLut);

	bool getInterpolationCoefficientsValues(const int &idCombinaison,
											std::vector<double> &coefficients,
											std::vector<std::string> &nomParametres,
											std::vector<std::string> &valeurParametres,
											std::vector<int> &indexsLut);

	bool isKey(	const int &idSpectralBand,
				const int &idSunAngle) {
		return (_idSpectralBand == idSpectralBand && idSunAngle == _idSunAngle);
	}
};

#endif /* INTERPOLATIONCOEFFICIENTSKEYS_HPP_ */
