/*
 * LidarProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_LIDARPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_LIDARPRODUCTS_HPP_
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarProducts {
	private:
		bool convolvedWaveform;
		bool photonsInformations;
		bool photonsImages;
		bool lidarImagePanelInformation;
	public:
		LidarProducts() :
				convolvedWaveform(true), photonsInformations(false), photonsImages(false), lidarImagePanelInformation(false) {}

		const bool & getConvolvedWaveform() const {
			return convolvedWaveform;
		}

		const bool & getPhotonsInformations() const {
			return photonsInformations;
		}

		const bool & getPhotonsImages() const {
			return photonsImages;
		}

		const bool & getLidarImagePanelInformation() const {
			return photonsImages;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				convolvedWaveform = node.get("convolvedWaveform").as<bool>();
				photonsInformations = node.get("photonsInformations").as<bool>();
				photonsImages = node.get("photonsImages").as<bool>();
				lidarImagePanelInformation = node.get("lidarImagePanelInformation").as<bool>();
			}
		}

	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_LIDARPRODUCTS_HPP_ */
