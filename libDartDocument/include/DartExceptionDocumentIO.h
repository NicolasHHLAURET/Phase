/*! Time-stamp: <@(#) DartExceptionDocumentIO.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : DartExceptionDocumentIO.h
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
 * CVS $Log : DartExceptionDocumentIO.h,v$
 */

#ifndef _DARTEXCEPTIONDOCUMENTIO_H_
#define _DARTEXCEPTIONDOCUMENTIO_H_

#include "DartException.h"

/**
 *  @ingroup DartDocument
 * 
 *  Classe de gestion d'exception.
 */

class DartExceptionDocumentIO: public DartException {
public:
	
	/**
	 * Constructeur.
	 */
	DartExceptionDocumentIO();

	/**
	 * Constructeur.
	 */
	DartExceptionDocumentIO(string asMessage);

	/**
	 * Constructeur.
	 */
	DartExceptionDocumentIO(exception *apoException);

	/**
	 * Constructeur.
	 */
	DartExceptionDocumentIO(exception *apoException,
							string asMessage);

	/**
	 * Destructeur.
	 */
	virtual ~DartExceptionDocumentIO() throw ();

private:
	
};

#endif //_DARTEXCEPTIONDOCUMENTIO_H_
