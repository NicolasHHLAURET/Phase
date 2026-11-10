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
#include <string>
#include <vector>

#include "../../libTinyXPath/include/xpath_static.h"
#include "../../librairieDart/include/Conversions.hpp"
#include "../../libDartEnv/include/DartEnv.h"

using namespace std;
using namespace TinyXPath;

class DartXML {
public:
	DartXML();
	static void setPath(string& path2);
	static bool openFile(string fileName);
	static bool openAbsolutePathFile(	string absolutePath,
										string fileName);
	template<typename T>
	static T get(const string & key) throw(BadFormatException);
	template<typename T>
	static T get(	const string & fileName,
					const string & keyDart) throw(BadFormatException);
	static void set(const string & key,
					const string & value);
	static std::string getStringWithPossibleWhiteSpace(const string & keyDart) throw(BadFormatException);
	static bool exists(const string & key);
	static bool exists(	const string & fileName,
						const string & key);
	static int getNbOf(const string & key);
	static int getNbOf(	const string & fileName,
						const string & key);
	static void closeFiles();
	template<typename T>
	static T getXPath(	const string & fileName,
						const string & key);
	static bool getXPathBool(	const string & fileName,
								const string & key,
								string & valeur);
	static TiXmlElement * getRootElement(string fileName);
	static TiXmlDocument * getTiXmlDocument(string fileName);
	static vector<string> getAllKeys();
	static bool parseStream(std::istream & stream,
							const string& name);
	static int getKeyIndex(const string & keyDart);
	static int getKeyIndex(const string & fileName, const string & keyDart);

private:
	static string path;
	static map<string, TiXmlDocument> docs;
	static map<string, map<string, string> > keyMap;
	static map<string, map<string, int> > nbOccurences;
	static map<string, map<string, vector<string> > > keyIndex;

	static string formatKey(const string & fileName,
							const string & key);
	static void buildMap(string & fileName);
	static void addFirstChilds(	string & fileName,
								TiXmlElement * elem);
	static void addChilds(	string & fileName,
							string path,
							TiXmlElement * elem);
	static string getFileName(const string & key);
	static void addIndexElement(const string & fileName, const string & key, const string & value);
};

/** renvoit la valeur correspondant a la cle Dart */
template<typename T>
T DartXML::get(const string & keyDart) throw(BadFormatException) {
	return DartXML::get<T>(getFileName(keyDart), keyDart);
}

/**
 * renvoit la valeur, pour un string sans un bug chelou quand il y a un espace
 * dans le paramètre (commentaire en FR parce que le reste est en FR)
 */
std::string DartXML::getStringWithPossibleWhiteSpace(const string & keyDart) throw(BadFormatException) {
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
T DartXML::get(	const string & fileName,
				const string & keyDart) throw(BadFormatException) {
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

/** renvoit la valeur correspondant a la cle XPath */
template<typename T>
T DartXML::getXPath(const string & fileName,
					const string & key) {
	return Conversions::convertitStringVersAutre<T>(S_xpath_string(docs[fileName].RootElement(), key.c_str()).c_str());
}
inline TiXmlElement* DartXML::getRootElement(string fileName) {
	return docs[fileName].RootElement();
}
inline TiXmlDocument * DartXML::getTiXmlDocument(string fileName) {
	return &docs[fileName];
}

/** renvoit un booleen pour savoir si la clef existe , la valeur correspondant a la cle XPath est dans la liste des parametres*/
inline bool DartXML::getXPathBool(	const string & fileName,
									const string & key,
									string & valeur) {
	TIXML_STRING resultat;
	if (o_xpath_string(docs[fileName].RootElement(), key.c_str(), resultat)) {
		valeur.append(resultat.c_str());
		return true;
	} else {
		return false;
	}
}
#endif /* DARTXML_HPP_ */
