/*
 * RectangleGroundArea.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_GROUND_RECTANGLEGROUNDAREA_HPP_
#define INCLUDE_MODEL_MODELISATION_GROUND_RECTANGLEGROUNDAREA_HPP_

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
	class RectangleGroundArea: public IDartGroundArea {
	private:
		shared_ptr<DoubletDouble> groundRectangleUpperLeftCorner;
		shared_ptr<DoubletDouble> groundRectangleSize;
		double groundRectangleRotation;
	public:
		RectangleGroundArea() :
				groundRectangleRotation(0) {}

		const shared_ptr<DoubletDouble> & getGroundRectangleUpperLeftCorner() const {
			return groundRectangleUpperLeftCorner;
		}
		const shared_ptr<DoubletDouble> & getGroundRectangleSize() const {
			return groundRectangleSize;
		}
		const double & getGroundRectangleRotation() const {
			return groundRectangleRotation;
		}

		void operator =(const YAML::Node& node) {
			IDartGroundArea::operator=(node);
			if (!node.IsNull()) {
				groundRectangleRotation = node.get("groundRectangleRotation").as<double>();
				if (!node.get("groundRectangleUpperLeftCorner").IsNull()) {
					groundRectangleUpperLeftCorner = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*groundRectangleUpperLeftCorner) = node.get("groundRectangleUpperLeftCorner");
				}
				if (!node.get("groundRectangleSize").IsNull()) {
					groundRectangleSize = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*groundRectangleSize) = node.get("groundRectangleSize");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_GROUND_RECTANGLEGROUNDAREA_HPP_ */
