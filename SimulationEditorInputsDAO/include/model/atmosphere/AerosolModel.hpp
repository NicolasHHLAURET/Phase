/*
 * AerosolModel.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_AEROSOLMODEL_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_AEROSOLMODEL_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../object/database/atmosphereDatabase/AerosolModelDatabase.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AerosolModel {
	private:
		std::string modelName;
		bool isEnabledModel;
		double aerosolOpticalDepthMultiplicativeFactor;
		shared_ptr<AerosolModelDatabase> aerosolDatabase;

	public:
		AerosolModel() :
				isEnabledModel(true), aerosolOpticalDepthMultiplicativeFactor(1) {}

		const double & getAerosolOpticalDepthMultiplicativeFactor() const {
			return aerosolOpticalDepthMultiplicativeFactor;
		}

		const std::string & getModelName() const {
			return modelName;
		}

		const bool & getIsEnabledModel() const {
			return isEnabledModel;
		}

		const shared_ptr<AerosolModelDatabase> & getAerosolDatabase() const {
			return aerosolDatabase;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				aerosolOpticalDepthMultiplicativeFactor = node.get("aerosolOpticalDepthMultiplicativeFactor").as<double>();
				isEnabledModel = node.get("isEnabledModel").as<bool>();
				modelName = node.get("modelName").as<std::string>();
				if (!node.get("aerosolDatabase").IsNull()) {
					aerosolDatabase = shared_ptr<AerosolModelDatabase>(new AerosolModelDatabase());
					(*aerosolDatabase) = node.get("aerosolDatabase");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_AEROSOLMODEL_HPP_ */
