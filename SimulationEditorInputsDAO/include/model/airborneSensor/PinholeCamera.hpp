/*
 * PinholeCamera.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLECAMERA_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLECAMERA_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ISensor.hpp"
#include "yaml-cpp/yaml.h"
#include "../object/genericObject/TripletDouble.hpp"
#include "pinholeDefinition/IPinholeCameraDefinition.hpp"
#include "pinholeDefinition/hemispherical/HemisphericalCameraDefinition.hpp"
#include "pinholeDefinition/noHemispherical/PinholeCameraDefinition.hpp"
#include "pinholeDefinition/PinholeCameraRotation.hpp"
namespace DartInputs {
	class PinholeCamera: public ISensor {
	private:
		shared_ptr<IPinholeCameraDefinition> cameraDefinition;
		shared_ptr<PinholeCameraRotation> cameraRotation;
		shared_ptr<TripletDouble> cameraPosition;
		double cameraIntrinsicRotation;

	public:
		PinholeCamera() :
				cameraIntrinsicRotation(0) {}

		const shared_ptr<IPinholeCameraDefinition> & getCameraDefinition() const {
			return cameraDefinition;
		}

		const shared_ptr<PinholeCameraRotation> & getCameraRotation() const {
			return cameraRotation;
		}

		const shared_ptr<TripletDouble> & getCameraPosition() const {
			return cameraPosition;
		}

		const double & getCameraIntrinsicRotation() const {
			return cameraIntrinsicRotation;
		}

		void operator=(const YAML::Node& node) {
			ISensor::operator =(node); // <-> super.method();
			if (!node.IsNull()) {
				cameraIntrinsicRotation = node.get("cameraIntrinsicRotation").as<double>();
				if (!node.get("cameraPosition").IsNull()) {
					if (node.get("cameraPosition").Tag() == "!object.genericObject.Triplet") {
						cameraPosition = shared_ptr<TripletDouble>(new TripletDouble());
						(*cameraPosition) = node.get("cameraPosition");
					}
				}
				if (!node.get("cameraRotation").IsNull()) {
					if (node.get("cameraRotation").Tag() == "!airborneSensor.pinholeDefinition.PinholeCameraRotation") {
						cameraRotation = shared_ptr<PinholeCameraRotation>(new PinholeCameraRotation());
						(*cameraRotation) = node.get("cameraRotation");
					}
				}

				if (!node.get("cameraDefinition").IsNull()) {
					if (node.get("cameraDefinition").Tag() == "!airborneSensor.pinholeDefinition.hemispherical.HemisphericalCameraDefinition") {
						cameraDefinition = shared_ptr<IPinholeCameraDefinition>(new HemisphericalCameraDefinition());
						static_cast<HemisphericalCameraDefinition &>(*cameraDefinition) = node.get("cameraDefinition");
					} else if (node.get("cameraDefinition").Tag() == "!airborneSensor.pinholeDefinition.noHemispherical.PinholeCameraDefinition") {
						cameraDefinition = shared_ptr<IPinholeCameraDefinition>(new PinholeCameraDefinition());
						static_cast<PinholeCameraDefinition &>(*cameraDefinition) = node.get("cameraDefinition");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLECAMERA_HPP_ */
