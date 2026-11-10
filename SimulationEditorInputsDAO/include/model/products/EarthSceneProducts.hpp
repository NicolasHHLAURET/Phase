/*
 * EarthSceneProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "earthSceneProducts/LAIProducts.hpp"
#include "earthSceneProducts/TriangleGroundCoverProducts.hpp"
#include "earthSceneProducts/TrianglePerCellAreaProducts.hpp"
namespace DartInputs {
	class EarthSceneProducts {
	private:
		shared_ptr<LAIProducts> laiProducts;
		shared_ptr<TriangleGroundCoverProducts> triangleGroundCoverProducts;
		shared_ptr<TrianglePerCellAreaProducts> trianglePerCellAreaProducts;
		bool demProducts;
	public:
		EarthSceneProducts() :
				demProducts(false) {}

		const shared_ptr<LAIProducts> & getLaiProducts() const {
			return laiProducts;
		}

		const shared_ptr<TriangleGroundCoverProducts> & getTriangleGroundCoverProducts() const {
			return triangleGroundCoverProducts;
		}

		const shared_ptr<TrianglePerCellAreaProducts> & getTrianglePerCellAreaProducts() const {
			return trianglePerCellAreaProducts;
		}

		const bool & getDemProducts() const {
			return demProducts;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("laiProducts").IsNull()) {
					laiProducts = shared_ptr<LAIProducts>(new LAIProducts());
					(*laiProducts) = node.get("laiProducts");
				}
				if (!node.get("triangleGroundCoverProducts").IsNull()) {
					triangleGroundCoverProducts = shared_ptr<TriangleGroundCoverProducts>(new TriangleGroundCoverProducts());
					(*triangleGroundCoverProducts) = node.get("triangleGroundCoverProducts");
				}
				if (!node.get("trianglePerCellAreaProducts").IsNull()) {
					trianglePerCellAreaProducts = shared_ptr<TrianglePerCellAreaProducts>(new TrianglePerCellAreaProducts());
					(*trianglePerCellAreaProducts) = node.get("trianglePerCellAreaProducts");
				}
				demProducts = node.get("demProducts").as<bool>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_EARTHSCENEPRODUCTS_HPP_ */
