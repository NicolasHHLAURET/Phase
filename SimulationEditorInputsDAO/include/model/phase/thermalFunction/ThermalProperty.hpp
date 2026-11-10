/*
 * ThermalProperty.hpp
 *
 *  Created on: 2 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_THERMALFUNCTION_THERMALPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_THERMALFUNCTION_THERMALPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IThermalProperty.hpp"
#include "precomputedIPARs/PrecomputedIPARs.hpp"
#include "../phaseFunction/triDimensionalMultiplicativeFactor/TriDimensionalMultiplicativeFactor.hpp"

namespace DartInputs {
	class ThermalProperty: public IThermalProperty {
	private:
		double meanTemperature;
		double deltaTemperature;
		bool override3DMatrixOfTemperature;
		bool singleTemperatureSurface;
		shared_ptr<PrecomputedIPARs> precomputedIPARs;
		shared_ptr<TriDimensionalMultiplicativeFactor> triDimensionalMultiplicativeFactor;

	public:
		ThermalProperty() :
				meanTemperature(300), deltaTemperature(20), override3DMatrixOfTemperature(false), singleTemperatureSurface(true) {}

		const double & getMeanTemperature() const {
			return meanTemperature;
		}
		const double & getDeltaTemperature() const {
			return deltaTemperature;
		}
		const bool & getOverride3DMatrixOfTemperature() const {
			return override3DMatrixOfTemperature;
		}
		const bool & getSingleTemperatureSurface() const {
			return singleTemperatureSurface;
		}
		const shared_ptr<PrecomputedIPARs> & getPrecomputedIPARs() const {
			return precomputedIPARs;
		}
		const shared_ptr<TriDimensionalMultiplicativeFactor> & getTriDimensionalMultiplicativeFactor() const {
			return triDimensionalMultiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			IThermalProperty::operator =(node);
			if (!node.IsNull()) {
				meanTemperature = node.get("meanTemperature").as<double>();
				deltaTemperature = node.get("deltaTemperature").as<double>();
				override3DMatrixOfTemperature = node.get("override3DMatrixOfTemperature").as<bool>();
				singleTemperatureSurface = node.get("singleTemperatureSurface").as<bool>();

				if (!node.get("precomputedIPARs").IsNull()) {
					precomputedIPARs = shared_ptr<PrecomputedIPARs>(new PrecomputedIPARs());
					*precomputedIPARs = node.get("precomputedIPARs");
				}
				if (!node.get("triDimensionalMultiplicativeFactor").IsNull()) {
					triDimensionalMultiplicativeFactor = shared_ptr<TriDimensionalMultiplicativeFactor>(new TriDimensionalMultiplicativeFactor());
					(*triDimensionalMultiplicativeFactor) = node.get("triDimensionalMultiplicativeFactor");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_THERMALFUNCTION_THERMALPROPERTY_HPP_ */
