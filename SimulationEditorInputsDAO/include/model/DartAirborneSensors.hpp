/*
 * DartAirborneSensors.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTAIRBORNESENSORS_HPP_
#define INCLUDE_MODEL_DARTAIRBORNESENSORS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "../yaml/YaMLReader.hpp"
#include "airborneSensor/ISensor.hpp"
#include "airborneSensor/InsideSensor.hpp"
#include "airborneSensor/PinholeCamera.hpp"
#include "airborneSensor/PushbroomScanner.hpp"
namespace DartInputs {
	class DartAirborneSensors {
	private:
		std::vector<shared_ptr<ISensor> > listAirborneSensor;
		std::string additionnalSensorFile;
	public:
		DartAirborneSensors() {}

		const std::vector<shared_ptr<ISensor> > & getListAirborneSensor() const {
			return listAirborneSensor;
		}

		const std::string & getAdditionnalSensorFile() const {
			return additionnalSensorFile;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			const YAML::Node & node = yamlReader.getFirstNode("listAirborneSensor");
			if (!node.IsNull() && node.IsSequence()) {
				for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
					shared_ptr<ISensor> sensor;
					if (it->Tag() == "!airborneSensor.InsideSensor") {
						sensor = shared_ptr<ISensor>(new InsideSensor());
						static_cast<InsideSensor &>(*sensor) = (*it);
					} else if (it->Tag() == "!airborneSensor.PinholeCamera") {
						sensor = shared_ptr<ISensor>(new PinholeCamera());
						static_cast<PinholeCamera &>(*sensor) = (*it);
					} else if (it->Tag() == "!airborneSensor.PushbroomScanner") {
						sensor = shared_ptr<ISensor>(new PushbroomScanner());
						static_cast<PushbroomScanner &>(*sensor) = (*it);
					} else {
						break;
					}
					listAirborneSensor.push_back(sensor);
				}
			}
			if (!yamlReader.getFirstNode("additionnalSensorFile").IsNull()) {
				additionnalSensorFile = yamlReader.getFirstNode("additionnalSensorFile").get("dartFilePath").get("relativePath").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTAIRBORNESENSORS_HPP_ */
