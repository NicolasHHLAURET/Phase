/*
 * FluxTrackingAdvancedParameters.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGADVANCEDPARAMETERS_HPP_
#define INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGADVANCEDPARAMETERS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluxTrackingAdvancedParameters {
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
		double distanceBetweenIlluminationSubCenters;

	public:
		FluxTrackingAdvancedParameters() :
				propagationThreshold(0.0000001), sceneAlbedoOrThermalExitanceThreshold(0.0000001), numberOfCentroidPerCellsPerAxis(2), nbIllumSourcesPerCellsPerAxis(
					8), illumSourcesSpatialDistribution(SEMI_RANDOM), distanceBetweenIlluminationSubCenters(0.1) {}

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

		const double & getDistanceBetweenIlluminationSubCenters() const {
			return distanceBetweenIlluminationSubCenters;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				propagationThreshold = node.get("propagationThreshold").as<double>();
				sceneAlbedoOrThermalExitanceThreshold = node.get("sceneAlbedoOrThermalExitanceThreshold").as<double>();
				numberOfCentroidPerCellsPerAxis = node.get("numberOfCentroidPerCellsPerAxis").as<int>();
				nbIllumSourcesPerCellsPerAxis = node.get("nbIllumSourcesPerCellsPerAxis").as<int>();
				distanceBetweenIlluminationSubCenters = node.get("distanceBetweenIlluminationSubCenters").as<double>();
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
#endif /* INCLUDE_MODEL_RADIATION_FLUXTRACKING_FLUXTRACKINGADVANCEDPARAMETERS_HPP_ */
