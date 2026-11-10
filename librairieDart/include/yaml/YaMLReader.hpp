/*
 * YaMLReader.hpp
 *
 *  Created on: 13 nov. 2017
 *      Author: guilleuxj
 */

#ifndef INCLUDE_YAML_YAMLREADER_HPP_
#define INCLUDE_YAML_YAMLREADER_HPP_

#include "../../include/yaml/YaMLContainer.hpp"
#include "../../librairieDart/include/FichierProperties.hpp"

class YaMLReader {
private:
	YaMLContainer _yamlContainer;
	FichierProperties _sequenceProperties;

	void updateYaMLFromSequencer();
public:
	void openReader();

	const YAML::Node & getFirstNode(const std::string & nodeName) const;
	const YAML::Node getNode(	const YAML::Node & node,
								const std::string & nodeName) const;
	void throwReadingError(const std::string & message) const;
	void throwReadingWarning(const std::string & message) const;

	double getDouble(	const YAML::Node & parentNode,
						const std::string & nodeName) const;
	double getDouble(const std::string & nodeName);

	unsigned int getUnsignedInt(const YAML::Node & parentNode,
								const std::string & nodeName) const;
	unsigned int getUnsignedInt(const std::string & nodeName);

	int getInt(	const YAML::Node & parentNode,
				const std::string & nodeName) const;
	int getInt(const std::string & nodeName);

	bool getBoolean(const YAML::Node & parentNode,
					const std::string & nodeName);
	bool getBoolean(const std::string & nodeName);

	string getScalarValue(	const YAML::Node & parentNode,
							const std::string & nodeName);
	string getScalarValue(const std::string & nodeName);

	bool getIsNull(	const YAML::Node & parentNode,
					const std::string & nodeName);
	bool getIsNull(const std::string & nodeName);

	bool isTag(	const YAML::Node & parentNode,
				const std::string & nodeName,
				const std::string & tagName);
	bool isTag(	const YAML::Node & node,
				const std::string & tagName);
	bool isTag(	const std::string & nodeName,
				const std::string & tagName);

	bool isScalarValue(	const YAML::Node & parentNode,
						const std::string & nodeName,
						const std::string & scalarName);
	bool isScalarValue(	const YAML::Node & parentNode,
						const std::string & scalarName);
	bool isScalarValue(	const std::string & nodeName,
						const std::string & scalarName);

	void closeReader();

};

#endif /* INCLUDE_YAML_YAMLREADER_HPP_ */
