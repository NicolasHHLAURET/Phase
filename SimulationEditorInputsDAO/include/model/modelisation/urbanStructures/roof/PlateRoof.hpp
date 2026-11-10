/*
 * PlateRoof.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_PLATEROOF_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_PLATEROOF_HPP_

#include "yaml-cpp/yaml.h"
#include "IHouseRoof.hpp"
#include "../IBuildingObject.hpp"
namespace DartInputs {
	class PlateRoof: public IHouseRoof,public IBuildingObject {
	private:
		double height;
	public:
		PlateRoof() :
				height(0.05) {}

		const double & getHeight() const {
			return height;
		}

		const int getNbInputVertex() const {
			return 0;
		}
		const int getNbFaceProperties() const {
			return 5;
		}

		void operator =(const YAML::Node& node) {
			IBuildingObject::operator =(node);
			if (!node.IsNull()) {
				height = node.get("height").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_ROOF_PLATEROOF_HPP_ */
