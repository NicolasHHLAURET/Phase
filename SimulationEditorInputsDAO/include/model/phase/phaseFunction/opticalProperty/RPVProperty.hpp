/*
 * RPVProperty.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_RPVPROPERTY_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_RPVPROPERTY_HPP_

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
	class RPVProperty: public IPhaseProperty {
	private:
		shared_ptr<DartModelDatabase> transmitanceDatabase;
		shared_ptr<MultiplicativeFactor> rpvMultiplicativeFactor;
	public:
		RPVProperty() {}

		const shared_ptr<DartModelDatabase> & getTransmitanceDatabase() const {
			return transmitanceDatabase;
		}
		const shared_ptr<MultiplicativeFactor> & getRpvMultiplicativeFactor() const {
			return rpvMultiplicativeFactor;
		}
		void operator =(const YAML::Node& node) {
			IPhaseProperty::operator=(node);
			if (!node.IsNull()) {
				if (!node.get("rpvMultiplicativeFactor").IsNull()) {
					if (node.get("rpvMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorForAllBands") {
						rpvMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorForAllBands());
						static_cast<MultiplicativeFactorForAllBands &>(*rpvMultiplicativeFactor) = node.get("rpvMultiplicativeFactor");
					} else if (node.get("rpvMultiplicativeFactor").Tag() == "!phase.phaseFunction.multiplicativeFactor.MultiplicativeFactorPerBand") {
						rpvMultiplicativeFactor = shared_ptr<MultiplicativeFactor>(new MultiplicativeFactorPerBand());
						static_cast<MultiplicativeFactorPerBand &>(*rpvMultiplicativeFactor) = node.get("rpvMultiplicativeFactor");
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

#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_OPTICALPROPERTY_RPVPROPERTY_HPP_ */
