/*
 * DartOpticalProperties.hpp
 *
 *  Created on: 1 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTOPTICALPROPERTIES_HPP_
#define INCLUDE_MODEL_DARTOPTICALPROPERTIES_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "../yaml/YaMLReader.hpp"
#include "phase/phaseFunction/opticalProperty/IPhaseProperty.hpp"
#include "phase/phaseFunction/opticalProperty/FluidProperty.hpp"
#include "phase/phaseFunction/opticalProperty/HapkeProperty.hpp"
#include "phase/phaseFunction/opticalProperty/LambertianProperty.hpp"
#include "phase/phaseFunction/opticalProperty/RPVProperty.hpp"
#include "phase/phaseFunction/opticalProperty/VegetationProperty.hpp"
#include "phase/phaseFunction/opticalProperty/PhaseExtern.hpp"
#include "phase/phaseFunction/OpticalGeneralProperties.hpp"
#include "phase/thermalFunction/IThermalProperty.hpp"
#include "phase/thermalFunction/ThermalProperty.hpp"

namespace DartInputs {
	class DartOpticalProperties {
	private:
		std::vector<shared_ptr<IPhaseProperty> > opticalProperties;

		// Fonctions de surface (les pointeurs sont gérés par le shared_ptr<IPhaseProperty>)
		std::vector<LambertianProperty*> lambertianProperties;
		std::vector<HapkeProperty*> hapkeProperties;
		std::vector<Hapke2Property*> hapke2Properties;
		std::vector<RPVProperty*> rpvProperties;
		std::vector<PhaseExtern*> phaseExternProperties;

		std::vector<shared_ptr<FluidProperty> > fluidProperties;
		std::vector<shared_ptr<VegetationProperty> > vegetationProperties;
		std::vector<shared_ptr<IThermalProperty> > thermalProperties;

		shared_ptr<OpticalGeneralProperties> opticalGeneralProperties;

	public:
		DartOpticalProperties() {}

		const std::vector<shared_ptr<IPhaseProperty> > & getOpticalProperties() const {
			return opticalProperties;
		}
		const std::vector<LambertianProperty*> & getLambertianProperties() const {
			return lambertianProperties;
		}
		const std::vector<HapkeProperty*> & getHapkeProperties() const {
			return hapkeProperties;
		}
		const std::vector<Hapke2Property*> & getHapke2Properties() const {
			return hapke2Properties;
		}
		const std::vector<RPVProperty*> & getRPVProperties() const {
			return rpvProperties;
		}
		const std::vector<PhaseExtern*> & getPhaseExternProperties() const {
			return phaseExternProperties;
		}

		const std::vector<shared_ptr<FluidProperty> > & getFluidProperties() const {
			return fluidProperties;
		}
		const std::vector<shared_ptr<VegetationProperty> > & getVegetationProperties() const {
			return vegetationProperties;
		}
		const std::vector<shared_ptr<IThermalProperty> > & getThermalProperties() const {
			return thermalProperties;
		}
		const shared_ptr<OpticalGeneralProperties> & getOpticalGeneralProperties() const {
			return opticalGeneralProperties;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			if (!yamlReader.getFirstNode("opticalGeneralProperties").IsNull()) {
				opticalGeneralProperties = shared_ptr<OpticalGeneralProperties>(new OpticalGeneralProperties());
				(*opticalGeneralProperties) = yamlReader.getFirstNode("opticalGeneralProperties");
			}

			fluidProperties.clear();
			if (!yamlReader.getFirstNode("fluidProperties").IsNull() && yamlReader.getFirstNode("fluidProperties").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("fluidProperties").begin();
						it != yamlReader.getFirstNode("fluidProperties").end(); ++it) {
					shared_ptr<FluidProperty> fluidPropertie = shared_ptr<FluidProperty>(new FluidProperty());
					(*fluidPropertie) = (*it);
					fluidProperties.push_back(fluidPropertie);
				}
			}

			vegetationProperties.clear();
			if (!yamlReader.getFirstNode("vegetationProperties").IsNull() && yamlReader.getFirstNode("vegetationProperties").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("vegetationProperties").begin();
						it != yamlReader.getFirstNode("vegetationProperties").end(); ++it) {
					shared_ptr<VegetationProperty> vegetationPropertie = shared_ptr<VegetationProperty>(new VegetationProperty());
					(*vegetationPropertie) = (*it);
					vegetationProperties.push_back(vegetationPropertie);
				}
			}

			thermalProperties.clear();
			if (!yamlReader.getFirstNode("thermalProperties").IsNull() && yamlReader.getFirstNode("thermalProperties").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("thermalProperties").begin();
						it != yamlReader.getFirstNode("thermalProperties").end(); ++it) {
					shared_ptr<IThermalProperty> thermalPropertie = shared_ptr<IThermalProperty>(new ThermalProperty());
					static_cast<ThermalProperty &>(*thermalPropertie) = (*it);
					thermalProperties.push_back(thermalPropertie);
				}
			}

			opticalProperties.clear();
			if (!yamlReader.getFirstNode("opticalProperties").IsNull() && yamlReader.getFirstNode("opticalProperties").IsSequence()) {
				for (YAML::const_iterator it = yamlReader.getFirstNode("opticalProperties").begin();
						it != yamlReader.getFirstNode("opticalProperties").end(); ++it) {
					shared_ptr<IPhaseProperty> opticalPropertie;
					if (it->Tag() == "!phase.phaseFunction.opticalProperty.LambertianProperty") {
						opticalPropertie = shared_ptr<IPhaseProperty>(new LambertianProperty());
						static_cast<LambertianProperty &>(*opticalPropertie) = (*it);
						lambertianProperties.push_back(static_cast<LambertianProperty *>(opticalPropertie.get()));
					} else if (it->Tag() == "!phase.phaseFunction.opticalProperty.HapkeProperty") {
						opticalPropertie = shared_ptr<IPhaseProperty>(new HapkeProperty());
						static_cast<HapkeProperty &>(*opticalPropertie) = (*it);
						hapkeProperties.push_back(static_cast<HapkeProperty *>(opticalPropertie.get()));
					} else if (it->Tag() == "!phase.phaseFunction.opticalProperty.Hapke2Property") {
						opticalPropertie = shared_ptr<IPhaseProperty>(new Hapke2Property());
						static_cast<Hapke2Property &>(*opticalPropertie) = (*it);
						hapke2Properties.push_back(static_cast<Hapke2Property *>(opticalPropertie.get()));
					} else if (it->Tag() == "!phase.phaseFunction.opticalProperty.RPVProperty") {
						opticalPropertie = shared_ptr<IPhaseProperty>(new RPVProperty());
						static_cast<RPVProperty &>(*opticalPropertie) = (*it);
						rpvProperties.push_back(static_cast<RPVProperty *>(opticalPropertie.get()));
					} else if (it->Tag() == "!phase.phaseFunction.opticalProperty.PhaseExtern") {
						opticalPropertie = shared_ptr<IPhaseProperty>(new PhaseExtern());
						static_cast<PhaseExtern &>(*opticalPropertie) = (*it);
						phaseExternProperties.push_back(static_cast<PhaseExtern *>(opticalPropertie.get()));
					} else {
						break;
					}
					opticalProperties.push_back(opticalPropertie);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTOPTICALPROPERTIES_HPP_ */
