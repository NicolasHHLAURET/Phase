/*
 * IDartObject.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_IDARTOBJECT_HPP_
#define INCLUDE_MODEL_MODELISATION_IDARTOBJECT_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class IDartObject {
	private:
		std::string objectName;
		int objectColorId;
	public:
		IDartObject() : objectColorId(1) {}
		virtual ~IDartObject() {}

		const std::string & getObjectName() const {
			return objectName;
		}
		const int & getObjectColor() const {
			return objectColorId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				objectName = node.get("objectName").as<std::string>();
				objectColorId = node.get("objectColorId").as<int>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_IDARTOBJECT_HPP_ */
