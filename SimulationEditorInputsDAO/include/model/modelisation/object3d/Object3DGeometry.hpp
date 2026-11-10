/*
 * Object3DGeometry.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DGEOMETRY_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DGEOMETRY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../../object/genericObject/TripletDouble.hpp"
#include "../../object/geometry/EulerRotation.hpp"
namespace DartInputs {
	class Object3DGeometry {
	private:
		shared_ptr<TripletDouble> originalObjectDimensions;

		shared_ptr<TripletDouble> scaleFactor;
		shared_ptr<TripletDouble> scaleFactorGaussDeviation;

		shared_ptr<EulerRotation> eulerRotation;
		shared_ptr<TripletDouble> eulerRotationGaussDeviation;
	public:
		Object3DGeometry() {}

		const shared_ptr<TripletDouble> & getOriginalObjectDimensions() const {
			return originalObjectDimensions;
		}

		const shared_ptr<TripletDouble> & getScaleFactor() const {
			return scaleFactor;
		}
		const shared_ptr<TripletDouble> & getScaleFactorGaussDeviation() const {
			return scaleFactorGaussDeviation;
		}

		const shared_ptr<EulerRotation> & getEulerRotation() const {
			return eulerRotation;
		}
		const shared_ptr<TripletDouble> & getEulerRotationGaussDeviation() const {
			return eulerRotationGaussDeviation;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("originalObjectDimensions").IsNull()) {
					originalObjectDimensions = shared_ptr<TripletDouble>(new TripletDouble());
					(*originalObjectDimensions) = node.get("originalObjectDimensions");
				}
				if (!node.get("scaleFactor").IsNull()) {
					scaleFactor = shared_ptr<TripletDouble>(new TripletDouble());
					(*scaleFactor) = node.get("scaleFactor");
				}
				if (!node.get("scaleFactorGaussDeviation").IsNull()) {
					scaleFactorGaussDeviation = shared_ptr<TripletDouble>(new TripletDouble());
					(*scaleFactorGaussDeviation) = node.get("scaleFactorGaussDeviation");
				}
				if (!node.get("eulerRotation").IsNull()) {
					eulerRotation = shared_ptr<EulerRotation>(new EulerRotation());
					(*eulerRotation) = node.get("eulerRotation");
				}
				if (!node.get("eulerRotationGaussDeviation").IsNull()) {
					eulerRotationGaussDeviation = shared_ptr<TripletDouble>(new TripletDouble());
					(*eulerRotationGaussDeviation) = node.get("eulerRotationGaussDeviation");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DGEOMETRY_HPP_ */
