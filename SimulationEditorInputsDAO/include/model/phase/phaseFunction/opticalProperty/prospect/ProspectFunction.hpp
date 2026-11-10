/*
 * ProspectFunction.hpp
 *
 *  Created on: 4 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_PROSPECTFUNCTION_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_PROSPECTFUNCTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "FluorescenceFunction.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ProspectFunction {
	private:
		shared_ptr<double> leafMassPerArea; // can be null ?!! :o
		double brownPigment;
		double carotenoid;
		double equivalentWaterThickness;
		double clorophyll;
		double structureCoefficient;
		std::string opticalParametersFile;
		double anthocyanin;
		shared_ptr<FluorescenceFunction> fluorescence;
	public:
		ProspectFunction() :
				brownPigment(0), carotenoid(10), equivalentWaterThickness(0.012), clorophyll(30), structureCoefficient(1.8), anthocyanin(0) {}

		const shared_ptr<double> & getLeafMassPerArea() const {
			return leafMassPerArea;
		}
		const double & getBrownPigment() const {
			return brownPigment;
		}
		const double & getCarotenoid() const {
			return carotenoid;
		}
		const double & getEquivalentWaterThickness() const {
			return equivalentWaterThickness;
		}
		const double & getClorophyll() const {
			return clorophyll;
		}
		const double & getStructureCoefficient() const {
			return structureCoefficient;
		}
		const std::string & getOpticalParametersFile() const {
			return opticalParametersFile;
		}
		const double & getAnthocyanin() const {
			return anthocyanin;
		}
		const shared_ptr<FluorescenceFunction> & getFluorescence() const {
			return fluorescence;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				brownPigment = node.get("brownPigment").as<double>();
				carotenoid = node.get("carotenoid").as<double>();
				equivalentWaterThickness = node.get("equivalentWaterThickness").as<double>();
				clorophyll = node.get("clorophyll").as<double>();
				structureCoefficient = node.get("structureCoefficient").as<double>();
				anthocyanin = node.get("anthocyanin").as<double>();
				if (!node.get("opticalParametersFile").IsNull()) {
					opticalParametersFile = node.get("opticalParametersFile").get("dartFilePath").get("relativePath").as<std::string>();
				}
				if (!node.get("leafMassPerArea").IsNull()) {
					leafMassPerArea = shared_ptr<double>(new double(0.01));
					(*leafMassPerArea) = node.get("leafMassPerArea").as<double>();
				}
				if (!node.get("fluorescence").IsNull()) {
					fluorescence = shared_ptr<FluorescenceFunction>(new FluorescenceFunction());
					(*fluorescence) = node.get("fluorescence");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_PROSPECT_PROSPECTFUNCTION_HPP_ */
