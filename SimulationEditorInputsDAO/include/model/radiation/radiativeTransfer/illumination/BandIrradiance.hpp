/*
 * BandIrradiance.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDIRRADIANCE_HPP_
#define INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDIRRADIANCE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IAtmosphericIlumination.hpp"
#include "solarRadiationDefinition/ISolarRadiationDefinition.hpp"
#include "solarRadiationDefinition/SolarConstant.hpp"
#include "solarRadiationDefinition/SolarIrradiance.hpp"

namespace DartInputs {
	class BandIrradiance: public IAtmosphericIlumination {
	private:
		shared_ptr<ISolarRadiationDefinition> solarRadiationDefinition;

	public:
		BandIrradiance() {}

		const shared_ptr<ISolarRadiationDefinition> & getSolarRadiationDefinition() const {
			return solarRadiationDefinition;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("solarRadiationDefinition").IsNull()) {
					if (node.get("solarRadiationDefinition").Tag() == "!radiation.radiativeTransfer.illumination.solarRadiationDefinition.SolarConstant") {
						solarRadiationDefinition = shared_ptr<ISolarRadiationDefinition>(new SolarConstant());
						static_cast<SolarConstant &>(*solarRadiationDefinition) = node.get("solarRadiationDefinition");
					} else if (node.get("solarRadiationDefinition").Tag() == "!radiation.radiativeTransfer.illumination.solarRadiationDefinition.SolarIrradiance") {
						solarRadiationDefinition = shared_ptr<ISolarRadiationDefinition>(new SolarIrradiance());
						static_cast<SolarIrradiance &>(*solarRadiationDefinition) = node.get("solarRadiationDefinition");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_ILLUMINATION_BANDIRRADIANCE_HPP_ */
