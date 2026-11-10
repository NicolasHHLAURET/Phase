/*
 * SquareWidthAzimuth.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHAZIMUTH_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHAZIMUTH_HPP_

#include "ISquareWidthDefinition.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SquareWidthAzimuth: public ISquareWidthDefinition {
	private:
		double deltaAzimuth;
	public:
		SquareWidthAzimuth() :
				deltaAzimuth(2) {}

		const double & getDeltaAzimuth() const {
			return deltaAzimuth;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				deltaAzimuth = node.get("deltaAzimuth").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHAZIMUTH_HPP_ */
