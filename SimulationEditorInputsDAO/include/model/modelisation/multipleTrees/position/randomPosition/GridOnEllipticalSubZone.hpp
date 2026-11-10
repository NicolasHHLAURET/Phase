/*
 * GridOnEllipticalSubZone.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONELLIPTICALSUBZONE_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONELLIPTICALSUBZONE_HPP_

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
	class GridOnEllipticalSubZone: public IRandomSpatialDistribution {
	private:
		shared_ptr<DoubletDouble> ellipseCenter;
		shared_ptr<DoubletDouble> ellipseRadius;
	public:
		GridOnEllipticalSubZone() {}

		const shared_ptr<DoubletDouble> & getEllipseCenter() const {
			return ellipseCenter;
		}

		const shared_ptr<DoubletDouble> & getEllipseRadius() const {
			return ellipseRadius;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("ellipseCenter").IsNull()) {
					ellipseCenter = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*ellipseCenter) = node.get("ellipseCenter");
				}
				if (!node.get("ellipseRadius").IsNull()) {
					ellipseRadius = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*ellipseRadius) = node.get("ellipseRadius");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_RANDOMPOSITION_GRIDONELLIPTICALSUBZONE_HPP_ */
