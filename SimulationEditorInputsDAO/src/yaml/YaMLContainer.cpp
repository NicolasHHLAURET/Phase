/*
 * YaMLContainer.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: lauretn
 */

#include "../../include/yaml/YaMLContainer.hpp"

const YAML::Node YaMLContainer::nullNode = YAML::Node();

void YaMLContainer::addYaMLFile(const std::string filePath) {
	try {
		YAML::Node node = YAML::LoadFile(filePath);
		if (node) {
			if (node["_data"]) {
				for (YAML::const_iterator it = node["_data"].begin(); it != node["_data"].end(); ++it) {
					_yamlNodes[it->first.Scalar()] = it->second;
				}
			}
		}
	} catch (const YAML::BadFile & bfe) {
		bfe.what();
	}
}

const YAML::Node & YaMLContainer::getNode(const std::string & nodeName) const {
	std::map<std::string, YAML::Node>::const_iterator it = _yamlNodes.find(nodeName);
	if (it != _yamlNodes.end()) {
		return it->second;
	} else {
		cerr << " YaML Node not found : " << nodeName << endl;
		return nullNode;
	}
}

/**
 * Retourne la node YAML à modifier selon le chemin découpé et passé en paramètre (du séquenceur)
 * @see https://stackoverflow.com/questions/32084611/hold-node-reference-in-yaml-cpp before optimizations
 */
YAML::Node YaMLContainer::getModifiableNodeFromPath(std::queue<std::string> & nodePath) {

	const std::string nextNodeName = nodePath.front();
	nodePath.pop();
	return getNextModifiableNode(_yamlNodes[nextNodeName], nodePath);
}

/**
 * Parcours le reccursivement le chemin et retourne la node à modifier
 * @see https://stackoverflow.com/questions/32084611/hold-node-reference-in-yaml-cpp before optimizations
 */
YAML::Node YaMLContainer::getNextModifiableNode(YAML::Node node,
												std::queue<std::string> & nodePath) {
	if (!nodePath.empty()) {
		const std::string nextNodeName = nodePath.front();
		nodePath.pop();
		return getNextModifiableNode(node[nextNodeName], nodePath);
	} else {
		return node;
	}
}

void YaMLContainer::updateNode(	const std::string & key,
								std::string value) {
	stringstream ss(key);
	string token;
	if (getline(ss, token, '.')) {
		// On ignore (de manière déguelasse) le premier token, c'est le nom du fichier.
		std::queue<std::string> nodePath;
		while (getline(ss, token, '.')) {
			nodePath.push(token);
		}
		YAML::Node node = getModifiableNodeFromPath(nodePath);
		node = value;
	}
}

void YaMLContainer::clear() {
	_yamlNodes.clear();
}

const YAML::Node & YaMLContainer::getNullNode() const {
	return nullNode;
}
