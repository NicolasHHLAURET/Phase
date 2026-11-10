/*
 * Object3D.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3D_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3D_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../IDartObject.hpp"

#include "Group.hpp"
#include "GroupLeaf.hpp"
#include "GroupTwig.hpp"

#include "Object3DGeometry.hpp"

#include "IObject3DNumber.hpp"
#include "Object3DSingle.hpp"
#include "Object3DField.hpp"
namespace DartInputs {
	class Object3D: public IDartObject {
	private:
		shared_ptr<Group> groups;
		std::string sourceFile;
		shared_ptr<Object3DGeometry> objectGeometry;
		shared_ptr<IObject3DNumber> objectNumber;

	public:
		Object3D() {}

		const shared_ptr<Group> & getGroups() const {
			return groups;
		}
		const std::string & getSourceFile() const {
			return sourceFile;
		}
		const shared_ptr<Object3DGeometry> & getObjectGeometry() const {
			return objectGeometry;
		}
		const shared_ptr<IObject3DNumber> & getObjectNumber() const {
			return objectNumber;
		}

		void operator =(const YAML::Node& node) {
			IDartObject::operator =(node);
			if (!node.IsNull()) {
				if (!node.get("sourceFile").IsNull()) {
					sourceFile = node.get("sourceFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
				if (!node.get("groups").IsNull()) {
					if (node.get("groups").Tag() == "!modelisation.object3d.GroupLeaf") {
						groups = shared_ptr<Group>(new GroupLeaf());
						static_cast<GroupLeaf &>(*groups) = node.get("groups");
					} else if (node.get("groups").Tag() == "!modelisation.object3d.GroupTwig") {
						groups = shared_ptr<Group>(new GroupTwig());
						static_cast<GroupTwig &>(*groups) = node.get("groups");
					}
				}
				if (!node.get("objectGeometry").IsNull()) {
					objectGeometry = shared_ptr<Object3DGeometry>(new Object3DGeometry());
					(*objectGeometry) = node.get("objectGeometry");
				}
				if (!node.get("objectNumber").IsNull()) {
					if (node.get("objectNumber").Tag() == "!modelisation.object3d.Object3DSingle") {
						objectNumber = shared_ptr<IObject3DNumber>(new Object3DSingle());
						static_cast<Object3DSingle &>(*objectNumber) = node.get("objectNumber");
					} else if (node.get("objectNumber").Tag() == "!modelisation.object3d.Object3DField") {
						objectNumber = shared_ptr<IObject3DNumber>(new Object3DField());
						static_cast<Object3DField &>(*objectNumber) = node.get("objectNumber");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_OBJECT3D_HPP_ */
