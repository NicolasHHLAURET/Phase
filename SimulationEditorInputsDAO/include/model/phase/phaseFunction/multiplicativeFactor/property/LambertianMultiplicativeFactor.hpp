/*
 * LambertianMultiplicativeFactor.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_

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
	class LambertianMultiplicativeFactor: public IMultiplicativeFactor {
	private:
		double reflectanceFactor;
		double diffuseTransmitance;
		double directTransmitance;
		double specularIntensity;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		LambertianMultiplicativeFactor() :
				reflectanceFactor(1), diffuseTransmitance(1), directTransmitance(1), specularIntensity(1) {}

		const double & getReflectanceFactor() const {
			return reflectanceFactor;
		}
		const double & getDiffuseTransmitance() const {
			return diffuseTransmitance;
		}
		const double & getDirectTransmitance() const {
			return directTransmitance;
		}
		const double & getSpecularIntensity() const {
			return specularIntensity;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				reflectanceFactor = node.get("reflectanceFactor").as<double>();
				diffuseTransmitance = node.get("diffuseTransmitance").as<double>();
				directTransmitance = node.get("directTransmitance").as<double>();
				specularIntensity = node.get("specularIntensity").as<double>();
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_LAMBERTIANMULTIPLICATIVEFACTOR_HPP_ */
