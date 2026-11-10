/*
 * TripletDouble.hpp
 *
 *  Created on: 19 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GEOMETRY_EULERROTATION_HPP_
#define INCLUDE_MODEL_OBJECT_GEOMETRY_EULERROTATION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class EulerRotation {
	private:
		double z1;
		double y;
		double z2;
	public:
		EulerRotation() :
				z1(0), y(0), z2(0) {}

		const double & getZ1() {
			return z1;
		}

		const double & getY() {
			return y;
		}

		const double & getZ2() {
			return z2;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				z1 = node.get("z1").as<double>();
				y = node.get("y").as<double>();
				z2 = node.get("z2").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GEOMETRY_EULERROTATION_HPP_ */
