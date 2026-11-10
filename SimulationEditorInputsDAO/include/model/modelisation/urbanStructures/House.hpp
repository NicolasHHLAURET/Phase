/*
 * House.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_HOUSE_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_HOUSE_HPP_

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

#include "roof/IHouseRoof.hpp"
#include "roof/ClassicRoof.hpp"
#include "roof/ComplexRoof.hpp"
#include "roof/NoRoof.hpp"
#include "roof/PlateRoof.hpp"
namespace DartInputs {
	class House: public IDartObject {
	private:
		shared_ptr<WallBase> baseHouse;
		shared_ptr<IHouseRoof> roof;
		double rotationAroundCenter;

	public:
		House() :
				rotationAroundCenter(0) {}

		const shared_ptr<WallBase> & getBaseHouse() const {
			return baseHouse;
		}
		const shared_ptr<IHouseRoof> & getRoof() const {
			return roof;
		}
		const double & getRotationAroundCenter() const {
			return rotationAroundCenter;
		}

		void operator =(const YAML::Node& node) {
			IDartObject::operator =(node);
			if (!node.IsNull()) {
				rotationAroundCenter = node.get("rotationAroundCenter").as<double>();
				if (!node.get("baseHouse").IsNull()) {
					baseHouse = shared_ptr<WallBase>(new WallBase());
					(*baseHouse) = node.get("baseHouse");
				}
				if (!node.get("roof").IsNull()) {
					if (node.get("roof").Tag() == "!modelisation.urbanStructures.roof.ClassicRoof") {
						roof = shared_ptr<IHouseRoof>(new ClassicRoof());
						static_cast<ClassicRoof &>(*roof) = node.get("roof");
					} else if (node.get("roof").Tag() == "!modelisation.urbanStructures.roof.ComplexRoof") {
						roof = shared_ptr<IHouseRoof>(new ComplexRoof());
						static_cast<ComplexRoof &>(*roof) = node.get("roof");
					} else if (node.get("roof").Tag() == "!modelisation.urbanStructures.roof.NoRoof") {
						roof = shared_ptr<IHouseRoof>(new NoRoof());
						static_cast<NoRoof &>(*roof) = node.get("roof");
					} else if (node.get("roof").Tag() == "!modelisation.urbanStructures.roof.PlateRoof") {
						roof = shared_ptr<IHouseRoof>(new PlateRoof());
						static_cast<PlateRoof &>(*roof) = node.get("roof");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_HOUSE_HPP_ */
