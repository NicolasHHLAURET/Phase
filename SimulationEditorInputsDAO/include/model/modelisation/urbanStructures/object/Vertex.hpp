/*
 * Vertex.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_VERTEX_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_VERTEX_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class Vertex {
	private:
		double x;
		double y;
		double z;
	public:
		Vertex() :
				x(0), y(0), z(0) {}

		const double & getX() {
			return x;
		}

		const double & getY() {
			return y;
		}

		const double & getZ() {
			return z;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				x = node.get("x").as<double>();
				y = node.get("y").as<double>();
				z = node.get("z").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_OBJECT_VERTEX_HPP_ */
