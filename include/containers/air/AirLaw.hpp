/*
 * AirLaw.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */

#ifndef AIRLAW_HPP_
#define AIRLAW_HPP_

#include "libDartBase.h"
#include "NewtonCotes.hpp"
#include "../DirectionCartesienne.hpp"

#include "PointsListsNCIntegration.hpp"

class AirLaw {
private:
	vector<vector<vector<double> > > _ScatteringFunctionPerSpectralBand;
	vector<vector<vector<double> > > _ScatteringFunctionPerSpectralBandInDirecrionToOutDirection;
	static const vector<vector<vector<double> > > _EmptyScatteringFunction;

public:
	virtual ~AirLaw(){}

	virtual void computeScatteringFunction(	const vector<DirectionCartesienne> & listDirections,
											const unsigned int & nbBand,
											const unsigned int & nbSector) {
		_ScatteringFunctionPerSpectralBand.resize(nbBand, vector<vector<double> > (nbSector, vector<double> (
			listDirections.size() - 1, 0.)));
			_ScatteringFunctionPerSpectralBandInDirecrionToOutDirection.resize(nbBand, vector<vector<double> > (listDirections.size(), vector<double> (
				listDirections.size() - 1, 0.)));
		double sommeADiviser, valeurFonctionPhase;
		//number of points used for each dimension of newton-cotes integration.
		unsigned int numDiv = round(sqrt(10000.0 / listDirections.size()));
		if (numDiv < 2) {
			numDiv = 2;
		}
		PointsListsNCIntegration<DirectionCartesienne> pointsList(listDirections, numDiv,true);
		double unSur4PI = 1. / (4 * M_PI);

		for (unsigned int indBS = 0; indBS < nbBand; ++indBS) {
			vector < vector<double> > biDirectionnalScatteringFunction(listDirections.size(), vector<double> (
				listDirections.size() - 1, 0.));
			for (unsigned int directionI = 0; directionI < listDirections.size(); ++directionI) {
				//Pour toutes les directions d'entree du rayon
				sommeADiviser = 0;

				if (!listDirections[directionI].isVirtual() || (directionI == listDirections.size() - 1)) {
					for (unsigned int directionV = 0; directionV < listDirections.size() - 1; directionV++) {
						// pour toute direction de sortie (exceptee la direction solaire (on ne diffuse pas dans la direction solaire)
						vector < vector<double> > ffMap(numDiv, vector<double> (numDiv, 1));
						for (unsigned int i = 0; i < numDiv; i++) {
							for (unsigned int j = 0; j < numDiv; j++) {
								ffMap[i][j] = this->calculFonctionPhase(
									listDirections[directionI].getDirectionCartesienne().scalaire(
										pointsList.getPointsMapCartesian(directionV)[i][j]), indBS);
							}
						}
						NewtonCotesIntegrationCalculator<double> integration(ffMap, pointsList.getDeltaCosTheta(
							directionV), pointsList.getDeltaPhi(directionV) * 2);

						integration.setInput(ffMap);
						integration.setDeltaX(pointsList.getDeltaCosTheta(directionV));
						integration.setDeltaY(pointsList.getDeltaPhi(directionV) * 2);
						integration.calculateIntegration2D();
						valeurFonctionPhase = integration.getOutput() * unSur4PI;
						biDirectionnalScatteringFunction[directionI][directionV] = valeurFonctionPhase;

						if (!listDirections[directionV].isVirtual()) {
							sommeADiviser += valeurFonctionPhase;
						}
					}
					if (sommeADiviser == 0.) {
						sommeADiviser = 1.;
					}
					for (unsigned int directionV = 0; directionV < listDirections.size() - 1; directionV++) {
						// pour toute direction de sortie (exceptee la direction solaire (on ne diffuse pas dans la direction solaire)
						//						if (!listDirections[directionV].isVirtual()) {
						biDirectionnalScatteringFunction[directionI][directionV] /= sommeADiviser;
						//						}
					}
				}
			}
			_ScatteringFunctionPerSpectralBandInDirecrionToOutDirection[indBS] = biDirectionnalScatteringFunction;

			vector<double> solidAngleSumPerSector(nbSector, 0.);
			for (unsigned int directionI = 0; directionI < listDirections.size(); ++directionI) {
				if (listDirections[directionI].getProperty() != DirectionRoot::VIRTUAL) {
					solidAngleSumPerSector[listDirections[directionI].getAngularSector()]
							+= listDirections[directionI].getSolidAngle();
					for (unsigned int directionV = 0; directionV < listDirections.size() - 1; directionV++) {
						_ScatteringFunctionPerSpectralBand[indBS][listDirections[directionI].getAngularSector()][directionV]
								+= biDirectionnalScatteringFunction[directionI][directionV]
										* listDirections[directionI].getSolidAngle();
					}
				} else if (directionI == listDirections.size() - 1) {
					for (unsigned int directionV = 0; directionV < listDirections.size() - 1; directionV++) {
						_ScatteringFunctionPerSpectralBand[indBS][listDirections[directionI].getAngularSector()][directionV]
								+= biDirectionnalScatteringFunction[directionI][directionV];
					}
				}
			}
			for (unsigned int sector = 0; sector < nbSector; ++sector) {
				if (solidAngleSumPerSector[sector] > 0.) {
					for (unsigned int directionV = 0; directionV < listDirections.size() - 1; directionV++) {
						//						if (!listDirections[directionV].isVirtual()) {
						_ScatteringFunctionPerSpectralBand[indBS][sector][directionV] /= solidAngleSumPerSector[sector];
						//						}
					}
				}
			}
		}
	}

	virtual const double calculFonctionPhase(	const double & cosinusPsy,
												const unsigned int indSpectralBand) const = 0;

	virtual const unsigned int getType() const = 0;

	virtual void clearCoefficients() = 0;

	// Matrix per incident direction, per viewing direction per spectral band : SF[dirInc][dirView][SpectBand]
	const vector<vector<vector<double> > > & getScatteringFunction() {
		return _ScatteringFunctionPerSpectralBand;
	}
	const vector<vector<vector<double> > > & getScatteringFunctionInDirectionToOutDirection() {
		return _ScatteringFunctionPerSpectralBandInDirecrionToOutDirection;
	}

	void setScatteringFactor(	const unsigned int indSpectralBand,
								const unsigned int angularSector,
								const unsigned int indViewingDirection,
								const double & factor) {
		_ScatteringFunctionPerSpectralBand[indSpectralBand][angularSector][indViewingDirection] = factor;
	}

	virtual shared_ptr<AirLaw> clone() const = 0;

	// Matrix per incident direction, per viewing direction per spectral band : SF[dirInc][dirView][SpectBand]
	static const vector<vector<vector<double> > > & getEmptyScatteringFunction() {
		return _EmptyScatteringFunction;
	}
};

#endif /* AIRLAW_HPP_ */
