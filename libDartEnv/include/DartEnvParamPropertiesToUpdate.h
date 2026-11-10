/*! Time-stamp: <@(#) DartEnvParamPropertiesToUpdate.h   20 janv. 2005   vbt MAGELLIUM
 *********************************************************************
 *  @file    : DartEnvParamPropertiesToUpdate.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : vbt                               Date: 15/06/2005
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
 */

#ifndef _DARTENVPARAMPROPERTIESTOUPDATE_H_
#define _DARTENVPARAMPROPERTIESTOUPDATE_H_

/**
 *  @ingroup DartEnv
 * 
 * Classe de mise a jour des fichiers xml.
 */

#include <map>
#include "DartEnv.h"
#include "../../libDartBase/include/libDartBase.h"

class DartEnvParamPropertiesToUpdate {
public:
	
	/**
	 * Constructeur.
	 */
	DartEnvParamPropertiesToUpdate();

	/**
	 * Destructeur.
	 */
	virtual ~DartEnvParamPropertiesToUpdate();

	/**
	 * 
	 * @param string path : chemin dans le xml pour acceder a la variable.
	 * @param string value : valeur a affecter a la variable.
	 * 
	 * Ajoute dans la map au chemin passe en parametre, la valeur passee en 
	 * parametre.
	 */
	static void addParamPropertyToUpdate(	std::string path,
											std::string value);

	/**
	 * Lance la mise a jour du fichier xml.
	 */
	static void ApplyUpdate(void);

private:
	
	/**
	 * map contenant tous les couples chemin/valeur a mettre a jour.
	 */
	static std::map<std::string, std::string> _mapParamToUpdate;
};

#endif //_DARTENVPARAMPROPERTIESTOUPDATE_H_
