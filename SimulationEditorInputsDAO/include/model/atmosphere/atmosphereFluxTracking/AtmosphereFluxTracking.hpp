/*
 * AtmosphereFluxTracking.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ATMOSPHEREFLUXTRACKING_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ATMOSPHEREFLUXTRACKING_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ICalculationMode.hpp"
#include "CalculationOnTheFly.hpp"
#include "CalculationPreComputed.hpp"
namespace DartInputs {
	class AtmosphereFluxTracking {
	public:
		// TODO: bad implementation
		enum DownwardFluxes {
			_TOTAL, _DIRECT, _DIFFUSE
		};

		enum UpwardFluxes {
			TOTAL, DIRECT, DIFFUSE, NONE, TOTAL_SEPARATED
		};
	private:
		shared_ptr<ICalculationMode> calculationMode;

		// Component
		DownwardFluxes downwardFluxes;
		UpwardFluxes upwardFluxes;

		// Expert
		bool extrapolationOfAtmosphericRT;
		double irradianceScatteringTreshold;
		double transferFunctionScatteringTreshold;

	public:
		AtmosphereFluxTracking() :
				downwardFluxes(_TOTAL), upwardFluxes(TOTAL), extrapolationOfAtmosphericRT(true), irradianceScatteringTreshold(0.00001), transferFunctionScatteringTreshold(
					0.0001) {}

		const shared_ptr<ICalculationMode> & getCalculationMode() const {
			return calculationMode;
		}
		const DownwardFluxes & getDownwardFluxes() const {
			return downwardFluxes;
		}
		const UpwardFluxes & getUpwardFluxes() const {
			return upwardFluxes;
		}
		const bool & getExtrapolationOfAtmosphericRT() const {
			return extrapolationOfAtmosphericRT;
		}
		const double & getIrradianceScatteringTreshold() const {
			return irradianceScatteringTreshold;
		}
		const double & getTransferFunctionScatteringTreshold() const {
			return transferFunctionScatteringTreshold;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("calculationMode").IsNull()) {
					if (node.get("calculationMode").Tag() == "!atmosphere.atmosphereFluxTracking.CalculationOnTheFly") {
						calculationMode = shared_ptr<ICalculationMode>(new CalculationOnTheFly());
						static_cast<CalculationOnTheFly &>(*calculationMode) = node.get("calculationMode");
					} else if (node.get("calculationMode").Tag() == "!atmosphere.atmosphereFluxTracking.CalculationPreComputed") {
						calculationMode = shared_ptr<ICalculationMode>(new CalculationPreComputed());
						static_cast<CalculationPreComputed &>(*calculationMode) = node.get("calculationMode");
					}
				}

				extrapolationOfAtmosphericRT = node.get("extrapolationOfAtmosphericRT").as<bool>();
				irradianceScatteringTreshold = node.get("irradianceScatteringTreshold").as<double>();
				transferFunctionScatteringTreshold = node.get("transferFunctionScatteringTreshold").as<double>();

				if (node.get("downwardFluxes").as<std::string>() == "TOTAL") {
					downwardFluxes = _TOTAL;
				} else if (node.get("downwardFluxes").as<std::string>() == "DIRECT") {
					downwardFluxes = _DIRECT;
				} else {
					downwardFluxes = _DIFFUSE;
				}

				if (node.get("upwardFluxes").as<std::string>() == "TOTAL") {
					upwardFluxes = TOTAL;
				} else if (node.get("upwardFluxes").as<std::string>() == "DIRECT") {
					upwardFluxes = DIRECT;
				} else if (node.get("upwardFluxes").as<std::string>() == "DIFFUSE") {
					upwardFluxes = DIFFUSE;
				} else if (node.get("upwardFluxes").as<std::string>() == "NONE") {
					upwardFluxes = NONE;
				} else {
					upwardFluxes = TOTAL_SEPARATED;
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREFLUXTRACKING_ATMOSPHEREFLUXTRACKING_HPP_ */
