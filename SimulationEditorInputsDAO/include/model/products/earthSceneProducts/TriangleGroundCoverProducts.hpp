/*
 * TriangleGroundCoverProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEGROUNDCOVERPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEGROUNDCOVERPRODUCTS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class TriangleGroundCoverProducts {
	private:
		double triangleGroundCoverPrecision;
		bool triangleGroundCoverByTypeOrColor;
		bool triangleGroundCoverTotalFile;

	public:
		TriangleGroundCoverProducts() :
				triangleGroundCoverPrecision(0.1), triangleGroundCoverByTypeOrColor(false), triangleGroundCoverTotalFile(true) {}

		double getTriangleGroundCoverPrecision() {
			return triangleGroundCoverPrecision;
		}

		bool getTriangleGroundCoverByTypeOrColor() {
			return triangleGroundCoverByTypeOrColor;
		}

		bool getTriangleGroundCoverTotalFile() {
			return triangleGroundCoverTotalFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				triangleGroundCoverPrecision = node.get("triangleGroundCoverPrecision").as<double>();
				triangleGroundCoverByTypeOrColor = node.get("triangleGroundCoverByTypeOrColor").as<bool>();
				triangleGroundCoverTotalFile = node.get("triangleGroundCoverTotalFile").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEGROUNDCOVERPRODUCTS_HPP_ */
