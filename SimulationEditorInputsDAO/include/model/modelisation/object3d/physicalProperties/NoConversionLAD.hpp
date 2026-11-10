/*
 * NoConversionLAD.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_NOCONVERSIONLAD_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_NOCONVERSIONLAD_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ILADConversion.hpp"
namespace DartInputs {
	class NoConversionLAD: public ILADConversion {
	private:
		int frontFaceOpticalPropertyId;
		int frontFaceThermalPropertyId;

		shared_ptr<int> backFaceOpticalPropertyId;
		shared_ptr<int> backFaceThermalPropertyId;

	public:
		NoConversionLAD() :
				frontFaceOpticalPropertyId(1), frontFaceThermalPropertyId(1) {}

		const int & getFrontFaceOpticalPropertyId() {
			return frontFaceOpticalPropertyId;
		}
		const int & getFrontFaceThermalPropertyId() {
			return frontFaceThermalPropertyId;
		}

		const shared_ptr<int> & getBackFaceOpticalPropertyId() {
			return backFaceOpticalPropertyId;
		}
		const shared_ptr<int> & getBackFaceThermalPropertyId() {
			return backFaceThermalPropertyId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				frontFaceOpticalPropertyId = node.get("frontFaceOpticalPropertyId").as<int>();
				frontFaceThermalPropertyId = node.get("frontFaceThermalPropertyId").as<int>();

				if (!node.get("backFaceOpticalPropertyId").IsNull()) {
					backFaceOpticalPropertyId = shared_ptr<int>(new int());
					(*backFaceOpticalPropertyId) = node.get("backFaceOpticalPropertyId").as<int>();
				}
				if (!node.get("backFaceThermalPropertyId").IsNull()) {
					backFaceThermalPropertyId = shared_ptr<int>(new int());
					(*backFaceThermalPropertyId) = node.get("backFaceThermalPropertyId").as<int>();
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_NOCONVERSIONLAD_HPP_ */
