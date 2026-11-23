/*
 * FoliarComputation.hpp
 *
 *  Created on: 26 juin 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_FOLIAR_FOLIARCOMPUTATION_HPP_
#define INCLUDE_FOLIAR_FOLIARCOMPUTATION_HPP_

#define PHI_MAX 360 // Borne superieure maximale de l'azimut foliaire
#define SPHERIC_CASE 1

#include <stdio.h>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include "libDartEnv.h"
#include <boost/thread.hpp>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include "DartEnv.h"
#include "../include/PhaseUtils.hpp"
#include "containers/DirectionCartesienne.hpp"
#include "Doublet.hpp"
#include "containers/SphereDiscretizationMap.hpp"
#include "containers/DirectionGroups.hpp"
#include "NewtonCotes.hpp"
#include "PointsListsNCIntegration.hpp"
#include "PhaseUtils.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"

#include "database/atmosphereModel/AtmosphericDatabaseStructure.hpp"
#include "../../librairieDart/include/databases/DartDatabaseManager.hpp"
#include "database/PhaseDatabaseDAO.hpp"

#include "containers/atmosphere/AtmosphereComposition.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/FoliarDatabaseParameters.hpp"
#include "database/model/SpecularFoliarDatabaseParameters.hpp"

#include "FoliarModel.hpp"

#include "PhaseProperty.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
using namespace boost;
#endif

class FoliarComputation {
private:
	unsigned int _numberOfSpectralBands;
	unsigned int _numberOfFoliarModels;
	int _numberOfDirections;
	unsigned int _numberOfAngularSectors;
	std::vector<double> _albedoVector;
	std::vector<double> _albedoSpecVector;

	std::vector<std::vector<std::vector<std::vector<double> > > > _preDiffuseTransfertFunctionVector;

	std::vector<std::vector<std::vector<double> > > _diffuseTransfertFunctionVector;
	std::vector<std::vector<std::vector<double> > > _specularTransfertFunctionVector;
	std::vector<std::vector<std::vector<double> > > _polTransfertFunctionVector;
	std::vector<double> _clumpingIndex;

	std::vector<std::vector<double> > _foliarPrecomputedNormalisation;
	std::vector<std::vector<std::vector<double> > > _foliarCosinePsiVector;
	std::vector<std::vector<std::vector<double> > > _tIntspecFeuille;
	std::vector<std::vector<std::vector<double> > > _tIntdiffFeuille;
	std::vector<std::vector<double> > _tgOmegas;
	std::vector<std::vector<double> > _tgsOmegas;
	std::vector<std::vector<double> > _tgsOmegasNormalized;

	std::vector<double> _sineIntegrandVector;
	std::vector<std::vector<std::vector<double> > > _precomputedCosinePsiSaVector;

	double _rad2Deg;

	int _nbDisplayedWarningMessages;
//	int MAX_WARNING_MESSAGE;

	unsigned int _numberOfThreads;
	std::vector<unsigned int> _threadEnd;
	std::vector<DirectionCartesienne> _listDirections;

	std::vector<double> _hotSpotFactor;

	std::vector<double> & _leafAngleDistributionFunctionVector;
	std::vector<double> & _gFunctionForLadComputationFunctionVector;
	std::vector<FoliarModel::ParametersForFoliarModels> & _parametersForFoliarModels;
	std::vector<double> & _transmTransfertFunctionVector;

	bool _topFluorescenceComputation;
	bool _bottomFluorescenceComputation;
	std::vector<std::vector<std::vector<double> > > _frontFaceFluorescenceMatrix; // Spectral Band In, Spectral Band Out, [backward PSI, backward PSII, forward PSI, forwardPSII]
	std::vector<std::vector<std::vector<double> > > _backFaceFluorescenceMatrix; // Spectral Band In, Spectral Band Out, [backward PSI, backward PSII, forward PSI, forwardPSII]
	std::vector<std::vector<double> > _fluorescenceDirectionTransferMatrixPSI; // Spectral Band In * nbSpectralBands + Spectral Band Out, direction In * (nbDirections + 1) + Direction Out
	std::vector<std::vector<double> > _fluorescenceDirectionTransferMatrixPSII;// Spectral Band In * nbSpectralBands + Spectral Band Out, direction In * (nbDirections + 1) + Direction Out
	FoliarModel & _foliarModel;

public:
	FoliarComputation(FoliarModel & foliarModel) : _numberOfSpectralBands(1), _numberOfFoliarModels(1), _numberOfDirections(100), _numberOfAngularSectors(10), _rad2Deg(180. / M_PI), _nbDisplayedWarningMessages(0), _numberOfThreads(1), _leafAngleDistributionFunctionVector(foliarModel.getRefLeafAngleDistributionFunctionVecto()), _gFunctionForLadComputationFunctionVector(foliarModel.getRefGFunctionForLadComputationFunctionVector()), _parametersForFoliarModels(
				foliarModel.getRefParametersForFoliarModels()), _transmTransfertFunctionVector(foliarModel.getRefTransmTransfertFunctionVector()), _topFluorescenceComputation(false), _bottomFluorescenceComputation(false), _foliarModel(foliarModel) {
	}

	~FoliarComputation() {}

	void initFoliarComputation(	const unsigned int & numberOfDirections,
								const unsigned int & numberOfSpectralBands,
								const unsigned int & numberOfFoliarModels,
								const unsigned int & numberOfThreads,
								std::vector<unsigned int> & threadEnd,
								FoliarModel & foliarModel);

	void initFluorescenceMatrix();

	void reInitFoliarComputation();

	void runComputeFoliarPerThread(	std::vector<FoliarProperty> & foliarProperty,
									const unsigned int & modelIndex,
									const FTFoliar & fTFoliar,
									OpticalPropertyParameters & foliarPropertyParameters,
									const std::vector<DirectionCartesienne> & listDirections,
									const SphereDiscretizationMap & sphereMap,
									const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
									const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff);

	void computeFoliarPerThread(std::vector<FoliarProperty> & foliarProperty,
								const unsigned int modelIndex,
								const FTFoliar & fTFoliar,
								OpticalPropertyParameters & foliarPropertyParameters,
								const SphereDiscretizationMap & sphereMap,
								const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
								const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
								const unsigned int threadNumber);

	void computeFoliarPerBand(	FoliarProperty & foliarProperty,
								const unsigned int & spectralBandIndex,
								const FTFoliar & fTFoliar,
								OpticalPropertyParameters & foliarPropertyParameters,
								const std::vector<DirectionCartesienne> & listDirections,
								const SphereDiscretizationMap & sphereMap,
								const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
								const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff);

	void preComputeFRFoliar(const FTFoliar & fTFoliar,
							const unsigned int & modelIndex,
							const std::vector<DirectionCartesienne> & listDirections);

	void preComputeTransfertFunctionOfFoliarDiffuse(const std::vector<DirectionCartesienne> & listDirections,
													const int & incidentDirection,
													const int & outputDirection,
													const int & iterationStepPhi,
													const int & iterationStepTheta,
													const unsigned int & modelIndex);

	void computeFRFoliarSpecular(	const FTFoliar & fTFoliar,
									const unsigned int & spectralBandIndex,
									const unsigned int & modelIndex,
									const FoliarProperty & opticalProperties,
									const std::vector<DirectionCartesienne> & listDirections,
									const SphereDiscretizationMap & sphereMap,
									const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
									const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff);

	void diffuseFoliarTransferFunctionFromIntegration(	const std::vector<DirectionCartesienne> & listDirections,
														const int & incidentDirection,
														const int & outputDirection,
														const FoliarProperty & opticalProperties,
														const FTFoliar & fTFoliar,
														const SphereDiscretizationMap & sphereMap,
														const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
														const std::vector<std::vector<double> > & cosinePsiSaVector,
														const unsigned int & modelIndex,
														const unsigned int & reflectanceMode);
	void fluorescenceTransferFunctionFromIntegration(	const std::vector<DirectionCartesienne> & listDirections,
														const int & incidentDirection,
														const int & outputDirection,
														const FoliarProperty & opticalProperties,
														const FTFoliar & fTFoliar,
														const SphereDiscretizationMap & sphereMap,
														const PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
														const std::vector<std::vector<double> > & cosinePsiSaVector,
														const unsigned int & modelIndex,
														const unsigned int & reflectanceMode);
	void computeTransfertFunctionOfFoliarDiffuse(	const std::vector<DirectionCartesienne> & listDirections,
													const int & incidentDirection,
													const int & outputDirection,
													const FoliarProperty & opticalProperties,
													const int & iterationStepPhi,
													const int & iterationStepTheta,
													const unsigned int & modelIndex,
													const unsigned int & reflectanceMode);

	double computeIntegranddTheta(	const int & incidentDirection,
									const int & outputDirection,
									const FoliarProperty & opticalProperties,
									const int & iterationStepPhi,
									const int & iterationStepTheta,
									const unsigned int & modelIndex);

	double functiondTheta(	const int & thetaF,
							const int & incidentDirection,
							const int & outputDirection,
							const FoliarProperty & opticalProperties,
							const int & iterationStepPhi,
							const int & iterationStepTheta,
							const unsigned int & modelIndex);

	double computeIntegranddPhi(const int & thetaF,
								const int & incidentDirection,
								const int & outputDirection,
								const FoliarProperty & opticalProperties,
								const int & iterationStepPhi);

	double functionOfFoliarDiffuse(	const int & thetaF,
									const int & phiF,
									const int & incidentDirection,
									const int & outputDirection,
									const FoliarProperty & opticalProperties);

	void computeAndStoreTgOmegas(	const std::vector<DirectionCartesienne> & listDirections,
									int specularCheck,
									unsigned int modelIndex,
									unsigned int spectralBandIndex);

	void computeSpecularPolTransferFoliarFromIntegration(	const std::vector<DirectionCartesienne> & listDirections,
															const int & incidentDirection,
															const int & outputDirection,
															const FoliarProperty & opticalProperties,
															const int & casPDF,
															const unsigned int & numDiv,
															const PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
															const unsigned int & modelIndex,
															const unsigned int & reflectanceMode);

	void foliarPolaSpecTranferFunction(	const std::vector<DirectionCartesienne> & listDirections,
										const int & incidentDirection,
										const int & outputDirection,
										const FoliarProperty & opticalProperties,
										const int & casPDF,
										const unsigned int & modelIndex,
										const unsigned int & reflectanceMode);

	void tempAngleSpecularFoliar(	const DirectionCartesienne & incidentDir,
									const Doublet<double> & thePhi,
									double & PSIfs,
									double & ZFs);

	Vecteur<double> sphericalToCartesian(	const double & zenith,
											const double & azimuth);

	void computeFresnelCoefficientsSpecPol(	double & Rspe,
											double & Rpola,
											double & PSIfs,
											double & refractionIndex);

	void windmillForOccupiedDirectionIntegration(	std::vector<DirectionCartesienne> & dirList,
													const DirectionRoot & emDir);

	void calcDotPSIIntegrateSA(	const std::vector<std::vector<Vecteur<double> > > & dirsTPMatrix,
								const SphereDiscretizationMap & sphereMap,
								std::vector<std::vector<double> > & cosinePsiSaVector,
								const double & DELTA_AF);

	void generatePhaseMapDiff(	const std::vector<std::vector<double> > & cosinePsiSaVector,
								const unsigned int numberOfThetaSubdivisions,
								const unsigned int numberOfPhiSubdivisions,
								const int & incidentDirection,
								const FoliarProperty & opticalProperties,
								const SphereDiscretizationMap & sphereMap,
								std::vector<std::vector<double> > & phaseMap,
								const double & iterationStepPhi,
								const double & iterationStepTheta,
								const unsigned int & modelIndex);

	void generatePhaseMapFluo(	const std::vector<std::vector<double> > & cosinePsiSaVector,
								const unsigned int numberOfThetaSubdivisions,
								const unsigned int numberOfPhiSubdivisions,
								const int & incidentDirection,
								const FoliarProperty & opticalProperties,
								const SphereDiscretizationMap & sphereMap,
								std::vector<std::vector<double> > & phaseMapPSI,
								std::vector<std::vector<double> > & phaseMapPSII,
								const unsigned int & emissionBandInex,
								const double & iterationStepPhi,
								const double & iterationStepTheta,
								const unsigned int & modelIndex);

	void testAlbedo(const unsigned int & spectralBandIndex,
					const unsigned int & modelIndex);

	double getAlbedo(const unsigned int & spectralBandIndex);

	double getAlbedoSpec(const unsigned int & spectralBandIndex);

	void writeFoliar(	const unsigned int& modelIndex,
						const unsigned int& spectralBandIndex,
						const std::string & modelName,
						PhaseKeyMappingGenerator & pkmg,
						std::ofstream & outCoeffSpecProperties,
						std::vector<FoliarProperty> & foliarPropertyVector);

	void precomputeFoliarNormalisation(	const std::vector<DirectionCartesienne> & listDirections,
										const double & iterationStepPhi,
										const double & iterationStepTheta);

	void computeFoliarCosinePsi(const std::vector<DirectionCartesienne> & listOfDirections,
								const int & iterationStepPhi,
								const int & iterationStepTheta);

	void computeFRFoliarSpecularForModelIndex(	PointsListsNCIntegration<DirectionCartesienne> & pointsListSpec,
												PointsListsNCIntegration<DirectionCartesienne> & pointsListDiff,
												const std::vector<DirectionCartesienne> & listDirections,
												FTFoliar & fTFoliar,
												unsigned int modelIndex,
												const FoliarProperty & foliarProperty);

	void writeTransmittance(const FTFoliar & fTFoliar,
							const std::vector<DirectionCartesienne> & listDirections,
							const int & rtm,
							const unsigned int & accelEngine,
							const bool &isSpecular,
							const string & pathPhaseTransm,
							const string & pathPhaseDiff,
							const string & pathPhaseSpec,
							const string & pathHotSpot,
							const string & pathFluorescence);

	void computeClumpingIndex(	const std::vector<DirectionCartesienne> & listDirections,
								const DirectionCartesienne & sunDirection,
								const unsigned int & modelIndex);

	void transmissionTransferFunction(	const double & theta,
										const double & phi,
										const int & incidentDirection,
										const int modelIndex);

	double integrationWithSimpsonMethod_dPhif(	const double & theta,
												const unsigned int & modelIndex);

	double integrationWithSimpsonMethod_dTheta_f(	const double & theta,
													const double & phi_f,
													const unsigned int & modelIndex);

	double integrandOf_gTheta(	const double & theta,
								const double & theta_f,
								const double & phi_f,
								const unsigned int & modelIndex);

	double scalarProductOfDirections(	const double & theta,
										const double & theta_f,
										const double & phi_f);

	double integrationWithTrapezoidalMethod_dTheta_f(	const double & theta,
														const double & phi_f,
														const unsigned int & modelIndex);

//	void precomputeAbsoluteDotProducts_LeafNormal_Direction(vector<vector<double> > & scalars, const DirectionCartesienne & direction);
	void computeFoliarHotSpotDirectionalFactor(	unsigned int modelIndex,
												const std::vector<DirectionCartesienne> & listDirections,
												const int & iterationStepPhi,
												const int & iterationStepTheta);

	const std::vector<double>& getHotSpotFactor() const {
		return _hotSpotFactor;
	}

	void setHotSpotFactor(const std::vector<double>& hotSpotFactor) {
		_hotSpotFactor = hotSpotFactor;
	}

	void computeFluorescenceTransferFunction(const std::vector<DirectionCartesienne> & listDirections,
																const int & incidentDirection,
																const int & outputDirection,
																const FoliarProperty & opticalProperties,
																const int & iterationStepPhi,
																const int & iterationStepTheta,
																const unsigned int & modelIndex,
																const unsigned int & reflectanceMode);
	bool isTopFluorescenceComputation() const {
		return _topFluorescenceComputation;
	}
	void setTopFluorescenceComputation(bool fluorescenceComputation) {
		_topFluorescenceComputation = fluorescenceComputation;
	}
	bool isBottomFluorescenceComputation() const {
		return _bottomFluorescenceComputation;
	}
	void setBottomFluorescenceComputation(bool fluorescenceComputation) {
		_bottomFluorescenceComputation = fluorescenceComputation;
	}

	const std::vector<std::vector<double> > & getFluorescenceDirectionTransferMatrixPSI() const {
		return _fluorescenceDirectionTransferMatrixPSI;
	}
	const std::vector<std::vector<double> > & getFluorescenceDirectionTransferMatrixPSII() const {
		return _fluorescenceDirectionTransferMatrixPSII;
	}
	void setFluorescenceDirectionTransferMatrixPSI(const std::vector<std::vector<double> >& fluorescenceTransferMatrix) {
		_fluorescenceDirectionTransferMatrixPSI = fluorescenceTransferMatrix;
	}
	void setFluorescenceDirectionTransferMatrixPSII(const std::vector<std::vector<double> >& fluorescenceTransferMatrix) {
		_fluorescenceDirectionTransferMatrixPSII = fluorescenceTransferMatrix;
	}

	std::vector<std::vector<std::vector<double> > >& getBackFaceFluorescenceMatrix() {
		return _backFaceFluorescenceMatrix;
	}
	const std::vector<std::vector<std::vector<double> > >& getBackFaceFluorescenceMatrix() const {
		return _backFaceFluorescenceMatrix;
	}

	double getLeafAngleDistributionForAngle(const double & theta) const;

	void setBackFaceFluorescenceMatrix(const std::vector<std::vector<std::vector<double> > >& backFaceFluorescenceMatrix) {
		_backFaceFluorescenceMatrix = backFaceFluorescenceMatrix;
	}
	void resetBackFaceFluorescenceMatrix(unsigned nbSpectralBand);

	std::vector<std::vector<std::vector<double> > >& getFrontFaceFluorescenceMatrix() {
		return _frontFaceFluorescenceMatrix;
	}
	const std::vector<std::vector<std::vector<double> > >& getFrontFaceFluorescenceMatrix() const {
		return _frontFaceFluorescenceMatrix;
	}
	void setFrontFaceFluorescenceMatrix(const std::vector<std::vector<std::vector<double> > >& frontFaceFluorescenceMatrix) {
		_frontFaceFluorescenceMatrix = frontFaceFluorescenceMatrix;
	}
	void resetFrontFaceFluorescenceMatrix(unsigned nbSpectralBand);
};

#endif /* INCLUDE_FOLIAR_FOLIARCOMPUTATION_HPP_ */
