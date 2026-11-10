/*
 * ISpectralDomain.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_ISPECTRALDOMAIN_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_ISPECTRALDOMAIN_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ISpectralDomain {
	public:
		virtual ~ISpectralDomain() {}
		virtual void operator=(const YAML::Node& node) = 0;
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALDOMAIN_ISPECTRALDOMAIN_HPP_ */
