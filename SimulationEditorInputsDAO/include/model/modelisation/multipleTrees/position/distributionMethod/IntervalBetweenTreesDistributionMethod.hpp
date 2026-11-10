/*
 * IntervalBetweenTreesDistributionMethod.hpp
 *
 *  Created on: 3 déc. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_INTERVALBETWEENTREESDISTRIBUTIONMETHOD_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_INTERVALBETWEENTREESDISTRIBUTIONMETHOD_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ITreesDistributionMethod.hpp"

#include "../../../../object/genericObject/DoubletDouble.hpp"

namespace DartInputs {
	class IntervalBetweenTreesDistributionMethod: public ITreesDistributionMethod {
	private:
		shared_ptr<DoubletDouble> interTreeDistance;

	public:
		IntervalBetweenTreesDistributionMethod() {}

		const shared_ptr<DoubletDouble> & getInterTreeDistance() const {
			return interTreeDistance;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("interTreeDistance").IsNull()) {
					interTreeDistance = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*interTreeDistance) = node.get("interTreeDistance");
				}
			}
		}
	};
}



#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_POSITION_DISTRIBUTIONMETHOD_INTERVALBETWEENTREESDISTRIBUTIONMETHOD_HPP_ */
