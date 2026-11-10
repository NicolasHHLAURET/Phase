/*
 * RectangleShape.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_RECTANGLESHAPE_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_RECTANGLESHAPE_HPP_

#include "IDirectionShape.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class RectangleShape: public IDirectionShape {
	private:
		double deltaZenith;
		double deltaAzimuth;
	public:
		RectangleShape() :
				deltaZenith(2), deltaAzimuth(2) {}

		const double & getDeltaZenith() const {
			return deltaZenith;
		}
		const double & getDeltaAzimuth() const {
			return deltaAzimuth;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				deltaZenith = node.get("deltaZenith").as<double>();
				deltaAzimuth = node.get("deltaAzimuth").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_RECTANGLESHAPE_HPP_ */
