/*
 * MultiplicativeFactorForBand.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORFORBAND_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORFORBAND_HPP_

#include "yaml-cpp/yaml.h"
#include "IMultiplicativeFactorQuantifier.hpp"
#include "IMultiplicativeFactor.hpp"
namespace DartInputs {
	class MultiplicativeFactorForBand: public IMultiplicativeFactorQuantifier {
	private:
		int associatedSpectralBand;
	public:
		MultiplicativeFactorForBand() :
				associatedSpectralBand(1) {}

		const int & getAssociatedSpectralBand() const {
			return associatedSpectralBand;
		}

		void operator =(const YAML::Node& node) {
			IMultiplicativeFactorQuantifier::operator =(node);
			if (!node.IsNull()) {
				associatedSpectralBand = node.get("associatedSpectralBand").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_MULTIPLICATIVEFACTORFORBAND_HPP_ */
