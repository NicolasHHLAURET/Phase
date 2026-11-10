/*
 * DartSceneObjects.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTSCENEOBJECTS_HPP_
#define INCLUDE_MODEL_DARTSCENEOBJECTS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "../yaml/YaMLReader.hpp"

#include "modelisation/plots/extraPlots/ExtraPlots.hpp"

#include "modelisation/objectColor/DartColor.hpp"

#include "modelisation/IDartObject.hpp"
#include "modelisation/multipleTrees/MultipleTrees.hpp"
#include "modelisation/object3d/Object3D.hpp"
#include "modelisation/plots/Plot.hpp"
#include "modelisation/urbanStructures/House.hpp"
#include "modelisation/urbanStructures/SmallWall.hpp"
#include "modelisation/ground/IDartGroundArea.hpp"
#include "modelisation/ground/EllipseGroundArea.hpp"
#include "modelisation/ground/PolygonGroundArea.hpp"
#include "modelisation/ground/RectangleGroundArea.hpp"
namespace DartInputs {
	class DartSceneObjects {
	private:
		std::vector<shared_ptr<IDartObject> > earthSceneObjects;
		std::vector<shared_ptr<DartColor> > dartColor;
		std::vector<shared_ptr<IDartGroundArea> > earthGroundAreas;
		shared_ptr<ExtraPlots> extraPlots;

	public:
		DartSceneObjects() {}

		const std::vector<shared_ptr<IDartObject> > & getEarthSceneObjects() const {
			return earthSceneObjects;
		}
		const std::vector<shared_ptr<DartColor> > & getDartColor() const {
			return dartColor;
		}
		const std::vector<shared_ptr<IDartGroundArea> > & getEarthGroundAreas() const {
			return earthGroundAreas;
		}
		const shared_ptr<ExtraPlots> & getExtraPlots() const {
			return extraPlots;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			if (!yamlReader.getFirstNode("extraPlots").IsNull()) {
				extraPlots = shared_ptr<ExtraPlots>(new ExtraPlots());
				(*extraPlots) = yamlReader.getFirstNode("extraPlots");
			}

			dartColor.clear();
			if (!yamlReader.getFirstNode("dartColor").IsNull() && yamlReader.getFirstNode("dartColor").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("dartColor").begin(); it != yamlReader.getFirstNode("dartColor").end(); ++it) {
					shared_ptr<DartColor> dartColo = shared_ptr<DartColor>(new DartColor());
					(*dartColo) = (*it);
					dartColor.push_back(dartColo);
				}
			}

			earthGroundAreas.clear();
			if (!yamlReader.getFirstNode("earthGroundAreas").IsNull() && yamlReader.getFirstNode("earthGroundAreas").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("earthGroundAreas").begin();
						it != yamlReader.getFirstNode("earthGroundAreas").end(); ++it) {
					shared_ptr<IDartGroundArea> earthGroundAre;
					if (it->Tag() == "!modelisation.ground.EllipseGroundArea") {
						earthGroundAre = shared_ptr<IDartGroundArea>(new EllipseGroundArea());
						static_cast<EllipseGroundArea &>(*earthGroundAre) = (*it);
					} else if (it->Tag() == "!modelisation.ground.PolygonGroundArea") {
						earthGroundAre = shared_ptr<IDartGroundArea>(new PolygonGroundArea());
						static_cast<PolygonGroundArea &>(*earthGroundAre) = (*it);
					} else if (it->Tag() == "!modelisation.ground.RectangleGroundArea") {
						earthGroundAre = shared_ptr<IDartGroundArea>(new RectangleGroundArea());
						static_cast<RectangleGroundArea &>(*earthGroundAre) = (*it);
					} else {
						break;
					}
					earthGroundAreas.push_back(earthGroundAre);
				}
			}

			earthSceneObjects.clear();
			if (!yamlReader.getFirstNode("earthSceneObjects").IsNull() && yamlReader.getFirstNode("earthSceneObjects").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("earthSceneObjects").begin();
						it != yamlReader.getFirstNode("earthSceneObjects").end(); ++it) {
					shared_ptr<IDartObject> object;
					if (it->Tag() == "!modelisation.multipleTrees.MultipleTrees") {
						object = shared_ptr<IDartObject>(new MultipleTrees());
						static_cast<MultipleTrees &>(*object) = (*it);
					} else if (it->Tag() == "!modelisation.object3d.Object3D") {
						object = shared_ptr<IDartObject>(new Object3D());
						static_cast<Object3D &>(*object) = (*it);
					} else if (it->Tag() == "!modelisation.plots.Plot") {
						object = shared_ptr<IDartObject>(new Plot());
						static_cast<Plot &>(*object) = (*it);
					} else if (it->Tag() == "!modelisation.urbanStructures.House") {
						object = shared_ptr<IDartObject>(new House());
						static_cast<House &>(*object) = (*it);
					} else if (it->Tag() == "!modelisation.urbanStructures.SmallWall") {
						object = shared_ptr<IDartObject>(new SmallWall());
						static_cast<SmallWall &>(*object) = (*it);
					} else {
						break;
					}
					earthSceneObjects.push_back(object);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTSCENEOBJECTS_HPP_ */
