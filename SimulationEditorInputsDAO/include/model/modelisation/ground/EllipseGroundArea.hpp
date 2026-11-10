/*
 * EllipseGroundArea.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_GROUND_ELLIPSEGROUNDAREA_HPP_
#define INCLUDE_MODEL_MODELISATION_GROUND_ELLIPSEGROUNDAREA_HPP_

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
	class EllipseGroundArea: public IDartGroundArea {
	private:
		shared_ptr<DoubletDouble> groundEllipseCenter;
		shared_ptr<DoubletDouble> groundEllipseRadius;
		double groundEllipseRotation;
	public:
		EllipseGroundArea() :
				groundEllipseRotation(0) {}

		const shared_ptr<DoubletDouble> & getGroundEllipseCenter() const {
			return groundEllipseCenter;
		}
		const shared_ptr<DoubletDouble> & getGroundEllipseRadius() const {
			return groundEllipseRadius;
		}
		const double & getGroundEllipseRotation() const {
			return groundEllipseRotation;
		}

		void operator =(const YAML::Node& node) {
			IDartGroundArea::operator=(node);
			if (!node.IsNull()) {
				groundEllipseRotation = node.get("groundEllipseRotation").as<double>();
				if (!node.get("groundEllipseCenter").IsNull()) {
					groundEllipseCenter = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*groundEllipseCenter) = node.get("groundEllipseCenter");
				}
				if (!node.get("groundEllipseRadius").IsNull()) {
					groundEllipseRadius = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*groundEllipseRadius) = node.get("groundEllipseRadius");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_GROUND_ELLIPSEGROUNDAREA_HPP_ */
