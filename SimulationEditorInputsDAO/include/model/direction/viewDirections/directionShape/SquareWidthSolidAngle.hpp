/*
 * SquareWidthSolidAngle.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHSOLIDANGLE_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHSOLIDANGLE_HPP_

#include "ISquareWidthDefinition.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SquareWidthSolidAngle: public ISquareWidthDefinition {
	private:
		double deltaSolidAngle;
	public:
		SquareWidthSolidAngle() :
				deltaSolidAngle(2) {}

		const double & getDeltaSolidAngle() const {
			return deltaSolidAngle;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				deltaSolidAngle = node.get("deltaSolidAngle").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHSOLIDANGLE_HPP_ */
