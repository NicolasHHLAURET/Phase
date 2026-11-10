/*! Time-stamp: <@(#) Exit_if.c   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : Exit_if.c
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

#include "../include/Exit_if.h"

void exit_if(	const char *fichier,
				int ligne,
				int condition,
				const char *code,
				const char *message) {
	fflush(stdout);
	if (condition == 0)
		return;
	
	if (message == NULL)
		fprintf(stderr, "\n%s:%d: condition d'arret: < %s > \n", fichier, ligne, code);
	else
		fprintf(stderr, "\n%s:%d: %s\n", fichier, ligne, message);
	
	exit(EXIT_FAILURE);
}
