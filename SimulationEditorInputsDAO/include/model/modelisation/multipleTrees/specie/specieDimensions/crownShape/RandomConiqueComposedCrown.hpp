/*
 * RandomConiqueComposedCrown.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCONIQUECOMPOSEDCROWN_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCONIQUECOMPOSEDCROWN_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownShapeRandomDimension.hpp"
namespace DartInputs {
	class RandomConiqueComposedCrown: public ICrownShapeRandomDimension {
	private:
		double meanBottomRadius;
		double stdDevBottomRadius;

		double meanTopRadius;
		double stdDevTopRadius;

		double meanCylinderHeight;
		double stdDevCylinderHeight;

	public:
		RandomConiqueComposedCrown() :
				meanBottomRadius(4), stdDevBottomRadius(0), meanTopRadius(2), stdDevTopRadius(0), meanCylinderHeight(3), stdDevCylinderHeight(0) {}

		const double & getMeanBottomRadius() const {
			return meanBottomRadius;
		}
		const double & getStdDevBottomRadius() const {
			return stdDevBottomRadius;
		}

		const double & getMeanTopRadius() const {
			return meanTopRadius;
		}
		const double & getStdDevTopRadius() const {
			return stdDevTopRadius;
		}

		const double & getMeanCylinderHeight() const {
			return meanCylinderHeight;
		}
		const double & getStdDevCylinderHeight() const {
			return stdDevCylinderHeight;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				meanBottomRadius = node.get("meanBottomRadius").as<double>();
				stdDevBottomRadius = node.get("stdDevBottomRadius").as<double>();

				meanTopRadius = node.get("meanTopRadius").as<double>();
				stdDevTopRadius = node.get("stdDevTopRadius").as<double>();

				meanCylinderHeight = node.get("meanCylinderHeight").as<double>();
				stdDevCylinderHeight = node.get("stdDevCylinderHeight").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCONIQUECOMPOSEDCROWN_HPP_ */
