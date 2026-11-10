/*
 * DartXML.cpp
 *
 *  Created on: 9 août 2010
 *      Author: goulierv
 */

#include "../include/DartXML.hpp"

string DartXML::path;
map<string, unique_ptr<pugi::xml_document> > DartXML::docs;
map<string, map<string, string> > DartXML::keyMap;
map<string, map<string, int> > DartXML::nbOccurences;
map<string, map<string, vector<string> > > DartXML::keyIndex;

/** initialise le chemin d'acces */
void DartXML::setPath(string& path2) {
	path = path2;
}

/** ouvre et parse un fichier un fichier xml */
bool DartXML::openFile(string fileName) {
	// chargement du fichier
	string filePath = DartEnv::getFilePathForRead(INPUT_DIR, fileName);
	unique_ptr<pugi::xml_document> doc(new pugi::xml_document());

	pugi::xml_parse_result result = doc->load_file(filePath.c_str());
	if (!result) {
		cerr << "Unable to open file : " << fileName << "; error : " << result.description() << endl;
		return false;
	}

	// on passe la 1ere lettre en majuscule
	fileName[0] = toupper(fileName[0]);

	// on enregistre le document dans la map
	docs.insert(pair<string, unique_ptr<pugi::xml_document> >(fileName, move(doc)));
	keyMap.insert(pair<string, map<string, string> >(fileName, map<string, string>()));
	nbOccurences.insert(pair<string, map<string, int> >(fileName, map<string, int>()));
	keyIndex.insert(pair<string, map<string, vector<string> > >(fileName, map<string, vector<string> >()));
	buildMap(fileName);
	return true;
}

/** parse un flux */
bool DartXML::parseStream(std::istream & stream,
						const string& name) {
	// chargement du fichier
	unique_ptr<pugi::xml_document> doc(new pugi::xml_document());

	pugi::xml_parse_result result = doc->load(stream);
	if (!result) {
		cerr << "Unable to parse stream : " << name << "; error : " << result.description() << endl;
		return false;
	}

	// on passe la 1ere lettre en majuscule
	string fileName = name;
	fileName[0] = toupper(fileName[0]);

	// on enregistre le document dans la map
	docs.insert(pair<string, unique_ptr<pugi::xml_document> >(fileName, move(doc)));
	keyMap.insert(pair<string, map<string, string> >(fileName, map<string, string>()));
	nbOccurences.insert(pair<string, map<string, int> >(fileName, map<string, int>()));
	keyIndex.insert(pair<string, map<string, vector<string> > >(fileName, map<string, vector<string> >()));
	buildMap(fileName);
	return true;
}

/** ouvre et parse un fichier un fichier xml */
bool DartXML::openAbsolutePathFile(string absolutePath,
									string fileName) {
	// chargement du fichier
	unique_ptr<pugi::xml_document> doc(new pugi::xml_document());

	pugi::xml_parse_result result = doc->load_file((absolutePath + fileName).c_str());
	if (!result) {
		cerr << "Unable to open file : " << fileName << "; error : " << result.description() << endl;
		return false;
	}

	// on enregistre le document dans la map
	docs.insert(pair<string, unique_ptr<pugi::xml_document> >(fileName, move(doc)));
	keyMap.insert(pair<string, map<string, string> >(fileName, map<string, string>()));
	nbOccurences.insert(pair<string, map<string, int> >(fileName, map<string, int>()));
	keyIndex.insert(pair<string, map<string, vector<string> > >(fileName, map<string, vector<string> >()));
	buildMap(fileName);
	return true;
}

/** compte le nombre d'element correspondant a la cle */
int DartXML::getNbOf(const string & key) {
	return getNbOf(getFileName(key), key);
}
/** compte le nombre d'element correspondant a la cle */
int DartXML::getNbOf(const string & fileName,
					const string & key) {
	// si la cle est absente on la formatte
	//(au cas ou on n'a que un element d'une liste , on enleve les [0])
	int count = nbOccurences[fileName].count(key);
	if (count == 0) {
		string newKey = formatKey(fileName, key);
		return nbOccurences[fileName][newKey];
	} else {
		return nbOccurences[fileName][key];
	}
}

