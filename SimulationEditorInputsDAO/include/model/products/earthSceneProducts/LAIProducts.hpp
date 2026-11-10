/*
 * LAIProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_LAIPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_LAIPRODUCTS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class LAIProducts {
	private:
		bool laiNumberOfNoEmptyCellsByLayer;
		bool laiVerticalProfile;
		bool lai3Dmatrix;
	public:
		LAIProducts() :
				laiNumberOfNoEmptyCellsByLayer(false), laiVerticalProfile(false), lai3Dmatrix(false) {}

		bool getLaiNumberOfNoEmptyCellsByLayer() {
			return laiNumberOfNoEmptyCellsByLayer;
		}

		bool getLaiVerticalProfile() {
			return laiVerticalProfile;
		}

		bool getLai3Dmatrix() {
			return lai3Dmatrix;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				laiNumberOfNoEmptyCellsByLayer = node.get("laiNumberOfNoEmptyCellsByLayer").as<bool>();
				laiVerticalProfile = node.get("laiVerticalProfile").as<bool>();
				lai3Dmatrix = node.get("lai3Dmatrix").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_LAIPRODUCTS_HPP_ */
