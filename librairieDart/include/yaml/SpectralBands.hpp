/*
 * SpectralBands.hpp
 *
 *  Created on: 14 nov. 2017
 *      Author: guilleuxj
 */

#ifndef INCLUDE_YAML_SPECTRALBANDS_HPP_
#define INCLUDE_YAML_SPECTRALBANDS_HPP_

struct YamlSpectralBand {
	double centralWavelength;
	double spectralBandWidth;
};

namespace YAML {
	template<>
	struct convert<YamlSpectralBand> {
		static Node encode(const YamlSpectralBand& sb) {
			Node node;
			node["centralWavelength"] = sb.centralWavelength;
			node["spectralBandWidth"] = sb.spectralBandWidth;
			return node;
		}

		static bool decode(	const Node& node,
							YamlSpectralBand& sb) {
			sb.centralWavelength = node["centralWavelength"].as<double>();
			sb.spectralBandWidth = node["spectralBandWidth"].as<double>();
			return true;
		}
	};
}

//const YAML::Node & spectralBandsNode = _yamlContainer.getNode("listOfSpectralBand");
//YamlSpectralBand sb = spectralBandsNode[0].as<YamlSpectralBand>();
//
//cerr << "SPECTRAL BAND : " << sb.centralWavelength << endl;
//cout << "SPECTRAL BAND : " << sb.centralWavelength << endl;


#endif /* INCLUDE_YAML_SPECTRALBANDS_HPP_ */
