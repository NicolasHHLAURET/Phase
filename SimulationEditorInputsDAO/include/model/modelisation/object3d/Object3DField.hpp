/*
 * Object3DField.hpp
 *
 *  Created on: 12 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DFIELD_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DFIELD_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IObject3DNumber.hpp"
namespace DartInputs {
	class Object3DField: public IObject3DNumber {
	private:
		std::string fieldDescriptionFile;

	public:
		Object3DField() {}

		const std::string & getFieldDescriptionFile() const {
			return fieldDescriptionFile;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("fieldDescriptionFile").IsNull()) {
					fieldDescriptionFile = node.get("fieldDescriptionFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3DFIELD_HPP_ */
