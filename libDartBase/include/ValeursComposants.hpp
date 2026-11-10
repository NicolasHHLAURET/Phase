/*
 * ValeursComposants.hpp
 *
 *  Created on: 2 juin 2010
 *      Author: goulierv
 */

#ifndef VALEURSCOMPOSANTS_HPP_
#define VALEURSCOMPOSANTS_HPP_

/**
 * Namespace gerant les valeurs de representation des differents composants
 */
namespace ValeursComposants {
	// valeurs des composants (1 bit de presence par composant)
	const int Air = 1;
	const int Soil = 2;
	const int Water = 4;
	const int Road = 8;
	const int Plot = 16;
	const int PlotOpaque = 32;
	const int Trunc = 64;
	const int Canopy = 128;
	const int Twig = 256;
	const int Wall = 512;
	const int Roof = 1024;
	const int Object3D = 2048;
	
	// filtres logiques pour effectuer les tests
	const int filtreTurbide = Plot | Canopy | Twig;
	const int filtreTriangle = Soil | Water | Road | PlotOpaque | Trunc | Wall | Roof | Object3D;
	const int filtreTurbideTriangle = filtreTurbide | filtreTriangle;
	
	/** ajoute un composant a l'ensemble de valeur */
	inline int ajouterComposant(const int ensembleValeurs,
								const int valeurComposant) {
		return (ensembleValeurs | valeurComposant);
	}
	
	/** indique si dans un ensemble de valeurs le composant est present */
	inline bool contient(	const int ensembleValeurs,
							const int valeurComposant) {
		int resultat = ensembleValeurs & valeurComposant;
		return (resultat != 0);
	}
	
	/** indique si dans un ensemble de valeurs uniquement le composant est present */
	inline bool contientUniquement(	const int ensembleValeurs,
									const int valeurComposant) {
		int resultat = ensembleValeurs | valeurComposant;
		return (resultat == valeurComposant);
	}
	
	/** indique si un ensemble de valeurs contient des turbides */
	inline bool contientTurbides(const int ensembleValeurs) {
		int resultat = ensembleValeurs & filtreTurbide;
		return (resultat != 0);
	}
	
	/** indique si un ensemble de valeurs contient uniquement des turbides */
	inline bool contientUniquementTurbides(const int ensembleValeurs) {
		int resultat = ensembleValeurs | filtreTurbide;
		return (resultat == filtreTurbide);
	}
	
	/** indique si un ensemble de valeurs contient des triangles */
	inline bool contientTriangles(const int ensembleValeurs) {
		int resultat = ensembleValeurs & filtreTriangle;
		return (resultat != 0);
	}
	
	/** indique si un ensemble de valeurs contient uniquement des triangles */
	inline bool contientUniquementTriangles(const int ensembleValeurs) {
		int resultat = ensembleValeurs | filtreTriangle;
		return (resultat == filtreTriangle);
	}
	
	/** indique si la cellule contient des turbides et triangles */
	inline bool contientTurbidesEtTriangles(const int ensembleValeurs) {
		return (contientTurbides(ensembleValeurs) && contientTriangles(ensembleValeurs));
	}
}

#endif /* VALEURSCOMPOSANTS_HPP_ */
