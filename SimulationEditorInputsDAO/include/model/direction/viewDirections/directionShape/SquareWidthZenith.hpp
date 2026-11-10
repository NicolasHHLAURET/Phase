/*
 * SquareWidthZenith.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHZENITH_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHZENITH_HPP_

#include "ISquareWidthDefinition.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SquareWidthZenith: public ISquareWidthDefinition {
	private:
		double deltaZenith;
	public:
		SquareWidthZenith() :
				deltaZenith(2) {}

		const double & getDeltaZenith() const {
			return deltaZenith;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				deltaZenith = node.get("deltaZenith").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUAREWIDTHZENITH_HPP_ */
