/*
 * WallBase.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_BASE_WALLBASE_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_BASE_WALLBASE_HPP_

#include "yaml-cpp/yaml.h"
#include "../IBuildingObject.hpp"
namespace DartInputs {
	class WallBase: public IBuildingObject {
	private:
	public:
		WallBase() {}

		const int getNbInputVertex() const {
			return 4;
		}
		const int getNbFaceProperties() const {
			return 4;
		}

		void operator =(const YAML::Node& node) {
			IBuildingObject::operator =(node);
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_BASE_WALLBASE_HPP_ */
