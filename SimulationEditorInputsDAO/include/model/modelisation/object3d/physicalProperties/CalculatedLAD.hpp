/*
 * CalculatedLAD.hpp
 *
 *  Created on: 9 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_CALCULATEDLAD_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_CALCULATEDLAD_HPP_

#include "yaml-cpp/yaml.h"
#include "PredefinedLAD.hpp"
namespace DartInputs {
	class CalculatedLAD: public PredefinedLAD {
	private:
		double numberOfAngularSectors;
		int indexFirstPhaseFunction;	// Champ invisible pour l'utilisatueur

	public:
		CalculatedLAD() :
				numberOfAngularSectors(5), indexFirstPhaseFunction(0) {}

		const double & getNumberOfAngularSectors() const {
			return numberOfAngularSectors;
		}
		const int & getIndexFirstPhaseFunction() const {
			return indexFirstPhaseFunction;
		}

		void operator =(const YAML::Node& node) {
			PredefinedLAD::operator=(node);
			if (!node.IsNull()) {
				numberOfAngularSectors = node.get("numberOfAngularSectors").as<double>();
				indexFirstPhaseFunction = node.get("indexFirstPhaseFunction").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_PHYSICALPROPERTIES_CALCULATEDLAD_HPP_ */
