/*
 * MultiplicativeFactorPerBand.hpp
 *
 *  Created on: 3 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORPERBAND_HPP_
#define INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORPERBAND_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "yaml-cpp/yaml.h"
#include "MultiplicativeFactor.hpp"
#include "property/IMultiplicativeFactor.hpp"
#include "property/MultiplicativeFactorForBand.hpp"
namespace DartInputs {
	class MultiplicativeFactorPerBand: public MultiplicativeFactor {
	private:
		std::vector<shared_ptr<MultiplicativeFactorForBand> > multiplicativeFactor;
	public:
		MultiplicativeFactorPerBand() {}

		const std::vector<shared_ptr<MultiplicativeFactorForBand> > & getMultiplicativeFactor() const {
			return multiplicativeFactor;
		}

		void operator =(const YAML::Node& node) {
			if (!node.get("multiplicativeFactor").IsNull() && node.get("multiplicativeFactor").IsSequence()) {
				multiplicativeFactor.clear();
				for (YAML::const_iterator it = node.get("multiplicativeFactor").begin(); it != node.get("multiplicativeFactor").end(); ++it) {
					shared_ptr<MultiplicativeFactorForBand> mf = shared_ptr<MultiplicativeFactorForBand>(new MultiplicativeFactorForBand());
					(*mf) = (*it);
					multiplicativeFactor.push_back(mf);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PHASE_PHASEFUNCTION_MULTIPLICATIVEFACTOR_MULTIPLICATIVEFACTORPERBAND_HPP_ */
