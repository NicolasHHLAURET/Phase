/*
 * LambertianProperty.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_LAMBERTIANPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_LAMBERTIANPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPhaseProperty.hpp"
#include "../multiplicativeFactor/MultiplicativeFactor.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorForAllBands.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorPerBand.hpp"
#include "prospect/ProspectFunction.hpp"
namespace DartInputs {
	class LambertianProperty: public IPhaseProperty {
	private:
		double reflectanceVariability;
		shared_ptr<MultiplicativeFactor> lambertianMultiplicativeFactor;
		shared_ptr<ProspectFunction> prospectFunction;
	public:
		LambertianProperty() :
				reflectanceVariability(0.000) {}

		const double & getReflectanceVariability() const {
			return reflectanceVariability;
		}
		const shared_ptr<MultiplicativeFactor> & getLambertianMultiplicativeFactor() const {
			return lambertianMultiplicativeFactor;
		}
		const shared_ptr<ProspectFunction> & getProspectFunction() const {
			return prospectFunction;
		}

		void operator =(const YAML::Node& node) {
			IPhaseProperty::operator=(node);
			if (!node.IsNull()) {
				reflectanceVariability = node.get("reflectanceVariability").as<double>();

				if (!node.get("lambertianMultiplicativeFactor").IsNull()) {
					if (node.get("lambertianMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						lambertianMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*lambertianMultiplicativeFactor) = node.get("lambertianMultiplicativeFactor");
					} else if (node.get("lambertianMultiplicativeFactor").Tag()
							== "phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						lambertianMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*lambertianMultiplicativeFactor) = node.get("lambertianMultiplicativeFactor");
					}
				}

				if (!node.get("prospectFunction").IsNull()) {
					prospectFunction = shared_ptr<ProspectFunction>(new ProspectFunction());
					(*prospectFunction) = node.get("prospectFunction");
				}

			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_LAMBERTIANPROPERTY_HPP_ */
