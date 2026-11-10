/*
 * SquareShape.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUARESHAPE_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUARESHAPE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IDirectionShape.hpp"
#include "ISquareWidthDefinition.hpp"
#include "SquareWidthAzimuth.hpp"
#include "SquareWidthSolidAngle.hpp"
#include "SquareWidthZenith.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SquareShape: public IDirectionShape {
	private:
		shared_ptr<ISquareWidthDefinition> squareDefinition;

	public:
		SquareShape() {}

		const shared_ptr<ISquareWidthDefinition> & getSquareDefinition() const {
			return squareDefinition;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("squareDefinition").IsNull()) {
					if (node.get("squareDefinition").Tag() == "!direction.viewDirections.directionShape.SquareWidthAzimuth") {
						squareDefinition = shared_ptr<ISquareWidthDefinition>(new SquareWidthAzimuth());
						static_cast<SquareWidthAzimuth &>(*squareDefinition) = node.get("squareDefinition");
					} else if (node.get("squareDefinition").Tag() == "!direction.viewDirections.directionShape.SquareWidthSolidAngle") {
						squareDefinition = shared_ptr<ISquareWidthDefinition>(new SquareWidthSolidAngle());
						static_cast<SquareWidthSolidAngle &>(*squareDefinition) = node.get("squareDefinition");
					} else if (node.get("squareDefinition").Tag() == "!direction.viewDirections.directionShape.SquareWidthZenith") {
						squareDefinition = shared_ptr<ISquareWidthDefinition>(new SquareWidthZenith());
						static_cast<SquareWidthZenith &>(*squareDefinition) = node.get("squareDefinition");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_DIRECTIONSHAPE_SQUARESHAPE_HPP_ */
