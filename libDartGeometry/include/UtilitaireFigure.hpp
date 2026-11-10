/*
 * UtilitaireFigure.hpp
 *
 *  Created on: 25 juin 2010
 *      Author: lauretn
 */

#ifndef UTILITAIREFIGURE_HPP_
#define UTILITAIREFIGURE_HPP_

#include "MaquetteDescription.hpp"
#include "Vecteur.hpp"
//#include "classes/objetDART/DirectionsProprietes.hpp"
#include <map>
#include <list>
#include <vector>
#include <fstream>

#include "Plan.hpp"

#include "CoupleIndiceDistance.hpp"
#include "SimpleSphere.hpp"


class UtilitaireFigure {
public:
	static const int PLAN_X = 0;
	static const int PLAN_Y = 1;
	static const int PLAN_Z = 2;

	typedef std::vector<std::pair<Triplet<int>, std::vector<Point3<double> > > > mapFigureParCellule;
	typedef std::vector<std::list<Point3<double> > > mapFigureParAxe;
	typedef std::vector<std::pair<Triplet<int>, double> > mapProportionParCellule;

	/**
	 * Methode de copie d'une liste vers un vector
	 */
	template<typename T>
	static std::vector<T> copierListeVersVecteur(const std::list<T> & listeAConvertir) {
		//Cette syntaxe est l'equivalent d'une boucle for avec un iterateur; les performances seront malgre tout meilleure
		// car la copie est geree par la stl
		return (std::vector<T>(listeAConvertir.begin(), listeAConvertir.end()));
	}

	static std::vector<Point3<double> > ordonneFigureConvexeSensTrigonometrique_Const(	const std::vector<Point3<double> > & pointFigure,
																						const Vecteur<double> & normale) {
		std::vector<Point3<double> > pointsOrdonnes(pointFigure.size(), pointFigure[0]);

		if (pointFigure.size() > 2) {
			// On va calculer tous les vecteurs partant du premier point de la figure
			// l'algorithme consiste a verifier que le produit vectoriel de tous ces vecteurs, pris 2 a 2, sont orientés dans la meme direction,
			// Si ce n'est pas le cas, il faut les "inverser".
			std::vector<Vecteur<double> > vecteursIssusDuPremierPoint(pointFigure.size() - 1);
			for (unsigned int i = 1; i < pointFigure.size(); ++i) {
				vecteursIssusDuPremierPoint[i - 1] = pointFigure[i];
				vecteursIssusDuPremierPoint[i - 1] -= pointFigure[0];
			}

			Vecteur<double> vecteurTampon;
			for (unsigned int idVect1 = 0; idVect1 < vecteursIssusDuPremierPoint.size() - 1; ++idVect1) {
				for (unsigned int idVect2 = idVect1 + 1; idVect2 < vecteursIssusDuPremierPoint.size(); ++idVect2) {
					if (vecteursIssusDuPremierPoint[idVect1].vectoriel(vecteursIssusDuPremierPoint[idVect2]).scalaire(normale) < -1E-20) {
						// Si le scalaire entre la normale et le resultat du produit vectoriel est inferieur a 0
						// C'est que ces 2 premiers sont opposes, et que, donc, les 2 vecteurs ne sont pas orientes dans le sens trigo
						vecteurTampon = vecteursIssusDuPremierPoint[idVect1];
						vecteursIssusDuPremierPoint[idVect1] = vecteursIssusDuPremierPoint[idVect2];
						vecteursIssusDuPremierPoint[idVect2] = vecteurTampon;
					}
				}
			}

			// On rededuit les coordonnees des points en ajoutant les vecteurs
			for (unsigned int i = 1; i < pointFigure.size(); ++i) {
				pointsOrdonnes[i] += vecteursIssusDuPremierPoint[i - 1];
			}
		} else {
			for (unsigned int i = 1; i < pointFigure.size(); ++i) {
				pointsOrdonnes[i] = pointFigure[i];
			}
		}
		return pointsOrdonnes;
	}

	static void ordonneFigureConvexeSensTrigonometrique(std::vector<Point3<double> > & pointFigure,
														const Vecteur<double> & normale) {
		if (pointFigure.size() > 2) {
			// On va calculer tous les vecteurs partant du premier point de la figure
			// l'algorithme consiste a verifier que le produit vectoriel de tous ces vecteurs, pris 2 a 2, sont orientés dans la meme direction,
			// Si ce n'est pas le cas, il faut les "inverser".
			std::vector<Vecteur<double> > vecteursIssusDuPremierPoint(pointFigure.size() - 1);
			for (unsigned int i = 1; i < pointFigure.size(); ++i) {
				vecteursIssusDuPremierPoint[i - 1] = pointFigure[i];
				vecteursIssusDuPremierPoint[i - 1] -= pointFigure[0];
			}

			Vecteur<double> vecteurTampon;
			for (unsigned int idVect1 = 0; idVect1 < vecteursIssusDuPremierPoint.size() - 1; ++idVect1) {
				for (unsigned int idVect2 = idVect1 + 1; idVect2 < vecteursIssusDuPremierPoint.size(); ++idVect2) {
					if (vecteursIssusDuPremierPoint[idVect1].vectoriel(vecteursIssusDuPremierPoint[idVect2]).scalaire(normale)) {
						// Si le scalaire entre la normale et le resultat du produit vectoriel est inferieur a 0
						// C'est que ces 2 premiers sont opposes, et que, donc, les 2 vecteurs ne sont pas orientes dans le sens trigo
						vecteurTampon = vecteursIssusDuPremierPoint[idVect1];
						vecteursIssusDuPremierPoint[idVect1] = vecteursIssusDuPremierPoint[idVect2];
						vecteursIssusDuPremierPoint[idVect2] = vecteurTampon;
					}
				}
			}

			// On rededuit les coordonnees des points en ajoutant les vecteurs
			for (unsigned int i = 1; i < pointFigure.size(); ++i) {
				pointFigure[i] = pointFigure[0];
				pointFigure[i] += vecteursIssusDuPremierPoint[i - 1];
			}
		}
	}

	static double getAireFigureConvexe(const std::vector<Point3<double> > & pointsFigure) {
		double aireTotale = 0;
		std::vector<Vecteur<double> > vecteursIssusDuPremierPoint;
		const unsigned int pointsFigureSize = pointsFigure.size();

		if (pointsFigureSize > 2) {
			vecteursIssusDuPremierPoint.reserve(pointsFigureSize - 1);
			// Techniquement, on va calculer les aires des triangles composant le "triangle-fan" (Eventail de Triangles)
			// de la figure.
			// On recupere directement les vecteurs "AB" et "AC" de chacun de ces triangles.getFractionFigureHorizontaleParCe
			for (unsigned int m = 1; m < pointsFigureSize; m++) {
				vecteursIssusDuPremierPoint.push_back(pointsFigure[m] - pointsFigure[0]);
			}

			for (unsigned int g = 0; g < vecteursIssusDuPremierPoint.size() - 1; ++g) {
				// Aire du triangle ABC = ||AB x AC|| / 2 (longueur du vecteur issu du produit vectoriel de AB et AC, divise par 2.
				aireTotale += vecteursIssusDuPremierPoint[g].vectoriel(vecteursIssusDuPremierPoint[g + 1]).getLongueur() * 0.5;
			}
		}

		if (std::isnan(aireTotale) || std::isinf(aireTotale)) {
			aireTotale = 0.;
		}
		return aireTotale;
	}

	static double getAireFigureConvexe(const std::list<Point3<double> > & pointsFigure) {
		double aireTotale = 0;
		std::vector<Vecteur<double> > vecteursIssusDuPremierPoint;
		const unsigned int pointsFigureSize = pointsFigure.size();

		if (pointsFigureSize > 2) {
			vecteursIssusDuPremierPoint.reserve(pointsFigureSize - 1);
			// Techniquement, on va calculer les aires des triangles composant le "triangle-fan" (Eventail de Triangles)
			// de la figure.
			// On recupere directement les vecteurs "AB" et "AC" de chacun de ces triangles.getFractionFigureHorizontaleParCe
			std::list<Point3<double> >::const_iterator it = pointsFigure.begin(), itEnd = pointsFigure.end();
			const Point3<double> & premierPoint = pointsFigure.front();
			for (++it /* On commence au 2eme point */; it != itEnd; ++it) {
				vecteursIssusDuPremierPoint.push_back(*it - premierPoint);
			}

			for (unsigned int g = 0; g < vecteursIssusDuPremierPoint.size() - 1; g++) {
				// Aire du triangle ABC = ||AB x AC|| / 2 (longueur du vecteur issu du produit vectoriel de AB et AC, divise par 2.
				aireTotale += vecteursIssusDuPremierPoint[g].vectoriel(vecteursIssusDuPremierPoint[g + 1]).getLongueur() * 0.5;
			}
		}

		if (std::isnan(aireTotale) || std::isinf(aireTotale)) {
			aireTotale = 0.;
		}
		return aireTotale;
	}

