/*
 * IMultiplicativeFactorQuantifier.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTORQUANTIFIER_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTORQUANTIFIER_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IMultiplicativeFactor.hpp"
#include "HapkeMultiplicativeFactor.hpp"
#include "LambertianMultiplicativeFactor.hpp"
#include "RpvMultiplicativeFactor.hpp"
#include "VegetationMultiplicativeFactor.hpp"
#include "FluidMultiplicativeFactor.hpp"

namespace DartInputs {
	class IMultiplicativeFactorQuantifier {
	private:
		shared_ptr<IMultiplicativeFactor> multiplicativeFactor;
	public:
		IMultiplicativeFactorQuantifier() {}

		virtual ~IMultiplicativeFactorQuantifier(){}

		const shared_ptr<IMultiplicativeFactor> & getMultiplicativeFactor() const {
			return multiplicativeFactor;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.get("multiplicativeFactor").IsNull()) {
				if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.HapkeMultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new HapkeMultiplicativeFactor());
					static_cast<HapkeMultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				}else if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.Hapke2MultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new Hapke2MultiplicativeFactor());
					static_cast<Hapke2MultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				} else if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.LambertianMultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new LambertianMultiplicativeFactor());
					static_cast<LambertianMultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				} else if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.RpvMultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new RpvMultiplicativeFactor());
					static_cast<RpvMultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				} else if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.VegetationMultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new VegetationMultiplicativeFactor());
					static_cast<VegetationMultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				} else if (node.get("multiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.property.FluidMultiplicativeFactor") {
					multiplicativeFactor = shared_ptr<IMultiplicativeFactor>(new FluidMultiplicativeFactor());
					static_cast<FluidMultiplicativeFactor &>(*multiplicativeFactor) = node.get("multiplicativeFactor");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_IMULTIPLICATIVEFACTORQUANTIFIER_HPP_ */
