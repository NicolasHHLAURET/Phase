/*
 * YaMLReader.cpp
 *
 *  Created on: 13 nov. 2017
 *      Author: guilleuxj
 */

#include "../../include/yaml/YaMLReader.hpp"

void YaMLReader::openReader() {
	// Triangle File is still an XML
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartAirborneSensors.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartRadiation.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartAtmosphere.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartSceneObjects.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartDirections.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartSpectralBands.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartEarthSceneProperties.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartExecution.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartOpticalProperties.yml"));
	_yamlContainer.addYaMLFile(DartEnv::getFilePathForRead(INPUT_DIR, "DartProducts.yml"));

	/**
	 * Override nodes when run with sequencer
	 */
	updateYaMLFromSequencer();
}

/**
 * Met à jour les nodes YaML avec les paramètres séquencés présents dans le dart.sequenceur.properties
 */
void YaMLReader::updateYaMLFromSequencer() {
	std::string absolutePathToDartSequencerProperties = DartEnv::getFilePathForRead(OUTPUT_DIR, "dart.sequenceur.properties");

	if (!absolutePathToDartSequencerProperties.empty()) {
		_sequenceProperties.changerFichierProperties(absolutePathToDartSequencerProperties);
		string keyPrefix = "sequenceur.parametre";
		string keySuffixKey = ".nom";
		string keySuffixValue = ".valeur";
		int i = 0;

		std::stringstream sscheck;
		sscheck << keyPrefix << i << keySuffixKey;

		while (_sequenceProperties.exists(sscheck.str())) {
			std::stringstream sskey, ssValue;
			sskey << keyPrefix << i << keySuffixKey;
			ssValue << keyPrefix << i << keySuffixValue;

			string key, value;
			_sequenceProperties.valeurProperties(sskey.str(), key);
			_sequenceProperties.valeurProperties(ssValue.str(), value);

			_yamlContainer.updateNode(key, value);
			cout << "YaML Sequence - update node " << key << " to value " << value << endl;

			sscheck.clear();
			sscheck << keyPrefix << i << keySuffixKey;
			++i;
		}
	}
}

const YAML::Node & YaMLReader::getFirstNode(const std::string & nodeName) const {
	const YAML::Node & node = _yamlContainer.getNode(nodeName);
	if (&(node) == &(_yamlContainer.getNullNode())) {
		throwReadingError("(getFirstNode) missing mandatory master node : " + nodeName);
	}
	return node;
}

const YAML::Node YaMLReader::getNode(	const YAML::Node & node,
										const std::string & nodeName) const {
	if (node[nodeName]) {
		YAML::Node resNode = node[nodeName];
		return resNode;
	} else {
		throwReadingError("(getNode) missing mandatory node : " + nodeName);
		return node; // Unreachable
	}
}

double YaMLReader::getDouble(	const YAML::Node & parentNode,
								const std::string & nodeName) const {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {
			try {
				return boost::lexical_cast<double>(parentNode[nodeName].Scalar());
			} catch (const boost::bad_lexical_cast & blc) {
				throwReadingError("(getDouble) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
				return 0.0;
			}
		} else {
			throwReadingWarning("(getDouble) node is not scalar: " + nodeName + " value set to 0.0");
			return 0.0;
		}
	} else {
		throwReadingError("(getDouble) missing mandatory node : " + nodeName);
	}
	return 0.0; // Unreachable
}

double YaMLReader::getDouble(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	if (node.IsScalar()) {
		try {
			return boost::lexical_cast<double>(node.Scalar());
		} catch (const boost::bad_lexical_cast & blc) {
			throwReadingError("(getDouble) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
			return 0.0; // Unreachable
		}
	} else {
		throwReadingWarning("(getDouble) node is not scalar: " + nodeName + " value set to 0.0");
		return 0.0;
	}
}

unsigned int YaMLReader::getUnsignedInt(const YAML::Node & parentNode,
										const std::string & nodeName) const {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {
			try {
				return boost::lexical_cast<unsigned int>(parentNode[nodeName].Scalar());
			} catch (const boost::bad_lexical_cast & blc) {
				throwReadingError("(getUnsignedInt) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
				return 0; // Unreachable
			}
		} else {
			throwReadingWarning("(getUnsignedInt) node is not scalar: " + nodeName + " value set to 0");
			return 0;
		}
	} else {
		throwReadingError("(getUnsignedInt) missing mandatory node : " + nodeName);
	}
	return 0; // Unreachable
}

int YaMLReader::getInt(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	if (node.IsScalar()) {
		try {
			return boost::lexical_cast<int>(node.Scalar());
		} catch (const boost::bad_lexical_cast & blc) {
			throwReadingError("(getInt) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
			return 0; // Unreachable
		}
	} else {
		throwReadingWarning("(getInt) node is not scalar: " + nodeName + " value set to 0");
		return 0;
	}
}

int YaMLReader::getInt(	const YAML::Node & parentNode,
						const std::string & nodeName) const {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {
			try {
				return boost::lexical_cast<int>(parentNode[nodeName].Scalar());
			} catch (const boost::bad_lexical_cast & blc) {
				throwReadingError("(getInt) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
				return 0; // Unreachable
			}
		} else {
			throwReadingWarning("(getInt) node is not scalar: " + nodeName + " value set to 0");
			return 0;
		}
	} else {
		throwReadingError("(getInt) missing mandatory node : " + nodeName);
	}
	return 0; // Unreachable
}

unsigned int YaMLReader::getUnsignedInt(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	if (node.IsScalar()) {
		try {
			return boost::lexical_cast<unsigned int>(node.Scalar());
		} catch (const boost::bad_lexical_cast & blc) {
			throwReadingError("(getUnsignedInt) Error for node : " + nodeName + " bad lexical cast: " + blc.what());
			return 0;
		}
	} else {
		throwReadingWarning("(getUnsignedInt) node is not scalar: " + nodeName + " value set to 0");
		return 0;
	}
}

bool YaMLReader::getBoolean(const YAML::Node & parentNode,
							const std::string & nodeName) {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {
			return (parentNode[nodeName].Scalar() == "true");
		} else {
			throwReadingWarning("(getBoolean) node is not scalar: " + nodeName + " value set to false");
			return false;
		}
	} else {
		throwReadingError("(getBoolean) missing mandatory node : " + nodeName);
	}
	return false; // Unreachable
}

bool YaMLReader::getBoolean(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	if (node.IsScalar()) {
		return (node.Scalar() == "true");
	} else {
		throwReadingWarning("(getBoolean) node is not scalar: " + nodeName + " value set to false");
		return false;
	}
}

string YaMLReader::getScalarValue(	const YAML::Node & parentNode,
									const std::string & nodeName) {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {

			return parentNode[nodeName].Scalar();
		} else {
			throwReadingError("(getScalarValue) node is not scalar: " + nodeName);
			return ""; // Unreachable
		}
	} else {
		throwReadingError("(getScalarValue) missing mandatory node : " + nodeName);
	}
	return ""; // Unreachable
}

