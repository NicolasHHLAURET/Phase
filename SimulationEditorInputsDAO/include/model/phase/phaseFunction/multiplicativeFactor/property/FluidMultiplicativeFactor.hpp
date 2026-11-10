/*
 * FluidMultiplicativeFactor.hpp
 *
 *  Created on: 23 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_FLUIDMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_FLUIDMULTIPLICATIVEFACTOR_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IMultiplicativeFactor.hpp"
#include "../../triDimensionalMultiplicativeFactor/TriDimensionalMultiplicativeFactor.hpp"
#include "yaml-cpp/yaml.h"

namespace DartInputs {
	class FluidMultiplicativeFactor: public IMultiplicativeFactor {
	private:
		double albedo;
		double sigma;
		double a;
		double g1OrC;
		double g2;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		FluidMultiplicativeFactor() :
				albedo(1), sigma(1), a(1), g1OrC(1), g2(1) {}

		const double & getAlbedo() const {
			return albedo;
		}
		const double & getSigma() const {
			return sigma;
		}
		const double & getA() const {
			return a;
		}
		const double & getG1OrC() const {
			return g1OrC;
		}
		const double & getG2() const {
			return g2;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				albedo = node.get("albedo").as<double>();
				sigma = node.get("sigma").as<double>();
				a = node.get("a").as<double>();
				g1OrC = node.get("g1OrC").as<double>();
				g2 = node.get("g2").as<double>();
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_FLUIDMULTIPLICATIVEFACTOR_HPP_ */
