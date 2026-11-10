/*
 * GroupLeaf.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPLEAF_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPLEAF_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "physicalProperties/ILADConversion.hpp"
#include "physicalProperties/CalculatedLAD.hpp"
#include "physicalProperties/NoConversionLAD.hpp"
#include "physicalProperties/PredefinedLAD.hpp"
#include "Group.hpp"
namespace DartInputs {
	class GroupLeaf: public Group {
	private:
		int groupColorId;
		shared_ptr<ILADConversion> physicalProperties;
	public:
		GroupLeaf() :
				groupColorId(1) {}

		const int & getGroupColorId() const {
			return groupColorId;
		}

		const shared_ptr<ILADConversion> & getPhysicalProperties() const {
			return physicalProperties;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				groupColorId = node.get("groupColorId").as<int>();
				if (!node.get("physicalProperties").IsNull()) {
					if (node.get("physicalProperties").Tag() == "!modelisation.object3d.physicalProperties.CalculatedLAD") {
						physicalProperties = shared_ptr<ILADConversion>(new CalculatedLAD());
						static_cast<CalculatedLAD &>(*physicalProperties) = node.get("physicalProperties");
					} else if (node.get("physicalProperties").Tag() == "!modelisation.object3d.physicalProperties.NoConversionLAD") {
						physicalProperties = shared_ptr<ILADConversion>(new NoConversionLAD());
						static_cast<NoConversionLAD &>(*physicalProperties) = node.get("physicalProperties");
					} else if (node.get("physicalProperties").Tag() == "!modelisation.object3d.physicalProperties.PredefinedLAD") {
						physicalProperties = shared_ptr<ILADConversion>(new PredefinedLAD());
						static_cast<PredefinedLAD &>(*physicalProperties) = node.get("physicalProperties");
					}
				}
			}
		}

	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPLEAF_HPP_ */
