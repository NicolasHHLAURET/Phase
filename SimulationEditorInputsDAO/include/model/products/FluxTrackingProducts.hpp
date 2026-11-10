/*
 * FluxTrackingProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_FLUXTRACKINGPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_FLUXTRACKINGPRODUCTS_HPP_
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluxTrackingProducts {
	public:
		enum SpreadingOfRays {
			NONE, APPROXIMATIVE, PRECISE
		};

		enum OrthoImages {
			_NONE, ENERGY_CONSERVATION, REFLECTANCE_CONSERVATION, INDUSTRY_STANDARD
		};

		enum ProductsUnit {
			REFLECTANCE_TEMPERATURE, RADIANCE_AND_REFLECTANCE_TEMPERATURE, RADIANCE
		};
	private:
		bool allIterationsProducts;
		bool earthSceneFirstOrderScattering;

		bool extrapolationProduct;
		bool sensorPlaneImages;
		bool meanBrfBtfPerImage;
		bool images;
		SpreadingOfRays spreadingOfRays;
		bool keepNonProjectedImages;
		ProductsUnit radianceProducts;

		double maximalZenithAngleOfImages;
		int horizontalPlaneImageOversampling;
		int sensorPlaneImageOversampling;

		OrthoImages generateOrthoImages;

		bool useMaskedCells;
		int maskedCells;

		int numberOfRepetitiveScenes;

	public:
		FluxTrackingProducts() :
				allIterationsProducts(false), earthSceneFirstOrderScattering(false), extrapolationProduct(true), sensorPlaneImages(true), meanBrfBtfPerImage(
					true), images(true), spreadingOfRays(PRECISE), keepNonProjectedImages(false), radianceProducts(REFLECTANCE_TEMPERATURE), maximalZenithAngleOfImages(
					25.0), horizontalPlaneImageOversampling(1), sensorPlaneImageOversampling(1), generateOrthoImages(_NONE), useMaskedCells(false), maskedCells(
					0), numberOfRepetitiveScenes(1) {}

		const bool & getAllIterationsProducts() const {
			return allIterationsProducts;
		}

		const bool & getEarthSceneFirstOrderScattering() const {
			return earthSceneFirstOrderScattering;
		}

		const bool & getExtrapolationProduct() const {
			return extrapolationProduct;
		}

		const bool & getSensorPlaneImages() const {
			return sensorPlaneImages;
		}

		const bool & getMeanBrfBtfPerImage() const {
			return meanBrfBtfPerImage;
		}
		const bool & getImages() const {
			return images;
		}

		const SpreadingOfRays & getSpreadingOfRays() const {
			return spreadingOfRays;
		}

		const bool & getKeepNonProjectedImages() const {
			return keepNonProjectedImages;
		}

		const ProductsUnit & getRadianceProducts() const {
			return radianceProducts;
		}

		const double & getMaximalZenithAngleOfImages() const {
			return maximalZenithAngleOfImages;
		}

		const int & getHorizontalPlaneImageOversampling() const {
			return horizontalPlaneImageOversampling;
		}

		const int & getSensorPlaneImageOversampling() const {
			return sensorPlaneImageOversampling;
		}

		const OrthoImages & getGenerateOrthoImages() const {
			return generateOrthoImages;
		}

		const bool & getUseMaskedCells() const {
			return useMaskedCells;
		}

		const int & getMaskedCells() const {
			return maskedCells;
		}

		const int & getNumberOfRepetitiveScenes() const {
			return numberOfRepetitiveScenes;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				allIterationsProducts = node.get("allIterationsProducts").as<bool>();
				earthSceneFirstOrderScattering = node.get("earthSceneFirstOrderScattering").as<bool>();

				extrapolationProduct = node.get("extrapolationProduct").as<bool>();
				sensorPlaneImages = node.get("sensorPlaneImages").as<bool>();
				meanBrfBtfPerImage = node.get("meanBrfBtfPerImage").as<bool>();
				images = node.get("images").as<bool>();
				if (node.get("spreadingOfRays").as<std::string>() == "NONE") {
					spreadingOfRays = NONE;
				} else if (node.get("spreadingOfRays").as<std::string>() == "APPROXIMATIVE") {
					spreadingOfRays = APPROXIMATIVE;
				} else {
					spreadingOfRays = PRECISE;
				}
				keepNonProjectedImages = node.get("keepNonProjectedImages").as<bool>();

				if (node.get("radianceProducts").as<std::string>() == "REFLECTANCE_TEMPERATURE") {
					radianceProducts = REFLECTANCE_TEMPERATURE;
				} else if (node.get("spreadingOfRays").as<std::string>() == "RADIANCE_AND_REFLECTANCE_TEMPERATURE") {
					radianceProducts = RADIANCE_AND_REFLECTANCE_TEMPERATURE;
				} else {
					radianceProducts = RADIANCE;
				}

				maximalZenithAngleOfImages = node.get("maximalZenithAngleOfImages").as<double>();
				horizontalPlaneImageOversampling = node.get("horizontalPlaneImageOversampling").as<int>();
				sensorPlaneImageOversampling = node.get("sensorPlaneImageOversampling").as<int>();

				if (node.get("generateOrthoImages").as<std::string>() == "NONE") {
					generateOrthoImages = _NONE;
				} else if (node.get("generateOrthoImages").as<std::string>() == "ENERGY_CONSERVATION") {
					generateOrthoImages = ENERGY_CONSERVATION;
				} else if (node.get("generateOrthoImages").as<std::string>() == "REFLECTANCE_CONSERVATION") {
					generateOrthoImages = REFLECTANCE_CONSERVATION;
				} else {
					generateOrthoImages = INDUSTRY_STANDARD;
				}

				useMaskedCells = node.get("useMaskedCells").as<bool>();
				maskedCells = node.get("maskedCells").as<int>();

				numberOfRepetitiveScenes = node.get("numberOfRepetitiveScenes").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_FLUXTRACKINGPRODUCTS_HPP_ */
