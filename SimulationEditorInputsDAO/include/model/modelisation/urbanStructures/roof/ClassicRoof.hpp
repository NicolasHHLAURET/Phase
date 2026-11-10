/*
 * ClassicRoof.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_CLASSICROOF_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_CLASSICROOF_HPP_

#include "yaml-cpp/yaml.h"
#include "IHouseRoof.hpp"
#include "../IBuildingObject.hpp"
namespace DartInputs {
	class ClassicRoof: public IHouseRoof,public IBuildingObject {
	private:
		// No parameters
	public:
		ClassicRoof() {}

		const int getNbInputVertex() const {
			return 2;
		}
		const int getNbFaceProperties() const {
			return 4;
		}

		void operator =(const YAML::Node& node) {
			IBuildingObject::operator =(node);
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_CLASSICROOF_HPP_ */
