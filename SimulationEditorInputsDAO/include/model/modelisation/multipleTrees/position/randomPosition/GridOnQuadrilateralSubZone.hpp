/*
 * GridOnQuadrilateralSubZone.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONQUADRILATERALSUBZONE_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONQUADRILATERALSUBZONE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IRandomSpatialDistribution.hpp"
#include "../../../../object/genericObject/DoubletDouble.hpp"

namespace DartInputs {
	class GridOnQuadrilateralSubZone: public IRandomSpatialDistribution {
	private:
		shared_ptr<DoubletDouble> point1;
		shared_ptr<DoubletDouble> point2;
		shared_ptr<DoubletDouble> point3;
		shared_ptr<DoubletDouble> point4;
	public:
		GridOnQuadrilateralSubZone() {}

		const shared_ptr<DoubletDouble> & getPoint1() const {
			return point1;
		}
		const shared_ptr<DoubletDouble> & getPoint2() const {
			return point2;
		}
		const shared_ptr<DoubletDouble> & getPoint3() const {
			return point3;
		}
		const shared_ptr<DoubletDouble> & getPoint4() const {
			return point4;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("point1").IsNull()) {
					point1 = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*point1) = node.get("point1");
				}
				if (!node.get("point2").IsNull()) {
					point2 = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*point2) = node.get("point2");
				}
				if (!node.get("point3").IsNull()) {
					point3 = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*point3) = node.get("point3");
				}
				if (!node.get("point4").IsNull()) {
					point4 = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*point4) = node.get("point4");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONQUADRILATERALSUBZONE_HPP_ */
