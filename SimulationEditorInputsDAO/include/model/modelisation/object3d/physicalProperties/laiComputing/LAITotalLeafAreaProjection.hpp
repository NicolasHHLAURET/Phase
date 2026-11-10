/*
 * LAITotalLeafAreaProjection.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAITOTALLEAFAREAPROJECTION_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAITOTALLEAFAREAPROJECTION_HPP_

#include "yaml-cpp/yaml.h"
#include "ILAIComputing.hpp"
namespace DartInputs {
	class LAITotalLeafAreaProjection: public ILAIComputing {
	private:
		double totalSurface;
	public:
		LAITotalLeafAreaProjection() :
				totalSurface(0.05) {}

		const double & getTotalSurface() const {
			return totalSurface;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				totalSurface = node.get("totalSurface").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_LAICOMPUTING_LAITOTALLEAFAREAPROJECTION_HPP_ */