string YaMLReader::getScalarValue(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	if (node.IsScalar()) {
		return node.Scalar();
	} else {
		throwReadingError("(getScalarValue) node is not scalar: " + nodeName);
		return ""; // Unreachable
	}
}

bool YaMLReader::getIsNull(	const YAML::Node & parentNode,
							const std::string & nodeName) {
	if (parentNode[nodeName]) {
		return parentNode[nodeName].IsNull();
	} else {
		throwReadingError("(getIsNull) missing mandatory node : " + nodeName);
	}
	return false; // Unreachable
}

bool YaMLReader::getIsNull(const std::string & nodeName) {
	const YAML::Node & node = getFirstNode(nodeName);
	return node.IsNull();
}

bool YaMLReader::isTag(	const YAML::Node & parentNode,
						const std::string & nodeName,
						const std::string & tagName) {
	if (parentNode[nodeName]) {
		return parentNode[nodeName].Tag() == tagName;
	} else {
		throwReadingError("(isTag) missing mandatory node : " + nodeName);
	}
	return false; // Unreachable
}

bool YaMLReader::isTag(	const YAML::Node & node,
						const std::string & tagName) {
	if (node) {
		return node.Tag() == tagName;
	} else {
		throwReadingError("(isTag) missing unknown mandatory node, node need to be recovered with getNode method !!!!");
	}
	return false; // Unreachable
}

bool YaMLReader::isTag(	const std::string & nodeName,
						const std::string & tagName) {
	const YAML::Node & node = _yamlContainer.getNode(nodeName);
	return node.Tag() == tagName;
}

bool YaMLReader::isScalarValue(	const YAML::Node & parentNode,
								const std::string & nodeName,
								const std::string & scalarName) {
	if (parentNode[nodeName]) {
		if (parentNode[nodeName].IsScalar()) {
			return parentNode[nodeName].Scalar() == scalarName;
		} else {
			throwReadingWarning("(isScalarValue) node is not scalar: " + nodeName + " comparaison with " + scalarName + " set to false");
			return false;
		}
	} else {
		throwReadingError("(isScalarValue) missing mandatory node : " + nodeName);
	}
	return false; // Unreachable
}

bool YaMLReader::isScalarValue(	const YAML::Node & node,
								const std::string & scalarName) {
	if (node) {
		if (node.IsScalar()) {
			return node.Scalar() == scalarName;
		} else {
			throwReadingWarning("(isScalarValue) unknown node is not scalar, comparaison with " + scalarName + " set to false");
			return false;
		}
	} else {
		throwReadingError("(isScalarValue) missing unknown mandatory node, node need to be recovered with getNode method !!!!");
	}
	return false; // Unreachable
}

bool YaMLReader::isScalarValue(	const std::string & nodeName,
								const std::string & scalarName) {
	const YAML::Node & node = _yamlContainer.getNode(nodeName);
	if (node.IsScalar()) {
		return node.Scalar() == scalarName;
	} else {
		throwReadingWarning("(isScalarValue) node is not scalar: " + nodeName + " comparaison with " + scalarName + " set to false");
		return false;
	}
}

void YaMLReader::throwReadingError(const std::string & message) const {
	cerr << "YaML reader error - " << message << endl;
//	EXIT_IF(42 != 69, " .. Stop process .. ");
}

void YaMLReader::throwReadingWarning(const std::string & message) const {
	cerr << "YaML reader warning - " << message << endl;
}

void YaMLReader::closeReader() {
	_yamlContainer.clear();
}
