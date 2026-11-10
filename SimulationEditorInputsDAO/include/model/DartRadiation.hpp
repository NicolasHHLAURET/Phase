/*
 * DartRadiation.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTRADIATION_HPP_
#define INCLUDE_MODEL_DARTRADIATION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "radiation/radiativeTransfer/IRadiativeTransferMethod.hpp"
#include "radiation/radiativeTransfer/FluxTracking.hpp"
#include "radiation/radiativeTransfer/Lidar.hpp"
#include "radiation/radiativeTransfer/MonteCarlo.hpp"

#include "../yaml/YaMLReader.hpp"
namespace DartInputs {
	class DartRadiation {
	public:
		enum AtmosphericTransfer {
			ANALYTIC, NO_ATMOSPHERE, RADIATIVE_TRANSFER
		};

	private:
		shared_ptr<IRadiativeTransferMethod> radiativeTransferMethod;
		AtmosphericTransfer atmosphericTransfer;
		double atmosphereBrightnessTemperature;
		int numberOfFacetsPerAxisOfCellSubFace;

	public:
		DartRadiation() :
				atmosphereBrightnessTemperature(300.0), numberOfFacetsPerAxisOfCellSubFace(40), atmosphericTransfer(NO_ATMOSPHERE) {}

		const shared_ptr<IRadiativeTransferMethod> & getRadiativeTransferMethod() const {
			return radiativeTransferMethod;
		}

		const AtmosphericTransfer & getAtmosphericTransfer() const {
			return atmosphericTransfer;
		}

		const double & getAtmosphereBrightnessTemperature() const {
			return atmosphereBrightnessTemperature;
		}

		const int & getNumberOfFacetsPerAxisOfCellSubFace() const {
			return numberOfFacetsPerAxisOfCellSubFace;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			const YAML::Node & node = yamlReader.getFirstNode("radiativeTransferMethod");
			if (!node.IsNull()) {
				if (node.Tag() == "!radiation.radiativeTransfer.FluxTracking") {
					radiativeTransferMethod = shared_ptr<IRadiativeTransferMethod>(new FluxTracking());
					static_cast<FluxTracking &>(*radiativeTransferMethod) = node;
				} else if (node.Tag() == "!radiation.radiativeTransfer.Lidar") {
					radiativeTransferMethod = shared_ptr<IRadiativeTransferMethod>(new Lidar());
					static_cast<Lidar &>(*radiativeTransferMethod) = node;
				} else if (node.Tag() == "!radiation.radiativeTransfer.MonteCarlo") {
					radiativeTransferMethod = shared_ptr<IRadiativeTransferMethod>(new MonteCarlo());
					static_cast<MonteCarlo &>(*radiativeTransferMethod) = node;
				}
			}

			if (yamlReader.getFirstNode("atmosphericTransfer").as<std::string>() == "ANALYTIC") {
				atmosphericTransfer = ANALYTIC;
			} else if (yamlReader.getFirstNode("atmosphericTransfer").as<std::string>() == "RADIATIVE_TRANSFER") {
				atmosphericTransfer = RADIATIVE_TRANSFER;
			} else {
				atmosphericTransfer = NO_ATMOSPHERE;
			}

			atmosphereBrightnessTemperature = yamlReader.getFirstNode("atmosphereBrightnessTemperature").as<double>();
			numberOfFacetsPerAxisOfCellSubFace = yamlReader.getFirstNode("numberOfFacetsPerAxisOfCellSubFace").as<int>();
		}
	};
}
#endif /* INCLUDE_MODEL_DARTRADIATION_HPP_ */
