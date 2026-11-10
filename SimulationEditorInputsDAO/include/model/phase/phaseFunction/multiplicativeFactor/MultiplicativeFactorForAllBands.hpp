/*
 * MultiplicativeFactorForAllBands.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORFORALLBANDS_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORFORALLBANDS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "MultiplicativeFactor.hpp"
#include "property/IMultiplicativeFactor.hpp"
#include "property/MultiplicativeFactorGlobal.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class MultiplicativeFactorForAllBands: public MultiplicativeFactor {
	private:
		shared_ptr<MultiplicativeFactorGlobal> multiplicativeFactor;
	public:
		MultiplicativeFactorForAllBands() {}

		const shared_ptr<MultiplicativeFactorGlobal> & getMultiplicativeFactor() const {
			return multiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.get("multiplicativeFactor").IsNull()) {
				multiplicativeFactor = shared_ptr<MultiplicativeFactorGlobal>(new MultiplicativeFactorGlobal());
				(*multiplicativeFactor) = node.get("multiplicativeFactor");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORFORALLBANDS_HPP_ */
