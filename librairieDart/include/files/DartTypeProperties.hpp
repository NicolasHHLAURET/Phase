/*
 * DartTypeProperties.hpp
 *
 *  Created on: 29 oct. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_FILES_DARTTYPEPROPERTIES_HPP_
#define INCLUDE_FILES_DARTTYPEPROPERTIES_HPP_

/**
 * lecture du fichier dart.typ.properties
 */
class DartTypeProperties: public FichierProperties {

public:
	enum DEFINITION {
		COLOR, TEXTURE, UNDEFINED
	};

	DartTypeProperties(const std::string & cheminAbsolu) {
		DartTypeProperties::FichierProperties(cheminAbsolu, "=");
	}

	const std::set<int> getListeType() {
		std::set<int> setType;
		for (std::map<std::string, std::string>::iterator it = listeProprietes.begin(); it != listeProprietes.end(); ++it) {
			std::string elt = it->first.substr(0, it->first.find("."));
			setType.insert(std::stoi(elt));
		}
		return setType;
	}

	const DEFINITION getDefinition(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.definition";
		std::string def = FichierProperties::valeurProperties<std::string>(key);
		if (def == "color")
		{
			return COLOR;
		} else if (def == "texture")
		{
			return TEXTURE;
		} else
		{
			return UNDEFINED;
		}
	}

	const std::string getName(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.name";
		return FichierProperties::valeurProperties<std::string>(key);
	}

	const double getRed(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.red";
		return FichierProperties::valeurProperties<double>(key);
	}


	const double getGreen(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.green";
		return FichierProperties::valeurProperties<double>(key);
	}

	const double getBlue(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.blue";
		return FichierProperties::valeurProperties<double>(key);
	}


	const std::string getTexturePath(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.path";
		return FichierProperties::valeurProperties<std::string>(key);
	}

	const double getOpacity(const int & typeNumber) const
	{
		std::string key = std::to_string(typeNumber) + ".type.opacity";
		return FichierProperties::valeurProperties<double>(key);
	}
};

#endif /* INCLUDE_FILES_DARTTYPEPROPERTIES_HPP_ */