bool DartXML::exists(const string & key) {
	return exists(getFileName(key), key);
}

bool DartXML::exists(const string & fileName,
					const string & key) {
	if (keyMap[fileName].count(key) == 0) {
		string newKey = formatKey(fileName, key);
		return keyMap[fileName].count(newKey) != 0;
	} else {
		return true;
	}
}

/** ferme le fichier */
void DartXML::closeFiles() {
	docs.clear();
	keyMap.clear();
	nbOccurences.clear();
	keyIndex.clear();
}

/** formatte une cle et renvoi le nom de fichier */
string DartXML::formatKey(const string & fileName,
						const string & key) {
	// il y a un probleme de [0]
	unsigned int pos = key.find("[0]");
	string keyCopy = key;

	while ((pos != string::npos) && (pos < keyCopy.size())) {
		string s = keyCopy.substr(0, pos);
		// si la cle est valide
		if (nbOccurences[fileName][s] == 1) {
			// on altere la cle d'origine
			keyCopy.erase(pos, 3);
			pos -= 3;
		} else {
			pos += 3;
		}
		// on passe au crochet suivant
		pos = keyCopy.find("[0]", pos);
	}
	return keyCopy;
}

void DartXML::buildMap(string& fileName) {
	addFirstChilds(fileName, docs[fileName]->document_element());
}

void DartXML::addFirstChilds(string& fileName,
							pugi::xml_node elem) {
	// pour chaque enfant element
	for (pugi::xml_node child = elem.first_child(); child; child = child.next_sibling()) {
		if (child.type() != pugi::node_element) {
			continue;
		}
		string childName = child.name();
		// s'il est unique
		unsigned int nbSameName = 0;
		for (pugi::xml_node n = elem.first_child(); n; n = n.next_sibling()) {
			if (n.type() == pugi::node_element && string(n.name()) == childName) {
				++nbSameName;
			}
		}
		if (nbSameName == 1) {
			addChilds(fileName, childName, child);
		} else {
			// si c'est le premier element
			if (nbOccurences[fileName].count(childName) == 0) {
				nbOccurences[fileName].insert(pair<string, int>(childName, 1));
				addChilds(fileName, childName + "[0]", child);
				addIndexElement(fileName, fileName, childName + "[0]");
			}
			// sinon
			else {
				addChilds(fileName, childName + "[" + Conversions::convertitEnString(nbOccurences[fileName][childName]) + "]", child);
				addIndexElement(fileName, fileName, childName + "[" + Conversions::convertitEnString(nbOccurences[fileName][childName]) + "]");
				++nbOccurences[fileName][childName];
			}
		}
	}
}

void DartXML::addIndexElement(const string & fileName, const string & key, const string & value)
{
	if (keyIndex[fileName].find(key) != keyIndex[fileName].end())
	{
		keyIndex[fileName][key].push_back(value);
	} else
	{
		vector<string> newVector;
		newVector.push_back(value);
		keyIndex[fileName].insert(pair<string, vector<string> >(key, newVector));
	}
}

