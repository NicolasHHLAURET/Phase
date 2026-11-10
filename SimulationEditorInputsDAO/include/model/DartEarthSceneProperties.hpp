/*
 * DartEarthSceneProperties.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTEARTHSCENEPROPERTIES_HPP_
#define INCLUDE_MODEL_DARTEARTHSCENEPROPERTIES_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../yaml/YaMLReader.hpp"
#include "modelisation/EarthSceneProperties.hpp"
#include "modelisation/waterSurface/WaterSurfacesProperties.hpp"
namespace DartInputs {
	class DartEarthSceneProperties {
	private:
		shared_ptr<EarthSceneProperties> earthSceneProperties;
		shared_ptr<WaterSurfacesProperties> waterSurfacesProperties;

	public:
		DartEarthSceneProperties() {}

		const shared_ptr<EarthSceneProperties> & getEarthSceneProperties() const {
			return earthSceneProperties;
		}
		const shared_ptr<WaterSurfacesProperties> & getWaterSurfacesProperties() const {
			return waterSurfacesProperties;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			if (!yamlReader.getFirstNode("earthSceneProperties").IsNull()) {
				earthSceneProperties = shared_ptr<EarthSceneProperties>(new EarthSceneProperties());
				(*earthSceneProperties) = yamlReader.getFirstNode("earthSceneProperties");
			}
			if (!yamlReader.getFirstNode("waterSurfacesProperties").IsNull()) {
				waterSurfacesProperties = shared_ptr<WaterSurfacesProperties>(new WaterSurfacesProperties());
				(*waterSurfacesProperties) = yamlReader.getFirstNode("waterSurfacesProperties");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTEARTHSCENEPROPERTIES_HPP_ */
