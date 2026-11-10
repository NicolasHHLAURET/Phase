/*
 * InsideSensorViewDefinitionAngle.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONANGLE_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONANGLE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IInsideSensorViewDirection.hpp"
#include "yaml-cpp/yaml.h"
#include "../../object/geometry/AngleDirection.hpp"
namespace DartInputs {
	class InsideSensorViewDefinitionAngle: public IInsideSensorViewDirection {
	private:
		shared_ptr<AngleDirection> viewAngle;

	public:
		InsideSensorViewDefinitionAngle() {}

		const shared_ptr<AngleDirection> & getViewAngle() const {
			return viewAngle;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("viewAngle").IsNull()) {
					if (node.get("viewAngle").Tag() == "!object.geometry.AngleDirection") {
						viewAngle = shared_ptr<AngleDirection>(new AngleDirection());
						(*viewAngle) = node.get("viewAngle");
					}
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONANGLE_HPP_ */
