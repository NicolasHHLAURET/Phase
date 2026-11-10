/*
 * HapkeProperty.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_HAPKEPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_HAPKEPROPERTY_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPhaseProperty.hpp"
#include "../multiplicativeFactor/MultiplicativeFactor.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorForAllBands.hpp"
#include "../multiplicativeFactor/MultiplicativeFactorPerBand.hpp"
#include "../../../object/database/opticalDatabase/DartModelDatabase.hpp"
namespace DartInputs {


class HapkeProperty: public IPhaseProperty {
	private:
		shared_ptr<DartModelDatabase> transmitanceDatabase;
		shared_ptr<MultiplicativeFactor> hapkeMultiplicativeFactor;

		bool brfInversion;
	public:
		HapkeProperty() :
				brfInversion(false) {}

		const shared_ptr<DartModelDatabase> & getTransmitanceDatabase() const {
			return transmitanceDatabase;
		}
		const shared_ptr<MultiplicativeFactor> & getHapkeMultiplicativeFactor() const {
			return hapkeMultiplicativeFactor;
		}

		const bool & getBrfInversion() const {
			return brfInversion;
		}

		void operator =(const YAML::Node& node) {
			IPhaseProperty::operator=(node);
			if (!node.IsNull()) {
				brfInversion = node.get("brfInversion").as<bool>();

				if (!node.get("hapkeMultiplicativeFactor").IsNull()) {
					if (node.get("hapkeMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						hapkeMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*hapkeMultiplicativeFactor) = node.get("hapkeMultiplicativeFactor");
					} else if (node.get("hapkeMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						hapkeMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*hapkeMultiplicativeFactor) = node.get("hapkeMultiplicativeFactor");
					}
				}

				if (!node.get("transmitanceDatabase").IsNull()) {
					transmitanceDatabase = shared_ptr<DartModelDatabase>(new DartModelDatabase());
					(*transmitanceDatabase) = node.get("transmitanceDatabase");
				}
			}
		}
	};

class Hapke2Property: public IPhaseProperty {
	private:
		shared_ptr<DartModelDatabase> transmitanceDatabase;
		shared_ptr<MultiplicativeFactor> hapke2MultiplicativeFactor;

		bool brfInversion;
	public:
		Hapke2Property() :
		brfInversion(false) {}

		const shared_ptr<DartModelDatabase> & getTransmitanceDatabase() const {
			return transmitanceDatabase;
		}

		const shared_ptr<MultiplicativeFactor> & getHapke2MultiplicativeFactor() const {
			return hapke2MultiplicativeFactor;
		}
		const bool & getBrfInversion() const {
			return brfInversion;
		}

		void operator =(const YAML::Node& node) {
			IPhaseProperty::operator=(node);
			if (!node.IsNull()) {
				brfInversion = node.get("brfInversion").as<bool>();

				if (!node.get("hapke2MultiplicativeFactor").IsNull()) {
					if (node.get("hapke2MultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						hapke2MultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*hapke2MultiplicativeFactor) = node.get("hapke2MultiplicativeFactor");
					} else if (node.get("hapke2MultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						hapke2MultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*hapke2MultiplicativeFactor) = node.get("hapke2MultiplicativeFactor");
					}
				}

				if (!node.get("transmitanceDatabase").IsNull()) {
					transmitanceDatabase = shared_ptr<DartModelDatabase>(new DartModelDatabase());
					(*transmitanceDatabase) = node.get("transmitanceDatabase");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_HAPKEPROPERTY_HPP_ */
