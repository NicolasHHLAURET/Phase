/*
 * PinholeCameraFOV.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERAFOV_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERAFOV_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "PinholeCameraNumberPixelsInFOV.hpp"
#include "IPinholeCameraAngleDefinition.hpp"
#include "angleDefinition/PinholeCameraAngleOfView.hpp"
#include "angleDefinition/PinholeCameraFOVSize.hpp"
namespace DartInputs {
	class PinholeCameraFOV {
	private:
		shared_ptr<PinholeCameraNumberPixelsInFOV> numberOfPixels;
		shared_ptr<IPinholeCameraAngleDefinition> angleDefintion;

	public:
		PinholeCameraFOV() {}

		const shared_ptr<PinholeCameraNumberPixelsInFOV> & getNumberOfPixels() const {
			return numberOfPixels;
		}

		const shared_ptr<IPinholeCameraAngleDefinition> & getAngleDefintion() const {
			return angleDefintion;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("numberOfPixels").IsNull()) {
					numberOfPixels = shared_ptr<PinholeCameraNumberPixelsInFOV>(new PinholeCameraNumberPixelsInFOV());
					(*numberOfPixels) = node.get("numberOfPixels");
				}

				if (!node.get("angleDefintion").IsNull()) {
					if (node.get("angleDefintion").Tag() == "!airborneSensor.pinholeDefinition.noHemispherical.angleDefinition.PinholeCameraAngleOfView") {
						angleDefintion = shared_ptr<IPinholeCameraAngleDefinition>(new PinholeCameraAngleOfView());
						static_cast<PinholeCameraAngleOfView &>(*angleDefintion) = node.get("angleDefintion");
					} else if (node.get("angleDefintion").Tag()
							== "airborneSensor.pinholeDefinition.noHemispherical.angleDefinition.PinholeCameraFOVSize") {
						angleDefintion = shared_ptr<IPinholeCameraAngleDefinition>(new PinholeCameraFOVSize());
						static_cast<PinholeCameraFOVSize &>(*angleDefintion) = node.get("angleDefintion");
					}
				}

			}
		}
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PINHOLEDEFINITION_NOHEMISPHERICAL_PINHOLECAMERAFOV_HPP_ */
