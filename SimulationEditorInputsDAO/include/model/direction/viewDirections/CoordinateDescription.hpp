/*
 * CoordinateDescription.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_COORDINATEDESCRIPTION_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_COORDINATEDESCRIPTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../../object/geometry/EarthCoordinate.hpp"
#include "IDirectionDescription.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class CoordinateDescription: public IDirectionDescription {
	private:
		shared_ptr<EarthCoordinate> coordinates;

	public:
		CoordinateDescription() {}

		const shared_ptr<EarthCoordinate> & getCoordinates() const {
			return coordinates;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("coordinates").IsNull()) {
					coordinates = shared_ptr<EarthCoordinate>(new EarthCoordinate());
					(*coordinates) = node.get("coordinates");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_COORDINATEDESCRIPTION_HPP_ */
