/*
 * VegetationMultiplicativeFactor.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_VEGETATIONMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_VEGETATIONMULTIPLICATIVEFACTOR_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IMultiplicativeFactor.hpp"
#include "yaml-cpp/yaml.h"
#include "../../triDimensionalMultiplicativeFactor/TriDimensionalMultiplicativeFactor.hpp"

namespace DartInputs {
	class VegetationMultiplicativeFactor: public IMultiplicativeFactor {
	private:
		double abaxialReflectance;
		double adaxialReflectance;
		double leafTransmitance;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		VegetationMultiplicativeFactor() :
				abaxialReflectance(1), adaxialReflectance(1), leafTransmitance(1) {}

		const double & getAbaxialReflectance() const {
			return abaxialReflectance;
		}
		const double & getAdaxialReflectance() const {
			return adaxialReflectance;
		}
		const double & getLeafTransmitance() const {
			return leafTransmitance;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				abaxialReflectance = node.get("abaxialReflectance").as<double>();
				adaxialReflectance = node.get("adaxialReflectance").as<double>();
				leafTransmitance = node.get("leafTransmitance").as<double>();
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_VEGETATIONMULTIPLICATIVEFACTOR_HPP_ */
