/*
 * SubScene.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GEOMETRY_SUBSCENE_HPP_
#define INCLUDE_MODEL_OBJECT_GEOMETRY_SUBSCENE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../genericObject/DoubletDouble.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SubScene {
	private:
		double widthXOfSubScene;
		double widthYOfSubScene;
		shared_ptr<DoubletDouble> centerOfSubscene;

	public:
		SubScene() :
				widthXOfSubScene(0), widthYOfSubScene(0) {}

		const double & getWidthXOfSubScene() const {
			return widthXOfSubScene;
		}
		const double & getWidthYOfSubScene() const {
			return widthYOfSubScene;
		}
		const shared_ptr<DoubletDouble> & getCenterOfSubscene() const {
			return centerOfSubscene;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				widthXOfSubScene = node.get("widthXOfSubScene").as<double>();
				widthYOfSubScene = node.get("widthYOfSubScene").as<double>();
				if (!node.get("centerOfSubscene").IsNull()) {
					centerOfSubscene = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*centerOfSubscene) = node.get("centerOfSubscene");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_OBJECT_GEOMETRY_SUBSCENE_HPP_ */
