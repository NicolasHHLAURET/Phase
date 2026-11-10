/*
 * LeavesAndHoles.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLES_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLES_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownLeafDistribution.hpp"
namespace DartInputs {
	class LeavesAndHoles: public ICrownLeafDistribution {
	private:
		double proportionOfFullCells;
		double a;
		double b;
		double alpha;
		double beta;
		double gamma;
		double kappa;
	public:
		LeavesAndHoles() :
				proportionOfFullCells(0.70), a(0), b(1), alpha(0.10), beta(0.3), gamma(0.8), kappa(1) {}

		const double & getProportionOfFullCells() const {
			return proportionOfFullCells;
		}
		const double & getA() const {
			return a;
		}
		const double & getB() const {
			return b;
		}
		const double & getAlpha() const {
			return alpha;
		}
		const double & getBeta() const {
			return beta;
		}
		const double & getGamma() const {
			return gamma;
		}
		const double & getKappa() const {
			return kappa;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				proportionOfFullCells = node.get("proportionOfFullCells").as<double>();
				a = node.get("a").as<double>();
				b = node.get("b").as<double>();
				alpha = node.get("alpha").as<double>();
				beta = node.get("beta").as<double>();
				gamma = node.get("gamma").as<double>();
				kappa = node.get("kappa").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVESANDHOLES_HPP_ */
