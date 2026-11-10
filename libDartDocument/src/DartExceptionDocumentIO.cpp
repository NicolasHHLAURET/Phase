/* Time-stamp: <@(#) DartExceptionDocumentIO.cpp   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  File    : DartExceptionDocumentIO.cpp
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
 * CVS $Log :DartExceptionDocumentIO.cpp,v$
 */

#include "../include/DartExceptionDocumentIO.h"

// -----------------------------------------------------------------------------

DartExceptionDocumentIO::DartExceptionDocumentIO() {}
// -----------------------------------------------------------------------------

DartExceptionDocumentIO::DartExceptionDocumentIO(string asMessage) :
		DartException(asMessage) {}
// -----------------------------------------------------------------------------

DartExceptionDocumentIO::DartExceptionDocumentIO(exception *apoException) :
		DartException(apoException) {}
// -----------------------------------------------------------------------------

DartExceptionDocumentIO::DartExceptionDocumentIO(	exception *apoException,
													string asMessage) :
		DartException(apoException, asMessage) {}
// -----------------------------------------------------------------------------

DartExceptionDocumentIO::~DartExceptionDocumentIO() throw () {}
// -----------------------------------------------------------------------------
