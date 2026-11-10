/*
 * YaMLContainer.hpp
 *
 *  Created on: Jun 22, 2017
 *      Author: lauretn
 */

#ifndef YAMLCONTAINER_HPP_
#define YAMLCONTAINER_HPP_

#include "yaml-cpp/yaml.h"
#include <string>

class YaMLContainer {
private:
	std::map<std::string, YAML::Node> _yamlNodes;
	static const YAML::Node nullNode;

public:
	void addYaML(const std::string filePath);

	const YAML::Node & getNode(const std::string & nodeName) const {
		std::map<std::string, YAML::Node>::const_iterator it =_yamlNodes.find(nodeName);
		if (it != _yamlNodes.end()) {
			return it->second;
		} else {
			return nullNode;
		}
	}

	void clear() {
		_yamlNodes.clear();
	}
};

#endif /* YAMLCONTAINER_HPP_ */
