/*
 * FluidProperty.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_FLUIDPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_FLUIDPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../multiplicativeFactor/MultiplicativeFactor.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorForAllBands.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorPerBand.hpp"

#include "yaml-cpp/yaml.h"
#include "IOpticalProperty.hpp"

namespace DartInputs {
	class FluidProperty: public IOpticalProperty {
	private:
		shared_ptr<MultiplicativeFactor> fluidMultiplicativeFactor;

	public:
		FluidProperty() {}

		const shared_ptr<MultiplicativeFactor> & getFluidMultiplicativeFactor() const {
			return fluidMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			IOpticalProperty::operator=(node);
			if (!node.IsNull()) {
				if (!node.get("fluidMultiplicativeFactor").IsNull()) {
					if (node.get("fluidMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						fluidMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*fluidMultiplicativeFactor) = node.get("fluidMultiplicativeFactor");
					} else if (node.get("fluidMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						fluidMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*fluidMultiplicativeFactor) = node.get("fluidMultiplicativeFactor");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_FLUIDPROPERTY_HPP_ */
