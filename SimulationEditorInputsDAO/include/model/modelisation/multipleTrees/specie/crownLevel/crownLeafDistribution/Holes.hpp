/*
 * Holes.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_HOLES_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_HOLES_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownLeafDistribution.hpp"
namespace DartInputs {
	class Holes: public ICrownLeafDistribution {
	private:
		double proportionOfFullCells;
		double a;
		double b;
	public:
		Holes() :
				proportionOfFullCells(0.70), a(0), b(1) {}

		const double & getProportionOfFullCells() const {
			return proportionOfFullCells;
		}
		const double & getA() const {
			return a;
		}
		const double & getB() const {
			return b;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				proportionOfFullCells = node.get("proportionOfFullCells").as<double>();
				a = node.get("a").as<double>();
				b = node.get("b").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_HOLES_HPP_ */
