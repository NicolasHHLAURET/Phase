/*
 * HemisphericalCameraDefinition.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALCAMERADEFINITION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALCAMERADEFINITION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../IPinholeCameraDefinition.hpp"
#include "HemisphericalFOV.hpp"
namespace DartInputs {
	class HemisphericalCameraDefinition: public IPinholeCameraDefinition {
	private:
		double imageAngleResolution;
		shared_ptr<HemisphericalFOV> cameraFov;

	public:
		HemisphericalCameraDefinition() :
				imageAngleResolution(0) {}

		const shared_ptr<HemisphericalFOV> & getCameraFov() const {
			return cameraFov;
		}

		const double & getImageAngleResolution() const {
			return imageAngleResolution;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				imageAngleResolution = node.get("imageAngleResolution").as<double>();
				if (!node.get("cameraFov").IsNull()) {
					cameraFov = shared_ptr<HemisphericalFOV>(new HemisphericalFOV());
					(*cameraFov) = node.get("cameraFov");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_HEMISPHERICAL_HEMISPHERICALCAMERADEFINITION_HPP_ */
