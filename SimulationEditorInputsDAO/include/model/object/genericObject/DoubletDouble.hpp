/*
 * DoubletDouble.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETDOUBLE_HPP_
#define INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETDOUBLE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class DoubletDouble {
	private:
		double x;
		double y;
	public:
		DoubletDouble() :
				x(0), y(0) {}

		const double & getX() {
			return x;
		}

		const double & getY() {
			return y;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				x = node.get("x").as<double>();
				y = node.get("y").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GENERICOBJECT_DOUBLETDOUBLE_HPP_ */
