/*
 * SpecularParameters.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_SPECULAR_SPECULARPARAMETERS_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_SPECULAR_SPECULARPARAMETERS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../../../object/database/opticalDatabase/DartModelDatabase.hpp"
namespace DartInputs {
	class SpecularParameters {
	public:
		enum SpecularRadianceDistribution {
			LIKE_GAUSSIAN, PARABOLIC, CONSTANT_
		};
	private:
		shared_ptr<DartModelDatabase> specularDatabase;
		bool useRefraction;
		SpecularRadianceDistribution specularRadianceDistribution;

	public:
		SpecularParameters() :
				useRefraction(false), specularRadianceDistribution(LIKE_GAUSSIAN) {}

		const shared_ptr<DartModelDatabase> & getSpecularDatabase() const {
			return specularDatabase;
		}

		const bool & getUseRefraction() const {
			return useRefraction;
		}

		const SpecularRadianceDistribution & getSpecularRadianceDistribution() const {
			return specularRadianceDistribution;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("specularDatabase").IsNull()) {
					specularDatabase = shared_ptr<DartModelDatabase>(new DartModelDatabase());
					(*specularDatabase) = node.get("specularDatabase");
				}
				useRefraction = node.get("useRefraction").as<bool>();
				if (!node.get("specularRadianceDistribution").IsNull()) {
					if (node.get("specularRadianceDistribution").as<std::string>() == "LIKE_GAUSSIAN") {
						specularRadianceDistribution = LIKE_GAUSSIAN;
					} else if (node.get("specularRadianceDistribution").as<std::string>() == "PARABOLIC") {
						specularRadianceDistribution = PARABOLIC;
					} else if (node.get("specularRadianceDistribution").as<std::string>() == "CONSTANT") {
						specularRadianceDistribution = CONSTANT_;
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_SPECULAR_SPECULARPARAMETERS_HPP_ */
