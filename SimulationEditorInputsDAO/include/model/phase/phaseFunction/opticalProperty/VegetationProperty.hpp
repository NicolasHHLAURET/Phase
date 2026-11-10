/*
 * VegetationProperty.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_VEGETATIONPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_VEGETATIONPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IOpticalProperty.hpp"
#include "../multiplicativeFactor/MultiplicativeFactor.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorForAllBands.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorPerBand.hpp"
#include "../leafAngleDistribution/ILeafAngleDistribution.hpp"
#include "../leafAngleDistribution/BoundedUniform.hpp"
#include "../leafAngleDistribution/DeWitFunction.hpp"
#include "../leafAngleDistribution/Ellipsoidal.hpp"
#include "../leafAngleDistribution/Elliptical.hpp"
#include "../leafAngleDistribution/Erectophile.hpp"
#include "../leafAngleDistribution/Extremophile.hpp"
#include "../leafAngleDistribution/Horizontal.hpp"
#include "../leafAngleDistribution/Plagiophile.hpp"
#include "../leafAngleDistribution/Planophile.hpp"
#include "../leafAngleDistribution/Spherical.hpp"
#include "../leafAngleDistribution/Uniform.hpp"
#include "../leafAngleDistribution/UserDefined.hpp"
#include "../leafAngleDistribution/Vertical.hpp"
#include "../vegetationFunction/IDirectionalWeightOfGFunction.hpp"
#include "../vegetationFunction/DirectionalWeightOfGFunction.hpp"
#include "prospect/ProspectFunction.hpp"
namespace DartInputs {
	class VegetationProperty: public IOpticalProperty {
	private:
		double leafDimension;
		double thermalHotspotFactor;

		shared_ptr<IDirectionalWeightOfGFunction> functionGParameters;
		shared_ptr<ILeafAngleDistribution> leafAngleDistribution;
		shared_ptr<MultiplicativeFactor> vegetationMultiplicativeFactor;
		shared_ptr<ProspectFunction> prospectFunction;
	public:
		VegetationProperty() :
				leafDimension(0.01), thermalHotspotFactor(0.1) {}

		const double & getLeafDimension() const {
			return leafDimension;
		}
		const double & getThermalHotspotFactor() const {
			return leafDimension;
		}
		const shared_ptr<IDirectionalWeightOfGFunction> & getFunctionGParameters() const {
			return functionGParameters;
		}
		const shared_ptr<ILeafAngleDistribution> & getLeafAngleDistribution() const {
			return leafAngleDistribution;
		}
		const shared_ptr<MultiplicativeFactor> & getVegetationMultiplicativeFactor() const {
			return vegetationMultiplicativeFactor;
		}
		const shared_ptr<ProspectFunction> & getProspectFunction() const {
			return prospectFunction;
		}

		void operator =(const YAML::Node& node) {
			IOpticalProperty::operator=(node);
			if (!node.IsNull()) {
				leafDimension = node.get("leafDimension").as<double>();
				thermalHotspotFactor = node.get("thermalHotspotFactor").as<double>();

				if (!node.get("vegetationMultiplicativeFactor").IsNull()) {
					if (node.get("vegetationMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						vegetationMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*vegetationMultiplicativeFactor) = node.get("vegetationMultiplicativeFactor");
					} else if (node.get("vegetationMultiplicativeFactor").Tag()
							== "phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						vegetationMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*vegetationMultiplicativeFactor) = node.get("vegetationMultiplicativeFactor");
					}
				}

				if (!node.get("prospectFunction").IsNull()) {
					prospectFunction = shared_ptr<ProspectFunction>(new ProspectFunction());
					(*prospectFunction) = node.get("prospectFunction");
				}

				if (!node.get("functionGParameters").IsNull()) {
					if (node.get("functionGParameters").Tag() == "!phase.phaseFunction.vegetationFunction.DirectionalWeightOfGFunction") {
						functionGParameters = shared_ptr<IDirectionalWeightOfGFunction>(new DirectionalWeightOfGFunction());
						static_cast<DirectionalWeightOfGFunction &>(*functionGParameters) = node.get("functionGParameters");
					}
				}

				if (!node.get("leafAngleDistribution").IsNull()) {
					if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.BoundedUniform") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new BoundedUniform());
						static_cast<BoundedUniform &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.DeWitFunction") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new DeWitFunction());
						static_cast<DeWitFunction &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Ellipsoidal") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Ellipsoidal());
						static_cast<Ellipsoidal &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Elliptical") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Elliptical());
						static_cast<Elliptical &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Erectophile") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Erectophile());
						static_cast<Erectophile &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Extremophile") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Extremophile());
						static_cast<Extremophile &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Horizontal") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Horizontal());
						static_cast<Horizontal &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Plagiophile") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Plagiophile());
						static_cast<Plagiophile &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Planophile") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Planophile());
						static_cast<Planophile &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Spherical") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Spherical());
						static_cast<Spherical &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Uniform") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Uniform());
						static_cast<Uniform &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.UserDefined") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new UserDefined());
						static_cast<UserDefined &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					} else if (node.get("leafAngleDistribution").Tag() == "!phase.phaseFunction.leafAngleDistribution.Vertical") {
						leafAngleDistribution = shared_ptr<ILeafAngleDistribution>(new Vertical());
						static_cast<Vertical &>(*leafAngleDistribution) = node.get("leafAngleDistribution");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_VEGETATIONPROPERTY_HPP_ */
