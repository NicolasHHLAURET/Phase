/*
 * InsideSensor.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSOR_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSOR_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../object/genericObject/TripletDouble.hpp"
#include "../object/genericObject/DoubletDouble.hpp"
#include "../object/genericObject/DoubletInteger.hpp"
#include "insideSensorDefinition/IInsideSensorViewDirection.hpp"
#include "insideSensorDefinition/InsideSensorViewDefinitionAngle.hpp"
#include "insideSensorDefinition/InsideSensorViewDefinitionAxis.hpp"
#include "ISensor.hpp"
namespace DartInputs {
	class InsideSensor: public ISensor {
	private:
		shared_ptr<TripletDouble> sensorCentralPosition;
		shared_ptr<DoubletDouble> sensorDimensions;
		shared_ptr<DoubletInteger> imageResolution;
		shared_ptr<IInsideSensorViewDirection> sensorViewDirection;
		double focalViewDistance;
		double sensorRotation;

	public:
		InsideSensor() :
				focalViewDistance(0.1), sensorRotation(0) {}

		const shared_ptr<TripletDouble> & getSensorCentralPosition() const {
			return sensorCentralPosition;
		}

		const shared_ptr<DoubletDouble> & getSensorDimensions() const {
			return sensorDimensions;
		}

		const shared_ptr<DoubletInteger> & getImageResolution() const {
			return imageResolution;
		}

		const shared_ptr<IInsideSensorViewDirection> & getSensorViewDirection() const {
			return sensorViewDirection;
		}

		const double & getFocalViewDistance() const {
			return focalViewDistance;
		}

		const double & getSensorRotation() const {
			return sensorRotation;
		}

		void operator=(const YAML::Node& node) {
			ISensor::operator =(node); // <-> super.method();
			if (!node.IsNull()) {
				if (!node.get("sensorCentralPosition").IsNull()) {
					if (node.get("sensorCentralPosition").Tag() == "!object.genericObject.Triplet") {
						sensorCentralPosition = shared_ptr<TripletDouble>(new TripletDouble());
						(*sensorCentralPosition) = node.get("sensorCentralPosition");
					}
				}
				if (!node.get("sensorDimensions").IsNull()) {
					if (node.get("sensorDimensions").Tag() == "!object.genericObject.Doublet") {
						sensorDimensions = shared_ptr<DoubletDouble>(new DoubletDouble());
						(*sensorDimensions) = node.get("sensorDimensions");
					}
				}
				if (!node.get("imageResolution").IsNull()) {
					if (node.get("imageResolution").Tag() == "!object.genericObject.Doublet") {
						imageResolution = shared_ptr<DoubletInteger>(new DoubletInteger());
						(*imageResolution) = node.get("imageResolution");
					}
				}
				if (!node.get("sensorViewDirection").IsNull()) {
					if (node.get("sensorViewDirection").Tag() == "!airborneSensor.insideSensorDefinition.InsideSensorViewDefinitionAngle") {
						sensorViewDirection = shared_ptr<IInsideSensorViewDirection>(new InsideSensorViewDefinitionAngle());
						static_cast<InsideSensorViewDefinitionAngle &>(*sensorViewDirection) = node.get("sensorViewDirection");
					} else if (node.get("sensorViewDirection").Tag() == "!airborneSensor.insideSensorDefinition.InsideSensorViewDefinitionAxis") {
						sensorViewDirection = shared_ptr<IInsideSensorViewDirection>(new InsideSensorViewDefinitionAxis());
						static_cast<InsideSensorViewDefinitionAxis &>(*sensorViewDirection) = node.get("sensorViewDirection");
					}
				}
				focalViewDistance = node.get("focalViewDistance").as<double>();
				sensorRotation = node.get("sensorRotation").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSOR_HPP_ */
