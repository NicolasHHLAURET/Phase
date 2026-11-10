/*
 * TrianglePerCellAreaProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEPERCELLAREAPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEPERCELLAREAPRODUCTS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class TrianglePerCellAreaProducts {
	private:
		bool trianglePerCellAreaByTypeOrColor;
		bool trianglePerCellAreaTotalFile;
		bool trianglePerCellAreaProfile1D;
		bool trianglePerCellAreaMatrix3D;

	public:
		TrianglePerCellAreaProducts() :
				trianglePerCellAreaByTypeOrColor(false), trianglePerCellAreaTotalFile(true), trianglePerCellAreaProfile1D(false), trianglePerCellAreaMatrix3D(true) {}

		const bool & getTrianglePerCellAreaByTypeOrColor() const {
			return trianglePerCellAreaByTypeOrColor;
		}

		const bool & getTrianglePerCellAreaTotalFile() const {
			return trianglePerCellAreaTotalFile;
		}

		const bool & getTrianglePerCellAreaProfile1D() const {
			return trianglePerCellAreaProfile1D;
		}

		const bool & getTrianglePerCellAreaMatrix3D() const {
			return trianglePerCellAreaMatrix3D;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				trianglePerCellAreaByTypeOrColor = node.get("trianglePerCellAreaByTypeOrColor").as<bool>();
				trianglePerCellAreaTotalFile = node.get("trianglePerCellAreaTotalFile").as<bool>();
				if (!node.get("trianglePerCellAreaProfile1D").IsNull()) {
					trianglePerCellAreaProfile1D = node.get("trianglePerCellAreaProfile1D").as<bool>();
				}
				if (!node.get("trianglePerCellAreaMatrix3D").IsNull()) {
					trianglePerCellAreaMatrix3D = node.get("trianglePerCellAreaMatrix3D").as<bool>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_TRIANGLEPERCELLAREAPRODUCTS_HPP_ */
