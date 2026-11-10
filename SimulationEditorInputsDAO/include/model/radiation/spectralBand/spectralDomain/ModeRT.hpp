/*
 * ModeRT.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODERT_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODERT_HPP_

#include "ISpectralDomain.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ModeRT: public ISpectralDomain {
	public:
		ModeRT() {}

		void operator=(const YAML::Node& node) {}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODERT_HPP_ */
