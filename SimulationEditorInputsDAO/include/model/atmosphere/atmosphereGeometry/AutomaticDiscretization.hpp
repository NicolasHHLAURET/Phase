/*
 * AutomaticDiscretization.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_AUTOMATICDISCRETIZATION_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_AUTOMATICDISCRETIZATION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../../object/genericObject/DoubletDouble.hpp"
#include "IDiscretizationMode.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AutomaticDiscretization: public IDiscretizationMode {
	private:
		double TOAReflectanceRelativeError;
		double midAtmospherePercentInBackscatteredSignal;
		shared_ptr<DoubletDouble> midAtmosphereCellSize;
	public:
		AutomaticDiscretization() :
				TOAReflectanceRelativeError(0.003), midAtmospherePercentInBackscatteredSignal(0.95) {}

		const double & getTOAReflectanceRelativeError() const {
			return TOAReflectanceRelativeError;
		}
		const double & getMidAtmospherePercentInBackscatteredSignal() const {
			return midAtmospherePercentInBackscatteredSignal;
		}
		const shared_ptr<DoubletDouble> & getMidAtmosphereCellSize() const {
			return midAtmosphereCellSize;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				TOAReflectanceRelativeError = node.get("TOAReflectanceRelativeError").as<double>();
				midAtmospherePercentInBackscatteredSignal = node.get("midAtmospherePercentInBackscatteredSignal").as<double>();
				if (!node.get("midAtmosphereCellSize").IsNull()) {
					midAtmosphereCellSize = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*midAtmosphereCellSize) = node.get("midAtmosphereCellSize");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_AUTOMATICDISCRETIZATION_HPP_ */
