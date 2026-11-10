/*
 * Face.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_FACE_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_FACE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class Face {
	private:
		int frontFaceOpticalId;
		shared_ptr<int> backFaceOpticalId;

		int frontFaceThermalId;
		shared_ptr<int> backFaceThermalId;

	public:
		Face() :
				frontFaceOpticalId(1), frontFaceThermalId(1) {}

		const int & getFrontFaceOpticalId() {
			return frontFaceOpticalId;
		}
		const shared_ptr<int> & getBackFaceOpticalId() {
			return backFaceOpticalId;
		}

		const int & getFrontFaceThermalId() {
			return frontFaceThermalId;
		}
		const shared_ptr<int> & getBackFaceThermalId() {
			return backFaceThermalId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				frontFaceOpticalId = node.get("frontFaceOpticalId").as<int>();
				frontFaceThermalId = node.get("frontFaceThermalId").as<int>();

				if (!node.get("backFaceOpticalId").IsNull()) {
					backFaceOpticalId = shared_ptr<int>(new int());
					(*backFaceOpticalId) = node.get("backFaceOpticalId").as<int>();
				}
				if (!node.get("backFaceThermalId").IsNull()) {
					backFaceThermalId = shared_ptr<int>(new int());
					(*backFaceThermalId) = node.get("backFaceThermalId").as<int>();
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_FACE_HPP_ */
