/*
 * RandomTruncatedConeCrown.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRUNCATEDCONECROWN_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRUNCATEDCONECROWN_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownShapeRandomDimension.hpp"
namespace DartInputs {
	class RandomTruncatedConeCrown: public ICrownShapeRandomDimension {
	private:
		double meanBottomRadius;
		double stdDevBottomRadius;

		double meanTopRadius;
		double stdDevTopRadius;

	public:
		RandomTruncatedConeCrown() :
				meanBottomRadius(4), stdDevBottomRadius(0), meanTopRadius(2), stdDevTopRadius(0) {}

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

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				meanBottomRadius = node.get("meanBottomRadius").as<double>();
				stdDevBottomRadius = node.get("stdDevBottomRadius").as<double>();

				meanTopRadius = node.get("meanTopRadius").as<double>();
				stdDevTopRadius = node.get("stdDevTopRadius").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRUNCATEDCONECROWN_HPP_ */
