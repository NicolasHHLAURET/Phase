/*! Time-stamp: <@(#) MakInversionParser.cpp  05 nov. 2007   vbt MAGELLIUM
 *********************************************************************
 *  @file    : MakInversionParser.cpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 05/11/2007
 *
 *  Description : Classe de gestion du fichier de sortie de MAKET
 *
 *********************************************************************
 * Historique :
 *
 * V 1.0 05 nov. 2007  vbt : First Revision
 *
 *********************************************************************
 */

#include "../include/MakInversionParser.h"

using namespace std;

// noeud
static const string _sMaketOuput = "MaketOuput";
static const string _sProperty = "Property";

// attribut
static const string _sName = "name";
static const string _sValue = "value";
static const string _sIsInversable = "isInversable";
static const string _sVersion = "version";

static const string _sCurrentVersion = "1.0";

/**
 * Constructeur en ecriture par defaut 
 */
MakInversionParser::MakInversionParser() {

}

/**
 * Constructeur en lecture par defaut 
 */
MakInversionParser::MakInversionParser(string absoluteFileNameToMakOutputForInversion) {
	readMakOutputForInversion(absoluteFileNameToMakOutputForInversion);
}

/**
 * Destructeur par defaut
 */
MakInversionParser::~MakInversionParser() {

}

/**
 * Lecture et stockage des informations relative au fichier makOutputForInversion.xml
 */
bool MakInversionParser::readMakOutputForInversion(string& path) {
	TiXmlDocument doc((path + "makOutputForInversion.xml").c_str());
	if (!doc.LoadFile()) {
		cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << endl;
		return false;
	}
	
	TiXmlHandle hdl(&doc);
	TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();
	
	while (elem) {
		_vsInversible.push_back(elem->Attribute("isInversable"));
		_vsNames.push_back(elem->Attribute("name"));
		_vsValues.push_back(elem->Attribute("value"));
		elem = elem->NextSiblingElement();
	}
	return true;
}

/**
 * Fonction permettant de recuperer tous les attributs 'name' du fichier
 */
vector<string> MakInversionParser::getNames() {
	return _vsNames;
}

/**
 * Fonction permettant de recuperer tous les attributs 'value' du fichier
 */
vector<string> MakInversionParser::getValues() {
	return _vsValues;
}

/**
 * Fonction permettant de recuperer tous les attributs 'isInversable' du fichier
 */
vector<string> MakInversionParser::getInversibles() {
	return _vsInversible;
}

/**
 * Fonction d'ecriture du fichier mak_inv.xml
 */
void MakInversionParser::writeMak_invXML(	string absoluteFilePath,
											vector<string> vsCommentsToWrite,
											vector<float> vsValuesToWrite,
											vector<bool> vsIsInversable) {
	// creation du document XML
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "no");
	doc.LinkEndChild(decl);
	
	// creation du noeud racine
	TiXmlElement * root = new TiXmlElement(_sMaketOuput.c_str());
	doc.LinkEndChild(root);
	root->SetAttribute(_sVersion.c_str(), _sCurrentVersion.c_str());
	
	// creation des elements
	for (unsigned int i = 0; i < vsCommentsToWrite.size(); ++i) {
		TiXmlElement * property;
		property = new TiXmlElement(_sProperty.c_str());
		root->LinkEndChild(property);
		property->SetAttribute(_sIsInversable.c_str(), vsIsInversable[i]);
		property->SetAttribute(_sName.c_str(), Conversions::convertitEnString(vsCommentsToWrite[i]).c_str());
		property->SetDoubleAttribute(_sValue.c_str(), vsValuesToWrite[i]);
	}
	
	// enregistrement du fichier XML
	doc.SaveFile(absoluteFilePath.c_str());
}
