/*
 * EarthSceneProperties.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_EARTHSCENEPROPERTIES_HPP_
#define INCLUDE_MODEL_MODELISATION_EARTHSCENEPROPERTIES_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"

#include "../object/geometry/EarthCoordinate.hpp"
#include "../object/geometry/SubScene.hpp"
#include "../object/genericObject/DoubletDouble.hpp"
#include "topography/ITopography.hpp"
#include "topography/NoTopography.hpp"
#include "topography/TopographyFile.hpp"
#include "topography/TopographyBuilder.hpp"
namespace DartInputs {
	class EarthSceneProperties {
	public:
		enum PeriodicScene {
			REPETITIVE, ISOLATED, INFINITE_SLOPE
		};

	private:
		int earthSceneOpticalPropertyId;
		int earthSceneThermalPropertyId;

		shared_ptr<EarthCoordinate> earthSceneCoordinates;
		shared_ptr<ITopography> topography;
		shared_ptr<SubScene> earthSubScene;
		PeriodicScene periodicScene;
		shared_ptr<DoubletDouble> earthSceneDimension;
		shared_ptr<DoubletDouble> earthSceneCellDimension;
		shared_ptr<int> forcedRandomOperationSeed; //!\ Il peut être null  si non défini

	public:
		EarthSceneProperties() :
				earthSceneOpticalPropertyId(1), earthSceneThermalPropertyId(1), periodicScene(REPETITIVE) {}
		const PeriodicScene & getPeriodicScene() const {
			return periodicScene;
		}
		const shared_ptr<SubScene> & getEarthSubScene() const {
			return earthSubScene;
		}
		const shared_ptr<EarthCoordinate> & getEarthSceneCoordinates() const {
			return earthSceneCoordinates;
		}
		const shared_ptr<ITopography> & getTopography() const {
			return topography;
		}
		const shared_ptr<DoubletDouble> & getEarthSceneDimension() const {
			return earthSceneDimension;
		}
		const shared_ptr<DoubletDouble> & getEarthSceneCellDimension() const {
			return earthSceneCellDimension;
		}
		const int & getEarthSceneOpticalPropertyId() const {
			return earthSceneOpticalPropertyId;
		}
		const int & getEarthSceneThermalPropertyId() const {
			return earthSceneThermalPropertyId;
		}
		const shared_ptr<int> & getForcedRandomOperationSeed() const {
			return forcedRandomOperationSeed;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				earthSceneOpticalPropertyId = node.get("earthSceneOpticalPropertyId").as<int>();
				earthSceneThermalPropertyId = node.get("earthSceneThermalPropertyId").as<int>();
			}
			if (!node.get("forcedRandomOperationSeed").IsNull()) {
				forcedRandomOperationSeed = shared_ptr<int>(new int());
				(*forcedRandomOperationSeed) = node.get("forcedRandomOperationSeed").as<int>();
			}
			if (node.get("periodicScene").as<std::string>() == "REPETITIVE") {
				periodicScene = REPETITIVE;
			} else if (node.get("periodicScene").as<std::string>() == "ISOLATED") {
				periodicScene = ISOLATED;
			} else {
				periodicScene = INFINITE_SLOPE;
			}

			if (!node.get("earthSceneCoordinates").IsNull()) {
				earthSceneCoordinates = shared_ptr<EarthCoordinate>(new EarthCoordinate());
				(*earthSceneCoordinates) = node.get("earthSceneCoordinates");
			}
			if (!node.get("topography").IsNull()) {
				if (node.get("topography").Tag() == "!modelisation.topography.NoTopography") {
					topography = shared_ptr<ITopography>(new NoTopography());
					static_cast<NoTopography &>(*topography) = node.get("topography");
				} else if (node.get("topography").Tag() == "!modelisation.topography.TopographyFile") {
					topography = shared_ptr<ITopography>(new TopographyFile());
					static_cast<TopographyFile &>(*topography) = node.get("topography");
				} else if (node.get("topography").Tag() == "!modelisation.topography.TopographyBuilder") {
					topography = shared_ptr<ITopography>(new TopographyBuilder());
					static_cast<TopographyBuilder &>(*topography) = node.get("topography");
				}
			}

			if (!node.get("earthSubScene").IsNull()) {
				earthSubScene = shared_ptr<SubScene>(new SubScene());
				(*earthSubScene) = node.get("earthSubScene");
			}
			if (!node.get("earthSceneDimension").IsNull()) {
				earthSceneDimension = shared_ptr<DoubletDouble>(new DoubletDouble());
				(*earthSceneDimension) = node.get("earthSceneDimension");
			}
			if (!node.get("earthSceneCellDimension").IsNull()) {
				earthSceneCellDimension = shared_ptr<DoubletDouble>(new DoubletDouble());
				(*earthSceneCellDimension) = node.get("earthSceneCellDimension");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_EARTHSCENEPROPERTIES_HPP_ */
