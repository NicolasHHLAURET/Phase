/*
 * AtmosphereGeometry.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_ATMOSPHEREGEOMETRY_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_ATMOSPHEREGEOMETRY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IDiscretizationMode.hpp"
#include "AutomaticDiscretization.hpp"
#include "FastDiscretization.hpp"
#include "ManualDiscretization.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AtmosphereGeometry {
	private:
		shared_ptr<IDiscretizationMode> discretizationMode;
		double sensorAltitude;
		int integralStepForComputionAerosolPhaseFunction;

	public:
		AtmosphereGeometry() :
				sensorAltitude(3000), integralStepForComputionAerosolPhaseFunction(4) {}

		const shared_ptr<IDiscretizationMode> & getDiscretizationMode() const {
			return discretizationMode;
		}

		const double & getSensorAltitude() const {
			return sensorAltitude;
		}

		const int & getIntegralStepForComputionAerosolPhaseFunction() const {
			return integralStepForComputionAerosolPhaseFunction;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				sensorAltitude = node.get("sensorAltitude").as<double>();
				integralStepForComputionAerosolPhaseFunction = node.get("integralStepForComputionAerosolPhaseFunction").as<int>();

				if (!node.get("discretizationMode").IsNull()) {
					if (node.get("discretizationMode").Tag() == "!atmosphere.atmosphereGeometry.AutomaticDiscretization") {
						discretizationMode = shared_ptr<IDiscretizationMode>(new AutomaticDiscretization());
						static_cast<AutomaticDiscretization &>(*discretizationMode) = node.get("discretizationMode");
					} else if (node.get("discretizationMode").Tag() == "!atmosphere.atmosphereGeometry.FastDiscretization") {
						discretizationMode = shared_ptr<IDiscretizationMode>(new FastDiscretization());
						static_cast<FastDiscretization &>(*discretizationMode) = node.get("discretizationMode");
					} else if (node.get("discretizationMode").Tag() == "!atmosphere.atmosphereGeometry.ManualDiscretization") {
						discretizationMode = shared_ptr<IDiscretizationMode>(new ManualDiscretization());
						static_cast<ManualDiscretization &>(*discretizationMode) = node.get("discretizationMode");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_ATMOSPHEREGEOMETRY_HPP_ */
