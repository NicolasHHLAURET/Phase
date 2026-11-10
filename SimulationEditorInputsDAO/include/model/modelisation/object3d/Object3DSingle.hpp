/*
 * Object3DSingle.hpp
 *
 *  Created on: 12 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DSINGLE_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DSINGLE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IObject3DNumber.hpp"
#include "../../object/genericObject/TripletDouble.hpp"
namespace DartInputs {
	class Object3DSingle: public IObject3DNumber {
	private:
		shared_ptr<TripletDouble> objectPosition;

	public:
		Object3DSingle() {}

		const shared_ptr<TripletDouble> & getObjectPosition() const {
			return objectPosition;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("objectPosition").IsNull()) {
					objectPosition = shared_ptr<TripletDouble>(new TripletDouble());
					(*objectPosition) = node.get("objectPosition");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DSINGLE_HPP_ */
