/*
 * MonteCarloAdvancedParameters.hpp
 *
 *  Created on: 21 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOADVANCEDPARAMETERS_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOADVANCEDPARAMETERS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class MonteCarloAdvancedParameters {
	public:
		enum SpatialDistributionSources {
			REGULAR, RANDOM, SEMI_RANDOM, HELIX
		};

	private:
		double propagationThreshold;
		double sceneAlbedoOrThermalExitanceThreshold;
		int numberOfCentroidPerCellsPerAxis;
		int nbIllumSourcesPerCellsPerAxis;
		SpatialDistributionSources illumSourcesSpatialDistribution;

	public:
		MonteCarloAdvancedParameters() :
				propagationThreshold(0.0000001), sceneAlbedoOrThermalExitanceThreshold(0.0000001), numberOfCentroidPerCellsPerAxis(2), nbIllumSourcesPerCellsPerAxis(
					8), illumSourcesSpatialDistribution(SEMI_RANDOM) {}

		const double & getPropagationThreshold() const {
			return propagationThreshold;
		}

		const double & getSceneAlbedoOrThermalExitanceThreshold() const {
			return sceneAlbedoOrThermalExitanceThreshold;
		}

		const int & getNumberOfCentroidPerCellsPerAxis() const {
			return numberOfCentroidPerCellsPerAxis;
		}

		const int & getNbIllumSourcesPerCellsPerAxis() const {
			return nbIllumSourcesPerCellsPerAxis;
		}

		const SpatialDistributionSources & getIllumSourcesSpatialDistribution() const {
			return illumSourcesSpatialDistribution;
		}
		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				propagationThreshold = node.get("propagationThreshold").as<double>();
				sceneAlbedoOrThermalExitanceThreshold = node.get("sceneAlbedoOrThermalExitanceThreshold").as<double>();
				numberOfCentroidPerCellsPerAxis = node.get("numberOfCentroidPerCellsPerAxis").as<int>();
				nbIllumSourcesPerCellsPerAxis = node.get("nbIllumSourcesPerCellsPerAxis").as<int>();

				if (node.get("illumSourcesSpatialDistribution").as<std::string>() == "REGULAR") {
					illumSourcesSpatialDistribution = REGULAR;
				} else if (node.get("illumSourcesSpatialDistribution").as<std::string>() == "RANDOM") {
					illumSourcesSpatialDistribution = RANDOM;
				} else if (node.get("illumSourcesSpatialDistribution").as<std::string>() == "SEMI_RANDOM") {
					illumSourcesSpatialDistribution = SEMI_RANDOM;
				} else {
					illumSourcesSpatialDistribution = HELIX;
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_MONTECARLO_MONTECARLOADVANCEDPARAMETERS_HPP_ */
