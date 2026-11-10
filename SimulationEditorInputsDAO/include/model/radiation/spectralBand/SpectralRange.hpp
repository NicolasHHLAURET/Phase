/*
 * SpectralRange.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALRANGE_HPP_
#define INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALRANGE_HPP_

#include "SpectralBand.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SpectralRange: public SpectralBand {
	private:
		int numberOfBand;
		double intervalBetweenBands;
	public:
		SpectralRange() :
				numberOfBand(20), intervalBetweenBands(0.01) {}

		const int & getNumberOfBand() const {
			return numberOfBand;
		}

		const double & getIntervalBetweenBands() const {
			return intervalBetweenBands;
		}

		void operator =(const YAML::Node& node) {
			SpectralBand::operator =(node); // <-> super.method();
			if (!node.IsNull()) {
				numberOfBand = node.get("numberOfBand").as<int>();
				intervalBetweenBands = node.get("intervalBetweenBands").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_SPECTRALBAND_SPECTRALRANGE_HPP_ */
