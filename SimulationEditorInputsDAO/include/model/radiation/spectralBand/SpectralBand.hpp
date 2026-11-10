/*
 * SpectralBand.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALBAND_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALBAND_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "spectralDomain/ISpectralDomain.hpp"
#include "spectralDomain/ModeR.hpp"
#include "spectralDomain/ModeRT.hpp"
#include "spectralDomain/ModeT.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SpectralBand {
	private:
		// User name
		std::string spectralName;

		// Position
		double centralWavelength;
		double spectralBandWidth;

		// Mode
		shared_ptr<ISpectralDomain> spectralDomain;

		double spectralSolarConstant;
		double skyl;

		// Enabled boolean
		bool spectralBandEnabled;

		int itemId;

	public:
		virtual ~SpectralBand() {}

		const std::string & getSpectralName() const {
			return spectralName;
		}

		const double & getCentralWavelength() const {
			return centralWavelength;
		}

		const double & getSpectralBandWidth() const {
			return spectralBandWidth;
		}

		const shared_ptr<ISpectralDomain> & getSpectralDomain() const {
			return spectralDomain;
		}

		const double & getSpectralSolarConstant() const {
			return spectralSolarConstant;
		}

		const double & getSkyl() const {
			return skyl;
		}

		const bool & getSpectralBandEnabled() const {
			return spectralBandEnabled;
		}

		const int & getItemId() const {
			return itemId;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				spectralName = node.get("spectralName").as<std::string>();

				centralWavelength = node.get("centralWavelength").as<double>();
				spectralBandWidth = node.get("spectralBandWidth").as<double>();

				if (!node.get("spectralDomain").IsNull()) {
					if (node.get("spectralDomain").Tag() == "!radiation.spectralBand.spectralDomain.ModeR") {
						spectralDomain = shared_ptr<ISpectralDomain>(new ModeR());
						static_cast<ModeR &>(*spectralDomain) = node.get("spectralDomain");
					} else if (node.get("spectralDomain").Tag() == "!radiation.spectralBand.spectralDomain.ModeRT") {
						spectralDomain = shared_ptr<ISpectralDomain>(new ModeRT());
						static_cast<ModeRT &>(*spectralDomain) = node.get("spectralDomain");
					} else if (node.get("spectralDomain").Tag() == "!radiation.spectralBand.spectralDomain.ModeT") {
						spectralDomain = shared_ptr<ISpectralDomain>(new ModeT());
						static_cast<ModeT &>(*spectralDomain) = node.get("spectralDomain");
					}
				}

				spectralSolarConstant = node.get("spectralSolarConstant").as<double>();
				skyl = node.get("skyl").as<double>();

				spectralBandEnabled = node.get("spectralBandEnabled").as<bool>();
				itemId = node.get("itemId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALBAND_HPP_ */
