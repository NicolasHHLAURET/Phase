/*
 * DartSpectralBands.hpp
 *
 *  Created on: 25 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTSPECTRALBANDS_HPP_
#define INCLUDE_MODEL_DARTSPECTRALBANDS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "radiation/spectralBand/SpectralBand.hpp"
#include "radiation/spectralBand/SpectralRange.hpp"
#include "../yaml/YaMLReader.hpp"
namespace DartInputs {
	class DartSpectralBands {
	private:
		std::vector<shared_ptr<SpectralBand> > listOfSpectralBand;

	public:
		DartSpectralBands() {}

		const std::vector<shared_ptr<SpectralBand> > & getListOfSpectralBand() const {
			return listOfSpectralBand;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			const YAML::Node & node = yamlReader.getFirstNode("listOfSpectralBand");
			if (!node.IsNull() && node.IsSequence()) {
				for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
					shared_ptr<SpectralBand> spectralBand;
					if (it->Tag() == "!radiation.spectralBand.SpectralBand") {
						spectralBand = shared_ptr<SpectralBand>(new SpectralBand());
						static_cast<SpectralBand &>(*spectralBand) = (*it);
					} else if (it->Tag() == "!radiation.spectralBand.SpectralRange") {
						// ICI on pourrait traiter les ranges et les faire "disparaitre" de la structure
						spectralBand = shared_ptr<SpectralBand>(new SpectralRange());
						static_cast<SpectralRange &>(*spectralBand) = (*it);
					}
					listOfSpectralBand.push_back(spectralBand);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTSPECTRALBANDS_HPP_ */
