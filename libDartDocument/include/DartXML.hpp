/*
 * DartXML.hpp
 *
 *  Created on: 9 août 2010
 *      Author: goulierv
 */

#ifndef DARTXML_HPP_
#define DARTXML_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <pugixml.hpp>

#include "../../librairieDart/include/Conversions.hpp"
#include "../../libDartEnv/include/DartEnv.h"

using namespace std;

class DartXML {
public:
	DartXML();
	static void setPath(string& path2);
	static bool openFile(string fileName);
	static bool openAbsolutePathFile(string absolutePath,
									string fileName);
	template<typename T>
	static T get(const string & key);
	template<typename T>
	static T get(const string & fileName,
				const string & keyDart);
	static void set(const string & key,
					const string & value);
	static std::string getStringWithPossibleWhiteSpace(const string & keyDart);
	static bool exists(const string & key);
	static bool exists(const string & fileName,
						const string & key);
	static int getNbOf(const string & key);
	static int getNbOf(const string & fileName,
						const string & key);
	static void closeFiles();
	static bool parseStream(std::istream & stream,
							const string& name);
	static vector<string> getAllKeys();
	static int getKeyIndex(const string & keyDart);
	static int getKeyIndex(const string & fileName, const string & keyDart);

private:
	static string path;
	static map<string, unique_ptr<pugi::xml_document> > docs;
	static map<string, map<string, string> > keyMap;
	static map<string, map<string, int> > nbOccurences;
	static map<string, map<string, vector<string> > > keyIndex;

	static string formatKey(const string & fileName,
							const string & key);
	static void buildMap(string & fileName);
	static void addFirstChilds(string & fileName,
								pugi::xml_node elem);
	static void addChilds(string & fileName,
							string path,
							pugi::xml_node elem);
	static string getFileName(const string & key);
	static void addIndexElement(const string & fileName, const string & key, const string & value);
};

/** renvoit la valeur correspondant a la cle Dart */
template<typename T>
T DartXML::get(const string & keyDart) {
	return DartXML::get<T>(getFileName(keyDart), keyDart);
}

/**
 * renvoit la valeur, pour un string sans un bug chelou quand il y a un espace
 * dans le paramètre (commentaire en FR parce que le reste est en FR)
 */
inline std::string DartXML::getStringWithPossibleWhiteSpace(const string & keyDart) {
	string val = keyMap[getFileName(keyDart)][keyDart];

	// si on trouve la valeur on la renvoit
	if (val.compare("") != 0) {
		return val;
	}

	string newKey = formatKey(getFileName(keyDart), keyDart);
	val = keyMap[getFileName(keyDart)][newKey];

	// si on ne trouve pas la valeur
	if (val.compare("") == 0) {
		cerr << "Cle invalide :" << keyDart << endl;
		exit(66);
	}

	return val;
}

/** renvoit la valeur correspondant a la cle Dart */
template<typename T>
T DartXML::get(const string & fileName,
				const string & keyDart) {
	string val = keyMap[fileName][keyDart];

	// si on trouve la valeur on la renvoit
	if (val.compare("") != 0) {
		return Conversions::convertitStringVersAutre<T>(val);
	}

	string newKey = formatKey(fileName, keyDart);
	val = keyMap[fileName][newKey];

	// si on ne trouve pas la valeur
	if (val.compare("") == 0) {
		cerr << "Cle invalide :" << keyDart << endl;
		exit(66);
	}

	return Conversions::convertitStringVersAutre<T>(val);
}

#endif /* DARTXML_HPP_ */
