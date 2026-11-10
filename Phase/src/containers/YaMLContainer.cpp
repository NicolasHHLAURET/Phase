/*
 * YaMLContainer.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: lauretn
 */

#include "../../include/containers/YaMLContainer.hpp"

const YAML::Node YaMLContainer::nullNode = YAML::Node();

void YaMLContainer::addYaML(const std::string filePath) {
	try {
		YAML::Node node = YAML::LoadFile(filePath);
		if (node) {
			if (node["_data"]) {
				for(YAML::const_iterator it=node["_data"].begin();it!=node["_data"].end();++it) {
					_yamlNodes[it->first.Scalar()] = it->second;
				}
			}
		}
	} catch (const YAML::BadFile & bfe) {
		bfe.what();
	}
}
