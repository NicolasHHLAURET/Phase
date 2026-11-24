/*
 * AirHenleyGreenstein.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */

#ifndef AIRHENLEYGREENSTEIN_HPP_
#define AIRHENLEYGREENSTEIN_HPP_

#include "AirLaw.hpp"
#include "CoefficientHenleyGreenstein.hpp"

class AirHenleyGreenstein: public AirLaw {
private:
	vector<CoefficientHenleyGreenstein> _CoefficientPerSpectralBand;

public:
	void addCoefficient(const CoefficientHenleyGreenstein & coeff) {
		_CoefficientPerSpectralBand.push_back(coeff);
	}
	const vector<CoefficientHenleyGreenstein> & getCoefficients() const {
		return _CoefficientPerSpectralBand;
	}
	void clearCoefficients() {
		_CoefficientPerSpectralBand.clear();
	}
	
	const double calculFonctionPhase(	const double & cosinusPsy,
										const unsigned int indSpectralBand) const {
		double a = _CoefficientPerSpectralBand[indSpectralBand].getCoefficientA();
		double g1 = _CoefficientPerSpectralBand[indSpectralBand].getCoefficientG1();
		double g2 = -_CoefficientPerSpectralBand[indSpectralBand].getCoefficientG2();

		double numerateurPremierTerme = 0;
		double denominateurPremierTerme = 1;
		if (g1 != 1) {
			double g1Square = g1 * g1;
			numerateurPremierTerme = a * (1 - g1Square);
			denominateurPremierTerme = pow(1 + g1Square - 2 * g1 * cosinusPsy, 1.5);
		} else if (cosinusPsy == 1.) {
				numerateurPremierTerme = 1.;
		}

		double numerateurDeuxiemeTerme = 0;
		double denominateurDeuxiemeTerme = 1;
		if (g2 != 1) {
			double g2Square = g2 * g2;
			numerateurDeuxiemeTerme = (1 - a) * (1 - g2Square);
			denominateurDeuxiemeTerme = pow(1 + g2Square - 2 * g2 * cosinusPsy, 1.5);
		} else if (cosinusPsy == -1.) {
			numerateurPremierTerme = 1.;
		}
		return (numerateurPremierTerme / denominateurPremierTerme + numerateurDeuxiemeTerme / denominateurDeuxiemeTerme);
	}
	const unsigned int getType() const {
		return 0;
	}
	shared_ptr<AirLaw> clone() const {
		return shared_ptr < AirLaw > (new AirHenleyGreenstein(*this));
	}
	virtual ~AirHenleyGreenstein(){};
};

#endif /* AIRHENLEYGREENSTEIN_HPP_ */
