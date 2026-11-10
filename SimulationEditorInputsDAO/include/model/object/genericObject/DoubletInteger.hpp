/*
 * DoubletInteger.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETINTEGER_HPP_
#define INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETINTEGER_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class DoubletInteger {
	private:
		int x;
		int y;
	public:
		DoubletInteger() :
				x(0), y(0) {}

		const int & getX() {
			return x;
		}

		const int & getY() {
			return y;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				x = node.get("x").as<int>();
				y = node.get("y").as<int>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETINTEGER_HPP_ */