void DartXML::addChilds(string& fileName,
						string path,
						pugi::xml_node elem) {
	// on sauve les attributs
	for (pugi::xml_attribute att = elem.first_attribute(); att; att = att.next_attribute()) {
		keyMap[fileName].insert(pair<string, string>(path + "." + att.name(), att.value()));
	}

	// pour chaque enfant element
	for (pugi::xml_node child = elem.first_child(); child; child = child.next_sibling()) {
		if (child.type() != pugi::node_element) {
			continue;
		}
		std::string childName = child.name();
		std::string subPath = path + "." + childName;
		// s'il est unique
		unsigned int nbSameName = 0;
		for (pugi::xml_node n = elem.first_child(); n; n = n.next_sibling()) {
			if (n.type() == pugi::node_element && string(n.name()) == childName) {
				++nbSameName;
			}
		}
		if (nbSameName == 1) {
				nbOccurences[fileName].insert(pair<string, int>(subPath, 1));
				addChilds(fileName, subPath, child);
				addIndexElement(fileName, path, subPath);
		} else {
			// si c'est le premier element
			if (nbOccurences[fileName].count(subPath) == 0) {
				nbOccurences[fileName].insert(pair<string, int>(subPath, 1));
				addChilds(fileName, subPath + "[0]", child);
				addIndexElement(fileName, path, subPath + "[0]");
			}
			// sinon
			else {
				addChilds(fileName, subPath + "[" + Conversions::convertitEnString(nbOccurences[fileName][subPath]) + "]", child);
				addIndexElement(fileName, path, subPath + "[" + Conversions::convertitEnString(nbOccurences[fileName][subPath]) + "]");
				++nbOccurences[fileName][subPath];
			}
		}
	}
}

/**
 * Retourne toutes les cles de la map keyMap.
 * @return Un vecteur contenant toutes les cles de la map
 */
vector<string> DartXML::getAllKeys() {
	vector<string> v;
	for (map<string, map<string, string> >::iterator it = keyMap.begin(); it != keyMap.end(); ++it) {
		for (map<string, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			v.push_back(it2->first);
		}
	}
	return v;
}

/**
 * Modifie la valeur associee a une cle.
 * @param key Cle a modifier
 * @param value Nouvelle valeur de la cle
 */
void DartXML::set(const string & key,
				const string & value) {
	//recuperation du nom du fichier
	string fileName = getFileName(key);

	//si la cle existe alors on insere la valeur
	if (keyMap[fileName].find(key) != keyMap[fileName].end()) {
		keyMap[fileName][key] = value;
	} else {
		//si elle n'est pas trouvee dans un premier temps, on la formate
		string newKey = formatKey(fileName, key);
		//si la cle formatee existe alors on insere la valeur
		if (keyMap[fileName].find(newKey) != keyMap[fileName].end()) {
			keyMap[fileName][newKey] = value;
		} else {
			cerr << "Cle invalide :" << newKey << endl;
			exit(66);
		}
	}
}

string DartXML::getFileName(const string & key) {
	int indFirstPoint = key.find(".");
	string fileName(key.substr(0, indFirstPoint));
	fileName += ".xml";
	return fileName;
}

int DartXML::getKeyIndex(const string & keyDart)
{
	return DartXML::getKeyIndex(getFileName(keyDart), keyDart);
}

int DartXML::getKeyIndex(const string & fileName, const string & keyDart)
{
	std::string formatedKey = keyDart;
	// si la cle est absente on la formatte
	//(au cas ou on n'a que un element d'une liste , on enleve les [0])
	if (nbOccurences[fileName].count(keyDart) == 0)
	{
		formatedKey = formatKey(fileName, keyDart);
	}

	if (formatedKey.find_last_of(".") == string::npos)
	{
		// xml starts with a list, weird but okay.
		// it should work, i put the fileName as a parent in this case
		auto it = find(keyIndex[fileName][fileName].begin(), keyIndex[fileName][fileName].end(), formatedKey);
	    if (it != keyIndex[fileName][fileName].end())
	    {
	        return (it - keyIndex[fileName][fileName].begin());
	    }
	} else {
		// normal case
		std::string parent = formatedKey.substr(0, formatedKey.find_last_of("."));
		auto it = find(keyIndex[fileName][parent].begin(), keyIndex[fileName][parent].end(), formatedKey);
	    if (it != keyIndex[fileName][parent].end())
	    {
	        return (it - keyIndex[fileName][parent].begin());
	    }
	}
	return -1; // not found
}
