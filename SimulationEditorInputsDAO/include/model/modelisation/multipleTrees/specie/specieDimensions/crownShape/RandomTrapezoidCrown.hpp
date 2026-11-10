/*
 * RandomTrapezoidCrown.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRAPEZOIDCROWN_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRAPEZOIDCROWN_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownShapeRandomDimension.hpp"
namespace DartInputs {
	class RandomTrapezoidCrown: public ICrownShapeRandomDimension {
	private:
		double meanBaseLength;
		double stdDevBaseLength;

		double meanBaseWidth;
		double stdDevBaseWidth;

		double meanTopLength;
		double stdDevTopLength;

		double meanTopWidth;
		double stdDevTopWidth;

	public:
		RandomTrapezoidCrown() :
				meanBaseLength(4), stdDevBaseLength(0), meanBaseWidth(4), stdDevBaseWidth(0), meanTopLength(5), stdDevTopLength(0), meanTopWidth(4), stdDevTopWidth(
					0) {}

		const double & getMeanBaseLength() const {
			return meanBaseLength;
		}
		const double & getStdDevBaseLength() const {
			return stdDevBaseLength;
		}

		const double & getMeanBaseWidth() const {
			return meanBaseWidth;
		}
		const double & getStdDevBaseWidth() const {
			return stdDevBaseWidth;
		}

		const double & getMeanTopLength() const {
			return meanTopLength;
		}
		const double & getStdDevTopLength() const {
			return stdDevTopLength;
		}

		const double & getMeanTopWidth() const {
			return meanTopWidth;
		}
		const double & getStdDevTopWidth() const {
			return stdDevTopWidth;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				meanBaseLength = node.get("meanBaseLength").as<double>();
				stdDevBaseLength = node.get("stdDevBaseLength").as<double>();

				meanBaseWidth = node.get("meanBaseWidth").as<double>();
				stdDevBaseWidth = node.get("stdDevBaseWidth").as<double>();

				meanTopLength = node.get("meanTopLength").as<double>();
				stdDevTopLength = node.get("stdDevTopLength").as<double>();

				meanTopWidth = node.get("meanTopWidth").as<double>();
				stdDevTopWidth = node.get("stdDevTopWidth").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_CROWNSHAPE_RANDOMTRAPEZOIDCROWN_HPP_ */
