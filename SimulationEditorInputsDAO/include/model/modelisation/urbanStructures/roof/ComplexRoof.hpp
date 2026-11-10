/*
 * ComplexRoof.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_COMPLEXROOF_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_COMPLEXROOF_HPP_

#include "yaml-cpp/yaml.h"
#include "IHouseRoof.hpp"
#include "../IBuildingObject.hpp"
namespace DartInputs {
	class ComplexRoof: public IHouseRoof,public IBuildingObject {
	private:
		// No parameters
	public:
		ComplexRoof() {}

		const int getNbInputVertex() const {
			return 4;
		}
		const int getNbFaceProperties() const {
			return 5;
		}

		void operator =(const YAML::Node& node) {
			IBuildingObject::operator =(node);
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_COMPLEXROOF_HPP_ */
