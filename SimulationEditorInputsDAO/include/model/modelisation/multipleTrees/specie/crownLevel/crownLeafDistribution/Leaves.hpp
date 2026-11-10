/*
 * Leaves.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVES_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVES_HPP_

#include "yaml-cpp/yaml.h"
#include "ICrownLeafDistribution.hpp"
namespace DartInputs {
	class Leaves: public ICrownLeafDistribution {
	private:
		double alpha;
		double beta;
		double gamma;
		double kappa;
	public:
		Leaves() :
				alpha(0.10), beta(0.3), gamma(0.8), kappa(1) {}

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
				alpha = node.get("alpha").as<double>();
				beta = node.get("beta").as<double>();
				gamma = node.get("gamma").as<double>();
				kappa = node.get("kappa").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_CROWNLEVEL_CROWNLEAFDISTRIBUTION_LEAVES_HPP_ */
