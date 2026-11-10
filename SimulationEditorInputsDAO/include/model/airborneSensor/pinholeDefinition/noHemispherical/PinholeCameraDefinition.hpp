/*
 * PinholeCameraDefinition.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERADEFINITION_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERADEFINITION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../IPinholeCameraDefinition.hpp"
#include "PinholeCameraFOV.hpp"
namespace DartInputs {
	class PinholeCameraDefinition: public IPinholeCameraDefinition {
	private:
		shared_ptr<PinholeCameraFOV> cameraFov;

	public:
		PinholeCameraDefinition() {}

		const shared_ptr<PinholeCameraFOV> & getCameraFov() const {
			return cameraFov;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("cameraFov").IsNull()) {
					cameraFov = shared_ptr<PinholeCameraFOV>(new PinholeCameraFOV());
					(*cameraFov) = node.get("cameraFov");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERADEFINITION_HPP_ */