	static std::vector<Doublet<unsigned int> > getListeCellulesContenantFigureHorizontale(	const std::vector<Point3<double> > & pointsFigure,
																							const MaquetteDescription & descriptionMaquette) {
		std::vector<Doublet<unsigned int> > res;
		UtilitaireFigure::getListeCellulesContenantFigureHorizontale(pointsFigure, descriptionMaquette);
		return res;
	}
	static void getListeCellulesContenantFigureHorizontale(	std::vector<Doublet<int> > & listeCellule,
															const std::vector<Point3<double> > & pointsFigure,
															const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			// On decoupe la figure selon les plans X
			mapFigureParAxe figuresParFractionX;
			int firstCellIndexX = 0;
			UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

			// Pour chaque section en X, on va realiser le decoupage selon Y
			double min, max;
			int loopIndex = 0;
			for(const mapFigureParAxe::value_type & figure : figuresParFractionX) { // Recherche du min et max en Y
				min = 1E25;
				max = -1E25;
				for(const Point3<double> & point : figure) {
					if (min > point.getY()) {
						min = point.getY();
					}
					if (max < point.getY()) {
						max = point.getY();
					}
				}

				int maxIndex = ceil(max / descriptionMaquette.getDimensionsCellules().getY());
				for (int y = floor(min / descriptionMaquette.getDimensionsCellules().getY()); y <= maxIndex; ++y) {
					listeCellule.push_back(Doublet<int>(firstCellIndexX + loopIndex, y));
				}
				++loopIndex;
			}
		}
	}

	static mapFigureParCellule decoupeFigureHorizontaleParCellule(	const std::vector<Point3<double> > & pointsFigure,
																	const MaquetteDescription & descriptionMaquette) {
		mapFigureParCellule mapResultat;
		UtilitaireFigure::decoupeFigureHorizontaleParCellule(mapResultat, pointsFigure, descriptionMaquette);
		return mapResultat;
	}

	static void decoupeFigureHorizontaleParCellule(	mapFigureParCellule & mapResultat,
													const std::vector<Point3<double> > & pointsFigure,
													const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			const unsigned int indiceHauteurZ = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getZ();

			// On decoupe la figure selon les plans X
			mapFigureParAxe figuresParFractionX;
			int firstCellIndexX = 0;
			UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

			// Pour chaque section en X, on va realiser le decoupage selon Y
			for(const auto & figureX: figuresParFractionX) {
				mapFigureParAxe figuresParFractionY;
				int firstCellIndexY = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(figureX), descriptionMaquette, UtilitaireFigure::PLAN_Y);

				// On enregistre le resultat du decoupage en X et en Y dans la map resultat
				for(const auto & figureY: figuresParFractionY) {
					mapResultat.push_back(std::pair<Triplet<int>, std::vector<Point3<double> > >(Point3<int>(firstCellIndexX, firstCellIndexY, indiceHauteurZ), copierListeVersVecteur<Point3<double> >(figureY)));
					++firstCellIndexY;
				}
				++firstCellIndexX;
			}
		}
	}
	static void decoupeFigureVerticaleXParCellule(	mapFigureParCellule & mapResultat,
													const std::vector<Point3<double> > & pointsFigure,
													const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			const unsigned int indiceHauteurX = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getX();

			// On decoupe la figure selon les plans Z
			mapFigureParAxe figuresParFractionZ;
			int firstCellIndexZ = 0;
			UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

			// Pour chaque section en X, on va realiser le decoupage selon Y
			for(const auto & figureZ: figuresParFractionZ) {
				mapFigureParAxe figuresParFractionY;
				int firstCellIndexY = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(figureZ), descriptionMaquette, UtilitaireFigure::PLAN_Y);

				// On enregistre le resultat du decoupage en X et en Y dans la map resultat
				for(const auto & figureY: figuresParFractionY) {
					mapResultat.push_back(std::pair<Triplet<int>, std::vector<Point3<double> > >(Point3<int>(indiceHauteurX, firstCellIndexY, firstCellIndexZ), copierListeVersVecteur<Point3<double> >(figureY)));
					++firstCellIndexY;
				}
				++firstCellIndexZ;
			}
		}
	}

	static void getAreaPerVoxel(mapProportionParCellule & mapResultat,
								const std::vector<Point3<double> > & pointsFigure,
								const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			// On realise le decoupage selon les sections X
			mapFigureParAxe figuresParFractionX;
			int firstCellIndexX = 0;
			UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

			// Pour chaque section en X, on va realiser le decoupage selon Y
			for (const auto & fractionX : figuresParFractionX) {
				mapFigureParAxe figuresParFractionY;
				int firstCellIndexY = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(fractionX), descriptionMaquette, UtilitaireFigure::PLAN_Y);

				// Pour chaque sous-section en Y, on realise le decoupage selon Z
				for (const auto & fractionY : figuresParFractionY) {
					mapFigureParAxe figuresParFractionZ;
					int firstCellIndexZ = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, copierListeVersVecteur<Point3<double> >(fractionY), descriptionMaquette, UtilitaireFigure::PLAN_Z);

					// On enregistre chaque figure pour cette section X, Y et tous les Z.
					for (const auto & fractionZ : figuresParFractionZ) {
						mapResultat.push_back(std::pair<Triplet<int>, double>(Point3<int>(firstCellIndexX, firstCellIndexY, firstCellIndexZ), getAireFigureConvexe(fractionZ)));
						++firstCellIndexZ;
					}
					++firstCellIndexY;
				}
				++firstCellIndexX;
			}
		}
	}

	static void decoupeFigureParCellule(mapFigureParCellule & mapResultat,
										const std::vector<Point3<double> > & pointsFigure,
										const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			// On realise le decoupage selon les sections X
			mapFigureParAxe figuresParFractionX;
			int firstCellIndexX = 0;
			UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

			// Pour chaque section en X, on va realiser le decoupage selon Y
			for (const auto & fractionX : figuresParFractionX) {
				mapFigureParAxe figuresParFractionY;
				int firstCellIndexY = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(fractionX), descriptionMaquette, UtilitaireFigure::PLAN_Y);

				// Pour chaque sous-section en Y, on realise le decoupage selon Z
				for (const auto & fractionY : figuresParFractionY) {
					mapFigureParAxe figuresParFractionZ;
					int firstCellIndexZ = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, copierListeVersVecteur<Point3<double> >(fractionY), descriptionMaquette, UtilitaireFigure::PLAN_Z);

					// On enregistre chaque figure pour cette section X, Y et tous les Z.
					for (const auto & fractionZ : figuresParFractionZ) {
						mapResultat.push_back(std::pair<Triplet<int>, std::vector<Point3<double> > >(Point3<int>(firstCellIndexX, firstCellIndexY, firstCellIndexZ), copierListeVersVecteur<Point3<double> >(fractionZ)));
						++firstCellIndexZ;
					}
					++firstCellIndexY;
				}
				++firstCellIndexX;
			}
		}
	}

	static void getProportionPerCellForFigureAlignedOnPlane(mapProportionParCellule & mapResultat,
														const std::vector<Point3<double> > & pointsFigure,
														const int & alignementPlane,
														const MaquetteDescription & descriptionMaquette,
														double const& factor) {
		if (pointsFigure.size() > 2) {
			double aireFigureDepart = UtilitaireFigure::getAireFigureConvexe(pointsFigure);

			if (aireFigureDepart > 0) {
				unsigned int alignedPlaneIndex = 0.;
				std::pair<Triplet<int>, double> resPair;
				int firstCutPlane = UtilitaireFigure::PLAN_X;
				int secondCutPlane = UtilitaireFigure::PLAN_Y;
				if (alignementPlane == UtilitaireFigure::PLAN_X) {
					alignedPlaneIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getX();
					firstCutPlane = UtilitaireFigure::PLAN_Z;
				} else if (alignementPlane == UtilitaireFigure::PLAN_Y) {
					alignedPlaneIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getY();
					secondCutPlane = UtilitaireFigure::PLAN_Z;
				} else {
					alignedPlaneIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getZ();
				}

				mapFigureParAxe firstCutFractionList;
				int firstCutFirstIndex = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(firstCutFractionList, firstCutFirstIndex, pointsFigure, descriptionMaquette, firstCutPlane);

				// Pour chaque section en X, on va realiser le decoupage selon Y
				for (const auto & firstCutFraction : firstCutFractionList) {
					mapFigureParAxe secondCutFractionList;
					int secondCutFirstIndex = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(secondCutFractionList, secondCutFirstIndex, copierListeVersVecteur<Point3<double> >(firstCutFraction), descriptionMaquette, secondCutPlane);

					for (const auto & fractionY : secondCutFractionList) {
						if (alignementPlane == UtilitaireFigure::PLAN_X) {
							resPair.first.setXYZ(alignedPlaneIndex, secondCutFirstIndex, firstCutFirstIndex);
						} else if (alignementPlane == UtilitaireFigure::PLAN_Y) {
							resPair.first.setXYZ(firstCutFirstIndex, alignedPlaneIndex, secondCutFirstIndex);
						} else {
							resPair.first.setXYZ(firstCutFirstIndex, secondCutFirstIndex, alignedPlaneIndex);
						}
						resPair.second = UtilitaireFigure::getAireFigureConvexe(fractionY) * factor / aireFigureDepart;
						if (resPair.second > 1.) {
							resPair.second = 1.;
						}
						mapResultat.push_back(resPair);
						++secondCutFirstIndex;
					}
					++firstCutFirstIndex;
				}
			}
		}
	}
	static void getProportionFigureVerticaleXParCellule(mapProportionParCellule & mapResultat,
														const std::vector<Point3<double> > & pointsFigure,
														const MaquetteDescription & descriptionMaquette,
														double const& factor) {
		if (pointsFigure.size() > 2) {
			double aireFigureDepart = UtilitaireFigure::getAireFigureConvexe(pointsFigure);

			if (aireFigureDepart > 0) {
				const unsigned int indiceHauteurX = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getX();

				mapFigureParAxe figuresParFractionZ;
				int firstCellIndexZ = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_Z);

				// Pour chaque section en X, on va realiser le decoupage selon Y
				for (const auto & fractionZ : figuresParFractionZ) {
					mapFigureParAxe figuresParFractionY;
					int firstCellIndexY = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(fractionZ), descriptionMaquette, UtilitaireFigure::PLAN_Y);

					for (const auto & fractionY : figuresParFractionY) {
						mapResultat.push_back(std::pair<Triplet<int>, double>(Triplet<int>(indiceHauteurX, firstCellIndexY, firstCellIndexZ), UtilitaireFigure::getAireFigureConvexe(fractionY) * factor / aireFigureDepart));
						if (mapResultat.back().second > 1.) {
							mapResultat.back().second = 1.;
						}
						++firstCellIndexY;
					}
					++firstCellIndexZ;
				}
			}
		}
	}
	static void getProportionFigureVerticaleYParCellule(mapProportionParCellule & mapResultat,
														const std::vector<Point3<double> > & pointsFigure,
														const MaquetteDescription & descriptionMaquette,
														double const& factor) {
		if (pointsFigure.size() > 2) {
			double aireFigureDepart = UtilitaireFigure::getAireFigureConvexe(pointsFigure);

			if (aireFigureDepart > 0) {
				const unsigned int indiceHauteurY = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getY();

				mapFigureParAxe figuresParFractionZ;
				int firstCellIndexZ = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_Z);

				// Pour chaque section en X, on va realiser le decoupage selon Y
				for (const auto & fractionZ : figuresParFractionZ) {
					mapFigureParAxe figuresParFractionX;
					int firstCellIndexX = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, copierListeVersVecteur<Point3<double> >(fractionZ), descriptionMaquette, UtilitaireFigure::PLAN_X);

					for (const auto & fractionX : figuresParFractionX) {
						mapResultat.push_back(std::pair<Triplet<int>, double>(Triplet<int>(firstCellIndexX, indiceHauteurY, firstCellIndexZ), UtilitaireFigure::getAireFigureConvexe(fractionX) * factor / aireFigureDepart));
						if (mapResultat.back().second > 1.) {
							mapResultat.back().second = 1.;
						}
						if (std::isnan(mapResultat.back().second)) {
							mapResultat.back().second = 0.;
						}
						++firstCellIndexX;
					}
					++firstCellIndexZ;
				}
			}
		}
	}
	static void getProportionFigureHorizontaleParCellule(	mapProportionParCellule & mapResultat,
															const std::vector<Point3<double> > & pointsFigure,
															const MaquetteDescription & descriptionMaquette,
															double const& factor) {
		if (pointsFigure.size() > 2) {
			double aireFigureDepart = UtilitaireFigure::getAireFigureConvexe(pointsFigure);

			if (aireFigureDepart > 0.) {
				const unsigned int indiceHauteurZ = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[0]).getZ();

				mapFigureParAxe figuresParFractionX;
				int firstCellIndexX = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

				mapFigureParAxe::iterator fractionsXIterator, fractionsXEndIterator = figuresParFractionX.end();

				// Pour chaque section en X, on va realiser le decoupage selon Y
				for (fractionsXIterator = figuresParFractionX.begin(); fractionsXIterator != fractionsXEndIterator; ++fractionsXIterator) {
					mapFigureParAxe figuresParFractionY;
					int firstCellIndexY = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(*fractionsXIterator), descriptionMaquette, UtilitaireFigure::PLAN_Y);

					mapFigureParAxe::iterator fractionsYIterator, fractionsYEndIterator = figuresParFractionY.end();

					for (fractionsYIterator = figuresParFractionY.begin(); fractionsYIterator != fractionsYEndIterator; ++fractionsYIterator) {
						mapResultat.push_back(std::pair<Triplet<int>, double>(Triplet<int>(firstCellIndexX, firstCellIndexY, indiceHauteurZ), UtilitaireFigure::getAireFigureConvexe(*fractionsYIterator) * factor / aireFigureDepart));
						if (mapResultat.back().second > 1.) {
							mapResultat.back().second = 1.;
						}
						if (std::isnan(mapResultat.back().second)) {
							mapResultat.back().second = 0.;
						}
						++firstCellIndexY;
					}
					++firstCellIndexX;
				}
			}
		}
	}

	static void getProportionFigureParCellule(	mapProportionParCellule & mapResultat,
												const std::vector<Point3<double> > & pointsFigure,
												const MaquetteDescription & descriptionMaquette) {
		if (pointsFigure.size() > 2) {
			double aireFigureDepart = UtilitaireFigure::getAireFigureConvexe(pointsFigure);

			if (aireFigureDepart != 0) {
				// On realise le decoupage selon les sections X
				mapFigureParAxe figuresParFractionX;
				int firstCellIndexX = 0;
				UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionX, firstCellIndexX, pointsFigure, descriptionMaquette, UtilitaireFigure::PLAN_X);

				mapFigureParAxe::iterator fractionsXIterator, fractionsXEndIterator = figuresParFractionX.end();

				// Pour chaque section en X, on va realiser le decoupage selon Y
				for (fractionsXIterator = figuresParFractionX.begin(); fractionsXIterator != fractionsXEndIterator; ++fractionsXIterator) {
					mapFigureParAxe figuresParFractionY;
					int firstCellIndexY = 0;
					UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionY, firstCellIndexY, copierListeVersVecteur<Point3<double> >(*fractionsXIterator), descriptionMaquette, UtilitaireFigure::PLAN_Y);
					mapFigureParAxe::iterator fractionsYIterator, fractionsYEndIterator = figuresParFractionY.end();

					// Pour chaque sous-section en Y, on realise le decoupage selon Z
					for (fractionsYIterator = figuresParFractionY.begin(); fractionsYIterator != fractionsYEndIterator; ++fractionsYIterator) {
						mapFigureParAxe figuresParFractionZ;
						int firstCellIndexZ = 0;
						UtilitaireFigure::decoupeFigureSelonPlanCellule(figuresParFractionZ, firstCellIndexZ, copierListeVersVecteur<Point3<double> >(*fractionsYIterator), descriptionMaquette, UtilitaireFigure::PLAN_Z);
						mapFigureParAxe::iterator fractionsZIterator, fractionsZEndIterator = figuresParFractionZ.end();
						// On enregistre chaque figure pour cette section X, Y et tous les Z.
						for (fractionsZIterator = figuresParFractionZ.begin(); fractionsZIterator != fractionsZEndIterator; ++fractionsZIterator) {
							mapResultat.push_back(std::pair<Triplet<int>, double>(Triplet<int>(firstCellIndexX, firstCellIndexY, firstCellIndexZ), UtilitaireFigure::getAireFigureConvexe(*fractionsZIterator) / aireFigureDepart));
							if (mapResultat.back().second > 1.) {
								mapResultat.back().second = 1.;
							}
							if (std::isnan(mapResultat.back().second)) {
								mapResultat.back().second = 0.;
							}
							++firstCellIndexZ;
						}
						++firstCellIndexY;
					}
					++firstCellIndexX;
				}
			}
		}
	}

	static void decoupeFigureSelonPlanCellule(	mapFigureParAxe & figuresParCellules,
												int & firstCellIndex,
												const std::vector<Point3<double> > & pointsFigure,
												const MaquetteDescription & descriptionMaquette,
												const int & orientationPlan) {
		// Recherche du point se trouvant a l'extremite minimum selon le plan de decoupe (X, Y, ou Z)
		unsigned int idPointMin = 0;
		double min = 0;
		if (orientationPlan == PLAN_X) {
			min = pointsFigure[0].getX();
		} else if (orientationPlan == PLAN_Y) {
			min = pointsFigure[0].getY();
		} else {
			min = pointsFigure[0].getZ();
		}

		double coordTmp = 0;
		for (unsigned int i = 1; i < pointsFigure.size(); ++i) {
			if (orientationPlan == PLAN_X) {
				coordTmp = pointsFigure[i].getX();
			} else if (orientationPlan == PLAN_Y) {
				coordTmp = pointsFigure[i].getY();
			} else {
				coordTmp = pointsFigure[i].getZ();
			}
			if (coordTmp < min) {
				min = coordTmp;
				idPointMin = i;
			}
		}

		// Creation de la listes des points a parcourir, avec le point extreme en premiere place
		std::list<Point3<double> > listPointAParcourir;
		for (unsigned int i = idPointMin; i < pointsFigure.size(); ++i) {
			listPointAParcourir.push_back(pointsFigure[i]);
		}
		for (unsigned int i = 0; i < idPointMin; ++i) {
			listPointAParcourir.push_back(pointsFigure[i]);
		}

		// Recuperation du premier indice de parcours
		firstCellIndex = 0;

		if (orientationPlan == PLAN_X) {
			firstCellIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[idPointMin]).getX();
		} else if (orientationPlan == PLAN_Y) {
			firstCellIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[idPointMin]).getY();
		} else {
			firstCellIndex = descriptionMaquette.getIndiceCelluleDepuisCoordonneesAbsolues(pointsFigure[idPointMin]).getZ();
		}
		double seuilSectionSuivant = 0;

		Vecteur<double> vecteurVersSecteurSuivant;
		double facteurVersSeuil;
		Point3<double> pointIntersection;

		int indiceCelluleCourante = firstCellIndex;

		do {
			if (orientationPlan == PLAN_X) {
				seuilSectionSuivant = (indiceCelluleCourante + 1) * descriptionMaquette.getDimensionsCellules().getX();
			} else if (orientationPlan == PLAN_Y) {
				seuilSectionSuivant = (indiceCelluleCourante + 1) * descriptionMaquette.getDimensionsCellules().getY();
			} else {
				seuilSectionSuivant = (indiceCelluleCourante + 1) * descriptionMaquette.getDimensionsCellules().getZ();
			}

			// On cree/recupere la liste pour la section/indice courant
			figuresParCellules.push_back(std::list<Point3<double> >());
			std::list<Point3<double> > & listeFigureCourante = figuresParCellules.back();
			// Tant que l'on a pas parcouru toute la figure, on continue le traitement
			// On parcourt la figure d'abord dans le sens direct, puis dans le sens indirect
			// Attention, on realise un 'do while' ici, car on a la certitude qu'au moins le 1er point se trouve dans la liste
			do {
				listeFigureCourante.push_back(listPointAParcourir.front());
				listPointAParcourir.pop_front();
				if (orientationPlan == PLAN_X) {
					coordTmp = listPointAParcourir.front().getX();
				} else if (orientationPlan == PLAN_Y) {
					coordTmp = listPointAParcourir.front().getY();
				} else {
					coordTmp = listPointAParcourir.front().getZ();
				}
			} while (coordTmp <= seuilSectionSuivant && !listPointAParcourir.empty());

			// Si la liste des points a parcourir n'est pas vide, cela signifie que le dernier point rencontre se trouve
			// dans la "section" suivante
			if (!listPointAParcourir.empty()) {
				// Il faut donc calculer l'intersection entre la droite delimitant la section suivante, et le segment entre le dernier point mis dans la figure courante,
				// et le dernier point lu la liste a parcourir
				vecteurVersSecteurSuivant = listPointAParcourir.front() - listeFigureCourante.back();

				if (orientationPlan == PLAN_X) {
					facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.back().getX()) / vecteurVersSecteurSuivant.getX();
					pointIntersection.setX(seuilSectionSuivant);
					pointIntersection.setY(listeFigureCourante.back().getY() + vecteurVersSecteurSuivant.getY() * facteurVersSeuil);
					pointIntersection.setZ(listeFigureCourante.back().getZ() + vecteurVersSecteurSuivant.getZ() * facteurVersSeuil);
				} else if (orientationPlan == PLAN_Y) {
					facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.back().getY()) / vecteurVersSecteurSuivant.getY();
					pointIntersection.setX(listeFigureCourante.back().getX() + vecteurVersSecteurSuivant.getX() * facteurVersSeuil);
					pointIntersection.setY(seuilSectionSuivant);
					pointIntersection.setZ(listeFigureCourante.back().getZ() + vecteurVersSecteurSuivant.getZ() * facteurVersSeuil);
				} else {
					facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.back().getZ()) / vecteurVersSecteurSuivant.getZ();
					pointIntersection.setX(listeFigureCourante.back().getX() + vecteurVersSecteurSuivant.getX() * facteurVersSeuil);
					pointIntersection.setY(listeFigureCourante.back().getY() + vecteurVersSecteurSuivant.getY() * facteurVersSeuil);
					pointIntersection.setZ(seuilSectionSuivant);
				}

				// On ajoute le point d'intersection aux 2 listes : il s'agit du point de depart de la figure à parcourir dans le(s) secteur(s) suivant(s)
				// et du dernier point de la figure de la section courante
				listPointAParcourir.push_front(pointIntersection);			// -> le premier point dans la liste se trouve donc dans la section suivante
				listeFigureCourante.push_back(pointIntersection);

				if (orientationPlan == PLAN_X) {
					coordTmp = listPointAParcourir.back().getX();
				} else if (orientationPlan == PLAN_Y) {
					coordTmp = listPointAParcourir.back().getY();
				} else {
					coordTmp = listPointAParcourir.back().getZ();
				}

				// On doit maintenant parcourir la liste dans le sens indirect
				while (coordTmp <= seuilSectionSuivant && !listPointAParcourir.empty()) {
					listeFigureCourante.push_front(listPointAParcourir.back());
					listPointAParcourir.pop_back();

					if (orientationPlan == PLAN_X) {
						coordTmp = listPointAParcourir.back().getX();
					} else if (orientationPlan == PLAN_Y) {
						coordTmp = listPointAParcourir.back().getY();
					} else {
						coordTmp = listPointAParcourir.back().getZ();
					}
				}

				// Si la liste des points a parcourir n'est pas vide, cela signifie que le dernier point rencontre se trouve
				// dans la "section" suivante
				if (!listPointAParcourir.empty()) {
					// Il faut donc calculer l'intersection entre la droite delimitant la section suivante, et le segment entre le dernier point mis dans la figure courante,
					// et le dernier point lu la liste a parcourir
					vecteurVersSecteurSuivant = listPointAParcourir.back() - listeFigureCourante.front();
					if (orientationPlan == PLAN_X) {
						facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.front().getX()) / vecteurVersSecteurSuivant.getX();
						pointIntersection.setX(seuilSectionSuivant);
						pointIntersection.setY(listeFigureCourante.front().getY() + vecteurVersSecteurSuivant.getY() * facteurVersSeuil);
						pointIntersection.setZ(listeFigureCourante.front().getZ() + vecteurVersSecteurSuivant.getZ() * facteurVersSeuil);
					} else if (orientationPlan == PLAN_Y) {
						facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.front().getY()) / vecteurVersSecteurSuivant.getY();
						pointIntersection.setX(listeFigureCourante.front().getX() + vecteurVersSecteurSuivant.getX() * facteurVersSeuil);
						pointIntersection.setY(seuilSectionSuivant);
						pointIntersection.setZ(listeFigureCourante.front().getZ() + vecteurVersSecteurSuivant.getZ() * facteurVersSeuil);
					} else {
						facteurVersSeuil = (seuilSectionSuivant - listeFigureCourante.front().getZ()) / vecteurVersSecteurSuivant.getZ();
						pointIntersection.setX(listeFigureCourante.front().getX() + vecteurVersSecteurSuivant.getX() * facteurVersSeuil);
						pointIntersection.setY(listeFigureCourante.front().getY() + vecteurVersSecteurSuivant.getY() * facteurVersSeuil);
						pointIntersection.setZ(seuilSectionSuivant);
					}

					// On ajoute le point d'intersection aux 2 listes : il s'agit du point de depart de la figure à parcourir dans le(s) secteur(s) suivant(s)
					// et du dernier point de la figure de la section courante
					listPointAParcourir.push_back(pointIntersection);			// -> Le dernier point dans la liste se trouve dans la section suivante
					listeFigureCourante.push_front(pointIntersection);
				} // La liste ne peut pas etre vide a ce niveau... TODO : lancer exception
			} // Si la liste des points a parcourir est vide, on a fini de construire les figures par section

			++indiceCelluleCourante;
		} while (!listPointAParcourir.empty());
	}

	static std::vector<Point3<double> > getFractionFigureDansCellule(	const std::vector<Point3<double> > & pointsFigure,
																		const MaquetteDescription & descriptionMaquette,
																		const Triplet<int> & indicesCellule) {
		vector<Point3<double> > fraction;
		getFractionFigureDansCellule(fraction, pointsFigure, descriptionMaquette, indicesCellule);
		return fraction;
	}

	static void getFractionFigureDansCellule(	vector<Point3<double> > & fraction,
												const vector<Point3<double> > & pointsFigure,
												const MaquetteDescription & descriptionMaquette,
												const Triplet<int> & indicesCellule) {
		// Decoupage selon les plans X
		// Cut along the planes X
		fraction.clear();
		double seuilMin = indicesCellule.getX() * descriptionMaquette.getDimensionsCellules().getX();
		double seuilMax = seuilMin + descriptionMaquette.getDimensionsCellules().getX();

		Vecteur<double> vecteurDirecteur;
		double facteurDirecteur;
		const unsigned int pointsFigureSize = pointsFigure.size();

		for (unsigned int x = 0; x < pointsFigureSize; ++x) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (x == 0 ? pointsFigure.back() : pointsFigure[x - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = pointsFigure[x];
			const Point3<double> & pointSuivant = (x == pointsFigureSize - 1 ? pointsFigure.front() : pointsFigure[x + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getX() < seuilMin) {
				if (pointPrecedent.getX() >= seuilMin) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getX()) / vecteurDirecteur.getX();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getX() >= seuilMin) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getX()) / vecteurDirecteur.getX();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getX() > seuilMax) {
				if (pointPrecedent.getX() <= seuilMax) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getX()) / vecteurDirecteur.getX();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getX() <= seuilMax) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getX()) / vecteurDirecteur.getX();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction X
				fraction.push_back(pointCourant);
			}
		}

