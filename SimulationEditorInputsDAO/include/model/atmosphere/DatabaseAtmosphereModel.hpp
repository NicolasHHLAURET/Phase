/*
 * DatabaseAtmosphereModel.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_DATABASEATMOSPHEREMODEL_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_DATABASEATMOSPHEREMODEL_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../object/database/atmosphereDatabase/GasModelDatabase.hpp"
#include "IAtmosphereModel.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class DatabaseAtmosphereModel: public IAtmosphereModel {
	public:
		enum BandModelCorrection {
			NONE, TRANSUPDOWN, DELTATAU1, DELTATAU2
		};
	private:
		shared_ptr<GasModelDatabase> atmosphereDatabase;
		BandModelCorrection bandModelCorrection;
		// Peut être null
		shared_ptr<double> precipitableAmountWater;
	public:
		DatabaseAtmosphereModel() :
				bandModelCorrection(TRANSUPDOWN) {}

		const shared_ptr<GasModelDatabase> & getAtmosphereDatabase() const {
			return atmosphereDatabase;
		}

		const BandModelCorrection & getBandModelCorrection() const {
			return bandModelCorrection;
		}

		const shared_ptr<double> & getPrecipitableAmountWater() const {
			return precipitableAmountWater;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("atmosphereDatabase").IsNull()) {
					atmosphereDatabase = shared_ptr<GasModelDatabase>(new GasModelDatabase());
					(*atmosphereDatabase) = node.get("atmosphereDatabase");
				}
				if (!node.get("precipitableAmountWater").IsNull()) {
					precipitableAmountWater = shared_ptr<double>(new double());
					(*precipitableAmountWater) = node.get("precipitableAmountWater").as<double>();
				}

				if (node.get("bandModelCorrection").as<std::string>() == "TRANSUPDOWN") {
					bandModelCorrection = TRANSUPDOWN;
				} else if (node.get("bandModelCorrection").as<std::string>() == "DELTATAU1") {
					bandModelCorrection = DELTATAU1;
				} else if (node.get("bandModelCorrection").as<std::string>() == "DELTATAU2") {
					bandModelCorrection = DELTATAU2;
				} else {
					bandModelCorrection = NONE;
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_DATABASEATMOSPHEREMODEL_HPP_ */
