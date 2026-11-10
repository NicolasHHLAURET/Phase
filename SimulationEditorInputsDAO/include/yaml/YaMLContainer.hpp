/*
 * YaMLContainer.hpp
 *
 *  Created on: Jun 22, 2017
 *      Author: lauretn
 */

#ifndef YAMLCONTAINER_HPP_
#define YAMLCONTAINER_HPP_

#include "libDartEnv.h"
#include "yaml-cpp/yaml.h"
#include <string>
#include <queue>

using namespace std;

class YaMLContainer {
private:
	std::map<std::string, YAML::Node> _yamlNodes;
	static const YAML::Node nullNode;

	YAML::Node getModifiableNodeFromPath(std::queue<std::string> & nodePath);
	YAML::Node getNextModifiableNode(	YAML::Node node,
										std::queue<std::string> & nodePath);
public:
	void addYaMLFile(const std::string filePath);

	const YAML::Node & getNode(const std::string & nodeName) const;

	void clear();

	const YAML::Node & getNullNode() const;

	/**
	 * Overide le contenu d'un noeud YaML, cela est notament utilisé lorsque la simulation
	 * est lancée en mode séquence, la simulation mère est chargée et les noeuds sont
	 * mis à jour avec les valeurs séquencées
	 */
	void updateNode(const std::string & key,
					std::string value);
};

#endif /* YAMLCONTAINER_HPP_ */
