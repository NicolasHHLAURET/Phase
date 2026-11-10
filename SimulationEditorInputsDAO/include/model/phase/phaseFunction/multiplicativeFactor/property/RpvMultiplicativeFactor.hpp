/*
 * RpvMultiplicativeFactor.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_RPVMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_RPVMULTIPLICATIVEFACTOR_HPP_

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
	class RpvMultiplicativeFactor: public IMultiplicativeFactor {
	private:
		double directTransmitance;
		double diffuseTransmitance;
		double k;
		double h;
		double rho0;
		double g;
		double specularIntensity;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		RpvMultiplicativeFactor() :
				directTransmitance(1), diffuseTransmitance(1), k(1), h(1), rho0(1), g(1), specularIntensity(1) {}

		const double & getDirectTransmitance() const {
			return directTransmitance;
		}
		const double & getDiffuseTransmitance() const {
			return diffuseTransmitance;
		}
		const double & getK() const {
			return k;
		}
		const double & getH() const {
			return h;
		}
		const double & getRho0() const {
			return rho0;
		}
		const double & getG() const {
			return g;
		}
		const double & getSpecularIntensity() const {
			return specularIntensity;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				directTransmitance = node.get("directTransmitance").as<double>();
				diffuseTransmitance = node.get("diffuseTransmitance").as<double>();
				k = node.get("k").as<double>();
				h = node.get("h").as<double>();
				rho0 = node.get("rho0").as<double>();
				g = node.get("g").as<double>();
				specularIntensity = node.get("specularIntensity").as<double>();
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_RPVMULTIPLICATIVEFACTOR_HPP_ */
