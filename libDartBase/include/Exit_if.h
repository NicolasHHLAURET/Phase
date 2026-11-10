/*! Time-stamp: <@(#) Exit_if.h   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : Exit_if.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : GPR                               Date: 07/03/2005
 *
 *  Description : Utilitaire permettant de faire divers controle et de quitter proprement.
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
 * V 1.0   07/03/2005  GPR : First Revision
 *
 *********************************************************************
 */

#ifndef EXIT_IF_H
#define EXIT_IF_H

#include <stdlib.h>
#include <stdio.h>

/**
 *  @ingroup DartBase
 * 
 * 	Utilitaire de test de controle. 
 */

#define EXIT_IF(expression, message) \
	exit_if (__FILE__, __LINE__, expression, #expression, message)

extern void exit_if(const char *fichier,
					int ligne,
					int condition,
					const char *code,
					const char *message);
#endif //EXIT_IF_H
