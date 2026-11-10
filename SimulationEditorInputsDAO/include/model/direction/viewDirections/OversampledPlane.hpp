/*
 * OversampledPlane.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OVERSAMPLEDPLANE_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OVERSAMPLEDPLANE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "object/OversampledThetaRange.hpp"
#include "IAddedViewDirections.hpp"
namespace DartInputs {
	class OversampledPlane: public IAddedViewDirections {
	private:
		double azimuthalPlane;
		double directionZenithStep;
		double directionAzimuthStep;

		shared_ptr<OversampledThetaRange> thetaRange;

	public:
		OversampledPlane() :
				azimuthalPlane(65), directionZenithStep(10), directionAzimuthStep(0.001) {}

		const double & getAzimuthalPlane() const {
			return azimuthalPlane;
		}
		const double & getDirectionZenithStep() const {
			return directionZenithStep;
		}
		const double & getDirectionAzimuthStep() const {
			return directionAzimuthStep;
		}
		const shared_ptr<OversampledThetaRange> & getThetaRange() const {
			return thetaRange;
		}

		void operator =(const YAML::Node& node) {
			IAddedViewDirections::operator=(node);
			if (!node.IsNull()) {
				azimuthalPlane = node.get("azimuthalPlane").as<double>();
				directionZenithStep = node.get("directionZenithStep").as<double>();
				directionAzimuthStep = node.get("directionAzimuthStep").as<double>();

				if (!node.get("thetaRange").IsNull()) {
					thetaRange = shared_ptr<OversampledThetaRange>(new OversampledThetaRange());
					(*thetaRange) = node.get("thetaRange");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OVERSAMPLEDPLANE_HPP_ */
