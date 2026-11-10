/*
 * AirRayleigh.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */

#ifndef AIRRAYLEIGH_HPP_
#define AIRRAYLEIGH_HPP_

#include "AirLaw.hpp"
#include "CoefficientRayleigh.hpp"

class AirRayleigh: public AirLaw {
private:
	vector<CoefficientRayleigh> _CoefficientPerSpectralBand;

public:
	void addCoefficient(const CoefficientRayleigh & coeff) {
		_CoefficientPerSpectralBand.push_back(coeff);
	}
	const vector<CoefficientRayleigh> & getCoefficients() const {
		return _CoefficientPerSpectralBand;
	}
	void clearCoefficients() {
		_CoefficientPerSpectralBand.clear();
	}
	
	const double calculFonctionPhase(	const double & cosinusPsy,
										const unsigned int spectralBandIndex) const {
		return (cosinusPsy * cosinusPsy + 1) * _CoefficientPerSpectralBand[spectralBandIndex].getCoefficientA()
				+ _CoefficientPerSpectralBand[spectralBandIndex].getCoefficientC();
	}
	const unsigned int getType() const {
		return 1;
	}
	shared_ptr<AirLaw> clone() const {
		return shared_ptr<AirLaw>(new AirRayleigh(*this));
	}
	virtual ~AirRayleigh() {}
	;
};

#endif /* AIRRAYLEIGH_HPP_ */
