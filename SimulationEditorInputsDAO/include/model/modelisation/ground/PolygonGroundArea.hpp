/*
 * PolygonGroundArea.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_GROUND_POLYGONGROUNDAREA_HPP_
#define INCLUDE_MODEL_MODELISATION_GROUND_POLYGONGROUNDAREA_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IDartGroundArea.hpp"
#include "../../object/genericObject/DoubletDouble.hpp"
namespace DartInputs {
	class PolygonGroundArea: public IDartGroundArea {
	private:
		std::vector<shared_ptr<DoubletDouble> > groundPolygonVertex;
		double groundPolygonRotation;
	public:
		PolygonGroundArea() :
				groundPolygonRotation(0) {}

		const std::vector<shared_ptr<DoubletDouble> > & getGroundPolygonVertex() const {
			return groundPolygonVertex;
		}
		const double & getGroundPolygonRotation() const {
			return groundPolygonRotation;
		}

		void operator =(const YAML::Node& node) {
			IDartGroundArea::operator=(node);

			if (!node.IsNull()) {
				groundPolygonRotation = node.get("groundPolygonRotation").as<double>();

				groundPolygonVertex.clear();
				if (!node.get("groundPolygonVertex").IsNull() && node.get("groundPolygonVertex").IsSequence()) {
					for (YAML::const_iterator it = node.get("groundPolygonVertex").begin(); it != node.get("groundPolygonVertex").end(); ++it) {
						shared_ptr<DoubletDouble> doubletDouble = shared_ptr<DoubletDouble>(new DoubletDouble());
						(*doubletDouble) = (*it);
						groundPolygonVertex.push_back(doubletDouble);
					}
				}

			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_GROUND_POLYGONGROUNDAREA_HPP_ */
