/*
 * ModeR.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODER_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODER_HPP_

#include "ISpectralDomain.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ModeR: public ISpectralDomain {
	public:
		ModeR() {}

		void operator=(const YAML::Node& node) {}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_MODER_HPP_ */
