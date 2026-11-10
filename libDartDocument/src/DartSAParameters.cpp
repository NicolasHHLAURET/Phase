/* Time-stamp: <@(#) DartSAParameters.cpp   14 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  File    : DartSAParameters.cpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 14 janv. 2005
 *
 *  Description :
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.1 17/09/2008  Pierre Chalier : Ajout de Object_3d.xml
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 11 fev. 2005  vbt : First Revision
 *
 *********************************************************************
 * CVS $Log : DartSAParameters.cpp,v$
 */

#include "../include/DartSAParameters.h"

// -----------------------------------------------------------------------------

string DartSAParameters::currentPath;

// -----------------------------------------------------------------------------

DartSAParameters::DartSAParameters() {}
// -----------------------------------------------------------------------------

DartSAParameters::~DartSAParameters() {}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							double &dValue) {
	dValue = DartXML::get<double>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							long int &liValue) {
	liValue = DartXML::get<long int>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							bool &bValue) {
	bValue = DartXML::get<bool>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							int &iValue) {
	iValue = DartXML::get<int>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							unsigned int &uiValue) {
	uiValue = DartXML::get<unsigned int>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							float &fValue) {
	fValue = DartXML::get<float>(sParameterPath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::get(	string sParameterPath,
							string &sValue) {
	sValue = DartXML::get<string>(sParameterPath);
}


void DartSAParameters::getStringWithPossibleWhiteSpace(string sParameterPath,
													   std::string &sValue) {
	sValue = DartXML::getStringWithPossibleWhiteSpace(sParameterPath);
}

// -----------------------------------------------------------------------------

int DartSAParameters::getNbOf(string sPropertiesNodePath) {
	return DartXML::getNbOf(sPropertiesNodePath);
}
// -----------------------------------------------------------------------------
int DartSAParameters::getNbOf(	string nomXML,
								string sPropertiesNodePath) {
	return DartXML::getNbOf(nomXML, sPropertiesNodePath);
}
// -----------------------------------------------------------------------------
int DartSAParameters::getKeyIndex(string sPropertiesNodePath) {
	return DartXML::getKeyIndex(sPropertiesNodePath);
}
// -----------------------------------------------------------------------------
int DartSAParameters::getKeyIndex(	string nomXML,
								string sPropertiesNodePath) {
	return DartXML::getKeyIndex(nomXML, sPropertiesNodePath);
}
// -----------------------------------------------------------------------------

bool DartSAParameters::exists(string sPropertiesNodePath) {
	return DartXML::exists(sPropertiesNodePath);
}
// -----------------------------------------------------------------------------
bool DartSAParameters::exists(	string nomXML,
								string sPropertiesNodePath) {
	return DartXML::exists(nomXML, sPropertiesNodePath);
}
// -----------------------------------------------------------------------------

void DartSAParameters::initialize(string path) {
	DartXML::setPath(path);
}
// -----------------------------------------------------------------------------

void DartSAParameters::openFile(string fileName) {
	DartXML::openFile(fileName);
}
// -----------------------------------------------------------------------------
void DartSAParameters::parseStream(	std::istream & stream,
									const string& name) {
	DartXML::parseStream(stream, name);
}
// -----------------------------------------------------------------------------

void DartSAParameters::terminate(void) {
	DartXML::closeFiles();
}
// -----------------------------------------------------------------------------

string DartSAParameters::getFileNameFromDataPath(string sFileName) {
	// conversion d'une string en char
	size_t size = sFileName.size() + 1;
	char * buffer = new char[size];
	strncpy(buffer, sFileName.c_str(), size);
	
	string fileName = strtok(buffer, ".");
	delete[] buffer;
	fileName += ".xml";
	
	return fileName;
}
// -----------------------------------------------------------------------------

void DartSAParameters::set(	const string & key,
							const string & value) {
	DartXML::set(key, value);
}
// -----------------------------------------------------------------------------

vector<string> DartSAParameters::getAllKeys() {
	return DartXML::getAllKeys();
}
