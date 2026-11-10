/*
 * DartExecution.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTEXECUTION_HPP_
#define INCLUDE_MODEL_DARTEXECUTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../yaml/YaMLReader.hpp"
#include "execution/ExternalScripts.hpp"
#include "execution/ExecutionParameters.hpp"
namespace DartInputs {
	class DartExecution {
	private:
		shared_ptr<ExternalScripts> externalScripts;
		shared_ptr<ExecutionParameters> executionParameters;

	public:
		DartExecution() {}

		const shared_ptr<ExternalScripts> & getExternalScripts() const {
			return externalScripts;
		}
		const shared_ptr<ExecutionParameters> & getExecutionParameters() const {
			return executionParameters;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			if (!yamlReader.getFirstNode("externalScripts").IsNull()) {
				externalScripts = shared_ptr<ExternalScripts>(new ExternalScripts());
				(*externalScripts) = yamlReader.getFirstNode("externalScripts");
			}
			if (!yamlReader.getFirstNode("executionParameters").IsNull()) {
				executionParameters = shared_ptr<ExecutionParameters>(new ExecutionParameters());
				(*executionParameters) = yamlReader.getFirstNode("executionParameters");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTEXECUTION_HPP_ */
