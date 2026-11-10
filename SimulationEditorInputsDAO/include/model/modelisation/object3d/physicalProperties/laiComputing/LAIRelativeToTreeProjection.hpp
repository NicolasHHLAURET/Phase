/*
 * LAIRelativeToTreeProjection.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAIRELATIVETOTREEPROJECTION_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAIRELATIVETOTREEPROJECTION_HPP_

#include "yaml-cpp/yaml.h"
#include "ILAIComputing.hpp"
namespace DartInputs {
	class LAIRelativeToTreeProjection: public ILAIComputing {
	private:
		double lai;
	public:
		LAIRelativeToTreeProjection() :
				lai(0.05) {}

		const double & getLai() const {
			return lai;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				lai = node.get("lai").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAIRELATIVETOTREEPROJECTION_HPP_ */
