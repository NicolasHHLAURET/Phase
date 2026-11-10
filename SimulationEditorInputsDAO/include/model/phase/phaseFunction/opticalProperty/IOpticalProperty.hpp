/*
 * IOpticalProperty.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IOPTICALPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IOPTICALPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../specular/SpecularParameters.hpp"
#include "../../../object/database/opticalDatabase/DartModelDatabase.hpp"
namespace DartInputs {
	class IOpticalProperty {
	private:
		shared_ptr<SpecularParameters> specularParameters;
		shared_ptr<DartModelDatabase> opticalDatabase;
		std::string opticalPropertyName;
		int itemId;

	public:
		IOpticalProperty() :
				itemId(1) {}

		virtual ~IOpticalProperty() {}

		const shared_ptr<SpecularParameters> & getSpecularParameters() const {
			return specularParameters;
		}
		const shared_ptr<DartModelDatabase> & getOpticalDatabase() const {
			return opticalDatabase;
		}
		const std::string & getOpticalPropertyName() const {
			return opticalPropertyName;
		}
		const int & getItemId() const {
			return itemId;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				itemId = node.get("itemId").as<int>();
				opticalPropertyName = node.get("opticalPropertyName").as<std::string>();

				if (!node.get("specularParameters").IsNull()) {
					specularParameters = shared_ptr<SpecularParameters>(new SpecularParameters());
					(*specularParameters) = node.get("specularParameters");
				}
				if (!node.get("opticalDatabase").IsNull()) {
					opticalDatabase = shared_ptr<DartModelDatabase>(new DartModelDatabase());
					(*opticalDatabase) = node.get("opticalDatabase");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_IOPTICALPROPERTY_HPP_ */
