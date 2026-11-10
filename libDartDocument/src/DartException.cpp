/* Time-stamp: <@(#) DartException.cpp  20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  File    : DartException.cpp
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 10/02/2005
 *
 *  Description : 
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 4.0.0 01/07/2005  VBT : Livraison
 * V 1.0 20 janv. 2005  vbt : First Revision
 *
 *********************************************************************
 * CVS $Log : DartException.cpp,v$
 */

#include "../include/DartException.h"

// -----------------------------------------------------------------------------

DartException::DartException() {}
// -----------------------------------------------------------------------------

DartException::DartException(exception *apoOriginException) {
	_poOriginException = apoOriginException;
}
// -----------------------------------------------------------------------------

DartException::DartException(string aOriginMessage) {
	_OriginMessage = aOriginMessage;
}
// -----------------------------------------------------------------------------   

DartException::DartException(	exception *apoOriginException,
								string aOriginMessage) {
	_poOriginException = apoOriginException;
	_OriginMessage = aOriginMessage;
}
// -----------------------------------------------------------------------------

DartException::~DartException() throw () {}
// -----------------------------------------------------------------------------

string DartException::getInformation(void) {
	string sMessage = "";
	
	/*
	 * Pierre Chalier - 24/06/2008
	 * old : if (strcmp (_OriginMessage.c_str (), NULL) != 0)
	 */
	if (!_OriginMessage.empty())
		sMessage += "message  : \" " + _OriginMessage + " \"\n";
	
	if (_poOriginException != NULL)
		sMessage += "exception  \"";
	
	return sMessage;
}
// -----------------------------------------------------------------------------    

exception *DartException::getOriginException(void) {
	return _poOriginException;
}
// -----------------------------------------------------------------------------

string DartException::getOriginMessage(void) {
	return _OriginMessage;
}
// -----------------------------------------------------------------------------
