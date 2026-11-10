/*
 * LidarImageProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_LIDARIMAGEPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_LIDARIMAGEPRODUCTS_HPP_
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LidarImageProducts {
	public:
		enum NumberPrecision {
			FLOAT, DOUBLE
		};
	private:
		bool storeStaticSection;
		bool storeNonConvolvedWaveform;
		bool storeFirstOrderWaveform;
		NumberPrecision numberPrecision;

	public:
		LidarImageProducts() :
				storeStaticSection(true), storeNonConvolvedWaveform(true), storeFirstOrderWaveform(true), numberPrecision(DOUBLE) {}

		const bool & getStoreStaticSection() const {
			return storeStaticSection;
		}

		const bool & getStoreNonConvolvedWaveform() const {
			return storeNonConvolvedWaveform;
		}

		const bool & getStoreFirstOrderWaveform() const {
			return storeFirstOrderWaveform;
		}

		const NumberPrecision & getNumberPrecision() const {
			return numberPrecision;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				storeStaticSection = node.get("storeStaticSection").as<bool>();
				storeNonConvolvedWaveform = node.get("storeNonConvolvedWaveform").as<bool>();
				storeFirstOrderWaveform = node.get("storeFirstOrderWaveform").as<bool>();
				if (node.get("numberPrecision").as<std::string>() == "FLOAT") {
					numberPrecision = FLOAT;
				} else {
					numberPrecision = DOUBLE;
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_LIDARIMAGEPRODUCTS_HPP_ */
