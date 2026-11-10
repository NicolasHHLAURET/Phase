/*
 * SmallWall.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_SMALLWALL_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_SMALLWALL_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "base/WallBase.hpp"
#include "../IDartObject.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SmallWall: public IDartObject {
	private:
		shared_ptr<WallBase> baseWall;
		double rotationAroundCenter;

	public:
		SmallWall() :
				rotationAroundCenter(0) {}

		const shared_ptr<WallBase> & getBaseWall() const {
			return baseWall;
		}
		const double & getRotationAroundCenter() {
			return rotationAroundCenter;
		}

		void operator =(const YAML::Node& node) {
			IDartObject::operator =(node);
			if (!node.IsNull()) {
				rotationAroundCenter = node.get("rotationAroundCenter").as<double>();
				if (!node.get("baseWall").IsNull()) {
					baseWall = shared_ptr<WallBase>(new WallBase());
					(*baseWall) = node.get("baseWall");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_SMALLWALL_HPP_ */
