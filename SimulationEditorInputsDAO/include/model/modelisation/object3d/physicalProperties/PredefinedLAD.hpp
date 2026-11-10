/*
 * PredefinedLAD.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_PREDEFINEDLAD_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_PREDEFINEDLAD_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ILADConversion.hpp"

#include "laiComputing/ILAIComputing.hpp"
#include "laiComputing/LAICalculated.hpp"
#include "laiComputing/LAIRelativeToTreeProjection.hpp"
#include "laiComputing/LAITotalLeafAreaProjection.hpp"
namespace DartInputs {
	class PredefinedLAD: public ILADConversion {
	private:
		int vegetationObjectOpticalPropertyId;
		int vegetationObjectThermalPropertyId;

		shared_ptr<ILAIComputing> laiComputing;

	public:
		PredefinedLAD() :
				vegetationObjectOpticalPropertyId(1), vegetationObjectThermalPropertyId(1) {}

		const int & getVegetationObjectOpticalPropertyId() {
			return vegetationObjectOpticalPropertyId;
		}
		const int & getVegetationObjectThermalPropertyId() {
			return vegetationObjectThermalPropertyId;
		}

		const shared_ptr<ILAIComputing> & getLaiComputing() {
			return laiComputing;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				vegetationObjectOpticalPropertyId = node.get("vegetationObjectOpticalPropertyId").as<int>();
				vegetationObjectThermalPropertyId = node.get("vegetationObjectThermalPropertyId").as<int>();

				if (!node.get("laiComputing").IsNull()) {
					if (node.get("laiComputing").Tag() == "!modelisation.object3d.physicalProperties.laiComputing.LAICalculated") {
						laiComputing = shared_ptr<ILAIComputing>(new LAICalculated());
						static_cast<LAICalculated &>(*laiComputing) = node.get("laiComputing");
					} else if (node.get("laiComputing").Tag() == "!modelisation.object3d.physicalProperties.laiComputing.LAIRelativeToTreeProjection") {
						laiComputing = shared_ptr<ILAIComputing>(new LAIRelativeToTreeProjection());
						static_cast<LAIRelativeToTreeProjection &>(*laiComputing) = node.get("laiComputing");
					} else if (node.get("laiComputing").Tag() == "!modelisation.object3d.physicalProperties.laiComputing.LAITotalLeafAreaProjection") {
						laiComputing = shared_ptr<ILAIComputing>(new LAITotalLeafAreaProjection());
						static_cast<LAITotalLeafAreaProjection &>(*laiComputing) = node.get("laiComputing");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_PREDEFINEDLAD_HPP_ */
