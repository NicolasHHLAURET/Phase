/*! Time-stamp: <@(#) DartException.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : DartException.h
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
 * CVS $Log : DartException.h,v$
 */

#ifndef __IDartException__
#define __IDartException__

#include <string>
#include <stdexcept>
using namespace std;

/**
 *  @ingroup DartDocument
 * 
 *  Classe de gestion d'exception.
 */

class DartException: public std::exception {
public:
	
	/**
	 * Constructeur.
	 */
	DartException();

	/**
	 * Constructeur.
	 */
	DartException(std::exception *apoOriginException);

	/**
	 * Constructeur.
	 */
	DartException(string aOriginMessage);

	/**
	 * Constructeur.
	 */
	DartException(	std::exception *apoOriginException,
					string aOriginMessage);

	/**
	 * Destructeur.
	 */
	virtual ~DartException() throw ();

	/**
	 * @return des informations sur l'exception.
	 */
	string getInformation(void);

	/**
	 * @return l'origine de l'exception.
	 */
	std::exception *getOriginException(void);

	/**
	 * @return le message d'origine.
	 */
	string getOriginMessage(void);

private:
	
	/**
	 * contient les informations sur l'origine de l'exception.
	 */
	std::exception *_poOriginException;

	/**
	 * Message de l'exception.
	 */
	string _OriginMessage;
	
};

#endif //__IDartException__