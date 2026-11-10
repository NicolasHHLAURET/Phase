/*
 * DartColor.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTCOLOR_HPP_
#define INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTCOLOR_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "DartsRGB.hpp"

namespace DartInputs {
	class DartColor {
	private:
		std::string name;
		shared_ptr<DartsRGB> color;
		std::string textureFile;
		int itemId;
	public:
		DartColor() :
				itemId(1) {}

		const std::string & getName() const {
			return name;
		}
		const shared_ptr<DartsRGB> & getColor() const {
			return color;
		}
		const std::string & getTextureFile() const {
			return textureFile;
		}
		const int & getItemId() const {
			return itemId;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				name = node.get("name").as<std::string>();
				if (!node.get("color").IsNull()) {
					if (node.get("color").Tag() == "!modelisation.objectColor.DartsRGB") {
						color = shared_ptr<DartsRGB>(new DartsRGB());
						static_cast<DartsRGB &>(*color) = node.get("color");
					}
				}
				if (!node.get("textureFile").IsNull()) {
					textureFile = node.get("textureFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
				itemId = node.get("itemId").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GENERICOBJECT_DARTCOLOR_HPP_ */
