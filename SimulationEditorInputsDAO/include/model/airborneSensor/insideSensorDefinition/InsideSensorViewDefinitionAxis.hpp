/*
 * InsideSensorViewDefinitionAxis.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONAXIS_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONAXIS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IInsideSensorViewDirection.hpp"
#include "yaml-cpp/yaml.h"
#include "../../object/genericObject/TripletDouble.hpp"
namespace DartInputs {
	class InsideSensorViewDefinitionAxis: public IInsideSensorViewDirection {
	private:
		shared_ptr<TripletDouble> directionAxisView;

	public:
		InsideSensorViewDefinitionAxis() {}

		const shared_ptr<TripletDouble> & getDirectionAxisView() const {
			return directionAxisView;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("directionAxisView").IsNull()) {
					if (node.get("directionAxisView").Tag() == "!object.genericObject.Triplet") {
						directionAxisView = shared_ptr<TripletDouble>(new TripletDouble());
						(*directionAxisView) = node.get("directionAxisView");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_INSIDESENSORDEFINITION_INSIDESENSORVIEWDEFINITIONAXIS_HPP_ */
