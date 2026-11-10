/*
 * ExecutionParameters.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_EXECUTIONPARAMETERS_HPP_
#define INCLUDE_MODEL_EXECUTION_EXECUTIONPARAMETERS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "accelerationEngine/IAccelerationEngine.hpp"
#include "accelerationEngine/EmbreeAccelerationEngine.hpp"
#include "accelerationEngine/LuxCoreAccelerationEngine.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ExecutionParameters {
	private:
		int numberOfThreads;
		shared_ptr<IAccelerationEngine> accelerationEngine;

	public:
		ExecutionParameters() :
				numberOfThreads(1) {}

		const int & getNumberOfThreads() const {
			return numberOfThreads;
		}
		const shared_ptr<IAccelerationEngine> & getAccelerationEngine() const {
			return accelerationEngine;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				numberOfThreads = node.get("numberOfThreads").as<int>();
				if (!node.get("accelerationEngine").IsNull()) {
					if (node.get("accelerationEngine").Tag() == "!execution.accelerationEngine.EmbreeAccelerationEngine") {
						accelerationEngine = shared_ptr<IAccelerationEngine>(new EmbreeAccelerationEngine());
						static_cast<EmbreeAccelerationEngine &>(*accelerationEngine) = node.get("accelerationEngine");
					} else if (node.get("accelerationEngine").Tag() == "!execution.accelerationEngine.LuxCoreAccelerationEngine") {
						accelerationEngine = shared_ptr<IAccelerationEngine>(new LuxCoreAccelerationEngine());
						static_cast<LuxCoreAccelerationEngine &>(*accelerationEngine) = node.get("accelerationEngine");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_EXECUTION_EXECUTIONPARAMETERS_HPP_ */
