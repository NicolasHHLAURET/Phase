/*
 * ViewingAngle.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_VIEWINGANGLE_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_VIEWINGANGLE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ISunAngleDefinition.hpp"
#include "../../object/geometry/AngleDirection.hpp"
namespace DartInputs {
	class ViewingAngle: public ISunAngleDefinition {
	private:
		shared_ptr<AngleDirection> sunAngleUpward;
		int dayYear;

	public:
		ViewingAngle() : dayYear(-1) {}

		const shared_ptr<AngleDirection> & getSunAngleUpward() const {
			return sunAngleUpward;
		}
		const int & getDayYear() const {
			return dayYear;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("sunAngleUpward").IsNull()) {
					sunAngleUpward = shared_ptr<AngleDirection>(new AngleDirection());
					(*sunAngleUpward) = node.get("sunAngleUpward");
				}
				dayYear = node.get("dayYear").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_VIEWINGANGLE_HPP_ */
