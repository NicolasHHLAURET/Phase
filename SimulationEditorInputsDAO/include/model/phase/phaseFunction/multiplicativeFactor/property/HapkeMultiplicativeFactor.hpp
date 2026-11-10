/*
 * HapkeMultiplicativeFactor.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_HAPKEMULTIPLICATIVEFACTOR_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_HAPKEMULTIPLICATIVEFACTOR_HPP_

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
	class HapkeMultiplicativeFactor: public IMultiplicativeFactor {
	private:
		double singleScatteringAlbedo;
		double w;
		double c1;
		double c2;
		double c3;
		double c4;
		double h1;
		double h2;
		double specularIntensity;
		double directTransmittance;
		double diffuseTransmittance;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		HapkeMultiplicativeFactor() :
				singleScatteringAlbedo(1), w(1), c1(1), c2(1), c3(1), c4(1), h1(1), h2(1), specularIntensity(1), directTransmittance(1), diffuseTransmittance(
					1) {}

		const double & getSingleScatteringAlbedo() const {
			return singleScatteringAlbedo;
		}
		const double & getW() const {
			return w;
		}
		const double & getC1() const {
			return c1;
		}
		const double & getC2() const {
			return c2;
		}
		const double & getC3() const {
			return c3;
		}
		const double & getC4() const {
			return c4;
		}
		const double & getH1() const {
			return h1;
		}
		const double & getH2() const {
			return h2;
		}
		const double & getSpecularIntensity() const {
			return specularIntensity;
		}
		const double & getDirectTransmittance() const {
			return directTransmittance;
		}
		const double & getDiffuseTransmittance() const {
			return diffuseTransmittance;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				singleScatteringAlbedo = node.get("singleScatteringAlbedo").as<double>();
				w = node.get("w").as<double>();
				c1 = node.get("c1").as<double>();
				c2 = node.get("c2").as<double>();
				c3 = node.get("c3").as<double>();
				c4 = node.get("c4").as<double>();
				h1 = node.get("h1").as<double>();
				h2 = node.get("h2").as<double>();
				specularIntensity = node.get("specularIntensity").as<double>();
				directTransmittance = node.get("directTransmittance").as<double>();
				diffuseTransmittance = node.get("diffuseTransmittance").as<double>();
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};


	class Hapke2MultiplicativeFactor: public IMultiplicativeFactor {
		private:
			double singleScatteringAlbedo;
			double w;
			double c;
			double b;
			double h;
			double b0;
			double theta;
			double specularIntensity;
			double directTransmittance;
			double diffuseTransmittance;
			shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

		public:
			Hapke2MultiplicativeFactor() :
					singleScatteringAlbedo(1), w(1), c(1), b(1), h(1), b0(1), theta(1), specularIntensity(1), directTransmittance(1), diffuseTransmittance(
						1) {}

			const double & getSingleScatteringAlbedo() const {
				return singleScatteringAlbedo;
			}
			const double & getW() const {
				return w;
			}
			const double & getC() const {
				return c;
			}
			const double & getB() const {
				return b;
			}
			const double & getH() const {
				return h;
			}
			const double & getB0() const {
				return b0;
			}
			const double & getTh() const {
				return theta;
			}

			const double & getSpecularIntensity() const {
				return specularIntensity;
			}
			const double & getDirectTransmittance() const {
				return directTransmittance;
			}
			const double & getDiffuseTransmittance() const {
				return diffuseTransmittance;
			}
			const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
				return triDimensionalMultiplicativeFactor;
			}

			void operator =(const YAML::Node& node) {
				if (!node.IsNull()) {
					singleScatteringAlbedo = node.get("singleScatteringAlbedo").as<double>();
					w = node.get("w").as<double>();
					c = node.get("c").as<double>();
					b = node.get("b").as<double>();
					h = node.get("h").as<double>();
					b0 = node.get("b0").as<double>();
					theta = node.get("theta").as<double>();
					specularIntensity = node.get("specularIntensity").as<double>();
					directTransmittance = node.get("directTransmittance").as<double>();
					diffuseTransmittance = node.get("diffuseTransmittance").as<double>();
					if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
						triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
						(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
					}
				}
			}
		};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_PROPERTY_HAPKEMULTIPLICATIVEFACTOR_HPP_ */
