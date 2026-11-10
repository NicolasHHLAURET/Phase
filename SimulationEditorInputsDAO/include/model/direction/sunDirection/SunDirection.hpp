/*
 * SunDirection.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNDIRECTION_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNDIRECTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "ISunAngleDefinition.hpp"
#include "DateSunAngle.hpp"
#include "DownwardSunAngle.hpp"
#include "ViewingAngle.hpp"

#include "sunPenumbra/ISunPenumbra.hpp"
#include "sunPenumbra/IcospherePenumbra.hpp"
#include "sunPenumbra/MonteCarloPenumbra.hpp"
#include "sunPenumbra/NoSunPenumbra.hpp"

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SunDirection {
	private:
		bool cosineWeighted;
		shared_ptr<ISunAngleDefinition> sunAngleDefinition;
		shared_ptr<ISunPenumbra> sunPenumbra;

	public:
		SunDirection() :
				cosineWeighted(false) {}

		const bool & getCosineWeighted() const {
			return cosineWeighted;
		}
		const shared_ptr<ISunAngleDefinition> & getSunAngleDefinition() const {
			return sunAngleDefinition;
		}
		const shared_ptr<ISunPenumbra> & getSunPenumbra() const {
			return sunPenumbra;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				cosineWeighted = node.get("cosineWeighted").as<bool>();

				if (!node.get("sunAngleDefinition").IsNull()) {
					if (node.get("sunAngleDefinition").Tag() == "!direction.sunDirection.DateSunAngle") {
						sunAngleDefinition = shared_ptr<ISunAngleDefinition>(new DateSunAngle());
						static_cast<DateSunAngle &>(*sunAngleDefinition) = node.get("sunAngleDefinition");
					} else if (node.get("sunAngleDefinition").Tag() == "!direction.sunDirection.DownwardSunAngle") {
						sunAngleDefinition = shared_ptr<ISunAngleDefinition>(new DownwardSunAngle());
						static_cast<DownwardSunAngle &>(*sunAngleDefinition) = node.get("sunAngleDefinition");
					} else if (node.get("sunAngleDefinition").Tag() == "!direction.sunDirection.ViewingAngle") {
						sunAngleDefinition = shared_ptr<ISunAngleDefinition>(new ViewingAngle());
						static_cast<ViewingAngle &>(*sunAngleDefinition) = node.get("sunAngleDefinition");
					}
				}

				if (!node.get("sunPenumbra").IsNull()) {
					if (node.get("sunPenumbra").Tag() == "!direction.sunDirection.sunPenumbra.IcospherePenumbra") {
						sunPenumbra = shared_ptr<ISunPenumbra>(new IcospherePenumbra());
						static_cast<IcospherePenumbra &>(*sunPenumbra) = node.get("sunPenumbra");
					} else if (node.get("sunPenumbra").Tag() == "!direction.sunDirection.sunPenumbra.MonteCarloPenumbra") {
						sunPenumbra = shared_ptr<ISunPenumbra>(new MonteCarloPenumbra());
						static_cast<MonteCarloPenumbra &>(*sunPenumbra) = node.get("sunPenumbra");
					} else if (node.get("sunPenumbra").Tag() == "!direction.sunDirection.sunPenumbra.NoSunPenumbra") {
						sunPenumbra = shared_ptr<ISunPenumbra>(new NoSunPenumbra());
						static_cast<NoSunPenumbra &>(*sunPenumbra) = node.get("sunPenumbra");
					}
				}

			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_SUNDIRECTION_HPP_ */
