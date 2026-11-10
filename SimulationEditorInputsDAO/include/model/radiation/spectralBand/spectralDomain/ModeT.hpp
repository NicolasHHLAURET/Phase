/*
 * ModeT.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODET_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODET_HPP_

#include "ISpectralDomain.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ModeT: public ISpectralDomain {
	public:
		enum ThermalEmissionLaw {
			BOLTSZMAN_LAW, PLANK_LAW
		};
	private:
		ThermalEmissionLaw thermalEmissionLaw;
	public:
		ModeT() :
				thermalEmissionLaw(PLANK_LAW) {}

		const ThermalEmissionLaw & getThermalEmissionLaw() const {
			return thermalEmissionLaw;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("thermalEmissionLaw").as<std::string>() == "BOLTSZMAN_LAW") {
					thermalEmissionLaw = BOLTSZMAN_LAW;
				} else {
					thermalEmissionLaw = PLANK_LAW;
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODET_HPP_ */
