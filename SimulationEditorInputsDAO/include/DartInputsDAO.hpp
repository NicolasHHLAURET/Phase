/*
 * DartInputsDAO.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_DARTINPUTSDAO_HPP_
#define INCLUDE_DARTINPUTSDAO_HPP_

#include "yaml/YaMLReader.hpp"

#include "model/DartProducts.hpp"
#include "model/DartRadiation.hpp"
#include "model/DartSpectralBands.hpp"
#include "model/DartAirborneSensors.hpp"
#include "model/DartAtmosphere.hpp"
#include "model/DartDirections.hpp"
#include "model/DartEarthSceneProperties.hpp"
#include "model/DartExecution.hpp"
#include "model/DartOpticalProperties.hpp"
#include "model/DartSceneObjects.hpp"
namespace DartInputs {
	class DartInputsDAO {
	private:
		DartProducts dartProducts;
		DartRadiation dartRadiation;
		DartSpectralBands dartSpectralBands;
		DartAirborneSensors dartAirborneSensors;
		DartAtmosphere dartAtmosphere;
		DartDirections dartDirections;
		DartEarthSceneProperties dartEarthSceneProperties;
		DartExecution dartExecution;
		DartOpticalProperties dartOpticalProperties;
		DartSceneObjects dartSceneObjects;
	public:
		DartInputsDAO() {}

		/**
		 * Call this method at start of any DART exec that need some parameters from simulation editor.
		 */
		void loadInputsFromYaML() {
			YaMLReader reader;
			reader.openReader(); // Note: Ce truc prend en compte le séquenceur, c'est cool
			dartAirborneSensors.loadFromYaml(reader);
			dartAtmosphere.loadFromYaml(reader);
			dartDirections.loadFromYaml(reader);
			dartEarthSceneProperties.loadFromYaml(reader);
			dartProducts.loadFromYaml(reader);
			dartRadiation.loadFromYaml(reader);
			dartSpectralBands.loadFromYaml(reader);
			dartExecution.loadFromYaml(reader);
			dartOpticalProperties.loadFromYaml(reader);
			dartSceneObjects.loadFromYaml(reader);
			reader.closeReader();
		}

		// Exemple d'accès à un paramètre
		void exampleForGetADatum() {
			if (getDartRadiation().getRadiativeTransferMethod()) {
				if (FluxTracking* ft = dynamic_cast<FluxTracking*>(getDartRadiation().getRadiativeTransferMethod().get())) {
					if (ft->getFluxTrackingParameters()) {
						cout << "Gauss siedel = " << ft->getFluxTrackingParameters()->getGaussSiedel() << endl;
					}
				} else {
					cout << "RadiativeTransferMethod != FluxTracking" << endl;
				}
			} else {
				cout << "RadiativeTransferMethod is not defined" << endl;
			}
		}

		/**
		 * TODO
		 * La structure des données dans les XMLs étant différente, glhf
		 */
		void loadInputsFromXML() {}

		/**
		 * Acess to data with these methods.
		 */
		const DartProducts & getDartProducts() const {
			return dartProducts;
		}

		const DartRadiation & getDartRadiation() const {
			return dartRadiation;
		}

		const DartSpectralBands & getDartSpectralBands() const {
			return dartSpectralBands;
		}

		const DartAirborneSensors & getDartAirborneSensors() const {
			return dartAirborneSensors;
		}

		const DartAtmosphere & getDartAtmosphere() const {
			return dartAtmosphere;
		}

		const DartDirections & getDartDirections() const {
			return dartDirections;
		}

		const DartEarthSceneProperties & getDartEarthSceneProperties() const {
			return dartEarthSceneProperties;
		}

		const DartExecution & getDartExecution() const {
			return dartExecution;
		}

		const DartOpticalProperties & getDartOpticalProperties() const {
			return dartOpticalProperties;
		}

		const DartSceneObjects & getDartSceneObjects() const {
			return dartSceneObjects;
		}

		// Correspondance itemid <-> index
		// @Deprecated
		const unsigned int getXml2DPropertyIndex(const int & itemId) const {
			for (unsigned int i = 0; i < dartOpticalProperties.getLambertianProperties().size(); ++i) {
				if (dartOpticalProperties.getLambertianProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getHapkeProperties().size(); ++i) {
				if (dartOpticalProperties.getHapkeProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getHapke2Properties().size(); ++i) {
				if (dartOpticalProperties.getHapke2Properties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getRPVProperties().size(); ++i) {
				if (dartOpticalProperties.getRPVProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getPhaseExternProperties().size(); ++i) {
				if (dartOpticalProperties.getPhaseExternProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			cerr << "2D property at id " << itemId << " not found" << endl;
			return -1;
		}
		// @Deprecated
		const unsigned int getXmlFluidPropertyIndex(const int & itemId) const {
			for (unsigned int i = 0; i < dartOpticalProperties.getFluidProperties().size(); ++i) {
				if (dartOpticalProperties.getFluidProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			cerr << "Fluid property at id " << itemId << " not found" << endl;
			return -1;
		}
		// @Deprecated
		const unsigned int getXmlVegetationPropertyIndex(const int & itemId) const {
			for (unsigned int i = 0; i < dartOpticalProperties.getVegetationProperties().size(); ++i) {
				if (dartOpticalProperties.getVegetationProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			cerr << "Fluid property  at id " << itemId << " not found" << endl;
			return -1;
		}
		// @Deprecated
		const unsigned int getXmlThermalPropertyIndex(const int & itemId) const {
			for (unsigned int i = 0; i < dartOpticalProperties.getThermalProperties().size(); ++i) {
				if (dartOpticalProperties.getThermalProperties().at(i)->getItemId() == itemId) {
					return i;
				}
			}
			cerr << "Thermal property  at id " << itemId << " not found" << endl;
			return -1;
		}
		// @Deprecated
		const unsigned int getXml2DPropertyType(const int & itemId) const {
			for (unsigned int i = 0; i < dartOpticalProperties.getLambertianProperties().size(); ++i) {
				if (dartOpticalProperties.getLambertianProperties().at(i)->getItemId() == itemId) {
					return 0;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getHapkeProperties().size(); ++i) {
				if (dartOpticalProperties.getHapkeProperties().at(i)->getItemId() == itemId) {
					return 2;
				}
			}

			for (unsigned int i = 0; i < dartOpticalProperties.getHapke2Properties().size(); ++i) {
				if (dartOpticalProperties.getHapke2Properties().at(i)->getItemId() == itemId) {
					return 23;   //HAPKE2_SPEC
				}
			}

			for (unsigned int i = 0; i < dartOpticalProperties.getRPVProperties().size(); ++i) {
				if (dartOpticalProperties.getRPVProperties().at(i)->getItemId() == itemId) {
					return 4;
				}
			}
			for (unsigned int i = 0; i < dartOpticalProperties.getPhaseExternProperties().size(); ++i) {
				if (dartOpticalProperties.getPhaseExternProperties().at(i)->getItemId() == itemId) {
					return 11;
				}
			}
			cerr << "Eq Xml Type for 2d function unknown (2d function id= " << itemId << ")" << endl;
			return -1;
		}
	};
}
#endif /* INCLUDE_DARTINPUTSDAO_HPP_ */
