/*
 * RandomComposedEllipsoidCrown.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCOMPOSEDELLIPSOIDCROWN_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCOMPOSEDELLIPSOIDCROWN_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownShapeRandomDimension.hpp"
namespace DartInputs {
	class RandomComposedEllipsoidCrown: public ICrownShapeRandomDimension {
	private:
		double meanFirstAxis;
		double stdDevFirstAxis;

		double meanSecondAxis;
		double stdDevSecondAxis;

		double meanIntermediateHeight;
		double stdDevIntermediateHeight;

	public:
		RandomComposedEllipsoidCrown() :
				meanFirstAxis(4), stdDevFirstAxis(0), meanSecondAxis(4), stdDevSecondAxis(0), meanIntermediateHeight(2), stdDevIntermediateHeight(0) {}

		const double & getMeanFirstAxis() const {
			return meanFirstAxis;
		}
		const double & getStdDevFirstAxis() const {
			return stdDevFirstAxis;
		}

		const double & getMeanSecondAxis() const {
			return meanSecondAxis;
		}
		const double & getStdDevSecondAxis() const {
			return stdDevSecondAxis;
		}

		const double & getMeanIntermediateHeight() const {
			return meanIntermediateHeight;
		}
		const double & getStdDevIntermediateHeight() const {
			return stdDevIntermediateHeight;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				meanFirstAxis = node.get("meanFirstAxis").as<double>();
				stdDevFirstAxis = node.get("stdDevFirstAxis").as<double>();

				meanSecondAxis = node.get("meanSecondAxis").as<double>();
				stdDevSecondAxis = node.get("stdDevSecondAxis").as<double>();

				meanIntermediateHeight = node.get("meanIntermediateHeight").as<double>();
				stdDevIntermediateHeight = node.get("stdDevIntermediateHeight").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMCOMPOSEDELLIPSOIDCROWN_HPP_ */
