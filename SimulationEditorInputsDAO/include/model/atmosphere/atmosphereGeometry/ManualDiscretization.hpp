/*
 * ManualDiscretization.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_MANUALDISCRETIZATION_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_MANUALDISCRETIZATION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../../object/genericObject/TripletDouble.hpp"

#include "IDiscretizationMode.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ManualDiscretization: public IDiscretizationMode {
	private:
		shared_ptr<TripletDouble> midAtmosphereCellDimensions;
		double midAtmosphereTotalThickness;

		double hightAtmosphereTotalThickness;
		double thicknessOfEachHightAtmosphereLayer;
	public:
		ManualDiscretization() :
				midAtmosphereTotalThickness(4000), hightAtmosphereTotalThickness(8000), thicknessOfEachHightAtmosphereLayer(2000) {}

		const shared_ptr<TripletDouble> getMidAtmosphereCellDimensions() const {
			return midAtmosphereCellDimensions;
		}
		const double & getMidAtmosphereTotalThickness() const {
			return midAtmosphereTotalThickness;
		}
		const double & getHightAtmosphereTotalThickness() const {
			return hightAtmosphereTotalThickness;
		}
		const double & getThicknessOfEachHightAtmosphereLayer() const {
			return thicknessOfEachHightAtmosphereLayer;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				midAtmosphereTotalThickness = node.get("midAtmosphereTotalThickness").as<double>();
				hightAtmosphereTotalThickness = node.get("hightAtmosphereTotalThickness").as<double>();
				thicknessOfEachHightAtmosphereLayer = node.get("thicknessOfEachHightAtmosphereLayer").as<double>();
				if (!node.get("midAtmosphereCellDimensions").IsNull()) {
					midAtmosphereCellDimensions = shared_ptr<TripletDouble>(new TripletDouble());
					(*midAtmosphereCellDimensions) = node.get("midAtmosphereCellDimensions");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_ATMOSPHEREGEOMETRY_MANUALDISCRETIZATION_HPP_ */
