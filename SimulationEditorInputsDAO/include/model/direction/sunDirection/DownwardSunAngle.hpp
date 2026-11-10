/*
 * DownwardSunAngle.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DOWNWARDSUNANGLE_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DOWNWARDSUNANGLE_HPP_

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
	class DownwardSunAngle: public ISunAngleDefinition {
	private:
		shared_ptr<AngleDirection> sunAngleDownward;
		int dayYear;

	public:
		DownwardSunAngle() : dayYear(-1) {}

		const shared_ptr<AngleDirection> & getSunAngleDownward() const {
			return sunAngleDownward;
		}
		const int & getDayYear() const {
			return dayYear;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("sunAngleDownward").IsNull()) {
					sunAngleDownward = shared_ptr<AngleDirection>(new AngleDirection());
					(*sunAngleDownward) = node.get("sunAngleDownward");
				}
				dayYear = node.get("dayYear").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DOWNWARDSUNANGLE_HPP_ */