// Decoupage selon les plans Y
		std::vector<Point3<double> > fractionY;

		seuilMin = indicesCellule.getY() * descriptionMaquette.getDimensionsCellules().getY();
		seuilMax = seuilMin + descriptionMaquette.getDimensionsCellules().getY();

		const unsigned int fractionXSize = fraction.size();

		for (unsigned int y = 0; y < fractionXSize; ++y) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (y == 0 ? fraction.back() : fraction[y - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = fraction[y];
			const Point3<double> & pointSuivant = (y == fractionXSize - 1 ? fraction.front() : fraction[y + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getY() < seuilMin) {
				if (pointPrecedent.getY() >= seuilMin) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getY() >= seuilMin) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getY() > seuilMax) {
				if (pointPrecedent.getY() <= seuilMax) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getY() <= seuilMax) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction Y
				fractionY.push_back(pointCourant);
			}
		}

// Decoupage selon les plans Z
//		std::vector<Point3<double> > fractionZ;
		fraction.clear(); //

		seuilMin = indicesCellule.getZ() * descriptionMaquette.getDimensionsCellules().getZ();
		seuilMax = seuilMin + descriptionMaquette.getDimensionsCellules().getZ();

		const unsigned int fractionYSize = fractionY.size();

		for (unsigned int z = 0; z < fractionYSize; ++z) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (z == 0 ? fractionY.back() : fractionY[z - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = fractionY[z];
			const Point3<double> & pointSuivant = (z == fractionYSize - 1 ? fractionY.front() : fractionY[z + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getZ() < seuilMin) {
				if (pointPrecedent.getZ() >= seuilMin) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getZ() >= seuilMin) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getZ() > seuilMax) {
				if (pointPrecedent.getZ() <= seuilMax) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getZ() <= seuilMax) {
					fraction.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fraction.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction Z
				fraction.push_back(pointCourant);
			}
		}
	}

	static std::vector<Point3<double> > getFractionFigureDansCube(	const std::vector<Point3<double> > & pointsFigure,
																	const Point3<double> & lowerCorner,
																	const Point3<double> & upperCorner) {
// Decoupage selon les plans X
// Cut along the planes X
		std::vector<Point3<double> > fractionX;

		double seuilMin = lowerCorner.getX();
		double seuilMax = upperCorner.getX();

		Vecteur<double> vecteurDirecteur;
		double facteurDirecteur;

		const unsigned int pointsFigureSize = pointsFigure.size();

		for (unsigned int x = 0; x < pointsFigureSize; ++x) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (x == 0 ? pointsFigure.back() : pointsFigure[x - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = pointsFigure[x];
			const Point3<double> & pointSuivant = (x == pointsFigureSize - 1 ? pointsFigure.front() : pointsFigure[x + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getX() < seuilMin) {
				if (pointPrecedent.getX() >= seuilMin) {
					fractionX.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getX()) / vecteurDirecteur.getX();
					fractionX.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getX() >= seuilMin) {
					fractionX.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getX()) / vecteurDirecteur.getX();
					fractionX.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getX() >= seuilMax) {
				if (pointPrecedent.getX() < seuilMax) {
					fractionX.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getX()) / vecteurDirecteur.getX();
					fractionX.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getX() < seuilMax) {
					fractionX.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getX()) / vecteurDirecteur.getX();
					fractionX.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction X
				fractionX.push_back(pointCourant);
			}
		}

// Decoupage selon les plans Y
		std::vector<Point3<double> > fractionY;

		seuilMin = lowerCorner.getY();
		seuilMax = upperCorner.getY();

		const unsigned int fractionXSize = fractionX.size();

		for (unsigned int y = 0; y < fractionXSize; ++y) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (y == 0 ? fractionX.back() : fractionX[y - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = fractionX[y];
			const Point3<double> & pointSuivant = (y == fractionXSize - 1 ? fractionX.front() : fractionX[y + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getY() < seuilMin) {
				if (pointPrecedent.getY() >= seuilMin) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getY() >= seuilMin) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getY() >= seuilMax) {
				if (pointPrecedent.getY() < seuilMax) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getY() < seuilMax) {
					fractionY.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getY()) / vecteurDirecteur.getY();
					fractionY.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction Y
				fractionY.push_back(pointCourant);
			}
		}

// Decoupage selon les plans Z
		std::vector<Point3<double> > fractionZ;

		seuilMin = lowerCorner.getZ();
		seuilMax = upperCorner.getZ();

		const unsigned int fractionYSize = fractionY.size();

		for (unsigned int z = 0; z < fractionYSize; ++z) {
			// LAURET Nicolas : Desole pour l'operateur conditionnel ternaire...
			const Point3<double> & pointPrecedent = (z == 0 ? fractionY.back() : fractionY[z - 1]); // Le point precedent du point d'indice 0 se trouve a la fin du vector
			const Point3<double> & pointCourant = fractionY[z];
			const Point3<double> & pointSuivant = (z == fractionYSize - 1 ? fractionY.front() : fractionY[z + 1]); // Le point suivant suivant du point en fin du vector est le premier point du vector

			if (pointCourant.getZ() < seuilMin) {
				if (pointPrecedent.getZ() >= seuilMin) {
					fractionZ.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fractionZ.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getZ() >= seuilMin) {
					fractionZ.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMin - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fractionZ.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else if (pointCourant.getZ() >= seuilMax) {
				if (pointPrecedent.getZ() < seuilMax) {
					fractionZ.push_back(pointCourant);

					vecteurDirecteur = pointPrecedent - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fractionZ.back() += vecteurDirecteur * facteurDirecteur;
				}
				if (pointSuivant.getZ() < seuilMax) {
					fractionZ.push_back(pointCourant);

					vecteurDirecteur = pointSuivant - pointCourant;
					facteurDirecteur = (seuilMax - pointCourant.getZ()) / vecteurDirecteur.getZ();
					fractionZ.back() += vecteurDirecteur * facteurDirecteur;
				}
			} else { // Compris entre les 2 plans de la cellule, dans la direction Z
				fractionZ.push_back(pointCourant);
			}
		}
		return fractionZ;
	}

	static std::vector<Point3<double> > getProjectionHorizontale(	const std::vector<Point3<double> > & pointsFigure,
																	const double & hauteur,
																	const Vecteur<double> & direction) {
		std::vector<Point3<double> > figureProjetee = pointsFigure;

		getProjectionHorizontale(figureProjetee, hauteur, direction);

		return figureProjetee;
	}
	static void getProjectionVerticaleX(std::vector<Point3<double> > & pointsFigure,
										const double & hauteur,
										const Vecteur<double> & direction) {
		for (unsigned int i = 0; i < pointsFigure.size(); ++i) {
			pointsFigure[i] += direction * (hauteur - pointsFigure[i].getX()) / direction.getX();
		}
	}
	static void getProjectionVerticaleY(std::vector<Point3<double> > & pointsFigure,
										const double & hauteur,
										const Vecteur<double> & direction) {
		for (unsigned int i = 0; i < pointsFigure.size(); ++i) {
			pointsFigure[i] += direction * (hauteur - pointsFigure[i].getY()) / direction.getY();
		}
	}
	static void getProjectionHorizontale(	std::vector<Point3<double> > & pointsFigure,
											const double & hauteur,
											const Vecteur<double> & direction) {
		for (unsigned int i = 0; i < pointsFigure.size(); ++i) {
			pointsFigure[i] += direction * (hauteur - pointsFigure[i].getZ()) / direction.getZ();
		}
	}

//	static std::vector<Point3<double> > getProjectionHorizontaleFromDirIndex(const std::vector<Point3<double> > & pointsFigure, const double & hauteur, unsigned int const& dirIndex) {
//		std::vector<Point3<double> > figureProjetee = pointsFigure;
//
//		getProjectionHorizontale(figureProjetee, hauteur, dirIndex);
//
//		return figureProjetee;
//	}

	static std::vector<Point3<double> > calculSousCentresReguliersFigure(	const std::vector<Point3<double> > & figure,
																			const unsigned int & nbSubdivisions) {
		std::vector<Point3<double> > res;
		calculSousCentresReguliersFigure(res, figure, nbSubdivisions);
		return res;
	}

	static void calculSousCentresReguliersFigure(	std::vector<Point3<double> > & vecteurSousCentres,
													const std::vector<Point3<double> > & figure,
													const unsigned int & nbSubdivisions) {
		switch (figure.size()) {
		case 0:
			break;
		case 1:
			vecteurSousCentres.push_back(figure[0]);
			break;
		case 2:
			vecteurSousCentres.push_back((figure[0] + figure[1]) / 2);
			break;
		case 3:
			calculSousCentresTriangleRecursif(vecteurSousCentres, figure[0], figure[1], figure[2], nbSubdivisions);
			break;
		default:
			Point3<double> barycentre = figure[0];
			for (unsigned int i = 1; i < figure.size(); ++i) {
				barycentre += figure[i];
			}
			barycentre /= figure.size();

			vecteurSousCentres.push_back(barycentre);

			if (nbSubdivisions > 0) {
				// Appel recursif sur le triangle compose du dernier point, du barycentre et du 1er point.
				calculSousCentresTriangleRecursif(vecteurSousCentres, figure[figure.size() - 1], barycentre, figure[0], nbSubdivisions - 1);

				// Appels recursif sur les triangles compose de 2 points successifs et du barycentre
				for (unsigned int i = 0; i < figure.size() - 1; ++i) {
					calculSousCentresTriangleRecursif(vecteurSousCentres, figure[i], barycentre, figure[i + 1], nbSubdivisions - 1);
				}
			}
			break;
		}
	}

	static std::vector<Point3<double> > calculSousCentresTriangleRecursif(	const Point3<double> & point1,
																			const Point3<double> & point2,
																			const Point3<double> & point3,
																			const int & etape) {
		std::vector<Point3<double> > res;
		calculSousCentresTriangleRecursif(res, point1, point2, point3, etape);
		return res;
	}

	static void calculSousCentresTriangleRecursif(	std::vector<Point3<double> > & vecteurSousCentres,
													const Point3<double> & point1,
													const Point3<double> & point2,
													const Point3<double> & point3,
													const int & etape) {
		if (etape == 0) {
			// calcul et ajout du baycentre
			vecteurSousCentres.push_back((point1 + point2 + point3) / 3);
		} else {
			// Calcul des milieux
			// relance de la methode sur les 4 sous triangles avec etape -1
			Point3<double> milieu31 = (point3 + point1) / 2;
			Point3<double> milieu12 = (point1 + point2) / 2;
			Point3<double> milieu23 = (point2 + point3) / 2;
			int etapeSuivante = etape - 1;
			calculSousCentresTriangleRecursif(vecteurSousCentres, milieu31, point1, milieu12, etapeSuivante);
			calculSousCentresTriangleRecursif(vecteurSousCentres, milieu12, point2, milieu23, etapeSuivante);
			calculSousCentresTriangleRecursif(vecteurSousCentres, milieu23, point3, milieu31, etapeSuivante);
			calculSousCentresTriangleRecursif(vecteurSousCentres, milieu12, milieu23, milieu31, etapeSuivante);
		}
	}

	/**
	 * Volume
	 */
	static double calculVolumeTetrahedre(	const Point3<double> & pointA,
											const Point3<double> & pointB,
											const Point3<double> & pointC,
											const Point3<double> & pointD) {
		Vecteur<double> vAB = pointB - pointA;
		Vecteur<double> vAC = pointC - pointA;
		Vecteur<double> vAD = pointD - pointA;

//		double volume = fabs(vAB.getX() * vAC.getY() * vAD.getZ() + vAB.getZ() * vAC.getX() * vAD.getY() + vAB.getY() * vAC.getZ() * vAD.getX()
//				- vAB.getX() * vAC.getZ() * vAD.getY() - vAB.getY() * vAC.getX() * vAD.getZ() - vAB.getZ() * vAC.getY() * vAD.getX()) / 6.;
		return fabs(vAB.scalaire(vAC.vectoriel(vAD))) / 6.;
	}

	template<typename T>
	static bool contient(	const T & vecteur,
							const typename T::value_type & valeur) {
		if (!vecteur.empty()) {
			for(const typename T::value_type & element : vecteur) {
				if (element == valeur) {
					return true;
				}
			}
		}
		return false;
	}

	static double calculVolumePolyhedre(const std::vector<std::vector<Point3<double> > > & listeFace) {
// Recherche de la liste des points communs
		std::vector<Point3<double> > pointsCommuns;
		for(const std::vector<Point3<double> > & face : listeFace) {
			for(const Point3<double> & point : face) {
				if (!contient<std::vector<Point3<double> > >(pointsCommuns, point)) {
					pointsCommuns.push_back(point);
				}
			}
		}
// Calcul du centroide
		Point3<double> centroide;
		for(const Point3<double> & point : pointsCommuns) {
			centroide += point;
		}
		centroide /= pointsCommuns.size();

// Calcul des volumes entre le centroide et chaque face
		double volume = 0;
		for(const std::vector<Point3<double> > & face : listeFace) {
			Vecteur<double> normale = Vecteur<double>(face[1] - face[0]).vectoriel(Vecteur<double>(face[2] - face[0]));
			std::vector<Point3<double> > faceOrdonnee = ordonneFigureConvexeSensTrigonometrique_Const(face, normale);

			for (unsigned int i = 2; i < faceOrdonnee.size(); ++i) {
				volume += calculVolumeTetrahedre(centroide, faceOrdonnee[0], faceOrdonnee[i - 1], faceOrdonnee[i]);
			}
		}
		return volume;
	}

	static void afficheFacesPolyhedre(std::vector<std::vector<Point3<double> > > listePointsPourVolume) {
		std::cout << std::endl << "************* afficheFacesPolyhedre **************" << std::endl;
		for (unsigned int i = 0; i < listePointsPourVolume.size(); ++i) {
			std::cout << "Face : " << i << std::endl;
			for (unsigned int j = 0; j < listePointsPourVolume[i].size(); ++j) {
				std::cout << listePointsPourVolume[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "************* FIN afficheFacesPolyhedre **************" << std::endl;

	}

	static void creerObjPolyhedre(	vector<vector<Point3<double> > > listePointsPourVolume,
									string nom = "polyhedre") {
		std::ofstream fichierObj;
		string nomFichier = "/home/lauretn/" + nom + ".obj";
		fichierObj.open(nomFichier.c_str(), ios::out);
		fichierObj << "# This file uses centimeters as units for non-parametric coordinates." << endl;
		fichierObj << "mtllib Square.mtl" << endl;
		fichierObj << "g default" << endl;

		for (unsigned int i = 0; i < listePointsPourVolume.size(); ++i) {
			for (unsigned int j = 0; j < listePointsPourVolume[i].size(); ++j) {
				fichierObj << "v " << listePointsPourVolume[i][j].getX() << " " << listePointsPourVolume[i][j].getY() << " " << listePointsPourVolume[i][j].getZ() << endl;
			}
		}
		fichierObj << "s off" << endl;
		fichierObj << "usemtl initialShadingGroup" << endl;

		int indice = 1;
		for (unsigned int i = 0; i < listePointsPourVolume.size(); ++i) {
			fichierObj << "f ";
			for (unsigned int j = 0; j < listePointsPourVolume[i].size(); ++j) {
				fichierObj << indice << " ";
				++indice;
			}
			fichierObj << endl;
		}
		fichierObj.close();
	}

	static Vecteur<double> calculNormale(	const Point3<double> & p1,
											const Point3<double> & p2,
											const Point3<double> & p3) {
		Vecteur<double> vectorAB(p2);
		vectorAB -= p1;

		Vecteur<double> vectorAC(p3);
		vectorAC -= p1;

		return vectorAB.vectoriel(vectorAC).normaliser();
	}

private:
	static SimpleSphere getSphereCirconscrite(	const Point3<double> & p1,
												const Point3<double> & p2,
												const Point3<double> & p3) {
		PointIntersection<double> pointIntersection = Plan(p1, p2, p3).calculPointIntersection(Plan(p1 - p2, p1 + p2 / 2), Plan(p2 - p3, p2 + p3 / 2));
		return SimpleSphere(p1.getDistance(pointIntersection), pointIntersection);
	}

	static SimpleSphere getSphereCirconscrite(	const Point3<double> & p1,
												const Point3<double> & p2,
												const Point3<double> & p3,
												const Point3<double> & p4) {
		PointIntersection<double> pointIntersection = Plan(p1 - p2, p1 + p2 / 2).calculPointIntersection(Plan(p2 - p3, p2 + p3 / 2), Plan(p3 - p4, p3 + p4 / 2));
		return SimpleSphere(p1.getDistance(pointIntersection), pointIntersection);
	}

	static int delaunay_trouveIndicePointCreantSphereVide(	list<int> & listeIndice,
															const int & p1,
															const int & p2,
															const vector<Point3<double> > & listePoints) {
// Calcul du centre C1 de [P1-P2] - Recherche du point P3 le plus proche de C1 (sous-methode) - retire P3 de la liste intermediaire
		Point3<double> centre = (listePoints[p1] + listePoints[p2]) / 2;
		classeIndicesParProximite(listeIndice, centre, listePoints);
		int p3 = listeIndice.front();
		bool contenu;
// Calcul de la sphere circonscrite au triangle [P1-P2-P4] (sous-methode : intersection des plans mediateurs de 3 segments [P1-P2] [P2-P3] [P3-P4])
		do {
			trouveEtRetirePremiereOccurence(listeIndice, p3);
			SimpleSphere sphere = getSphereCirconscrite(listePoints[p1], listePoints[p2], listePoints[p3]);
			contenu = false;
			list<int>::const_iterator it = listeIndice.begin(), end = listeIndice.end();
			while (it != end && !contenu) {
				if (sphere.contient(listePoints[*it])) {
					p3 = *it;
					contenu = true;
				}
				++it;
			}
		} while (contenu);
		return p3;
	}

	static void afficheIndices(const list<int> & listeIndice) {
		cout << " Indices : {";
		for(const int & indice : listeIndice) {
			cout << indice << ", ";
		}
		cout << "}" << endl;
	}

	static int delaunay_trouveIndicePointCreantSphereVide(	list<int> & listeIndice,
															const int & p1,
															const int & p2,
															const int & p3,
															const vector<Point3<double> > & listePoints) {
		Point3<double> barycentre = (listePoints[p1] + listePoints[p2] + listePoints[p3]) / 3;
		afficheIndices(listeIndice);
		classeIndicesParProximite(listeIndice, barycentre, listePoints);
		afficheIndices(listeIndice);
		int p4 = listeIndice.front();
		bool contenu;
// Calcul de la sphere circonscrite au tetraedre [P1-P2-P3-P4] (sous-methode : intersection des plans mediateurs de 3 segments [P1-P2] [P2-P3] [P3-P4])
		do {
			trouveEtRetirePremiereOccurence(listeIndice, p4);
			afficheIndices(listeIndice);
			SimpleSphere sphere = getSphereCirconscrite(listePoints[p1], listePoints[p2], listePoints[p3], listePoints[p4]);
			contenu = false;
			list<int>::const_iterator it = listeIndice.begin(), end = listeIndice.end();
			while (it != end && !contenu) {
				if (sphere.contient(listePoints[*it])) {
					p4 = *it;
					contenu = true;
				}
				++it;
			}
		} while (contenu);
		return p4;
	}

	template<typename T>
	static bool trouveEtRetirePremiereOccurence(T & vecteur,
												const typename T::value_type & valeur) {
		bool found = false;
		if (!vecteur.empty()) {
			typename T::iterator it = vecteur.begin(), end = vecteur.end();
			while (!found && it != end) {
				if (*it == valeur) {
					it = vecteur.erase(it);
					found = true;
				} else {
					++it;
				}
			}
		}
		return found;
	}

	static bool retireCombinaisonLineaire(	list<Triplet<int> > & vecteur,
											const Triplet<int> & valeur) {
		bool found = false;
		if (!vecteur.empty()) {
			list<Triplet<int> >::iterator it = vecteur.begin(), end = vecteur.end();
			while (!found && it != end) {
				if ((it->getX() == valeur.getX() && // Cas 1
						((it->getY() == valeur.getY() && it->getZ() == valeur.getZ()) || (it->getY() == valeur.getZ() && it->getZ() == valeur.getY()))) || (it->getX() == valeur.getY() && // Cas 2
						((it->getY() == valeur.getX() && it->getZ() == valeur.getZ()) || (it->getY() == valeur.getZ() && it->getZ() == valeur.getX()))) || (it->getX() == valeur.getZ() && // Cas 3
						((it->getY() == valeur.getX() && it->getZ() == valeur.getY()) || (it->getY() == valeur.getY() && it->getZ() == valeur.getX())))) {
					found = true;
					it = vecteur.erase(it);
				} else {
					++it;
				}
			}
		}
		return found;
	}

	static void eliminePointsArrieresTriangle(	list<int> & listeIndice,
												const int & p1,
												const int & p2,
												const int & p3,
												const vector<Point3<double> > & listePoints) {
		Vecteur<double> normale = calculNormale(listePoints[p1], listePoints[p2], listePoints[p3]);

		list<int>::iterator it = listeIndice.begin(), end = listeIndice.end();
		Vecteur<double> vecteurP1Point;
		while (it != end) {
			vecteurP1Point = listePoints[*it] - listePoints[p1];
			if (normale.scalaire(vecteurP1Point) <= 0) {
				it = listeIndice.erase(it);
			} else {
				++it;
			}
		}
	}

	static void classeIndicesParProximite(	list<int> & listeIndice,
											const Point3<double> & p1,
											const vector<Point3<double> > & listePoints) {
		if (listeIndice.size() > 1) {
			list<CoupleIndiceDistance> listeDistance;

			for(const int & indice : listeIndice) {
				listeDistance.push_back(CoupleIndiceDistance(p1.getDistanceSquared(listePoints[indice]), indice));
			}
			listeDistance.sort();

			listeIndice.clear();
			for(const CoupleIndiceDistance & coupleIndiceDistance : listeDistance) {
				listeIndice.push_back(coupleIndiceDistance.getIndice());
			}
		}
	}

public:
	static void delaunayTriangulation(	vector<vector<Point3<double> > > & listeTriangles,
										const vector<Point3<double> > & listePoints) {
// Calcul du centroide
		Point3<double> centroide(0, 0, 0);
		list<int> listeIndiceGlobale;
		int indiceCourant = 0;
		for(const Point3<double> & vertex : listePoints) {
			centroide += vertex;
			listeIndiceGlobale.push_back(indiceCourant);
			++indiceCourant;
		}
		centroide /= listePoints.size();
		cout << "centroide : " << centroide << endl;

// Attention, travail avec les indices des points, en lieu et place des points eux-memes
// NB : equation du plan donner par : Ax + By + Cz = D, ou (A, B, C) = normale au plan, et, donc, D deduit en remplacant dans equation (x, y, z) par un point dans le plan)
// Recherche du point P1 le plus proche de ce centroide - retirer de la liste de points - Etape 1
		list<int> listeIndiceIntermediaire = listeIndiceGlobale;

		classeIndicesParProximite(listeIndiceIntermediaire, centroide, listePoints);
		int p1 = listeIndiceIntermediaire.front();
		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p1);
//		afficheIndices(listeIndiceGlobale);

		cout << "Point central : " << listePoints[p1] << endl;

// Recherche du point P2 le plus proche de P1 - retirer de la liste de points - Etape 2
//		listeIndiceIntermediaire = listeIndiceGlobale;
		classeIndicesParProximite(listeIndiceIntermediaire, listePoints[p1], listePoints);
		int p2 = listeIndiceIntermediaire.front();
		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p2);
//		afficheIndices(listeIndiceGlobale);

		cout << "Second point : " << listePoints[p2] << endl;

// Creation liste point intermediaire (indices!), copie de liste de points - Etape 3
//		listeIndiceIntermediaire = listeIndiceGlobale;
		int p3 = delaunay_trouveIndicePointCreantSphereVide(listeIndiceIntermediaire, p1, p2, listePoints);
//		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p3);
//		afficheIndices(listeIndiceGlobale);
// Si point de liste intermediaire contenu dans sphere -> devient nouveau P3 - retirer de la liste intermediaire - repete calcul sphere etc, jusqu'a aucun point liste intermediaire dans sphere (ce qui arrivera car liste intermediaire se vide)
// Si aucun point de liste contenu dans sphere -> retire P3 liste de points - Fin Etape 3

		cout << "Troisieme point : " << listePoints[p3] << endl;

// Creation liste point intermediaire (indices!), copie de liste de points - Etape 4
		listeIndiceIntermediaire = listeIndiceGlobale;
		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p1);
		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p2);
		trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, p3);
// Calcul barycentre B1 de [P1-P2-P3] - Recherche du point P4 le plus proche de B1 (sous-methode) - retirer P4 de la liste intermediaire
		int p4 = delaunay_trouveIndicePointCreantSphereVide(listeIndiceIntermediaire, p1, p2, p3, listePoints);
//		trouveEtRetirePremiereOccurence(listeIndiceGlobale, p4);
//		afficheIndices(listeIndiceGlobale);
// Si point de liste intermediaire contenu dans sphere -> devient nouveau P4 - retirer de la liste intermediaire - repete calcul sphere etc, jusqu'a aucun point liste intermediaire dans sphere (ce qui arrivera car liste intermediaire se vide)
// Si aucun point de liste contenu dans sphere -> retire P4 liste de points - Fin Etape 4

		cout << "Quatrieme point : " << listePoints[p4] << endl;

// Creation liste de triangles 'front' - Triangle du tetraedre [P1-P2-P3-P4] - penser a orienter les triangles dans sens trigo (par exemple pour triangle [P1-P2-P3] -> sens normale = sens oppose vecteur [P1-P4] - Etape 5
		list<Triplet<int> > front;
		list<Triplet<int> > back;
		Point3<double> barycentre = (listePoints[p1] + listePoints[p2] + listePoints[p3]) / 3;
		Vecteur<double> b1P4(listePoints[p4] - barycentre);
		Vecteur<double> normale = calculNormale(listePoints[p1], listePoints[p2], listePoints[p3]);
		if (b1P4.scalaire(normale) < 0) {
			front.push_back(Triplet<int>(p1, p2, p3));
			front.push_back(Triplet<int>(p4, p3, p2));
			front.push_back(Triplet<int>(p3, p4, p1));
			front.push_back(Triplet<int>(p2, p1, p4));
		} else {
			front.push_back(Triplet<int>(p1, p3, p2));
			front.push_back(Triplet<int>(p4, p2, p3));
			front.push_back(Triplet<int>(p2, p4, p1));
			front.push_back(Triplet<int>(p3, p1, p4));
		}
// Tant que liste 'front' n'est pas vide (et liste de points aussi) - [whileI]
		while (!front.empty()) {
			// recuperation premier triangle
			Triplet<int> indicesTriangle = front.back(); // on va retirer l'element de la liste => copie obligatoire. Pas de reference!
			front.pop_back();
			back.push_back(indicesTriangle);
			cout << "Triangle courant : " << listePoints[indicesTriangle.getX()] << ", " << listePoints[indicesTriangle.getY()] << ", " << listePoints[indicesTriangle.getZ()] << endl;
			// Creation liste point intermediaire (indices!), copie de liste de points !se trouvant du cote de la normale du triangle! (scalaire normale/vecteur [point sur triangle-point teste]
			listeIndiceIntermediaire = listeIndiceGlobale;
			trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, indicesTriangle.getX());
			trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, indicesTriangle.getY());
			trouveEtRetirePremiereOccurence(listeIndiceIntermediaire, indicesTriangle.getZ());

			eliminePointsArrieresTriangle(listeIndiceIntermediaire, indicesTriangle.getX(), indicesTriangle.getY(), indicesTriangle.getZ(), listePoints);
			cout << "Indice avant" << endl;
			if (listeIndiceIntermediaire.empty()) {
				cout << "Vide" << endl;
// si aucun point, retirer triangle T1 de la liste 'front' - rajouter a liste finale - retour a [whileI]
				vector<Point3<double> > triangleSurface;
				triangleSurface.push_back(listePoints[indicesTriangle.getX()]);
				triangleSurface.push_back(listePoints[indicesTriangle.getY()]);
				triangleSurface.push_back(listePoints[indicesTriangle.getZ()]);
				listeTriangles.push_back(triangleSurface);
			} else {
				cout << "Delaunay 2" << endl;
				int pN = delaunay_trouveIndicePointCreantSphereVide(listeIndiceIntermediaire, indicesTriangle.getX(), indicesTriangle.getY(), indicesTriangle.getZ(), listePoints);
//				trouveEtRetirePremiereOccurence(listeIndiceGlobale, pN);
//  retirer triangle T1 de la liste 'front'
//	calculer 3 nouveaux triangles du tetraedre - si triangle(s) deja dans 'front', retirer de 'front', sinon rajouter dans 'front' - retour a [whileI]

				cout << "Trouve et retire 1" << endl;
				Triplet<int> t2(indicesTriangle.getX(), indicesTriangle.getY(), pN);
				if (!retireCombinaisonLineaire(front, t2) /*&& !contient(back, t2)*/) {
					front.push_back(t2);
				}
				cout << "Trouve et retire 2" << endl;
				Triplet<int> t3(indicesTriangle.getY(), indicesTriangle.getZ(), pN);
				if (!retireCombinaisonLineaire(front, t3) /*&& !contient(back, t3)*/) {
					front.push_back(t3);
				}
				cout << "Trouve et retire 3" << endl;
				Triplet<int> t4(indicesTriangle.getZ(), indicesTriangle.getX(), pN);
				if (!retireCombinaisonLineaire(front, t4) /*&& !contient(back, t4)*/) {
					front.push_back(t4);
				}
//  (Si triangle depart [1-2-3] et nouveau point 4 alors nouveaux triangles : [1-2-4] - [2-3-4] - [3-1-4])
			}
		}
// fin algo
	}
};

#endif /* UTILITAIREFIGURE_HPP_ */
