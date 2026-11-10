/*
 * AngleDescription.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_ANGLEDESCRIPTION_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_ANGLEDESCRIPTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../../object/geometry/AngleDirection.hpp"
#include "IDirectionDescription.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AngleDescription: public IDirectionDescription {
	private:
		shared_ptr<AngleDirection> angleDirection;

	public:
		AngleDescription() {}

		const shared_ptr<AngleDirection> & getAngleDirection() const {
			return angleDirection;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("angleDirection").IsNull()) {
					angleDirection = shared_ptr<AngleDirection>(new AngleDirection());
					(*angleDirection) = node.get("angleDirection");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_ANGLEDESCRIPTION_HPP_ */
