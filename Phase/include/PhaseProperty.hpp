/*
 * PhaseProperty.hpp
 *
 *  Created on: Oct 10, 2013
 *      Author: yint
 */

#ifdef __CDT_PARSER__
#undef BOOST_FOREACH
#define BOOST_FOREACH(a, b) for(a; ; )
#endif
#ifndef PHASEPROPERTY_HPP_
#define PHASEPROPERTY_HPP_
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include "libDartEnv.h"
#include <boost/thread.hpp>
#include "PhaseStructure.hpp"
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
#include "PhaseDTOs.hpp"
#include "PhaseUtils.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"

#include "database/atmosphereModel/AtmosphericDatabaseStructure.hpp"
#include "../../librairieDart/include/databases/DartDatabaseManager.hpp"
#include "database/PhaseDatabaseDAO.hpp"

#include "containers/atmosphere/AtmosphereComposition.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/FoliarDatabaseParameters.hpp"
#include "database/model/SpecularFoliarDatabaseParameters.hpp"
#include "database/model/PhaseExternDatabaseParameters.hpp"

#include "../../librairieDart/include/databases/crossSection/CrossSectionDatabaseDAO.hpp"

//typedef double Tab[POLY_LEGENDRE];

#define phaseFunctionMolecule 0
//#define phaseFunctionAerosol 1

//Netcdf
#ifdef DART_WITH_NETCDF
#include "../../librairieDart/include/namespaces/netcdf/AtmosphereNcKeys.hpp"
#include "../../librairieDart/include/netcdf/DartNetcdf.hpp"
#include <netcdf>
using namespace netCDF;
using namespace AtmosphereNcKeys;
#endif

using namespace soci;

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
using namespace boost;
#endif

class PhaseProperty {
public:
	typedef std::vector<std::pair<double, double> > listOfYields;

//private:
	static double _Epulse;
	static double _sunZenithAngleInRadians;
	static std::vector<SolarIrradianceValues> _solarIrradianceValues;
	static bool _isComputedIrradianceBottomOfAtmosphereCheck;
	static int _irradianceFromDatabaseComputationMethod;
	static int _calculatorMethod;
	static bool _ifSimuSolarNoise;
	static unsigned int _numberOfSpectralBands;
	static std::string _tableSolarIrradiance;
	static std::string _columnSolarIrradiance;
	static std::string _solarConstantDatabaseName;
	static int _computationMethodeOfToaToBoa;
	static std::string _completeNameDatabase;

// User spectral bands
	static unsigned int _numberOfUserSpectralBands;
	static vector<UserSpectralBand> _userSpectralBands;

	static unsigned int _numberOfThreads;
	static std::vector<MultiplicativeFactorForRpv> _multiplicativeFactorForRpv;
	static std::vector<MultiplicativeFactorForSpecular> _multiplicativeFactorForSpecular;
	static std::vector<MultiplicativeFactorForWaterInterface> _multiplicativeFactorForWaterInterface;
	static std::vector<MultiplicativeFactorForLambertien> _multiplicativeFactorForLambertien;
	static std::vector<MultiplicativeFactorForHapke> _multiplicativeFactorForHapke;
	static std::vector<MultiplicativeFactorForHapke2> _multiplicativeFactorForHapke2;
	static ReflectanceModelsParameters _reflectanceModelsParameters;
	static std::vector<unsigned int> _threadEnd;
//	static int _numberOfDirections;
//	static int _numberOfAngularSectors;
	static int _numberOfAla;
	static int _nthAla;
	static std::string _atmosphericDatabasePath;
	static bool _weightingOfAtmosphereParameters;
	static bool _weightingOfReflectanceParameters;
	static time_t _tInit;
	static time_t _tMiddle;
	static unsigned int _tModulo;
	static double _tToPercent;
	static unsigned int _tIteration;
	static std::vector<std::vector<std::vector<double> > > _waterInterfacePhaseFunction;
	static std::vector<std::vector<std::vector<std::vector<double> > > > _atmospherePhaseFunction;
	static unsigned int _numberOfAerosols;
	static std::vector<std::vector<double> > _parameterForAerosolPhaseFunctionG1;
	static std::vector<std::vector<double> > _parameterForAerosolPhaseFunctionG2;
	static std::vector<std::vector<double> > _parameterForAerosolPhaseFunctionA;
	static int _nbDisplayedWarningMessages;
	static bool _polarization;
	static bool _fluorescence;
	static bool _combinedPS;
	static listOfYields _fluorescenceYields;
	static std::string _fluorescenceFileName;
	static boost::mutex _atmosphereMutex;

	/**
	 * Mutexed increment related ti the phase extern direction reading.
	 */
	static boost::mutex _phaseExternMutex;
	static unsigned int _phaseExternMutexedIncrement;

//public:
	static const std::string CONNEXION_ERREUR_VERROU;
	static const int CONNEXION_MAX_ESSAI; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
	static const int CONNEXION_ATTENTE; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)
	static const int MAX_WARNING_MESSAGE; // anti spam

	static const std::vector<std::vector<std::vector<std::vector<double> > > > & getAtmospherePhaseFunction() {
		return _atmospherePhaseFunction;
	}

	static const listOfYields & getFluorescenceYields() {
		return _fluorescenceYields;
	}

	static void setTimeInit(const time_t & tInit) {
		_tInit = tInit;
	}
	/* Modulo for printing progress*/
	static void setParametersForPrintingProgress() {
		unsigned int totalNumberOfIterations = (_reflectanceModelsParameters.numberOfFoliarModels + _reflectanceModelsParameters.numberOfRPVModels
				+ _reflectanceModelsParameters.numberOfHapkeModels + + _reflectanceModelsParameters.numberOfHapke2Models + _reflectanceModelsParameters.numberOfLambertianModels + _reflectanceModelsParameters.numberOfSpecularModels
				+ _reflectanceModelsParameters.numberOfPhaseExternModels) * _numberOfSpectralBands;
		if (totalNumberOfIterations > 20) {
			_tModulo = totalNumberOfIterations / 20;
		} else {
			_tModulo = 1;
		}
		_tToPercent = 0;
		_tIteration = 0;
	}

	static void checkProgress() {
		if (_tIteration > _tModulo) {
			unsigned int moduloFactor = _tIteration / _tModulo;
			_tToPercent += 5 * moduloFactor;
			if (_tToPercent > 100) {
				_tToPercent = 100;
			}
			time(&_tMiddle);
			_tIteration = _tIteration % _tModulo;
			int processingTimeMiddle;
			processingTimeMiddle = _tMiddle - _tInit;
			cout << "	" << _tToPercent << " % ";
			if (processingTimeMiddle >= 3600) {
				cout << "  -> Processing Time : " << processingTimeMiddle / 3600 << " hours " << (processingTimeMiddle % 3600) / 60 << " minutes "
						<< processingTimeMiddle % 60 << "  seconds" << endl;
			} else if (processingTimeMiddle >= 60) {
				cout << "  -> Processing Time : " << (processingTimeMiddle % 3600) / 60 << " minutes " << processingTimeMiddle % 60 << "  seconds"
						<< endl;
			} else {
				cout << "  -> Processing Time : " << processingTimeMiddle % 60 << "  seconds" << endl;
			}
		}
	}

	static void reinit() {
		_sunZenithAngleInRadians = 0;
		_isComputedIrradianceBottomOfAtmosphereCheck = false;
		_irradianceFromDatabaseComputationMethod = -1;
		_ifSimuSolarNoise = false;
		PhaseUtils::setOutputFolder("lib_phase");
		_solarIrradianceValues.clear();
		_multiplicativeFactorForRpv.clear();
		_multiplicativeFactorForSpecular.clear();
		_multiplicativeFactorForLambertien.clear();
		_multiplicativeFactorForHapke.clear();
		_multiplicativeFactorForHapke2.clear();
		_threadEnd.clear();
		_numberOfSpectralBands = 0;
		_numberOfThreads = 1;
		_polarization = false;
		_fluorescence = false;
	}

	static void resizeSolarIrradianceValues(unsigned int& n) {
		_solarIrradianceValues.resize(n);
		for (auto & siv : _solarIrradianceValues) {
			siv.temperatureAtmosphereAbsorption = 1.;
		}
		_numberOfSpectralBands = n;
	}

	static void initiateSolarIrradianceValues() {
		for (unsigned int k = 0; k < _numberOfSpectralBands; k++) {
			_solarIrradianceValues[k].meanIrradianceTopOfAtmosphere = 0;
			_solarIrradianceValues[k].meanIrradianceBottomOfAtmosphere = 0;
			_solarIrradianceValues[k].numberOfIrradianceSpectralValues = 0;
			_solarIrradianceValues[k].numberOfGasSpectralValue = 0;
			_solarIrradianceValues[k].irradianceBottomOfAtmosphereVector.clear();
			_solarIrradianceValues[k].irradianceTopOfAtmosphereVector.clear();
			_solarIrradianceValues[k].irradianceForPonderationVector.clear();
			_solarIrradianceValues[k].irradianceSpectralWavelengthVector.clear();
			_solarIrradianceValues[k].skylVector.clear();
			_solarIrradianceValues[k].temperatureAtmosphereAbsorptionVector.clear();
		}
	}

	static unsigned int getThreadEnd(const unsigned int& spectralBand) {
		return _threadEnd[spectralBand];
	}

	static unsigned int getNumberOfAerosol() {
		return _numberOfAerosols;
	}
	/* LiDAR pulse energy J*/
	static double getLiDARpulseEnergy(){
		return _Epulse;
	}

	static unsigned int getNumberOfThreads() {
		return _numberOfThreads;
	}

	/* Lambertian multiplicative factor */
	static void setDiffuseTransmittanceAccelerationForLambertian(	const unsigned int& indice,
																	const double & value) {
		_multiplicativeFactorForLambertien[indice].diffuseTransmittanceAccelerationFactor = value;
	}
	static double getDiffuseTransmittanceAccelerationForLambertian(const unsigned int& indice) {
		return _multiplicativeFactorForLambertien[indice].diffuseTransmittanceAccelerationFactor;
	}

	static void setMultiplicativeFactorReflectanceForLambertian(const unsigned int& indice,
																const unsigned int& spectralBand,
																const double & value) {
		_multiplicativeFactorForLambertien[indice].reflectanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorReflectanceForLambertian(	const unsigned int& indice,
																	const unsigned int& spectralBand) {
		return _multiplicativeFactorForLambertien[indice].reflectanceVector[spectralBand];
	}

	static void setMultiplicativeFactorDirectTransmittanceFactoForLambertian(	const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
		_multiplicativeFactorForLambertien[indice].directTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDirectTransmittanceFactoForLambertian(	const unsigned int& indice,
																				const unsigned int& spectralBand) {
		return _multiplicativeFactorForLambertien[indice].directTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorDiffuseTransmittanceFactoForLambertian(	const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
		_multiplicativeFactorForLambertien[indice].diffuseTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDiffuseTransmittanceFactoForLambertian(const unsigned int& indice,
																				const unsigned int& spectralBand) {
		return _multiplicativeFactorForLambertien[indice].diffuseTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorSpecularIntensityForLambertian(	const unsigned int& indice,
																		const unsigned int& spectralBand,
																		const double & value) {
		_multiplicativeFactorForLambertien[indice].specularIntensityVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorSpecularIntensityForLambertian(const unsigned int& indice,
																		const unsigned int& spectralBand) {
		return _multiplicativeFactorForLambertien[indice].specularIntensityVector[spectralBand];
	}

	/* Hapke multiplicative factor */
	static void setMultiplicativeFactorWForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].W[spectralBand] = value;
	}
	static double getMultiplicativeFactorWForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].W[spectralBand];
	}

	static void setMultiplicativeFactorC1ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].C1[spectralBand] = value;
	}
	static double getMultiplicativeFactorC1ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].C1[spectralBand];
	}

	static void setMultiplicativeFactorC2ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].C2[spectralBand] = value;
	}
	static double getMultiplicativeFactorC2ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].C2[spectralBand];
	}

	static void setMultiplicativeFactorC3ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].C3[spectralBand] = value;
	}
	static double getMultiplicativeFactorC3ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].C3[spectralBand];
	}

	static void setMultiplicativeFactorC4ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].C4[spectralBand] = value;
	}
	static double getMultiplicativeFactorC4ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].C4[spectralBand];
	}

	static void setMultiplicativeFactorH1ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].H1[spectralBand] = value;
	}
	static double getMultiplicativeFactorH1ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].H1[spectralBand];
	}

	static void setMultiplicativeFactorH2ForHapke(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForHapke[indice].H2[spectralBand] = value;
	}
	static double getMultiplicativeFactorH2ForHapke(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].H2[spectralBand];
	}

	static void setMultiplicativeFactorDirectTransmittanceFactorForHapke(	const unsigned int& indice,
																			const unsigned int& spectralBand,
																			const double & value) {
		_multiplicativeFactorForHapke[indice].directTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDirectTransmittanceFactorForHapke(	const unsigned int& indice,
																			const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].directTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorDiffuseTransmittanceFactorForHapke(	const unsigned int& indice,
																			const unsigned int& spectralBand,
																			const double & value) {
		_multiplicativeFactorForHapke[indice].diffuseTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDiffuseTransmittanceFactorForHapke(const unsigned int& indice,
																			const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].diffuseTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorSpecularIntensityForHapke(	const unsigned int& indice,
																	const unsigned int& spectralBand,
																	const double & value) {
		_multiplicativeFactorForHapke[indice].specularIntensityVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorSpecularIntensityForHapke(	const unsigned int& indice,
																	const unsigned int& spectralBand) {
		return _multiplicativeFactorForHapke[indice].specularIntensityVector[spectralBand];
	}


/*
 * Hapke2 mult factor
 */
	/* Hapke multiplicative factor */
		static void setMultiplicativeFactorWForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {

			_multiplicativeFactorForHapke2[indice].W[spectralBand] = value;
		}
		static double getMultiplicativeFactorWForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand) {
//			cout<<"        getMultiplicativeFactorWForHapke = "<<_multiplicativeFactorForHapke2[indice].W[spectralBand]<<endl;
			return _multiplicativeFactorForHapke2[indice].W[spectralBand];
		}

		static double getMultiplicativeFactorCForHapke2(const unsigned int& indice,
														const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].C[spectralBand];
		}

		static void setMultiplicativeFactorCForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
			_multiplicativeFactorForHapke2[indice].C[spectralBand] = value;
		}
		static double getMultiplicativeFactorBForHapke2(const unsigned int& indice,
														const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].B[spectralBand];
		}

		static void setMultiplicativeFactorBForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
			_multiplicativeFactorForHapke2[indice].B[spectralBand] = value;
		}
		static double getMultiplicativeFactorHForHapke2(const unsigned int& indice,
														const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].H[spectralBand];
		}

		static void setMultiplicativeFactorHForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
			_multiplicativeFactorForHapke2[indice].H[spectralBand] = value;
		}
		static double getMultiplicativeFactorB0ForHapke2(const unsigned int& indice,
														const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].B0[spectralBand];
		}

		static void setMultiplicativeFactorB0ForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
			_multiplicativeFactorForHapke2[indice].B0[spectralBand] = value;
		}
		static double getMultiplicativeFactorThForHapke2(const unsigned int& indice,
														const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].Th[spectralBand];
		}

		static void setMultiplicativeFactorThForHapke2(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
			_multiplicativeFactorForHapke2[indice].Th[spectralBand] = value;
		}


		static void setMultiplicativeFactorDirectTransmittanceFactorForHapke2(	const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
			_multiplicativeFactorForHapke2[indice].directTransmittanceVector[spectralBand] = value;
		}
		static double getMultiplicativeFactorDirectTransmittanceFactorForHapke2(	const unsigned int& indice,
																				const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].directTransmittanceVector[spectralBand];
		}

		static void setMultiplicativeFactorDiffuseTransmittanceFactorForHapke2(	const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
			_multiplicativeFactorForHapke2[indice].diffuseTransmittanceVector[spectralBand] = value;
		}
		static double getMultiplicativeFactorDiffuseTransmittanceFactorForHapke2(const unsigned int& indice,
																				const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].diffuseTransmittanceVector[spectralBand];
		}

		static void setMultiplicativeFactorSpecularIntensityForHapke2(	const unsigned int& indice,
																		const unsigned int& spectralBand,
																		const double & value) {
			_multiplicativeFactorForHapke2[indice].specularIntensityVector[spectralBand] = value;
		}
		static double getMultiplicativeFactorSpecularIntensityForHapke2(	const unsigned int& indice,
																		const unsigned int& spectralBand) {
			return _multiplicativeFactorForHapke2[indice].specularIntensityVector[spectralBand];
		}


	/* Specular Multiplicative factor*/
	static void setMultiplicativeFactorNiForSpecular(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
		_multiplicativeFactorForSpecular[indice].niVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorNiForSpecular(const unsigned int& indice,
														const unsigned int& spectralBand) {
		return _multiplicativeFactorForSpecular[indice].niVector[spectralBand];
	}

	static void setMultiplicativeFactorNtForSpecular(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
		_multiplicativeFactorForSpecular[indice].ntVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorNtForSpecular(const unsigned int& indice,
														const unsigned int& spectralBand) {
		return _multiplicativeFactorForSpecular[indice].ntVector[spectralBand];
	}

	static void setMultiplicativeFactorTauForSpecular(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
		_multiplicativeFactorForSpecular[indice].opticalDepthVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorTauForSpecular(const unsigned int& indice,
														const unsigned int& spectralBand) {
		return _multiplicativeFactorForSpecular[indice].opticalDepthVector[spectralBand];
	}

	static void setMultiplicativeFactorReflectionForSpecular(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
		_multiplicativeFactorForSpecular[indice].reflectionIntensityVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorReflectionForSpecular(const unsigned int& indice,
														const unsigned int& spectralBand) {
		return _multiplicativeFactorForSpecular[indice].reflectionIntensityVector[spectralBand];
	}

	static void setMultiplicativeFactorTransmissionForSpecular(	const unsigned int& indice,
														const unsigned int& spectralBand,
														const double & value) {
		_multiplicativeFactorForSpecular[indice].transmissionIntensityVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorTransmissionForSpecular(const unsigned int& indice,
														const unsigned int& spectralBand) {
		return _multiplicativeFactorForSpecular[indice].transmissionIntensityVector[spectralBand];
	}



	/* RPV Multiplicative factor*/

	static void setMultiplicativeFactorRho0ForRPV(	const unsigned int& indice,
													const unsigned int& spectralBand,
													const double & value) {
		_multiplicativeFactorForRpv[indice].rpvParameterRho0Vector[spectralBand] = value;
	}
	static double getMultiplicativeFactorRho0ForRPV(const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].rpvParameterRho0Vector[spectralBand];
	}

	static void setMultiplicativeFactorKForRPV(	const unsigned int& indice,
												const unsigned int& spectralBand,
												const double & value) {
		_multiplicativeFactorForRpv[indice].rpvParameterKVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorKForRPV(	const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].rpvParameterKVector[spectralBand];
	}

	static void setMultiplicativeFactorHForRPV(	const unsigned int& indice,
												const unsigned int& spectralBand,
												const double & value) {
		_multiplicativeFactorForRpv[indice].rpvParameterHVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorHForRPV(	const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].rpvParameterHVector[spectralBand];
	}

	static void setMultiplicativeFactorGForRPV(	const unsigned int& indice,
												const unsigned int& spectralBand,
												const double & value) {
		_multiplicativeFactorForRpv[indice].rpvParameterGVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorGForRPV(	const unsigned int& indice,
													const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].rpvParameterGVector[spectralBand];
	}

	static void setMultiplicativeFactorDirectTransmittanceForRPV(	const unsigned int& indice,
																	const unsigned int& spectralBand,
																	const double & value) {
		_multiplicativeFactorForRpv[indice].directTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDirectTransmittanceForRPV(	const unsigned int& indice,
																	const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].directTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorDiffuseTransmittanceForRPV(	const unsigned int& indice,
																	const unsigned int& spectralBand,
																	const double & value) {
		_multiplicativeFactorForRpv[indice].diffuseTransmittanceVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorDiffuseTransmittanceForRPV(const unsigned int& indice,
																	const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].diffuseTransmittanceVector[spectralBand];
	}

	static void setMultiplicativeFactorSpecularIntensityForRPV(	const unsigned int& indice,
																const unsigned int& spectralBand,
																const double & value) {
		_multiplicativeFactorForRpv[indice].specularIntensityVector[spectralBand] = value;
	}
	static double getMultiplicativeFactorSpecularIntensityForRPV(	const unsigned int& indice,
																	const unsigned int& spectralBand) {
		return _multiplicativeFactorForRpv[indice].specularIntensityVector[spectralBand];
	}

	/* Water interface */

	static void setMultiplicativeFactorUpwardRefractionIndexForWaterInterface(	const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
		_multiplicativeFactorForWaterInterface[indice].upwardRefractionIndexVector[spectralBand] = value;
	}
	static void setMultiplicativeFactorDownwardRefractionIndexForWaterInterface(const unsigned int& indice,
																				const unsigned int& spectralBand,
																				const double & value) {
		_multiplicativeFactorForWaterInterface[indice].downwardRefractionIndexVector[spectralBand] = value;
	}

	static double getMultiplicativeFactorUpwardRefractionIndexForWaterInterface(const unsigned int& modelIndex,
																				const unsigned int& spectralBandIndex) {

		return _multiplicativeFactorForWaterInterface[modelIndex].upwardRefractionIndexVector[spectralBandIndex];

	}
	static double getMultiplicativeFactorDownwardRefractionIndexForWaterModel(	const unsigned int& modelIndex,
																				const unsigned int& spectralBandIndex) {

		return _multiplicativeFactorForWaterInterface[modelIndex].downwardRefractionIndexVector[spectralBandIndex];

	}

	/* Foliar */
	static void resizeForMultiplicativeFactorforLUT() {
		for (unsigned int j = 0; j < _reflectanceModelsParameters.numberOfLambertianModels; j++) {
			_multiplicativeFactorForLambertien[j].reflectanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForLambertien[j].diffuseTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForLambertien[j].directTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForLambertien[j].specularIntensityVector.resize(_numberOfSpectralBands);

		}
		for (unsigned int j = 0; j < _reflectanceModelsParameters.numberOfHapkeModels; j++) {
			_multiplicativeFactorForHapke[j].directTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].diffuseTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].specularIntensityVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].W.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].C1.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].C2.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].C3.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].C4.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].H1.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke[j].H2.resize(_numberOfSpectralBands);
		}

		for (unsigned int j = 0; j < _reflectanceModelsParameters.numberOfHapke2Models; j++) {
			_multiplicativeFactorForHapke2[j].directTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].diffuseTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].specularIntensityVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].W.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].C.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].B.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].H.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].B0.resize(_numberOfSpectralBands);
			_multiplicativeFactorForHapke2[j].Th.resize(_numberOfSpectralBands);
				}

		for (unsigned int j = 0; j < _reflectanceModelsParameters.numberOfSpecularModels; j++) {
			_multiplicativeFactorForSpecular[j].niVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForSpecular[j].ntVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForSpecular[j].opticalDepthVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForSpecular[j].reflectionIntensityVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForSpecular[j].transmissionIntensityVector.resize(_numberOfSpectralBands);
		}

		for (unsigned int j = 0; j < _reflectanceModelsParameters.numberOfRPVModels; j++) {
			_multiplicativeFactorForRpv[j].rpvParameterRho0Vector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].rpvParameterKVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].rpvParameterGVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].rpvParameterHVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].directTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].diffuseTransmittanceVector.resize(_numberOfSpectralBands);
			_multiplicativeFactorForRpv[j].specularIntensityVector.resize(_numberOfSpectralBands);
		}
	}

	static void resizePhaseParametersVectors() {
		_multiplicativeFactorForSpecular.resize(_reflectanceModelsParameters.numberOfSpecularModels);
		_multiplicativeFactorForRpv.resize(_reflectanceModelsParameters.numberOfRPVModels);
		_multiplicativeFactorForHapke.resize(_reflectanceModelsParameters.numberOfHapkeModels);
		_multiplicativeFactorForHapke2.resize(_reflectanceModelsParameters.numberOfHapke2Models);
		_multiplicativeFactorForWaterInterface.resize(_reflectanceModelsParameters.numberOfWaterInterfaceModels);
		_multiplicativeFactorForLambertien.resize(_reflectanceModelsParameters.numberOfLambertianModels);
	}

	static void setNumberOfLambertianModels(const unsigned int& numberOfLambertianModels) {
		_reflectanceModelsParameters.numberOfLambertianModels = numberOfLambertianModels;
	}
	static void setNumberOfSpecularModels(const unsigned int& numberOfSpecularModels) {
		_reflectanceModelsParameters.numberOfSpecularModels = numberOfSpecularModels;
	}
	static void setNumberOfPhaseExternModels(const unsigned int& numberOfPhaseExternModels) {
		_reflectanceModelsParameters.numberOfPhaseExternModels = numberOfPhaseExternModels;
	}
	static void setNumberOfHapkeModels(const unsigned int& numberOfHapkeModels) {
		_reflectanceModelsParameters.numberOfHapkeModels = numberOfHapkeModels;
	}
	static void setNumberOfHapke2Models(const unsigned int& numberOfHapke2Models) {
			_reflectanceModelsParameters.numberOfHapke2Models = numberOfHapke2Models;
		}
	static void setNumberOfFoliarModels(const unsigned int& numberOfFoliarModels) {
		_reflectanceModelsParameters.numberOfFoliarModels = numberOfFoliarModels;
	}
	static void setNumberOfWaterInterfaceModels(const unsigned int& numberOfModels) {
		_reflectanceModelsParameters.numberOfWaterInterfaceModels = numberOfModels;
	}
	static void setNumberOfRPVModels(const unsigned int& numberOfRPVModels) {
		_reflectanceModelsParameters.numberOfRPVModels = numberOfRPVModels;
	}
	static void setNumberOfMixedMaterialModels(const unsigned int& numberOfMixedMaterialModels) {
		_reflectanceModelsParameters.numberOfMixedMaterialModels = numberOfMixedMaterialModels;
	}

	static unsigned int getNewEquivalentLambertianModelIndex() {
		return PhaseProperty::getNumberOfLambertianModels() + _reflectanceModelsParameters.numberOfEquivalentLambertianModels++;
	}
	static unsigned int getNumberOfLambertianModels() {
		return _reflectanceModelsParameters.numberOfLambertianModels;
	}
	static unsigned int getNumberOfSpecularModels() {
		return _reflectanceModelsParameters.numberOfSpecularModels;
	}
	static unsigned int getNumberOfEquivalentLambertianModels() {
		return _reflectanceModelsParameters.numberOfEquivalentLambertianModels;
	}
	static unsigned int getNumberOfPhaseExternModels() {
		return _reflectanceModelsParameters.numberOfPhaseExternModels;
	}
	static unsigned int getNumberOfHapkeModels() {
		return _reflectanceModelsParameters.numberOfHapkeModels;
	}

	static unsigned int getNumberOfHapke2Models() {
			return _reflectanceModelsParameters.numberOfHapke2Models;
		}

	static unsigned int getNumberOfFoliarModels() {
		return _reflectanceModelsParameters.numberOfFoliarModels;
	}
	static unsigned int getNumberOfRPVModels() {
		return _reflectanceModelsParameters.numberOfRPVModels;
	}
	static unsigned int getNumberOfWaterInterfaceModels() {
		return _reflectanceModelsParameters.numberOfWaterInterfaceModels;
	}
	static unsigned int getNumberOfMixedMaterialModels() {
		return _reflectanceModelsParameters.numberOfMixedMaterialModels;
	}

	static std::string getSolarConstantDatabaseName() {
		return _solarConstantDatabaseName;
	}
	static void setSolarConstantDatabaseName(const std::string & solarConstantDatabaseName) {
		_solarConstantDatabaseName = solarConstantDatabaseName;
	}

	static unsigned int getComputationMethodeOfToaToBoa() {
		return _computationMethodeOfToaToBoa;
	}

	static double getLowerBandWavelength(const unsigned int spectralBandIndex) {
		return _userSpectralBands[spectralBandIndex].lowerLambdaValue;
	}
	static double getUpperBandWavelength(const unsigned int spectralBandIndex) {
		return _userSpectralBands[spectralBandIndex].upperLambdaValue;
	}

	static void getIdsInAtmosphericTablesMultiThread() {
		boost::thread_group idsInAtmosphericTablesThreads;
		for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
			boost::thread* t = new boost::thread(getIdsInAtmosphericTablesPerThread, threadNumber);
			idsInAtmosphericTablesThreads.add_thread(t);
		}
		idsInAtmosphericTablesThreads.join_all();
	}

	/*  Opening database and setting parameters to allow each thread to just re-open the db with these parameters */
	static void getIdsInAtmosphericTablesPerThread(const unsigned int& threadNumber) {
		unsigned int spectralBandIndex = 0;

		DartDatabaseManager databaseManager(_solarConstantDatabaseName);
		if (databaseManager.connectDatabase()) {
			for (unsigned int i = 0; i < _threadEnd[threadNumber]; i++) {
				spectralBandIndex = threadNumber + _numberOfThreads * i;
				getIdsInAtmosphericTablesPerBand(spectralBandIndex, databaseManager);
			}
			databaseManager.deconnectDatabase();
		}
	}

	static void getIdsInAtmosphericTablesPerBand(	const unsigned int spectralBandIndex,
													DartDatabaseManager& currentDatabase) {
		int min = 0;
		int max = 0;

		PhaseDatabaseDAO::getIdSpectralBand(currentDatabase, _tableSolarIrradiance, _userSpectralBands[spectralBandIndex], min, max);

		if (min < 1) {
			PhaseDatabaseDAO::getMaxIdSpectralBandIfEndingOfBeginingOfTable(currentDatabase, _tableSolarIrradiance,
				_userSpectralBands[spectralBandIndex], min);
		}
		if (max < 1) {
			PhaseDatabaseDAO::getMaxIdSpectralBandIfEndingOfEndingOfTable(currentDatabase, _tableSolarIrradiance,
				_userSpectralBands[spectralBandIndex], max);
		}
		if (min < max) {
			max = 1;
		}

		unsigned int numberOfSpectralValues = min - max + 1;

		_solarIrradianceValues[spectralBandIndex].numberOfIrradianceSpectralValues = numberOfSpectralValues;
		_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector.resize(numberOfSpectralValues);
		_solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector.resize(numberOfSpectralValues);
		_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector.resize(numberOfSpectralValues);
		_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector.resize(numberOfSpectralValues);
		_solarIrradianceValues[spectralBandIndex].skylVector.resize(numberOfSpectralValues);
		_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorptionVector.resize(numberOfSpectralValues, 1.);
	}

	static void getIdsInAtmosphericTablesFromGasTable(const AtmosphereParameters& atmosphereParameters) {
		int min = 0;
		int max = 0;
		unsigned int numberOfSpectralValues = 0;
		const std::string gasModelExtension =
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::GAS_MODEL).getTableExtension();

		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			numberOfSpectralValues = 0;
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
				// Id reinit
				min = 0;
				max = 0;

				PhaseDatabaseDAO::getIdSpectralBand(databaseManager, atmosphereParameters.gasModelName + gasModelExtension,
					_userSpectralBands[spectralBandIndex], min, max);

				if (min < 1) {
					PhaseDatabaseDAO::getMaxIdSpectralBandIfEndingOfBeginingOfTable(databaseManager,
						atmosphereParameters.gasModelName + gasModelExtension, _userSpectralBands[spectralBandIndex], min);
				}
				if (max < 1) {
					PhaseDatabaseDAO::getMaxIdSpectralBandIfEndingOfEndingOfTable(databaseManager,
						atmosphereParameters.gasModelName + gasModelExtension, _userSpectralBands[spectralBandIndex], max);
				}
				if (min < max) {
					max = 1;
				}

				numberOfSpectralValues = min - max + 1;
				_solarIrradianceValues[spectralBandIndex].numberOfIrradianceSpectralValues = numberOfSpectralValues;
				_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector.resize(numberOfSpectralValues);
				_solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector.resize(numberOfSpectralValues);
				_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector.resize(numberOfSpectralValues);
				_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector.resize(numberOfSpectralValues);
				_solarIrradianceValues[spectralBandIndex].skylVector.resize(numberOfSpectralValues);
				_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorptionVector.resize(numberOfSpectralValues, 1.);
			}
			databaseManager.deconnectDatabase();
		}
	}

	static void setSolarConstantDatabaseParameters(	const std::string & tableSolarIrradiance,
													const std::string & columnSolarIrradiance,
													const std::string& databaseName,
													const int& computationMethodeOfToaToBoa) {
		_tableSolarIrradiance = tableSolarIrradiance;
		_columnSolarIrradiance = columnSolarIrradiance;
		_solarConstantDatabaseName = databaseName;
		_computationMethodeOfToaToBoa = computationMethodeOfToaToBoa;
	}

	static void prepareThreads() {
		_threadEnd.resize(_numberOfThreads);
		for (unsigned int i = 0; i < _numberOfThreads; i++) {
			_threadEnd[i] = 0;
		}
		if (_numberOfSpectralBands == 1) {
			_threadEnd[0] = 1;
		} else {
			unsigned int numberOfBandsPerThread = _numberOfSpectralBands / _numberOfThreads;
			unsigned int restOfDivision = _numberOfSpectralBands - numberOfBandsPerThread * _numberOfThreads;
			for (unsigned int i = 0; i < _numberOfThreads; i++) {
				_threadEnd[i] = numberOfBandsPerThread;
				if (i < restOfDivision) {
					_threadEnd[i] += 1;
				}
			}
		}
	}

	static void initiateSpectralBands(	const unsigned int & numberOfSpectralBands,
										const std::vector<double> & lowerBandWavelength,
										const std::vector<double> & upperBandWavelength,
										const unsigned int& numberOfThreads) {
		//_userSpectralBands
		_numberOfUserSpectralBands = numberOfSpectralBands;
		_userSpectralBands.resize(_numberOfUserSpectralBands);

		_numberOfSpectralBands = numberOfSpectralBands;
		_numberOfThreads = numberOfThreads;
		prepareThreads();
		resizeSolarIrradianceValues(_numberOfSpectralBands);
		initiateSolarIrradianceValues();
		std::string lowerBandWavelengthStr = "";
		std::string upperBandWavelengthStr = "";
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
			// User spectral bands
			_userSpectralBands[spectralBandIndex].lowerLambdaValue = lowerBandWavelength[spectralBandIndex];
			_userSpectralBands[spectralBandIndex].upperLambdaValue = upperBandWavelength[spectralBandIndex];
		}
	}

	static void setAtmosphericDatabasePath(const std::string & completeNameDatabase) {
		_atmosphericDatabasePath = completeNameDatabase;
	}

	static int & getIrradianceFromDatabaseComputationMethod() {
		return _irradianceFromDatabaseComputationMethod;
	}

	static const int & getCalculatorMethod() {
		return _calculatorMethod;
	}

	static const bool & getIfSimuSolarNoise() {
		return _ifSimuSolarNoise;
	}

	static std::vector<double> & getIrradianceTopOfAtmosphereVector(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector;
	}

	static double & getIrradianceTopOfAtmosphereValue(	const unsigned int & spectralBandIndex,
														const unsigned int & i) {
		return _solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector[i];
	}

	static unsigned int getNumberOfSpectralBands() {
		return _numberOfSpectralBands;
	}

	static const double & getSunZenithAngleInRadians() {
		return _sunZenithAngleInRadians;
	}

	static const bool & getPolarization() {
		return _polarization;
	}
	static const bool & getFluorescence() {
		return _fluorescence;
	}
	static const bool & getCombinedPS() {
		return _combinedPS;
	}
	static const std::string & getFluorescenceFileName() {
		return _fluorescenceFileName;
	}

	static void initYields() {
		if (_fluorescenceFileName.compare("") != 0) {
			std::string filePath = DartEnv::getFilePathForRead(INPUT_DIR, _fluorescenceFileName);
			std::ifstream stream(filePath.c_str(), std::ifstream::in);
			if (stream) {
				// Stream and global initialization
				double yieldPSI, yieldPSII;
				int iYieldPSI, iYieldPSII;
				int minYieldPSI = 9999, maxYieldPSI = -9999, minYieldPSII = 9999, maxYieldPSII = -9999;
				do {
					stream >> yieldPSI >> yieldPSII;
					if (stream) {
						iYieldPSI = yieldPSI * 1000;
						iYieldPSII = yieldPSII * 1000;
						if (iYieldPSI < minYieldPSI) {
							minYieldPSI = iYieldPSI;
						}
						if (iYieldPSII < minYieldPSII) {
							minYieldPSII = iYieldPSII;
						}
						if (iYieldPSI > maxYieldPSI) {
							maxYieldPSI = iYieldPSI;
						}
						if (iYieldPSII > maxYieldPSII) {
							maxYieldPSII = iYieldPSII;
						}
					}
				} while (stream);
				stream.close();
				if (maxYieldPSI - minYieldPSI == 0) {
					if (maxYieldPSII - minYieldPSII == 0) {
						// Single couple
						_fluorescenceYields.push_back(pair<double, double>(((double) maxYieldPSI) / 1000., ((double) maxYieldPSII) / 1000.));
					} else if (maxYieldPSII - minYieldPSII < 10) {
						// check for all available PSII
						for (int i = minYieldPSII; i <= maxYieldPSII; ++i) {
							_fluorescenceYields.push_back(pair<double, double>(((double) maxYieldPSI) / 1000., ((double) i) / 1000.));
						}
					} else {
						double step = double(maxYieldPSII - minYieldPSII) / 10;
						// check for all available PSII
						for (int i = 0; i <= 10; ++i) {
							_fluorescenceYields.push_back(
								pair<double, double>(((double) maxYieldPSI) / 1000., ((double) minYieldPSII + step * i) / 1000.));
						}
					}
				} else if (maxYieldPSI - minYieldPSI < 10) {
					if (maxYieldPSII - minYieldPSII == 0) {
						// Single couple
						for (int i = minYieldPSI; i <= maxYieldPSI; ++i) {
							_fluorescenceYields.push_back(pair<double, double>(((double) i) / 1000., ((double) maxYieldPSII) / 1000.));
						}
					} else if (maxYieldPSII - minYieldPSII < 10) {
						// check for all available PSII
						for (int i = minYieldPSI; i <= maxYieldPSI; ++i) {
							for (int j = minYieldPSII; j <= maxYieldPSII; ++j) {
								_fluorescenceYields.push_back(pair<double, double>(((double) i) / 1000., ((double) j) / 1000.));
							}
						}
					} else {
						double stepJ = double(maxYieldPSII - minYieldPSII) / 10;
						// check for all available PSII
						for (int i = minYieldPSI; i <= maxYieldPSI; ++i) {
							for (int j = 0; j <= 10; ++j) {
								_fluorescenceYields.push_back(
									pair<double, double>(((double) i) / 1000., ((double) minYieldPSII + stepJ * j) / 1000.));
							}
						}
					}
				} else {
					double stepI = double(maxYieldPSI - minYieldPSI) / 10;
					if (maxYieldPSII - minYieldPSII == 0) {
						// Single couple
						for (int i = 0; i <= 10; ++i) {
							_fluorescenceYields.push_back(
								pair<double, double>(((double) minYieldPSI + stepI * i) / 1000., ((double) maxYieldPSII) / 1000.));
						}
					} else if (maxYieldPSII - minYieldPSII < 10) {
						// check for all available PSII
						for (int i = 0; i <= 10; ++i) {
							for (int j = minYieldPSII; j <= maxYieldPSII; ++j) {
								_fluorescenceYields.push_back(pair<double, double>(((double) stepI * i + minYieldPSI) / 1000., ((double) j) / 1000.));
							}
						}
					} else {
						double stepJ = double(maxYieldPSII - minYieldPSII) / 10;
						// check for all available PSII
						for (int i = 0; i <= 10; ++i) {
							for (int j = 0; j <= 10; ++j) {
								_fluorescenceYields.push_back(
									pair<double, double>(((double) stepI * i + minYieldPSI) / 1000., ((double) minYieldPSII + stepJ * j) / 1000.));
							}
						}
					}
				}
			} else {
				_fluorescence = false;
			}
		} else {
			_fluorescenceYields.push_back(pair<double, double>(0.002, 0.01));
		}
	}

	/* LiDAR pulse energy J*/
	static void setLiDARpulseEnergy(const double & Epulse)  {
		_Epulse = Epulse;
	}

	static void setIsComputedIrradianceBottomOfAtmosphereCheck(const bool & isComputedIrradianceBottomOfAtmosphereCheck) {
		_isComputedIrradianceBottomOfAtmosphereCheck = isComputedIrradianceBottomOfAtmosphereCheck;
	}

	static void setIrradianceFromDatabaseComputationMethod(const int & irradianceFromDatabaseComputationMethod) {
		_irradianceFromDatabaseComputationMethod = irradianceFromDatabaseComputationMethod;
	}

	static void setIfSimuSolarNoise(const bool & ifSimuSolarNoise) {
		_ifSimuSolarNoise = ifSimuSolarNoise;
	}

	static void setCalculatorMethod(const int & calculatorMethod) {
		_calculatorMethod = calculatorMethod;
	}

	/* Solar irradiance values */
	static void setIrradianceBottomOfAtmosphereVector(	const unsigned int & spectralBandIndex,
														const std::vector<double> & irradianceBottomOfAtmosphere) {
		_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector = irradianceBottomOfAtmosphere;
	}
	static const std::vector<double> & getIrradianceBottomOfAtmosphereVector(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector;
	}
	static void setIrradianceForPonderationVector(	const unsigned int & spectralBandIndex,
													const std::vector<double> & irradianceForPonderation) {
		_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector = irradianceForPonderation;
	}
	static void updateIrradianceForPonderationVector(	const unsigned int & spectralBandIndex,
														const std::vector<double> & irradianceForPonderation,
														const int spectralMode,
														const double & averageTemperature) {
		_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector.clear();
		for (unsigned int i = 0; i < irradianceForPonderation.size(); ++i) {
			if (spectralMode == 0) {
				_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[i] = irradianceForPonderation[i];
			} else if (spectralMode == 1) {
				_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[i] = irradianceForPonderation[i] + temperatureToLuminanceForOneWavelength(averageTemperature, 1, _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[i] * 10E-6) * M_PI;
			} else if (spectralMode == 2) {
				_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[i] = temperatureToLuminanceForOneWavelength(averageTemperature, 1, _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[i] * 10E-6);
			}
		}
	}

	static void setIrradianceTopOfAtmosphereVector(	const unsigned int & spectralBandIndex,
													const unsigned int & i,
													const double & irradianceTopOfAtmosphere) {
		_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector[i] = irradianceTopOfAtmosphere;
	}
	static const double & getIrradianceTopOfAtmosphereVector(	const unsigned int & spectralBandIndex,
																const unsigned int & i) {
		return _solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector[i];
	}

	static void setMeanIrradianceTopOfAtmosphere(	const unsigned int & spectralBandIndex,
													const double & meanIrradianceTopOfAtmosphere) {
		_solarIrradianceValues[spectralBandIndex].meanIrradianceTopOfAtmosphere = meanIrradianceTopOfAtmosphere;
	}
	static double & getMeanIrradianceTopOfAtmosphere(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].meanIrradianceTopOfAtmosphere;
	}

	static void setMeanIrradianceBottomOfAtmosphere(const unsigned int & spectralBandIndex,
													const double & meanIrradianceBottomOfAtmosphere) {
		_solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere = meanIrradianceBottomOfAtmosphere;
	}
	static double & getMeanIrradianceBottomOfAtmosphere(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere;
	}

	static void setMeanSKYL(const unsigned int & spectralBandIndex,
							const double & meanSKYL) {
		_solarIrradianceValues[spectralBandIndex].meanSKYL = meanSKYL;
	}
	static double & getMeanSKYL(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].meanSKYL;
	}
	static double & getTemperatureAtmosphereAbsorption(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorption;
	}

	static unsigned int getNumberOfIrradianceSpectralValues(const unsigned int & spectralBandIndex) {
		return _solarIrradianceValues[spectralBandIndex].numberOfIrradianceSpectralValues;
	}

	static void setSunZenithAngleInRadians(const double & sunZenithAngleInRadians) {
		_sunZenithAngleInRadians = sunZenithAngleInRadians;
	}

	static void setPolarization(const bool & polarization) {
		_polarization = polarization;
	}
	static void setFluorescence(const bool & fluorescence) {
		_fluorescence = fluorescence;
	}
	static void setCombinedPS(const bool & combinedPS) {
		_combinedPS = combinedPS;
	}
	static void setFluorescenceFileName(const std::string & fluorescenceFileName) {
		_fluorescenceFileName = fluorescenceFileName;
	}

	static void computeLinearInterpolation(	const double & lowerLimitMultiplicativeCoefficient,
											const double & upperLimitMultiplicativeCoefficient,
											const double & lowerLimitValue,
											const double & upperLimitValue,
											const double & distanceBewteenLowerAndUpperLimits,
											double & interpolatedValue) {
		interpolatedValue = lowerLimitValue * upperLimitMultiplicativeCoefficient + upperLimitValue * lowerLimitMultiplicativeCoefficient;
		if (distanceBewteenLowerAndUpperLimits != 0.) {
			interpolatedValue /= distanceBewteenLowerAndUpperLimits;
		}
	}

	static void interpolationOfReflectanceValues(	const std::vector<double> & lowerLimitValues,
													const std::vector<double> & upperLimitValues,
													std::vector<double> & interpolatedValues,
													const double & lowerLimitWavelength,
													const double & upperLimitWavelength,
													const double & interpolatedWavelength) {
		double lowerLimitMultiplicativeCoefficient = interpolatedWavelength - lowerLimitWavelength;
		double upperLimitMultiplicativeCoefficient = upperLimitWavelength - interpolatedWavelength;
		double distanceBewteenLowerAndUpperLimits = upperLimitWavelength - lowerLimitWavelength;
		if (interpolatedWavelength < lowerLimitWavelength) {
			interpolatedValues = lowerLimitValues;
		} else if (interpolatedWavelength > upperLimitWavelength) {
			interpolatedValues = upperLimitValues;
		} else {
			for (unsigned int i = 0; i < interpolatedValues.size(); ++i) {
				computeLinearInterpolation(lowerLimitMultiplicativeCoefficient, upperLimitMultiplicativeCoefficient, lowerLimitValues.at(i),
					upperLimitValues.at(i), distanceBewteenLowerAndUpperLimits, interpolatedValues.at(i));
			}
		}
	}

	static void computeTrapezoidalMethod(	const double & lowerLimitValue,
											const double & upperLimitValue,
											const double & distanceBewteenLowerAndUpperLimits,
											double & integral) {
		integral += 0.5 * (upperLimitValue + lowerLimitValue) * distanceBewteenLowerAndUpperLimits;
	}

	static void computeTrapezeReflectanceWithSun(	const std::vector<double> & previousReflectanceParameters,
													const std::vector<double> & currentReflectanceParameters,
													std::vector<double> & reflectanceParametersMeanValues,
													double & intervalBetweenSuccesiveWavelength,
													double & currentWavelength,
													double & previousWavelength,
													double & totalIrradiance,
													unsigned int & sunIndice,
													const unsigned int & spectralBandIndex) {
		double sunCoefficient = 0;
		double miniGap = 0;
		unsigned int newIndice = 0;
		if (sunIndice > 0) {
			newIndice = sunIndice - 1;
			miniGap = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[sunIndice]
					- _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[newIndice];
			if (miniGap > 0) {
				computeTrapezoidalMethod(_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[newIndice],
					_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[sunIndice], miniGap, sunCoefficient);
//				cout << _solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[sunIndice] << " (BOA: " << _solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[sunIndice] << ")" << endl;
			}
		}

		while ((sunIndice < _solarIrradianceValues[spectralBandIndex].numberOfIrradianceSpectralValues - 1)
				&& _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[sunIndice] <= currentWavelength) {
			newIndice = sunIndice + 1;
			miniGap = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[newIndice]
					- _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[sunIndice];
			if (miniGap > 0) {
				computeTrapezoidalMethod(_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[sunIndice],
					_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[newIndice], miniGap, sunCoefficient);
//				cout << _solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[newIndice] << " (BOA: " << _solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[newIndice] << ")" << endl;
			}
			sunIndice++;
		}

		if (sunCoefficient <= 0) {
			sunCoefficient = 0.001;
		}
		totalIrradiance += sunCoefficient;
		intervalBetweenSuccesiveWavelength *= sunCoefficient / intervalBetweenSuccesiveWavelength;
		computeTrapezeReflectance(previousReflectanceParameters, currentReflectanceParameters, reflectanceParametersMeanValues,
			intervalBetweenSuccesiveWavelength);

	}

	static void getNextReflectanceParameters(	std::vector<double> & reflectanceParameters,
												const std::vector<std::vector<double> > & results,
												const int & currentColumnReturn) {
		for (unsigned int i = 0; i < reflectanceParameters.size(); ++i) {
			reflectanceParameters[i] = results.at(i + 1).at(currentColumnReturn);
		}
	}

	static void computeTrapezeReflectance(	const std::vector<double> & previousReflectanceParameters,
											const std::vector<double> & currentReflectanceParameters,
											std::vector<double> & reflectanceParametersMeanValues,
											double & intervalBetweenSuccesiveWavelength) {
		for (unsigned int i = 0; i < reflectanceParametersMeanValues.size(); ++i) {
			computeTrapezoidalMethod(previousReflectanceParameters.at(i), currentReflectanceParameters.at(i), intervalBetweenSuccesiveWavelength,
				reflectanceParametersMeanValues.at(i));
		}
	}

	static void normalization(	const double & normalisation,
								double & value) {
		value /= normalisation;
	}

	static void normalizeReflectanceParameters(	std::vector<double> & reflectanceParameters,
												const double & normalisation) {

		for (unsigned int i = 0; i < reflectanceParameters.size(); ++i) {
			normalization(normalisation, reflectanceParameters.at(i));

		}
	}

	static void getMeanReflectance(	const double & value1,
									const double & value2,
									double & result) {
		result = 0.5 * (value1 + value2);
	}

	static void readReflectanceParameters(	const std::vector<bool> & useSpectralIrradianceForReflectance,
											const shared_ptr<PhaseDatabaseParameters> & modelProperties) {

		if (modelProperties->getDatabaseType() == PHASE_EXTERN_DATABASE) {
			std::string completeNameDatabase = modelProperties.get()->getDatabasePath() + modelProperties.get()->getDatabaseName();
			DartDatabaseManager databaseManager(completeNameDatabase);
			if (databaseManager.connectDatabase()) {
				//cerr << "[BEGIN] Read Phase Extern from database + interpolate on the spectral band"<<endl;

				shared_ptr<PhaseExternDatabaseParameters> phaseExternDatabaseParameters = ::static_pointer_cast<PhaseExternDatabaseParameters>(
					modelProperties);
				std::vector<int> anglesId;
				std::vector<double> thetaView, phiView, thetaIncident, phiIncident;

				cout<<"Accessing database: <"<<phaseExternDatabaseParameters->getDatabaseModel()<<" > "<<flush;
				PhaseDatabaseDAO::readPhaseExternAnglesForModel(databaseManager, phaseExternDatabaseParameters->getDatabaseModel(), anglesId,
					thetaView, phiView, thetaIncident, phiIncident);
				cout<<"...done"<<endl;
				for (unsigned int i = 0; i < anglesId.size(); ++i) {
					phaseExternDatabaseParameters->addDirectionnalPhaseExternDatabaseParameters(anglesId[i], thetaView[i], phiView[i],
						thetaIncident[i], phiIncident[i]);
				}
				databaseManager.deconnectDatabase();

				vector<shared_ptr<PhaseDatabaseParameters> > directionnalPhaseExtern =
						phaseExternDatabaseParameters->getDirectionnalPhaseExternDatabaseParametersVector();
				_phaseExternMutexedIncrement = 0; // I hope this method is not multitheaded
				boost::thread_group phaseExternThreads;
				for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
					boost::thread *t = new boost::thread(readReflectanceParametersPerThreadForPhaseExtern,
							boost::ref(useSpectralIrradianceForReflectance), boost::ref(completeNameDatabase), boost::ref(directionnalPhaseExtern));
					phaseExternThreads.add_thread(t);
				}
				phaseExternThreads.join_all();
				//cerr << "[END] Read Phase Extern from database + interpolate on the spectral band"<<endl;
			}
		} else {
			if (modelProperties.get()->getDatabaseType() == TEMPERATURE_DATABASE
					|| modelProperties.get()->getDatabaseType() == TEMPERATURE_FOLIAR_DATABASE) {
				PhaseDatabaseDAO::readTemperatureParameters(modelProperties);
			} else {
				boost::thread_group reflectanceThreads;
				for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
					boost::thread *t = new boost::thread(readReflectanceParametersPerThread, boost::ref(useSpectralIrradianceForReflectance),
						boost::ref(modelProperties), threadNumber);
					reflectanceThreads.add_thread(t);
				}
				reflectanceThreads.join_all();
			}
		}
	}

	/**
	 * ""Fix"" Je suis oblige (pour eviter de tout recoder) de dupliquer la fonction de lecture pour phase extern,
	 * Dans la solution precedente, en utilisant la fonction generique il connecte/deconnecte la base pour chaque
	 * couple d'angles lu. Ce qui n'est pas efficace du tout (Et JeanPhilippe n'est pas content !)
	 */
	static void readReflectanceParametersPerThreadForPhaseExtern(
			const std::vector<bool> & useSpectralIrradianceForReflectance,
			const std::string & completeNameDatabase,
			vector<shared_ptr<PhaseDatabaseParameters> > & directionnalPhaseExtern) {
		unsigned int numberOfPhaseExternDirections = directionnalPhaseExtern.size();
		DartDatabaseManager databaseManager(completeNameDatabase);
		if (databaseManager.connectDatabase())
		{
			int iBande = -1;
			for (; iBande < (int) _numberOfSpectralBands;)
			{
				if (iBande > -1)
				{
					for (unsigned int iDirection = 0; iDirection < numberOfPhaseExternDirections; ++iDirection)
					{
						std::vector<double> values = readReflectanceParametersPerBand(directionnalPhaseExtern.at(iDirection), useSpectralIrradianceForReflectance[iBande],databaseManager,databaseManager, iBande);
						directionnalPhaseExtern.at(iDirection)->setParametersValuesForBand(iBande, values);
					}
				}
				_phaseExternMutex.lock();
				iBande = (int) _phaseExternMutexedIncrement;
				_phaseExternMutexedIncrement++;
				_phaseExternMutex.unlock();
			}

		}
	}

	static void readReflectanceParametersPerThread(	const std::vector<bool> & useSpectralIrradianceForReflectance,
													const shared_ptr<PhaseDatabaseParameters> & modelProperties,
													const unsigned int & threadNumber) {
		std::string completeNameDatabase = modelProperties.get()->getDatabasePath() + modelProperties.get()->getDatabaseName();
		unsigned int spectralBandIndex = 0;
		bool differentDatabaseForBottom = false;
		DartDatabaseManager databaseManager(completeNameDatabase);

		if (databaseManager.connectDatabase()) {
			std::string completeBottomNameDatabase = modelProperties.get()->getBottomDatabasePath() + modelProperties.get()->getBottomDatabaseName();
			if (modelProperties->hasBottomModel() && (databaseManager.getCompletePathToDatabase().compare(completeBottomNameDatabase) != 0)) {
				DartDatabaseManager bottomDatabaseManager(completeBottomNameDatabase);
				if (bottomDatabaseManager.connectDatabase()) {
					for (unsigned int i = 0; i < _threadEnd[threadNumber]; i++) {
						spectralBandIndex = threadNumber + _numberOfThreads * i;
						std::vector<double> values = readReflectanceParametersPerBand(modelProperties, useSpectralIrradianceForReflectance[spectralBandIndex],
							databaseManager,bottomDatabaseManager, spectralBandIndex);
						modelProperties->setParametersValuesForBand(spectralBandIndex, values);
						if (threadNumber == 0 && modelProperties->getDatabaseType() != PHASE_EXTERN_DIRECTIONNAL_DATABASE) {
							_tIteration += _numberOfThreads;
							checkProgress();
						}
					}
				}
				bottomDatabaseManager.deconnectDatabase();
			} else {
				for (unsigned int i = 0; i < _threadEnd[threadNumber]; i++) {
					spectralBandIndex = threadNumber + _numberOfThreads * i;
					std::vector<double> values = readReflectanceParametersPerBand(modelProperties, useSpectralIrradianceForReflectance[spectralBandIndex],
						databaseManager,databaseManager, spectralBandIndex);
					modelProperties->setParametersValuesForBand(spectralBandIndex, values);
					if (threadNumber == 0 && modelProperties->getDatabaseType() != PHASE_EXTERN_DIRECTIONNAL_DATABASE) {
						_tIteration += _numberOfThreads;
						checkProgress();
					}
				}
			}
			databaseManager.deconnectDatabase();
		}
	}

	static void getMeanFor2Iteration(	const std::vector<double> & currentReflectanceParameters,
										const std::vector<double> previousReflectanceParameters,
										std::vector<double> & reflectanceMeanParameters) {
		for (unsigned int i = 0; i < reflectanceMeanParameters.size(); ++i) {
			getMeanReflectance(currentReflectanceParameters.at(i), previousReflectanceParameters.at(i), reflectanceMeanParameters.at(i));
		}
	}

	static void interpoleReflectanceParameterOnUserSpectralBounds(	const unsigned int & spectralBandIndex,
																	std::vector<std::vector<double> > & results) {
		if (_userSpectralBands[spectralBandIndex].lowerLambdaValue < results.at(0).at(0)) {
			results.at(0).at(0) = _userSpectralBands[spectralBandIndex].lowerLambdaValue;
		} else if (_userSpectralBands[spectralBandIndex].lowerLambdaValue > results.at(0).at(0) && results.at(0).size() > 1) {
			if (_userSpectralBands[spectralBandIndex].lowerLambdaValue == results.at(0).at(1)) {
				// Cas particulier : On supprime le premier element
				// car le second element est egal à la borne min de la bande spectrale utilisatueur
				for (unsigned int i = 0; i < results.size(); ++i) {
					results.at(i).erase(results.at(i).begin());
				}
			} else {
				for (unsigned int i = 1; i < results.size(); ++i) {
					results.at(i).at(0) = Interpolation_lineaire(results.at(0).at(0), results.at(0).at(1),
						_userSpectralBands[spectralBandIndex].lowerLambdaValue, results.at(i).at(0), results.at(i).at(1));
				}
				results.at(0).at(0) = _userSpectralBands[spectralBandIndex].lowerLambdaValue;
			}
		}

		if (_userSpectralBands[spectralBandIndex].upperLambdaValue > results.at(0).at(results.at(0).size() - 1)) {
			results.at(0).at(results.at(0).size() - 1) = _userSpectralBands[spectralBandIndex].upperLambdaValue;
		} else if (_userSpectralBands[spectralBandIndex].upperLambdaValue < results.at(0).at(results.at(0).size() - 1) && results.at(0).size() > 1) {
			if (_userSpectralBands[spectralBandIndex].upperLambdaValue == results.at(0).at(results.at(0).size() - 2)) {
				// Cas particulier : On supprime le dernier element
				// car l'avant dernier element est egal à la borne max de la bande spectrale utilisatueur
				for (unsigned int i = 0; i < results.size(); ++i) {
					results.at(i).pop_back();
				}
			} else {
				for (unsigned int i = 0; i < results.size(); ++i) {
					results.at(i).at(results.at(i).size() - 1) = Interpolation_lineaire(results.at(0).at(results.at(0).size() - 2),
						results.at(0).at(results.at(0).size() - 1), _userSpectralBands[spectralBandIndex].upperLambdaValue,
						results.at(i).at(results.at(i).size() - 2), results.at(i).at(results.at(i).size() - 1));
				}
			}
		}
	}

	static std::vector<double> readReflectanceParametersPerBand(const shared_ptr<PhaseDatabaseParameters> & modelProperties,
																bool useSpectralIrradianceForReflectance,
																DartDatabaseManager & databaseManager,
																DartDatabaseManager & bottomDatabaseManager,
																const unsigned int & spectralBandIndex) {
		std::string completeNamePath = modelProperties.get()->getDatabasePath() + modelProperties.get()->getDatabaseName();

		std::vector<std::vector<double> > results;
		unsigned int numberOfSpectralValues = 0;

		if (modelProperties->getDatabaseType() == PHASE_EXTERN_DIRECTIONNAL_DATABASE) {
			// Lecture par angle
			shared_ptr<DirectionnalPhaseExternDatabaseParameters> phaseExternDatabaseParameters = ::static_pointer_cast<
					DirectionnalPhaseExternDatabaseParameters>(modelProperties);
			PhaseDatabaseDAO::readDirectionnalReflectanceParametersPerBand(databaseManager, spectralBandIndex,
				modelProperties.get()->getDatabaseModel(), modelProperties.get()->getColumnChoices(), completeNamePath, _solarIrradianceValues,
				results, numberOfSpectralValues, _userSpectralBands[spectralBandIndex].lowerLambdaValue,
				_userSpectralBands[spectralBandIndex].upperLambdaValue, phaseExternDatabaseParameters.get()->getIdAngle());
		} else {
			PhaseDatabaseDAO::readReflectanceParametersPerBand(databaseManager, spectralBandIndex, modelProperties.get()->getDatabaseModel(),
				modelProperties.get()->getColumnChoices(), completeNamePath, _solarIrradianceValues, results, numberOfSpectralValues,
				_userSpectralBands[spectralBandIndex].lowerLambdaValue, _userSpectralBands[spectralBandIndex].upperLambdaValue);
		}
		if (modelProperties.get()->hasBottomModel()) {
			interpoleReflectanceParameterOnUserSpectralBounds(spectralBandIndex, results);
			PhaseDatabaseDAO::readBottomReflectanceParametersPerBand(bottomDatabaseManager, spectralBandIndex,
				modelProperties.get()->getBottomDatabaseModel(), modelProperties.get()->getBottomColumnChoices(),
				modelProperties.get()->getBottomDatabasePath(), modelProperties.get()->getBottomOverrideResultPosition(), _solarIrradianceValues,
				results, numberOfSpectralValues, _userSpectralBands[spectralBandIndex].lowerLambdaValue,
				_userSpectralBands[spectralBandIndex].upperLambdaValue);
		}

		interpoleReflectanceParameterOnUserSpectralBounds(spectralBandIndex, results);
		double beforeBandWavelength = 0;
		double afterBandWavelength = 0;
		double currentWavelength = 0;
		double previousWavelength = 0;
		double normalisation = 0;
		double totalIrradiance = 0;
		unsigned int boaSpectralIrradianceIndex = 0;
		unsigned int iterator = 0;

		double intervalBetweenSuccesiveWavelength = 0;

		std::vector<double> reflectanceMeanParameters;
		reflectanceMeanParameters.resize(results.size() - 1);
		std::vector<double> currentReflectanceParameters;
		currentReflectanceParameters.resize(results.size() - 1);
		std::vector<double> previousReflectanceParameters;
		previousReflectanceParameters.resize(results.size() - 1);
		std::vector<double> reflectanceParametersBefore;
		reflectanceParametersBefore.resize(results.size() - 1);
		std::vector<double> reflectanceParametersAfter;
		reflectanceParametersAfter.resize(results.size() - 1);

		for (unsigned int nbColumnReturn = 0; nbColumnReturn < results.at(0).size(); ++nbColumnReturn) {
			if (iterator == 0) {
				getNextReflectanceParameters(reflectanceParametersBefore, results, nbColumnReturn);
				beforeBandWavelength = results.at(0).at(nbColumnReturn);
				currentWavelength = beforeBandWavelength;
			}
			if (iterator == 1) {
				currentWavelength = results.at(0).at(nbColumnReturn);
				getNextReflectanceParameters(currentReflectanceParameters, results, nbColumnReturn);
				interpolationOfReflectanceValues(reflectanceParametersBefore, currentReflectanceParameters, previousReflectanceParameters,
					beforeBandWavelength, currentWavelength, _userSpectralBands[spectralBandIndex].lowerLambdaValue);
				previousWavelength = _userSpectralBands[spectralBandIndex].lowerLambdaValue;
			}
			if (iterator == numberOfSpectralValues - 1) {
				afterBandWavelength = results.at(0).at(nbColumnReturn);
				getNextReflectanceParameters(reflectanceParametersAfter, results, nbColumnReturn);
				interpolationOfReflectanceValues(previousReflectanceParameters, reflectanceParametersAfter, currentReflectanceParameters,
					previousWavelength, afterBandWavelength, _userSpectralBands[spectralBandIndex].upperLambdaValue);
				currentWavelength = _userSpectralBands[spectralBandIndex].upperLambdaValue;
			}
			if (iterator != 0 && iterator != 1 && iterator != numberOfSpectralValues - 1) {
				currentWavelength = results.at(0).at(nbColumnReturn);
				getNextReflectanceParameters(currentReflectanceParameters, results, nbColumnReturn);
			}
			if (iterator != 0) {
				intervalBetweenSuccesiveWavelength = currentWavelength - previousWavelength;
				if (intervalBetweenSuccesiveWavelength > 0) {
					if (useSpectralIrradianceForReflectance) {
						computeTrapezeReflectanceWithSun(previousReflectanceParameters, currentReflectanceParameters, reflectanceMeanParameters,
							intervalBetweenSuccesiveWavelength, currentWavelength, previousWavelength, totalIrradiance, boaSpectralIrradianceIndex,
							spectralBandIndex);
					} else {
						computeTrapezeReflectance(previousReflectanceParameters, currentReflectanceParameters, reflectanceMeanParameters,
							intervalBetweenSuccesiveWavelength);
					}
				}

				if (iterator != numberOfSpectralValues - 1) {
					previousReflectanceParameters = currentReflectanceParameters;
					previousWavelength = currentWavelength;
				}
			}
			iterator++;
		}

		if (iterator == 1) {
			reflectanceMeanParameters = reflectanceParametersBefore;
		} else if (iterator == 2
				|| _userSpectralBands[spectralBandIndex].upperLambdaValue == _userSpectralBands[spectralBandIndex].lowerLambdaValue) {
			getMeanFor2Iteration(currentReflectanceParameters, previousReflectanceParameters, reflectanceMeanParameters);
		} else {
			if (!useSpectralIrradianceForReflectance) {
				normalisation = _userSpectralBands[spectralBandIndex].upperLambdaValue - _userSpectralBands[spectralBandIndex].lowerLambdaValue;
			} else {
				normalisation = totalIrradiance;
			}
			if (normalisation != 0) {
				normalizeReflectanceParameters(reflectanceMeanParameters, normalisation);
			}
		}

		return reflectanceMeanParameters;
	}

	static double computeMeanIrradianceWithTrapezoidalMethod(	const unsigned int & numberOfValues,
																const unsigned int & spectralBandIndex,
																const std::vector<double> & wavelength,
																const std::vector<double> & irradiance) {

		double meanIrradiance = 0;
		double intervalBetweenSuccesiveWavelength = 0;
		double normalisation = _userSpectralBands[spectralBandIndex].upperLambdaValue - _userSpectralBands[spectralBandIndex].lowerLambdaValue;

		if (numberOfValues == 1) {
			meanIrradiance = irradiance[0];
		} else if (normalisation == 0) {
			if (numberOfValues == 2) {
				meanIrradiance = Interpolation_lineaire(wavelength[0], wavelength[1], _userSpectralBands[spectralBandIndex].lowerLambdaValue,
					irradiance[0], irradiance[1]);
			} else {
				if (wavelength[0] == _userSpectralBands[spectralBandIndex].lowerLambdaValue) {
					meanIrradiance = irradiance[0];
				}
				// TODO : This method is usually called
			}
		} else {
			for (unsigned int i = 0; i < numberOfValues - 1; i++) {
				intervalBetweenSuccesiveWavelength = wavelength[i + 1] - wavelength[i];
				computeTrapezoidalMethod(irradiance[i], irradiance[i + 1], intervalBetweenSuccesiveWavelength, meanIrradiance);
			}
			meanIrradiance /= normalisation;
		}

		return meanIrradiance;
	}

	static void readDatabaseIrradiance(const std::string & irradianceDatabasePath) {
		double meanComputedIrradianceTopOfAtmosphere = 0;
//		unsigned int iteration0 = 0;
//		unsigned int iteration1 = 0;
//		double coefficient0 = 0;
//		double coefficient1 = 0;
//		double interpolation = 0;
//		double iterationDifference = 0;

		DartDatabaseManager databaseManager(irradianceDatabasePath);
		if (databaseManager.connectDatabase()) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
				PhaseDatabaseDAO::readIrradianceAtmosphericParametersPerBand(databaseManager, _tableSolarIrradiance, _columnSolarIrradiance,
					_userSpectralBands[spectralBandIndex], _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector,
					_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector);

				int maxId = _solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector.size() - 1;
				vector<double> & wavelength = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector;
				vector<double> & irradianceTOA = _solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector;

				// Cas de la borne minimum (Interpole la valeur min de la bande pour correspondre à la valeur utilisateur)
				irradianceTOA[0] = Interpolation_lineaire(wavelength[0], wavelength[1], _userSpectralBands[spectralBandIndex].lowerLambdaValue,
					irradianceTOA[0], irradianceTOA[1]);
				wavelength[0] = _userSpectralBands[spectralBandIndex].lowerLambdaValue;

				// Cas de la borne maximum (Interpole la valeur max de la bande pour correspondre à la valeur utilisateur)
				irradianceTOA[maxId] = Interpolation_lineaire(wavelength[maxId - 1], wavelength[maxId],
					_userSpectralBands[spectralBandIndex].upperLambdaValue, irradianceTOA[maxId - 1], irradianceTOA[maxId]);
				wavelength[maxId] = _userSpectralBands[spectralBandIndex].upperLambdaValue;

				meanComputedIrradianceTopOfAtmosphere = computeMeanIrradianceWithTrapezoidalMethod(
					_solarIrradianceValues[spectralBandIndex].numberOfIrradianceSpectralValues, spectralBandIndex, wavelength, irradianceTOA);
				_solarIrradianceValues[spectralBandIndex].meanIrradianceTopOfAtmosphere = meanComputedIrradianceTopOfAtmosphere;
			}
			databaseManager.deconnectDatabase();
		}
	}

	static double getValueColun(std::string columnName) {
		char C = 'f';
		columnName.erase(remove(columnName.begin(), columnName.end(), C), columnName.end());
		return atof(columnName.c_str()) * 0.1;
	}

	static void findWaterColumn(AtmosphereParameters & atmosphereParameters) {
		std::string currentColumnName = "f1";
		std::string previousColumnName = "f0";
		unsigned int iterator = 0;
		double currentColumnValue = 0;
		double previousColumnValue = 0;
		std::vector<std::string> tableColumnsName;
		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			databaseManager.executePragmaInfo(atmosphereParameters.gasModelName + "_gH2O", tableColumnsName);
			databaseManager.deconnectDatabase();

			atmosphereParameters.userDefinedWaterIsLowerThanFirstDBValue = true;
			// Iterator = 2; on ignore les 2 premieres colonnes (id et wavelength) de la DB pour calculer le coef. multiplicatif
			for (iterator = 2; iterator < tableColumnsName.size(); ++iterator) {
				currentColumnName = tableColumnsName.at(iterator);
				currentColumnValue = getValueColun(currentColumnName);

				if (atmosphereParameters.mulFactorH2O > currentColumnValue) {
					atmosphereParameters.userDefinedWaterIsLowerThanFirstDBValue = false;
				} else {
					break;
				}

				previousColumnValue = currentColumnValue;
				previousColumnName = currentColumnName;
			}

			if (!atmosphereParameters.userDefinedWaterIsLowerThanFirstDBValue && currentColumnValue >= atmosphereParameters.mulFactorH2O) {
				atmosphereParameters.userDefinedWaterFactor = (currentColumnValue - atmosphereParameters.mulFactorH2O)
						/ (currentColumnValue - previousColumnValue);
				atmosphereParameters.userDefinedWaterFactorFollowing = (atmosphereParameters.mulFactorH2O - previousColumnValue)
						/ (currentColumnValue - previousColumnValue);
				atmosphereParameters.userDefinedWaterTable = previousColumnName;
				atmosphereParameters.userDefinedWaterTableFollowing = currentColumnName;
			} else if (!atmosphereParameters.userDefinedWaterIsLowerThanFirstDBValue) {
				atmosphereParameters.userDefinedWaterFactor = 1;
				atmosphereParameters.userDefinedWaterFactorFollowing = 0;
				atmosphereParameters.userDefinedWaterTable = currentColumnName;
				atmosphereParameters.userDefinedWaterTableFollowing = currentColumnName;
			} else {
				atmosphereParameters.userDefinedWaterFactor = (currentColumnValue - atmosphereParameters.mulFactorH2O) / currentColumnValue;
				atmosphereParameters.userDefinedWaterFactorFollowing = atmosphereParameters.mulFactorH2O / currentColumnValue;
				atmosphereParameters.userDefinedWaterTable = currentColumnName;
				atmosphereParameters.userDefinedWaterTableFollowing = currentColumnName;
			}
		}

		std::string waterColumnChoice = atmosphereParameters.userDefinedWaterTable + "," + atmosphereParameters.userDefinedWaterTableFollowing;
		AtmosphericDatabaseStructure::setWaterAtmosphericTableStructure(waterColumnChoice, 2);
	}

	static void computeTotalGasTransmittanceForAbsoption(AtmosphereComposition & atmosphereComponenets) {
		double gasTransmittanceForAbsoption = 1;
		gasTransmittanceForAbsoption *= atmosphereComponenets.getCo2TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getCoTransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getCh4TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getO2TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getN2TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getO3TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getOtherGasTransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getN2oTransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getH2oTransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getNH3TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getNOTransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getNO2TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getSO2TransmittanceAbs();
		gasTransmittanceForAbsoption *= atmosphereComponenets.getHNO3TransmittanceAbs();

		atmosphereComponenets.setGasTransmittanceForAbsoption(gasTransmittanceForAbsoption);
	}

	static void computeComplementaryAtmosphereParameters(	AtmosphereComposition & atmosphereComponents,
															const double & mu_prime_gaz,
															const double & mu_prime_aerosols,
															const double & gas_u,
															const double & aerosols_u) {
		vector<double> aerosolTransmittanceForAbsoption;
		vector<double> aerosolFactorForAnalyticalTransmittance;
		for (int i = 0; i < atmosphereComponents.getnumberOfAerosol(); i++) {
			aerosolTransmittanceForAbsoption.push_back(
				exp(-(1 - atmosphereComponents.getAerosolSingleScatteringAlbedo(i)) * atmosphereComponents.getAerosolOpticalDepth(i)));
			aerosolFactorForAnalyticalTransmittance.push_back(
				atmosphereComponents.getAerosolOpticalDepth(i) * atmosphereComponents.getAerosolSingleScatteringAlbedo(i) / mu_prime_aerosols * aerosols_u);
//			if(atmosphereComponents.getAerosolSingleScatteringAlbedo(i)>1)
//				cout<<"ScatteringAlbedo: "<<atmosphereComponents.getAerosolSingleScatteringAlbedo(i)<<" "<<atmosphereComponents.getAerosolOpticalDepth(i)<<endl;
		}
		atmosphereComponents.setAerosolTransmittanceForAbsoption(aerosolTransmittanceForAbsoption);
		if (atmosphereComponents.getGasTransmittanceForScattering() > 0) {
			atmosphereComponents.setGasOpticalDepthForScattering(-log(atmosphereComponents.getGasTransmittanceForScattering()));
			atmosphereComponents.setGasFactorForAnalyticalTransmittance(gas_u * atmosphereComponents.getGasOpticalDepthForScattering() / mu_prime_gaz);
		} else {
			atmosphereComponents.setGasFactorForAnalyticalTransmittance(500);
		}
		atmosphereComponents.setAerosolFactorForAnalyticalTransmittance(aerosolFactorForAnalyticalTransmittance);
	}

	static void computeInterpolationForAtmosphereValues(const AtmosphereComposition & previousAtmosphereValues,
														const AtmosphereComposition & currentAtmosphereValues,
														AtmosphereComposition & interpolatedAtmosphereValues,
														const double & previousWavelength,
														const double & currentWavelength,
														const double & interpolatedWavelength) {
		for (int i = 0; i < AtmosphereComposition::NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS; ++i) {
			interpolatedAtmosphereValues.getModifiableAtmosphereComponentPerIndex(i) = Interpolation_lineaire(previousWavelength, currentWavelength,
				interpolatedWavelength, previousAtmosphereValues.getAtmosphereComponentPerIndex(i),
				currentAtmosphereValues.getAtmosphereComponentPerIndex(i));
		}

		for (int i = 0; i < currentAtmosphereValues.getNbOfAbsorbingGas(); i++) {
			for (int j = 0; j < 36; j++) {
				interpolatedAtmosphereValues.getModifiableCroSecPerIndex(i, j) = Interpolation_lineaire(previousWavelength, currentWavelength,
					interpolatedWavelength, previousAtmosphereValues.getCroSecPerIndex(i, j), currentAtmosphereValues.getCroSecPerIndex(i, j));
			}
		}

		for (int i = 0; i < currentAtmosphereValues.getnumberOfAerosol(); i++) {

			interpolatedAtmosphereValues.getModifiableAerosolOpticalDepth(i) = Interpolation_lineaire(previousWavelength, currentWavelength,
				interpolatedWavelength, previousAtmosphereValues.getAerosolOpticalDepth(i), currentAtmosphereValues.getAerosolOpticalDepth(i));

			interpolatedAtmosphereValues.getModifiableAerosolSingleScatteringAlbedo(i) = Interpolation_lineaire(previousWavelength, currentWavelength,
				interpolatedWavelength, previousAtmosphereValues.getAerosolSingleScatteringAlbedo(i),
				currentAtmosphereValues.getAerosolSingleScatteringAlbedo(i));
		}
	}

	static double computeTrapezeAtmosphere(	AtmosphereComposition & previousAtmosphereValues,
											AtmosphereComposition & atmospherePerBandValues,
											AtmosphereComposition & atmospherePerBandIntegralValues,
											double & intervalBetweenSuccesiveWavelength,
											unsigned int & iterator,
											const unsigned int & spectralBand) {
		double factor = 1.;
		if (_weightingOfAtmosphereParameters && _calculatorMethod == 0 && _irradianceFromDatabaseComputationMethod == 0) {
			factor = 0.5
					* (_solarIrradianceValues[spectralBand].irradianceTopOfAtmosphereVector[iterator]
							+ _solarIrradianceValues[spectralBand].irradianceTopOfAtmosphereVector[iterator - 1]);
		}
		if (intervalBetweenSuccesiveWavelength == 0) {
			factor = 0;
		}
		intervalBetweenSuccesiveWavelength *= factor;

		for (int i = 0; i < AtmosphereComposition::NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS; ++i) {
			computeTrapezoidalMethod(previousAtmosphereValues.getAtmosphereComponentPerIndex(i),
				atmospherePerBandValues.getAtmosphereComponentPerIndex(i), intervalBetweenSuccesiveWavelength,
				atmospherePerBandIntegralValues.getModifiableAtmosphereComponentPerIndex(i));
		}

		for (int i = 0; i < previousAtmosphereValues.getNbOfAbsorbingGas(); ++i) {
			for (int j = 0; j < 36; ++j) {
				computeTrapezoidalMethod(previousAtmosphereValues.getCroSecPerIndex(i, j), atmospherePerBandValues.getCroSecPerIndex(i, j),
					intervalBetweenSuccesiveWavelength, atmospherePerBandIntegralValues.getModifiableCroSecPerIndex(i, j));
			}
		}

		for (int i = 0; i < previousAtmosphereValues.getnumberOfAerosol(); ++i) {

			computeTrapezoidalMethod(previousAtmosphereValues.getAerosolOpticalDepth(i), atmospherePerBandValues.getAerosolOpticalDepth(i),
				intervalBetweenSuccesiveWavelength, atmospherePerBandIntegralValues.getModifiableAerosolOpticalDepth(i));
			computeTrapezoidalMethod(previousAtmosphereValues.getAerosolSingleScatteringAlbedo(i),
				atmospherePerBandValues.getAerosolSingleScatteringAlbedo(i), intervalBetweenSuccesiveWavelength,
				atmospherePerBandIntegralValues.getModifiableAerosolSingleScatteringAlbedo(i));
		}
		return intervalBetweenSuccesiveWavelength;
	}

	static void multiplyAtmosphereValues(	AtmosphereComposition & atmosphereValues,
											double & factor) {
		for (int i = 0; i < AtmosphereComposition::NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS; ++i) {
			atmosphereValues.setAtmosphereComponentPerIndex(i, (atmosphereValues.getAtmosphereComponentPerIndex(i) * factor));
		}
		for (int i = 0; i < atmosphereValues.getNbOfAbsorbingGas(); ++i) {
			for (int j = 0; j < 36; ++j) {
				atmosphereValues.setCroSec(i, j, atmosphereValues.getCroSecPerIndex(i, j) * factor);
			}
		}
		for (int i = 0; i < atmosphereValues.getnumberOfAerosol(); ++i) {
			atmosphereValues.setAerosolOpticalDepthPerIndex(i, (atmosphereValues.getAerosolOpticalDepth(i) * factor));
			atmosphereValues.setAerosolSingleScatteringAlbedoPerIndex(i, (atmosphereValues.getAerosolSingleScatteringAlbedo(i) * factor));
		}
	}

	static double computeMeanIrradianceWithTrapezoidalMethode(	const unsigned int & spectralBandIndex,
																std::vector<double> & wavelength,
																std::vector<double> & irradiance) {
		unsigned int numberOfValues = wavelength.size() > irradiance.size() ? irradiance.size() : wavelength.size();
		double meanIrradiance = 0;
		double intervalBetweenSuccesiveWavelength = 0;
		double normalisation = wavelength[numberOfValues - 1] - wavelength[0];
		if (numberOfValues == 2 || normalisation == 0) {
			meanIrradiance = 0.5 * (irradiance[0] + irradiance[numberOfValues - 1]);
		} else {
			for (unsigned int i = 0; i < numberOfValues - 1; i++) {
				intervalBetweenSuccesiveWavelength = wavelength[i + 1] - wavelength[i];
				if (intervalBetweenSuccesiveWavelength > 0) {
					computeTrapezoidalMethod(irradiance[i], irradiance[i + 1], intervalBetweenSuccesiveWavelength, meanIrradiance);
				}
			}

			meanIrradiance /= normalisation;
		}

		return meanIrradiance;
	}

	static double computeBOA(	double & computedSkyl,
								double & temperatureAtmosphereAbsorption,
								const double & irradianceTopOfAtmosphere,
								const AtmosphereComposition & atmosphereValues,
								const AtmosphereParameters & atmosphereParameters)
	{
		double aerosolTransmittanceForAbsoption = 1;
		double aerosolFactorForAnalyticalTransmittance = 0;
		double sumaerosolOpticalDepthSingleScatteringAlbedoProduct = 0.0;

		for (int i = 0; i < atmosphereValues.getnumberOfAerosol(); i++) {
			aerosolTransmittanceForAbsoption *= atmosphereValues.getAerosolTransmittanceForAbsoption()[i];
			aerosolFactorForAnalyticalTransmittance += atmosphereValues.getAerosolFactorForAnalyticalTransmittance()[i];
			sumaerosolOpticalDepthSingleScatteringAlbedoProduct += atmosphereValues.getAerosolOpticalDepth(i)
					* atmosphereValues.getAerosolSingleScatteringAlbedo(i);
		}

		double numerator = pow(atmosphereValues.getGasTransmittanceForAbsoption(), atmosphereParameters.mPrimeGaz)
				* pow(aerosolTransmittanceForAbsoption, atmosphereParameters.mPrimeAerosols);
		double denominator = 1.
				+ atmosphereValues.getGasFactorForAnalyticalTransmittance() * atmosphereParameters.mPrimeGaz * atmosphereParameters.muPrimeGaz
				+ aerosolFactorForAnalyticalTransmittance * atmosphereParameters.mPrimeAerosols * atmosphereParameters.muPrimeAerosols;
		double computedBOA = irradianceTopOfAtmosphere * numerator / denominator;

		computedSkyl = (1.
				- denominator * exp(-atmosphereValues.getGasOpticalDepthForScattering() * atmosphereParameters.mPrimeGaz)
						* exp(-sumaerosolOpticalDepthSingleScatteringAlbedoProduct * atmosphereParameters.mPrimeAerosols));

		// Analytical atmosphere temperature absoption coefficient, used in mode analytical + for band T or RT
		double sumAnalyticalAbsorption = 0., tempAnalyticalAbsorption = 0.;
		for (unsigned int thetaDir = 0; thetaDir < 89; thetaDir++)
		{
			tempAnalyticalAbsorption = pow(aerosolTransmittanceForAbsoption, atmosphereParameters.mPrimeAerosolsPerDirection[thetaDir]) * pow(atmosphereValues.getGasTransmittanceForScattering() * atmosphereValues.getGasTransmittanceForAbsoption(), atmosphereParameters.mPrimeGazPerDirection[thetaDir]);
			sumAnalyticalAbsorption += (1 - tempAnalyticalAbsorption) * 0.5 * (cos(deg2rad(2 * thetaDir)) - cos(deg2rad(2 * (thetaDir + 1))));
		}
		temperatureAtmosphereAbsorption = sumAnalyticalAbsorption;
		return computedBOA;
	}

	static void treatWater(	const AtmosphereParameters & atmosphereParameters,
							const std::vector<std::vector<double> >::const_iterator & itWaterResults,
							AtmosphereComposition & atmosphereComponents) {

		double tWater = 0;
		const std::vector<double> & rowWater = *itWaterResults;
		if (!atmosphereParameters.userDefinedWaterIsLowerThanFirstDBValue) {
			tWater = rowWater.at(1) * atmosphereParameters.userDefinedWaterFactor
					+ rowWater.at(2) * atmosphereParameters.userDefinedWaterFactorFollowing;
		} else {
			// Cas particulier d'une valeur < a la 1ere valeur de la DB, on interpole entre 1 et la 1ere valeur de la DB.
			tWater = 1 * atmosphereParameters.userDefinedWaterFactor + rowWater.at(2) * atmosphereParameters.userDefinedWaterFactorFollowing;
		}
		if (atmosphereComponents.getH2oTransmittanceAbs() > 0) {
			atmosphereComponents.setGasTransmittanceAbsUpDown(
				atmosphereComponents.getGasTransmittanceAbsUpDown()
						* (tWater / atmosphereComponents.getH2oTransmittanceAbs() * tWater / atmosphereComponents.getH2oTransmittanceAbs()));
		}
//		double tmpOldWaterTransmittance = atmosphereComponents.getH2oTransmittanceAbs();
		atmosphereComponents.setH2oTransmittanceAbs(tWater);
//		if (tmpOldWaterTransmittance != 0) {
//			atmosphereComponents.scaleGasTransmittanceAbsUpDown(tWater / tmpOldWaterTransmittance);
//		}
	}

	static bool checkSpectralBandSamplingInAtmosphericDatabase(	const std::vector<std::vector<double> > & aerosolsParametersResult,
																const std::vector<std::vector<double> > & gasModelResult) {
		if (aerosolsParametersResult.size() != gasModelResult.size()) {
			return false;
		}

		std::vector<std::vector<double> >::const_iterator itGasModelResults, itAerosolsResults;
		itGasModelResults = gasModelResult.begin();
		for (itAerosolsResults = aerosolsParametersResult.begin(); itAerosolsResults != aerosolsParametersResult.end();
				++itAerosolsResults, ++itGasModelResults) {
			const std::vector<double> & rowAerosols = *itAerosolsResults;
			const std::vector<double> & rowGas = *itGasModelResults;
			if (rowAerosols.at(0) != rowGas.at(0)) {
				return false;
			}
		}
		return true;
	}

	static void resamplingDatabadeAtmophericValues(	const unsigned int & spectralBandIndex,
													std::vector<std::vector<double> > & databaseModelResult) {
		unsigned int numberOfWavelength = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector.size();
		unsigned int numberOfValues = databaseModelResult[0].size();

		std::vector<std::vector<double> > interpolateResult;
		interpolateResult.resize(numberOfWavelength);

		unsigned int iterator = 0; //the iterator of wavelength in databaseModelResult
		unsigned int maxIterator = databaseModelResult.size();

		for (unsigned int i = 0; i < numberOfWavelength; ++i) {
			interpolateResult[i] = std::vector<double>();
			interpolateResult[i].resize(numberOfValues);
			interpolateResult[i][0] = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[i];

			while (iterator < maxIterator && databaseModelResult[iterator][0] < interpolateResult[i][0]) {
				iterator++;
			}
			if (iterator == 0) {
				// if minimum wavelength in databaseModelResult is larger than wavelength i in solarIrradianceValues,
				// the first i interpolated values are set as databaseModelResult[0]
				interpolateResult[i] = databaseModelResult[0];
				interpolateResult[i][0] = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[i];
			} else if (iterator >= maxIterator) {
				// if maximum wavelength in databaseModelResult is smaller than wavelength i in solarIrradianceValues,
				// the last (numberOfWavelength - i) interpolated values are set as databaseModelResult[maxIterator - 1]
				interpolateResult[i] = databaseModelResult[maxIterator - 1];
				interpolateResult[i][0] = _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector[i];
			} else {
				// wavelength i in solarIrradianceValues is just between two successive wavelength in databaseModelResult
				// if the spectral resolution of databaseModelResult is better than that of _solarIrradianceValues, we lose some spectral informations
				myGreatLinearInterpolation(interpolateResult[i], databaseModelResult[iterator - 1], databaseModelResult[iterator]);
			}
		}

		databaseModelResult.clear();
		databaseModelResult = interpolateResult;
	}

	static void resamplingDatabaseAtmophericValuesWithGasDatabase(	std::vector<std::vector<double> > gasModelResult,
																	std::vector<std::vector<double> > & databaseModelResult) {
		unsigned int numberOfWavelength = gasModelResult.size();
		unsigned int numberOfValues = databaseModelResult[0].size();

		std::vector<std::vector<double> > interpolateResult;
		interpolateResult.resize(numberOfWavelength);

		unsigned int iterator = 0;				//the iterator of wavelength in databaseModelResult
		unsigned int maxIterator = databaseModelResult.size();

		for (unsigned int i = 0; i < numberOfWavelength; ++i) {
			interpolateResult[i] = std::vector<double>();
			interpolateResult[i].resize(numberOfValues);
			interpolateResult[i][0] = gasModelResult[i][0];

			while (iterator < maxIterator && databaseModelResult[iterator][0] < interpolateResult[i][0]) {
				iterator++;
			}
			if (iterator == 0) {
				// if minimum wavelength in databaseModelResult is larger than wavelength i in solarIrradianceValues,
				// the first i interpolated values are set as databaseModelResult[0]
				interpolateResult[i] = databaseModelResult[0];
				interpolateResult[i][0] = gasModelResult[i][0];
			} else if (iterator >= maxIterator) {
				// if maximum wavelength in databaseModelResult is smaller than wavelength i in solarIrradianceValues,
				// the last (numberOfWavelength - i) interpolated values are set as databaseModelResult[maxIterator - 1]
				interpolateResult[i] = databaseModelResult[maxIterator - 1];
				interpolateResult[i][0] = gasModelResult[i][0];
			} else {
				// wavelength i in solarIrradianceValues is just between two successive wavelength in databaseModelResult
				// if the spectral resolution of databaseModelResult is better than that of _solarIrradianceValues, we lose some spectral informations
				myGreatLinearInterpolation(interpolateResult[i], databaseModelResult[iterator - 1], databaseModelResult[iterator]);
			}
		}

		databaseModelResult.clear();
		databaseModelResult = interpolateResult;
	}

	static void myGreatLinearInterpolation(	std::vector<double> & interpolateResult,
											const std::vector<double> & infModelResult,
											const std::vector<double> & supModelResult) {
		if (fabs(infModelResult[0] - supModelResult[0]) == 0) {
			cout << endl << "WARNING : Division by 0 in myGreatLinearInterpolation() !!!" << endl;
		}

		for (unsigned int i = 1; i < interpolateResult.size(); ++i) {
			interpolateResult[i] = ((infModelResult[i] * (supModelResult[0] - interpolateResult[0]))
					+ (supModelResult[i] * (interpolateResult[0] - infModelResult[0]))) / (supModelResult[0] - infModelResult[0]);
		}
	}

	static void computeSpectralTransmittancePerBand(PhaseGeneralParametersDTO & genDTO,
													const AtmosphereParameters & atmosphereParameters,
													AtmosphereComposition & atmospherePerBandMeanValues,
													const unsigned int & spectralBandIndex,
													DartDatabaseManager & databaseManager,
													DartDatabaseManager & _croSecDBmanager) {

		double beginBandWavelength = 0, endBandWavelength = 0;
		double currentWavelength = 0, previousWavelength = 0;
		double intervalBetweenSuccesiveWavelength = 0;
		double totalIrradianceTOA = 0;
		unsigned int iterator = 0, numberOfIrradianceTOA = 0;
		std::vector<std::vector<double> > aerosolsParametersResultPerAerosol, gasModelResult, waterResult;
		std::vector<std::vector<std::vector<double> > > aerosolsParametersResult, crossSectionResult; //per type, per lamda, per item (optical properties or altitude)
		std::vector<std::vector<double> >::const_iterator itWaterResults, itGasModelResults, itAerosolsResults;

		atmospherePerBandMeanValues.initializeAerosolPropertyVector(atmosphereParameters.numberOfAerosol);
		atmospherePerBandMeanValues.initializeCrossSectionVector(atmosphereParameters.numberOfAbsorbingGas);

		AtmosphereComposition currentAtmosphereComponents = AtmosphereComposition();
		currentAtmosphereComponents.initializeAerosolPropertyVector(atmosphereParameters.numberOfAerosol);
		currentAtmosphereComponents.initializeCrossSectionVector(atmosphereParameters.numberOfAbsorbingGas);
		AtmosphereComposition previousAtmosphereComponents(currentAtmosphereComponents);
		AtmosphereComposition beginBandAtmosphereComponents(currentAtmosphereComponents);
		AtmosphereComposition endBandAtmosphereComponents(currentAtmosphereComponents);

		for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
			PhaseDatabaseDAO::readAtmosphericParametersPerBand(databaseManager, atmosphereParameters.aerosolsModelName[i],
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::AEROSOLS_MODEL),
				_userSpectralBands[spectralBandIndex], aerosolsParametersResultPerAerosol);
			aerosolsParametersResult.push_back(aerosolsParametersResultPerAerosol);
		}
		PhaseDatabaseDAO::readAtmosphericParametersPerBand(databaseManager, atmosphereParameters.gasModelName,
			AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::GAS_MODEL), _userSpectralBands[spectralBandIndex],
			gasModelResult);

		CrossSectionDatabaseDAO::readAtmosphericParametersPerBand(_croSecDBmanager, atmosphereParameters.gasModelName,
			_userSpectralBands[spectralBandIndex].lowerLambdaValue, _userSpectralBands[spectralBandIndex].upperLambdaValue, crossSectionResult);

		// Si on n'utilise pas la DB solar_constant, on ne peut pas resampling la DB atmosphere dessus ...
		if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0) {
			for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
				resamplingDatabadeAtmophericValues(spectralBandIndex, aerosolsParametersResult[i]);
			}

			resamplingDatabadeAtmophericValues(spectralBandIndex, gasModelResult);
			for (int i = 0; i < atmosphereParameters.numberOfAbsorbingGas; i++) {
				resamplingDatabadeAtmophericValues(spectralBandIndex, crossSectionResult[i]);
			}
			// Verification si l'echantillonage des bandes spectrales est correct.
			if (!checkSpectralBandSamplingInAtmosphericDatabase(aerosolsParametersResult[0], gasModelResult)
					or !checkSpectralBandSamplingInAtmosphericDatabase(crossSectionResult[0], gasModelResult)) {
				cerr
						<< "Warning: spectral bands sampling is different between gas and aerosols or absorption cross section table, results may be wrong."
						<< endl;
			}
		}
		// wavelength number of gas and cross section within one user-defined band maybe different
		// (cross-secion introduced only in TIR domain)
		if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() != 0) {
			for (int i = 0; i < atmosphereParameters.numberOfAbsorbingGas; i++){
				resamplingDatabaseAtmophericValuesWithGasDatabase(gasModelResult, crossSectionResult[i]);
			}
		}

		// Lecture & resempling de la table h2o dans la DB atmosphere (_gh2o) pour un modele de gas donne
		if (atmosphereParameters.h2oAmountCkeckbox) {
			PhaseDatabaseDAO::readAtmosphericParametersPerBand(databaseManager, atmosphereParameters.gasModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::WATER), _userSpectralBands[spectralBandIndex],
				waterResult);
			// Si on n'utilise pas la DB solar_constant, on ne peut pas resampling la DB dessus ...
			if (PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0) {
				resamplingDatabadeAtmophericValues(spectralBandIndex, waterResult);
			}
			itWaterResults = waterResult.begin();
		}

		_solarIrradianceValues[spectralBandIndex].numberOfGasSpectralValue = aerosolsParametersResult[0].size();

		// iterator represents different wavelength within one user-defined band, itAerosolsResults and itGasModelResults are used
		// because the wavelength number of gas and aerosol within one user-defined band maybe different
		itGasModelResults = gasModelResult.begin();
		for (itAerosolsResults = aerosolsParametersResult[0].begin(); itAerosolsResults != aerosolsParametersResult[0].end();
				++itAerosolsResults, ++itGasModelResults)
		{

			std::vector<std::vector<double> > rowAerosols;
			std::vector<std::vector<double> > rowCroSec;

			for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
				rowAerosols.push_back((aerosolsParametersResult[i][iterator]));
			}

			for (int i = 0; i < atmosphereParameters.numberOfAbsorbingGas; i++) {
				rowCroSec.push_back((crossSectionResult[i][iterator]));
			}

			const std::vector<double> & firstAerosols = *itAerosolsResults;
			const std::vector<double> & rowGas = *itGasModelResults;

			if (iterator == 0) {
				//cout << "   -> compute1 begin  " <<iterator<< endl;
				beginBandWavelength = firstAerosols.at(0);
				currentWavelength = beginBandWavelength;
				beginBandAtmosphereComponents.setAtmosphericComponentsFromDatabase(rowAerosols, rowGas, rowCroSec, atmosphereParameters);

				computeTotalGasTransmittanceForAbsoption(beginBandAtmosphereComponents);

				computeComplementaryAtmosphereParameters(beginBandAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

				if (atmosphereParameters.h2oAmountCkeckbox) {
					treatWater(atmosphereParameters, itWaterResults, beginBandAtmosphereComponents);
				}
				//cout << "   -> compute1 end  " <<iterator<< endl;

			} else if (iterator == 1) {

				//cout << "   -> compute2 begin  " <<iterator<< endl;
				currentWavelength = firstAerosols.at(0);

				currentAtmosphereComponents.setAtmosphericComponentsFromDatabase(rowAerosols, rowGas, rowCroSec, atmosphereParameters);

				computeTotalGasTransmittanceForAbsoption(currentAtmosphereComponents);

				computeComplementaryAtmosphereParameters(currentAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

				if (atmosphereParameters.h2oAmountCkeckbox) {
					treatWater(atmosphereParameters, itWaterResults, currentAtmosphereComponents);
				}

				// as the wavelengths and the associated spectral values are alreay resampled, I think:
				// beginBandWavelength is always equal to _userSpectralBands[spectralBandIndex].lowerLambdaValue
				// this interpolation could be replaced by "previousAtmosphereComponents = beginBandAtmosphereComponents" ****** commented by Yingjie
				computeInterpolationForAtmosphereValues(beginBandAtmosphereComponents, currentAtmosphereComponents, previousAtmosphereComponents,
					beginBandWavelength, currentWavelength, _userSpectralBands[spectralBandIndex].lowerLambdaValue);

				previousWavelength = _userSpectralBands[spectralBandIndex].lowerLambdaValue;

				computeTotalGasTransmittanceForAbsoption(previousAtmosphereComponents);

				computeComplementaryAtmosphereParameters(previousAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

				if (!_isComputedIrradianceBottomOfAtmosphereCheck && _irradianceFromDatabaseComputationMethod != 1) {
					_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[0] = computeBOA(
						_solarIrradianceValues[spectralBandIndex].skylVector[0],
						_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorptionVector[0],
						_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector[0], previousAtmosphereComponents,
						atmosphereParameters);
					_solarIrradianceValues[spectralBandIndex].skylVector[0] *= _solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[0];
					_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[0] =
							_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[0];
					if (genDTO.spectralDomain[spectralBandIndex] == 1) {
						_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[0] += temperatureToLuminanceForOneWavelength(
							genDTO.meanTemperature, 1, previousWavelength * 10E-6) * M_PI;
					} else if (genDTO.spectralDomain[spectralBandIndex] == 2) {
						_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[0] = temperatureToLuminanceForOneWavelength(
							genDTO.meanTemperature, 1, previousWavelength * 10E-6);
					}
				}
				//cout << "   -> compute2 end  " <<iterator<< endl;
			}

			if (iterator == _solarIrradianceValues[spectralBandIndex].numberOfGasSpectralValue - 1) {

				endBandWavelength = firstAerosols.at(0);

				endBandAtmosphereComponents.setAtmosphericComponentsFromDatabase(rowAerosols, rowGas, rowCroSec, atmosphereParameters);

				computeTotalGasTransmittanceForAbsoption(endBandAtmosphereComponents);

				computeComplementaryAtmosphereParameters(endBandAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

				if (atmosphereParameters.h2oAmountCkeckbox) {
					treatWater(atmosphereParameters, itWaterResults, endBandAtmosphereComponents);
				}

				if (previousWavelength != 0) {
					computeInterpolationForAtmosphereValues(previousAtmosphereComponents, endBandAtmosphereComponents, currentAtmosphereComponents,
						previousWavelength, endBandWavelength, _userSpectralBands[spectralBandIndex].upperLambdaValue);
				} else if (beginBandWavelength != 0) {
					computeInterpolationForAtmosphereValues(beginBandAtmosphereComponents, endBandAtmosphereComponents, currentAtmosphereComponents,
						beginBandWavelength, endBandWavelength, _userSpectralBands[spectralBandIndex].upperLambdaValue);
				} else {
					currentAtmosphereComponents = endBandAtmosphereComponents;
				}

				currentWavelength = _userSpectralBands[spectralBandIndex].upperLambdaValue;
				computeTotalGasTransmittanceForAbsoption(currentAtmosphereComponents);
				computeComplementaryAtmosphereParameters(currentAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);
			}

			if (iterator != _solarIrradianceValues[spectralBandIndex].numberOfGasSpectralValue - 1 && iterator != 0 && iterator != 1) {

				currentWavelength = firstAerosols.at(0);
				currentAtmosphereComponents.setAtmosphericComponentsFromDatabase(rowAerosols, rowGas, rowCroSec, atmosphereParameters);
				computeTotalGasTransmittanceForAbsoption(currentAtmosphereComponents);
				computeComplementaryAtmosphereParameters(currentAtmosphereComponents, atmosphereParameters.muPrimeGaz,
					atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);
				if (atmosphereParameters.h2oAmountCkeckbox) {
					treatWater(atmosphereParameters, itWaterResults, currentAtmosphereComponents);
				}
			}

			if (iterator != 0) {
				intervalBetweenSuccesiveWavelength = currentWavelength - previousWavelength;
				totalIrradianceTOA += computeTrapezeAtmosphere(previousAtmosphereComponents, currentAtmosphereComponents, atmospherePerBandMeanValues,
					intervalBetweenSuccesiveWavelength, iterator, spectralBandIndex);

				numberOfIrradianceTOA++;
				if (!_isComputedIrradianceBottomOfAtmosphereCheck && _irradianceFromDatabaseComputationMethod != 1) {
					_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[iterator] = computeBOA(
						_solarIrradianceValues[spectralBandIndex].skylVector[iterator],
						_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorptionVector[iterator],
						_solarIrradianceValues[spectralBandIndex].irradianceTopOfAtmosphereVector[iterator], currentAtmosphereComponents,
						atmosphereParameters);
					_solarIrradianceValues[spectralBandIndex].skylVector[iterator] *= _solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[iterator];
					_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[iterator] =
							_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector[iterator];
					if (genDTO.spectralDomain[spectralBandIndex] == 1) {
						_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[iterator] += temperatureToLuminanceForOneWavelength(
							genDTO.meanTemperature, 1, previousWavelength * 10E-6) * M_PI;
					} else if (genDTO.spectralDomain[spectralBandIndex] == 2) {
						_solarIrradianceValues[spectralBandIndex].irradianceForPonderationVector[iterator] = temperatureToLuminanceForOneWavelength(
							genDTO.meanTemperature, 1, previousWavelength * 10E-6);
					}
				}
			}

			iterator++;
			previousAtmosphereComponents = currentAtmosphereComponents;
			previousWavelength = currentWavelength;

			if (atmosphereParameters.h2oAmountCkeckbox) {
				++itWaterResults;
			}
		}

		if (iterator == 1) {
			atmospherePerBandMeanValues = beginBandAtmosphereComponents;
			computeTotalGasTransmittanceForAbsoption(atmospherePerBandMeanValues);
			computeComplementaryAtmosphereParameters(atmospherePerBandMeanValues, atmosphereParameters.muPrimeGaz,
				atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);
		} else if (iterator == 2
				|| _userSpectralBands[spectralBandIndex].upperLambdaValue == _userSpectralBands[spectralBandIndex].lowerLambdaValue) {
			for (int i = 0; i < AtmosphereComposition::NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS; ++i) {
				atmospherePerBandMeanValues.setAtmosphereComponentPerIndex(i,
					0.5
							* (previousAtmosphereComponents.getAtmosphereComponentPerIndex(i)
									+ currentAtmosphereComponents.getAtmosphereComponentPerIndex(i)));
			}

			for (int i = 0; i < atmosphereParameters.numberOfAbsorbingGas; i++) {
				for (int j = 0; j < 36; j++) {
					atmospherePerBandMeanValues.setCroSec(i, j,
						0.5 * (previousAtmosphereComponents.getCroSecPerIndex(i, j) + currentAtmosphereComponents.getCroSecPerIndex(i, j)));
				}
			}

			for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
				atmospherePerBandMeanValues.setAerosolOpticalDepthPerIndex(i,
					0.5 * (previousAtmosphereComponents.getAerosolOpticalDepth(i) + currentAtmosphereComponents.getAerosolOpticalDepth(i)));
				atmospherePerBandMeanValues.setAerosolSingleScatteringAlbedoPerIndex(i,
					0.5
							* (previousAtmosphereComponents.getAerosolSingleScatteringAlbedo(i)
									+ currentAtmosphereComponents.getAerosolSingleScatteringAlbedo(i)));
			}
			computeTotalGasTransmittanceForAbsoption(atmospherePerBandMeanValues);

			computeComplementaryAtmosphereParameters(atmospherePerBandMeanValues, atmosphereParameters.muPrimeGaz,
				atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

		} else {

			if (totalIrradianceTOA != 0) {
				double sunFactor = 1. / totalIrradianceTOA;
				multiplyAtmosphereValues(atmospherePerBandMeanValues, sunFactor);
			}
//			cout<<"iter = "<<iterator<<"\t meanValue_co2 = "<<atmospherePerBandMeanValues.getCo2TransmittanceAbs()<<"\t od = "<<-log(atmospherePerBandMeanValues.getCo2TransmittanceAbs())<<endl;
//
//			atmospherePerBandMeanValues.setAtmosphereComponentPerIndex(2,1.978353987e-34);
//			cout<<"iter = "<<iterator<<"\t meanValue_co2 = "<<atmospherePerBandMeanValues.getCo2TransmittanceAbs()<<"\t od = "<<-log(atmospherePerBandMeanValues.getCo2TransmittanceAbs())<<endl;

			computeTotalGasTransmittanceForAbsoption(atmospherePerBandMeanValues);

			computeComplementaryAtmosphereParameters(atmospherePerBandMeanValues, atmosphereParameters.muPrimeGaz,
				atmosphereParameters.muPrimeAerosols, atmosphereParameters.gas_u, atmosphereParameters.aerosols_u);

		}
//		cout<<"iter = "<<iterator<<"\t meanValue_co2 = "<<atmospherePerBandMeanValues.getCo2TransmittanceAbs()<<"\t od = "<<-log(atmospherePerBandMeanValues.getCo2TransmittanceAbs())<<"\t total = "<<
//				-log(atmospherePerBandMeanValues.getGasTransmittanceForAbsoption())<<endl;

		if (_calculatorMethod != 2) {

			if (_irradianceFromDatabaseComputationMethod == 1) {
				_solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere = computeBOA(
					_solarIrradianceValues[spectralBandIndex].meanSKYL,
					_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorption,_solarIrradianceValues[spectralBandIndex].meanIrradianceTopOfAtmosphere,
					atmospherePerBandMeanValues, atmosphereParameters);
			} else if (_irradianceFromDatabaseComputationMethod == 2) {
				_solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere = computeBOA(
					_solarIrradianceValues[spectralBandIndex].meanSKYL,
					_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorption, getMeanIrradianceTopOfAtmosphere(spectralBandIndex),
					atmospherePerBandMeanValues, atmosphereParameters);
			} else if (!_isComputedIrradianceBottomOfAtmosphereCheck) {
				_solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere = computeMeanIrradianceWithTrapezoidalMethode(
					spectralBandIndex, _solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector,
					_solarIrradianceValues[spectralBandIndex].irradianceBottomOfAtmosphereVector);
				_solarIrradianceValues[spectralBandIndex].meanSKYL = computeMeanIrradianceWithTrapezoidalMethode(spectralBandIndex,
					_solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector,
					_solarIrradianceValues[spectralBandIndex].skylVector)
						/ _solarIrradianceValues[spectralBandIndex].meanIrradianceBottomOfAtmosphere;
				_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorption = computeMeanIrradianceWithTrapezoidalMethode(spectralBandIndex,
					_solarIrradianceValues[spectralBandIndex].irradianceSpectralWavelengthVector,
					_solarIrradianceValues[spectralBandIndex].temperatureAtmosphereAbsorptionVector);
			}
		}
	}

	static void computeSpectralTransmittancePerThread(	PhaseGeneralParametersDTO & genDTO,
														const AtmosphereParameters & atmosphereParameters,
														std::vector<AtmosphereComposition> & atmospherePerBandMeanValues,
														const std::string & databasePath,
														const unsigned int & threadNumber) {
		unsigned int spectralBandIndex = 0;
		session currentSession;
		std::string completeNameDatabase = databasePath + atmosphereParameters.databaseName;
		DartDatabaseManager databaseManager(completeNameDatabase);
		DartDatabaseManager _croSecDBmanager(DartEnv::getDatabasePath("equivalent_cross_section.db") + "equivalent_cross_section.db");
		if (databaseManager.connectDatabase() and _croSecDBmanager.connectDatabase()) {
			for (unsigned int i = 0; i < _threadEnd[threadNumber]; i++) {

				spectralBandIndex = threadNumber + _numberOfThreads * i;
				atmospherePerBandMeanValues[spectralBandIndex] = AtmosphereComposition();
				computeSpectralTransmittancePerBand(genDTO, atmosphereParameters, atmospherePerBandMeanValues[spectralBandIndex], spectralBandIndex,
					databaseManager, _croSecDBmanager);
			}
			databaseManager.deconnectDatabase();
			_croSecDBmanager.deconnectDatabase();
		}
	}

	static double computeMuPrime(	const double altitude,
									const double & H,
									const double & cosThetaSun) {
		double eta = (RAYON_TERRESTRE * 1000. + altitude) / H;
		double root = eta * eta * cosThetaSun * cosThetaSun + 2 * eta + 1;
		double sum = sqrt(root) - eta * cosThetaSun;
		double results = 1;
		if (sum > 0) {
			results = 1. / sum;
		}
		return results;
	}

	static void computeSpectralTransmittance(	PhaseGeneralParametersDTO & genDTO,
												AtmosphereParameters & atmosphereParameters,
												std::vector<AtmosphereComposition> & atmospherePerBandMeanValues,
												const vector<DirectionCartesienne> & listDirections) {

		atmosphereParameters.mus = fabs(cos(_sunZenithAngleInRadians));
		atmosphereParameters.muPrimeGaz = computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.gasScaleFactor,
			atmosphereParameters.mus);
		atmosphereParameters.muPrimeAerosols = computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.aerosolScaleFactor,
			atmosphereParameters.mus);
		atmosphereParameters.mPrimeGaz = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.gasScaleFactor)
				/ atmosphereParameters.muPrimeGaz;
		atmosphereParameters.mPrimeAerosols = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.aerosolScaleFactor)
				/ atmosphereParameters.muPrimeAerosols;
		std::string databasePath = DartEnv::getDatabasePath(atmosphereParameters.databaseName);

		computeMPrimeGasAndAerosolsForAnalyticalTemperature(atmosphereParameters, listDirections);

		boost::thread_group spectralTransmittanceThreads;
		//cout << "   -> number of aerosol transmi  " <<atmosphereParameters.numberOfAerosol<< endl;
		for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
			boost::thread *t = new boost::thread(computeSpectralTransmittancePerThread, boost::ref(genDTO), atmosphereParameters,
				boost::ref(atmospherePerBandMeanValues), databasePath, threadNumber);
			spectralTransmittanceThreads.add_thread(t);
		}
		spectralTransmittanceThreads.join_all();

		_isComputedIrradianceBottomOfAtmosphereCheck = true;
	}

	/**
	 * Compute mprimeGas & mprimeAerosol for each theta directions in order to be used for analytical temperature factor (mode T & RT)
	 */
	static void computeMPrimeGasAndAerosolsForAnalyticalTemperature(	AtmosphereParameters& atmosphereParameters,
																		const std::vector<DirectionCartesienne> & listDirections)
	{
		double mus = 0, muPrimeGaz = 0, muPrimeAerosols = 0, mPrimeGaz = 0, mPrimeAerosols = 0;
		atmosphereParameters.mPrimeGazPerDirection.resize(90, 1.);
		atmosphereParameters.mPrimeAerosolsPerDirection.resize(90, 1.);
		for (unsigned int thetaDir = 0; thetaDir < 90; thetaDir++)
		{
			mus = fabs(cos(deg2rad(thetaDir)));
			muPrimeGaz = PhaseProperty::computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.gasScaleFactor,mus);
			muPrimeAerosols = PhaseProperty::computeMuPrime(atmosphereParameters.sceneAltitude, atmosphereParameters.aerosolScaleFactor,mus);
			mPrimeGaz = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.gasScaleFactor) / muPrimeGaz;
			mPrimeAerosols = exp(-atmosphereParameters.sceneAltitude / atmosphereParameters.aerosolScaleFactor) / muPrimeAerosols;
			atmosphereParameters.mPrimeGazPerDirection[thetaDir] = mPrimeGaz;
			atmosphereParameters.mPrimeAerosolsPerDirection[thetaDir] = mPrimeAerosols;
		}
	}

	static double computeMeanDirectTransmittance(	const double & dTauDirect,
													const std::vector<DirectionCartesienne> & listDirections) {
		double sum = 0.0;
		double sumAngle = 0.0;
		double cosineAngle = 0;
		for (const DirectionCartesienne & dir : listDirections) {
			if (!dir.isVirtual() && dir.getZ() > 0) {
				sum += dir.getZ() * dir.getSolidAngle();
				sumAngle += dir.getSolidAngle();
			}
		}

		if (sumAngle == 0) {
			return 0;
		}
		return sum * dTauDirect / (sumAngle);
	}

	static double computeSpecularAlbedo(const double & multiplicativeFactorForSpecularReflectance,
										const double & medium1RefractiveIndex,
										const double & medium2RefractiveIndex,
										const unsigned int & numberOfSteps) {
		double specularAlbedo = 0.0;
		double thetaStep = M_PI05 / double(numberOfSteps);
		double middleTheta = thetaStep / 2.0;
		double criticalAngle = 0.0;
		double perpendicularFresnelCoefficient = 0.0;
		double parallelFresnelCoefficient = 0.0;

		if (multiplicativeFactorForSpecularReflectance > 0) {
			if (medium1RefractiveIndex > medium2RefractiveIndex) {
				criticalAngle = asin(medium2RefractiveIndex / medium1RefractiveIndex);
			}
			for (unsigned int step = 0; step < numberOfSteps - 1; step++) {
				middleTheta += thetaStep;

				if (middleTheta <= EPSILON) {
					perpendicularFresnelCoefficient = (medium1RefractiveIndex - medium2RefractiveIndex)
							/ (medium1RefractiveIndex + medium2RefractiveIndex);
					perpendicularFresnelCoefficient *= perpendicularFresnelCoefficient;
					parallelFresnelCoefficient = perpendicularFresnelCoefficient;
				} else if ((medium1RefractiveIndex < medium2RefractiveIndex)
						|| ((medium1RefractiveIndex > medium2RefractiveIndex) && (middleTheta < criticalAngle))) {
					perpendicularFresnelCoefficient = sin(middleTheta - asin(sin(middleTheta) * medium1RefractiveIndex / medium2RefractiveIndex))
							/ sin(middleTheta + asin(sin(middleTheta) * medium1RefractiveIndex / medium2RefractiveIndex));
					perpendicularFresnelCoefficient *= perpendicularFresnelCoefficient;
					parallelFresnelCoefficient = tan(middleTheta - asin(sin(middleTheta) * medium1RefractiveIndex / medium2RefractiveIndex))
							/ tan(middleTheta + asin(sin(middleTheta) * medium1RefractiveIndex / medium2RefractiveIndex));
					parallelFresnelCoefficient *= parallelFresnelCoefficient;
				} else if ((medium1RefractiveIndex > medium2RefractiveIndex) && (middleTheta >= criticalAngle)) {
					perpendicularFresnelCoefficient = 1.0;
					parallelFresnelCoefficient = 1.0;
				}
				specularAlbedo += 0.5 * (perpendicularFresnelCoefficient + parallelFresnelCoefficient) * cos(middleTheta) * sin(middleTheta)
						* thetaStep;
			}
			specularAlbedo *= multiplicativeFactorForSpecularReflectance * 2;
		}
		return (specularAlbedo);
	}

	static void initializeHenyeyGreensteinParameters(	vector<vector<HenyeyGreensteinParameters> > & henyeyGreensteinParameters,
														int nbOfAero,
														int spectralBandIndex) {
		for (int i = 0; i < nbOfAero; i++) {
			henyeyGreensteinParameters[i][spectralBandIndex].a = 0;
			henyeyGreensteinParameters[i][spectralBandIndex].g1 = 0;
			henyeyGreensteinParameters[i][spectralBandIndex].g2 = 0;
		}
	}

	static void interpolateHenyeyGreensteinParameters(	const HenyeyGreensteinParameters & lowerLimitValues,
														const HenyeyGreensteinParameters & upperLimitValues,
														HenyeyGreensteinParameters & interpolatedValues,
														const double & lowerLimitWavelength,
														const double & upperLimitWavelength,
														const double & interpolatedWavelength) {
		double lowerLimitMultiplicativeCoefficient = interpolatedWavelength - lowerLimitWavelength;
		double upperLimitMultiplicativeCoefficient = upperLimitWavelength - interpolatedWavelength;
		double distanceBewteenLowerAndUpperLimits = upperLimitWavelength - lowerLimitWavelength;

		interpolatedValues.a = Interpolation_lineaire(lowerLimitMultiplicativeCoefficient, upperLimitMultiplicativeCoefficient,
			distanceBewteenLowerAndUpperLimits, lowerLimitValues.a, upperLimitValues.a);

		interpolatedValues.g1 = Interpolation_lineaire(lowerLimitMultiplicativeCoefficient, upperLimitMultiplicativeCoefficient,
			distanceBewteenLowerAndUpperLimits, lowerLimitValues.g1, upperLimitValues.g1);

		interpolatedValues.g2 = Interpolation_lineaire(lowerLimitMultiplicativeCoefficient, upperLimitMultiplicativeCoefficient,
			distanceBewteenLowerAndUpperLimits, lowerLimitValues.g2, upperLimitValues.g2);

	}

	static void computeTrapezeHenleyGreenstein(	HenyeyGreensteinParameters & henleyGreensteinParametersPrec,
												HenyeyGreensteinParameters & henleyGreensteinParameters,
												HenyeyGreensteinParameters & henleyGreensteinParametersMeanValues,
												double & intervalBetweenSuccesiveWavelength) {
		computeTrapezoidalMethod(henleyGreensteinParametersPrec.a, henleyGreensteinParameters.a, intervalBetweenSuccesiveWavelength,
			henleyGreensteinParametersMeanValues.a);
		computeTrapezoidalMethod(henleyGreensteinParametersPrec.g1, henleyGreensteinParameters.g1, intervalBetweenSuccesiveWavelength,
			henleyGreensteinParametersMeanValues.g1);
		computeTrapezoidalMethod(henleyGreensteinParametersPrec.g2, henleyGreensteinParameters.g2, intervalBetweenSuccesiveWavelength,
			henleyGreensteinParametersMeanValues.g2);
	}

	static void normalizeHenleyGreenstein(	HenyeyGreensteinParameters & henleyGreensteinParameters,
											double & normalisation) {
		henleyGreensteinParameters.a /= normalisation;
		henleyGreensteinParameters.g1 /= normalisation;
		henleyGreensteinParameters.g2 /= normalisation;
	}

	static void saveHenyeyGreensteinParameters(std::vector<std::vector<HenyeyGreensteinParameters> > & henleyGreensteinMeanParameters) {
		unsigned int numberOfAerosol = henleyGreensteinMeanParameters.size();
		_numberOfAerosols = numberOfAerosol;
		_parameterForAerosolPhaseFunctionA.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));
		_parameterForAerosolPhaseFunctionG1.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));
		_parameterForAerosolPhaseFunctionG2.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));

		for (unsigned int i = 0; i < numberOfAerosol; i++) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
				_parameterForAerosolPhaseFunctionA[i][spectralBandIndex] = henleyGreensteinMeanParameters[i][spectralBandIndex].a;
				_parameterForAerosolPhaseFunctionG1[i][spectralBandIndex] = henleyGreensteinMeanParameters[i][spectralBandIndex].g1;
				_parameterForAerosolPhaseFunctionG2[i][spectralBandIndex] = henleyGreensteinMeanParameters[i][spectralBandIndex].g2;
			}
		}
	}
	static void saveHenyeyGreensteinParameters(const AtmosphereParameters& atmosphereParameters) {
		unsigned int numberOfAerosol = atmosphereParameters.numberOfAerosol;
		_numberOfAerosols = numberOfAerosol;
		_parameterForAerosolPhaseFunctionA.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));
		_parameterForAerosolPhaseFunctionG1.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));
		_parameterForAerosolPhaseFunctionG2.resize(numberOfAerosol, vector<double>(_numberOfSpectralBands));
		for (unsigned int i = 0; i < numberOfAerosol; i++) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
				_parameterForAerosolPhaseFunctionA[i][spectralBandIndex] = atmosphereParameters.henleyGreensteinParameters[i].a;
				_parameterForAerosolPhaseFunctionG1[i][spectralBandIndex] = atmosphereParameters.henleyGreensteinParameters[i].g1;
				_parameterForAerosolPhaseFunctionG2[i][spectralBandIndex] = atmosphereParameters.henleyGreensteinParameters[i].g2;
			}
		}
	}
	static void readHenyeyGreensteinParameters(	AtmosphereParameters & atmosphereParameters,
												std::vector<std::vector<HenyeyGreensteinParameters> > & henleyGreensteinMeanParameters) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {

			initializeHenyeyGreensteinParameters(henleyGreensteinMeanParameters, atmosphereParameters.numberOfAerosol, spectralBandIndex);
			DartDatabaseManager databaseManager(_atmosphericDatabasePath);
			if (databaseManager.connectDatabase()) {
				std::vector<std::vector<std::vector<double> > > hgResults;
				std::vector<std::vector<double> > hgResultsPerAerosol;
				for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {

					PhaseDatabaseDAO::readHgParametersPerBand(databaseManager, atmosphereParameters.hgParametersModelName[i],
						AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::HG_PARAMETERS),
						_userSpectralBands[spectralBandIndex], hgResultsPerAerosol);
					hgResults.push_back(hgResultsPerAerosol);
//					for(int j = 0; j<hgResultsPerAerosol.size();j++){
//						cout<<setprecision(8)<<"	aerosol "<<i <<" "<<atmosphereParameters.hgParametersModelName[i]<<": "<<spectralBandIndex<<" "
//								<<" "<<hgResultsPerAerosol[j][0]<<"\t"<<hgResultsPerAerosol[j][1]<<"\t"<<hgResultsPerAerosol[j][2]
//								<<"\t"<<hgResultsPerAerosol[j][3]<<endl;
//					}
				}

				databaseManager.deconnectDatabase();
				for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
					std::vector<std::vector<double> > res(hgResults[i]);
					res.resize(res.size() + 2, res[0]);
					Interpolation_Trapeze(_userSpectralBands[spectralBandIndex].lowerLambdaValue,
						_userSpectralBands[spectralBandIndex].upperLambdaValue, hgResults[i].size(), hgResults[i][0].size(), hgResults[i], res);
					henleyGreensteinMeanParameters[i][spectralBandIndex].a = res[res.size() - 1][1];
					henleyGreensteinMeanParameters[i][spectralBandIndex].g1 = res[res.size() - 1][2];
					henleyGreensteinMeanParameters[i][spectralBandIndex].g2 = res[res.size() - 1][3];
				}
			}
		}
	}

	static void readAndProcessGroundH2Oparameters(AtmosphereParameters & atmosphereParameters) {
		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			std::vector<std::vector<double> > gasParametersResults;
			PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.gasModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::WATER_AMOUNT_PARA), gasParametersResults);
			databaseManager.deconnectDatabase();

			for (std::vector<std::vector<double> >::const_iterator it = gasParametersResults.begin(); it != gasParametersResults.end(); ++it) {
				const std::vector<double> & row = *it;
				if (row.size() != 6) {
					cerr << "Inconsistent size at WATER_AMOUNT_PARA" << endl;
				}
				atmosphereParameters.ref_amount_H2O_atm_cm = row.at(0);
				atmosphereParameters.ref_amount_H2O_g_per_cm2 = row.at(1);
				atmosphereParameters.ref_density_H2O_atm_cm_per_km_0 = row.at(2);
				atmosphereParameters.ref_density_H2O_atm_cm_per_km_1 = row.at(3);
				atmosphereParameters.ref_temp_K_0 = row.at(4);
				atmosphereParameters.ref_temp_K_1 = row.at(5);
			}
		}
		if (atmosphereParameters.h2oAmountCkeckbox) {
			atmosphereParameters.processMulFactorForH2O();
		}

	}

	static unsigned int getNumberOfValuesInAllTable(std::string & table) {
		std::string stQuery = "SELECT COUNT(*) FROM " + table;
		int nbOfRows = 0;
		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			databaseManager.executeSingleColumnSingleValueReadingQuery(stQuery, nbOfRows);
			databaseManager.deconnectDatabase();
		}
		unsigned int ui_nbOfRows = nbOfRows; // compilo friendly :3
		return ui_nbOfRows;
	}

#ifdef DART_WITH_NETCDF
	static void readAndWriteAtmosphereComponentAsNcdfGroup(
			NcFile& ncFile,
			vector<AtmosphereComposition>& atmospherePerBandMeanValues,
			const AtmosphereParameters& atmosphereParameters) {

		/** GAS VERTICAL DISTRIBUTION **/
		std::string table = atmosphereParameters.aerosolCumulativeModelName[0] + "_aECP";
		unsigned int nbOfRows = getNumberOfValuesInAllTable(table);

		NcGroup vertDistribGroup = ncFile.addGroup(ATM_GRP_VERTICALDISTRIBUTION);

		NcDim gasAerosolsDimension = ncFile.addDim(ATM_DIM_ROWGASANDAEROSOL, 16+atmosphereParameters.numberOfAerosol);
		NcDim layerDimension = ncFile.addDim(ATM_DIM_LAYER, nbOfRows);

		vector<NcDim> vector2d_verticalDistribDimension;
		vector2d_verticalDistribDimension.push_back(layerDimension);
		vector2d_verticalDistribDimension.push_back(gasAerosolsDimension);

		NcVar gasVertDistribData = vertDistribGroup.addVar(ATM_VAR_GASANDAEROSOLVERTICALDISTRIBUTION, ncDouble, vector2d_verticalDistribDimension);
		gasVertDistribData.putAtt(ATM_ATT_Header, "Altitude(km) | ScatGas(-) | H2O(atm-cm/km) | O3(atm-cm/km) | N2(mol/cm2 km) | CO2(atm-cm/km) | CO(atm-cm/km) | CH4(atm-cm/km) | N2O(atm-cm/km) | O2(atm-cm/km) | NH3(atm-cm/km) | NO(atm-cm/km) | NO2(atm-cm/km) | SO2(atm-cm/km) | HNO3(atm-cm/km) | OtherGas(atm-cm/km) | N*Aerosols (550nm extinction (KM-1)) | (nb layers)* 36");
		std::vector<std::vector<double> > outData(layerDimension.getSize(), std::vector<double>(gasAerosolsDimension.getSize()));

		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {

			std::vector<std::vector<double> > resultsPerAerosol;
			std::vector<std::vector<std::vector<double> > > aerosolResults;
			for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
				PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.aerosolCumulativeModelName[i],
					AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::AEROSOLS_CUMULATIVE), resultsPerAerosol);
				aerosolResults.push_back(resultsPerAerosol);
			}

			std::vector<std::vector<double> > gasResults;
			PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.gasCumulativeModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::GAS_CUMLULATIVE), gasResults);
			databaseManager.deconnectDatabase();

			std::vector<std::vector<double> >::const_iterator itGas = gasResults.begin();
			int iter = 0;
			for (std::vector<std::vector<double> >::const_iterator itAerosols = aerosolResults[0].begin(); itAerosols != aerosolResults[0].end();
					++itAerosols, ++itGas, ++iter) {
				const std::vector<double> & rowGas = *itGas;
				for (int i = 0; i < 16; ++i) {
					outData[iter][i] = rowGas.at(i);
				}
				for (int j = 0; j < atmosphereParameters.numberOfAerosol; j++) {
					outData[iter][j+16] = aerosolResults[j][iter].at(1);
				}
			}
		}
		DartNetcdf::putNetcdfDataFrom2DVector(gasVertDistribData, outData);

		/** TEMPERATURE VERTICAL DISTRIBUTION **/
		session currentSession;
		std::string Table = atmosphereParameters.temperatureModelName + "_gTEMP";
		nbOfRows = getNumberOfValuesInAllTable(Table);

		NcDim temperatureAndPressureDimension = ncFile.addDim(ATM_DIM_ALTITUDEFORTEMP, nbOfRows);
		NcDim temperatureAndPressureDimension2 = ncFile.addDim(ATM_DIM_ROWALTTEMPPRESS, 3);
		vector<NcDim> vector2d_tempDistribDimension;
		vector2d_tempDistribDimension.push_back(temperatureAndPressureDimension);
		vector2d_tempDistribDimension.push_back(temperatureAndPressureDimension2);

		NcVar tempVertDistribData = vertDistribGroup.addVar(ATM_VAR_TEMPERATUREANDPRESSUREVERTDISTRIB, ncDouble, vector2d_tempDistribDimension);
		tempVertDistribData.putAtt(ATM_ATT_Header, "height(km) | temperature(K) | pressure(mb)");
		std::vector<std::vector<double> > outDataTemperatyre(temperatureAndPressureDimension.getSize(), std::vector<double>(temperatureAndPressureDimension2.getSize()));

		//DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			std::vector<std::vector<double> > temperatureResults;
			PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.temperatureModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::TEMPERATURE), temperatureResults);

			double offSetTemp = atmosphereParameters.calculateOffSetDefTemperature();
			unsigned int iter = 0;
			for (std::vector<std::vector<double> >::const_iterator it = temperatureResults.begin(); it != temperatureResults.end(); ++it) {
				const std::vector<double> & row = *it;
				outDataTemperatyre[iter][0] = row.at(0);
				outDataTemperatyre[iter][2] = row.at(2);
				if (atmosphereParameters.redefTemperature && row.at(0) < 11) {
					outDataTemperatyre[iter][1] = row.at(1)+offSetTemp;
				} else {
					outDataTemperatyre[iter][1] = row.at(1);
				}
				++iter;
			}
			databaseManager.deconnectDatabase();
		}
		DartNetcdf::putNetcdfDataFrom2DVector(tempVertDistribData, outDataTemperatyre);

		/** CROSS SECTION **/
		nbOfRows = atmospherePerBandMeanValues.size() * atmospherePerBandMeanValues[0].getCroSec().size();
		NcGroup eqAbsCrossSecGroup = ncFile.addGroup(ATM_GRP_EQABSCROSSEC);

		NcDim crossSectionLayerDimension = ncFile.addDim(ATM_DIM_CROSSSECTIONLAYER, 36+2);
		NcDim crossSectionMolecureDimension = ncFile.addDim(ATM_DIM_CROSSSECTIONMOLECULE, nbOfRows);
		vector<NcDim> crossSectionDistribDimension;
		crossSectionDistribDimension.push_back(crossSectionMolecureDimension);
		crossSectionDistribDimension.push_back(crossSectionLayerDimension);

		NcVar crossSecVertDistribData = eqAbsCrossSecGroup.addVar(ATM_VAR_EQABSCROSSECVERTDISTRIB, ncDouble, crossSectionDistribDimension);
		crossSecVertDistribData.putAtt(ATM_ATT_Header, "band index | molecule index (see list) | 0km(1/atm-cm) | "
				"1km(1/atm-cm) | 2km(1/atm-cm) | 3km(1/atm-cm) | 4km(1/atm-cm) | 5km(1/atm-cm) | 6km(1/atm-cm) | "
				" 7km(1/atm-cm) | 8km(1/atm-cm) | 9km(1/atm-cm) | 10km(1/atm-cm) | 11km(1/atm-cm) | 12km(1/atm-cm) | "
				" 13km(1/atm-cm) | 14km(1/atm-cm) | 15km(1/atm-cm) | 16km(1/atm-cm) | 17km(1/atm-cm) | 18km(1/atm-cm) | "
				" 19km(1/atm-cm) | 20km(1/atm-cm) | 21km(1/atm-cm) | 22km(1/atm-cm) | 23km(1/atm-cm) | 24km(1/atm-cm) | "
				" 25km(1/atm-cm) | 30km(1/atm-cm) | 35km(1/atm-cm) | 40km(1/atm-cm) | 45km(1/atm-cm) | 50km(1/atm-cm) | "
				" 55km(1/atm-cm) | 60km(1/atm-cm) | 70km(1/atm-cm) | 80km(1/atm-cm) | 100km(1/atm-cm)");
		crossSecVertDistribData.putAtt(ATM_ATT_Info,"Indexes starts at 0 and ends at N-1 (Computer-like listing)");
		std::vector<std::vector<double> > crossSecVertDistribOut(nbOfRows, std::vector<double>(36+2));
		for (unsigned int band = 0; band < atmospherePerBandMeanValues.size(); band++) {
			for (unsigned int mol = 0; mol < CrossSectionDatabaseDAO::_molecule.size(); mol++) {
				unsigned int iter = band*CrossSectionDatabaseDAO::_molecule.size()+mol;
				crossSecVertDistribOut[iter][0]=band;
				crossSecVertDistribOut[iter][1]=mol;
				//				crossSecVertDistribOut[iter][1]=CrossSectionDatabaseDAO::_molecule[mol];
				for (unsigned int z = 0; z < 36; z++) {
					crossSecVertDistribOut[iter][z+2]= atmospherePerBandMeanValues[band].getCroSecPerIndex(mol, z);
				}
			}
		}
		DartNetcdf::putNetcdfDataFrom2DVector(crossSecVertDistribData, crossSecVertDistribOut);

		// Molecule list
		ostringstream moleculeListOut;
		for (unsigned int mol = 0; mol < CrossSectionDatabaseDAO::_molecule.size(); mol++) {
			moleculeListOut << CrossSectionDatabaseDAO::_molecule[mol] << " ";
		}
		crossSecVertDistribData.putAtt(ATM_ATT_MOLECULELIST, moleculeListOut.str());
	}
#endif // DART_WITH_NETCDF


	static void readAndWriteOzone(	AtmosphereParameters & atmosphereParameters,
									std::ofstream & out) {
		std::string table = atmosphereParameters.aerosolCumulativeModelName[0] + "_aECP";
		unsigned int nbOfRows = getNumberOfValuesInAllTable(table);
		out
				<< "* Vertical Distribution: Altitude(km) ScatGas(-) H2O(atm-cm/km) O3(atm-cm/km) N2(mol/cm2 km) CO2(atm-cm/km) CO(atm-cm/km) CH4(atm-cm/km) N2O(atm-cm/km) O2(atm-cm/km) NH3(atm-cm/km) NO(atm-cm/km) NO2(atm-cm/km) SO2(atm-cm/km) HNO3(atm-cm/km) OtherGas(atm-cm/km)";
		for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
			out << _space_ << "Aerosol" << i + 1;
		}
		out << " (550nm extinction (KM-1)) (nb layers)*" << _space_ << nbOfRows << endl;

		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {

			std::vector<std::vector<double> > resultsPerAerosol;
			std::vector<std::vector<std::vector<double> > > aerosolResults;
			for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
				PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.aerosolCumulativeModelName[i],
					AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::AEROSOLS_CUMULATIVE), resultsPerAerosol);
				aerosolResults.push_back(resultsPerAerosol);
			}

			std::vector<std::vector<double> > gasResults;
			PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.gasCumulativeModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::GAS_CUMLULATIVE), gasResults);
			databaseManager.deconnectDatabase();

			std::vector<std::vector<double> >::const_iterator itGas = gasResults.begin();
			int iter = 0;
			for (std::vector<std::vector<double> >::const_iterator itAerosols = aerosolResults[0].begin(); itAerosols != aerosolResults[0].end();
					++itAerosols, ++itGas, ++iter) {
				//const std::vector<double> & firstAerosols = *itAerosols;
				const std::vector<double> & rowGas = *itGas;
				out << rowGas.at(0) << _space_ << rowGas.at(1) << _space_ << rowGas.at(2) << _space_ << rowGas.at(3) << _space_ << rowGas.at(4)
						<< _space_ << rowGas.at(5) << _space_ << rowGas.at(6) << _space_ << rowGas.at(7) << _space_ << rowGas.at(8) << _space_
						<< rowGas.at(9) << _space_ << rowGas.at(10) << _space_ << rowGas.at(11) << _space_ << rowGas.at(12) << _space_
						<< rowGas.at(13) << _space_ << rowGas.at(14) << _space_ << rowGas.at(15);  //<< _space_<< rowAerosols.at(1) << endl;
				for (int i = 0; i < atmosphereParameters.numberOfAerosol; i++) {
					out << _space_ << aerosolResults[i][iter].at(1);
				}
				out << endl;
			}
		}
	}

	static void readAndWriteCrossSection(	vector<AtmosphereComposition> & atmospherePerBandMeanValues,
											std::ofstream & out) {

		//vector<string> molecule = CrossSectionDatabaseDAO::_molecule;//{"CO2","H2O","O3","CH4","N2O"};
		unsigned int nbOfRows = atmospherePerBandMeanValues.size() * atmospherePerBandMeanValues[0].getCroSec().size();
		out << "* Equivalent Absorption Cross Section Vertical Distribution: band mol 0km(1/atm-cm) "
				"1km(1/atm-cm) 2km(1/atm-cm) 3km(1/atm-cm) 4km(1/atm-cm) 5km(1/atm-cm) 6km(1/atm-cm)"
				" 7km(1/atm-cm) 8km(1/atm-cm) 9km(1/atm-cm) 10km(1/atm-cm) 11km(1/atm-cm) 12km(1/atm-cm)"
				" 13km(1/atm-cm) 14km(1/atm-cm) 15km(1/atm-cm) 16km(1/atm-cm) 17km(1/atm-cm) 18km(1/atm-cm)"
				" 19km(1/atm-cm) 20km(1/atm-cm) 21km(1/atm-cm) 22km(1/atm-cm) 23km(1/atm-cm) 24km(1/atm-cm)"
				" 25km(1/atm-cm) 30km(1/atm-cm) 35km(1/atm-cm) 40km(1/atm-cm) 45km(1/atm-cm) 50km(1/atm-cm)"
				" 55km(1/atm-cm) 60km(1/atm-cm) 70km(1/atm-cm) 80km(1/atm-cm) 100km(1/atm-cm)" << _tab_ << "*" << _space_ << nbOfRows << endl;

		for (unsigned int band = 0; band < atmospherePerBandMeanValues.size(); band++) {
			for (unsigned int mol = 0; mol < CrossSectionDatabaseDAO::_molecule.size(); mol++) {
				out << band << _space_ << CrossSectionDatabaseDAO::_molecule[mol];
				for (unsigned int z = 0; z < 36; z++) {
					out << _space_ << atmospherePerBandMeanValues[band].getCroSecPerIndex(mol, z);
				}
				out << endl;
			}
		}
	}

	static void readAndWriteTemperature(AtmosphereParameters & atmosphereParameters,
									std::ofstream & out) {
		session currentSession;
		std::string Table = atmosphereParameters.temperatureModelName + "_gTEMP";
		unsigned int nbOfRows = getNumberOfValuesInAllTable(Table);
		out << "* Vertical Distribution: height(km) temperature(K) pressure(mb) " << _tab_ << "*" << _space_ << nbOfRows << endl;
		DartDatabaseManager databaseManager(_atmosphericDatabasePath);
		if (databaseManager.connectDatabase()) {
			std::vector<std::vector<double> > temperatureResults;
			PhaseDatabaseDAO::readAtmosphericParameters(databaseManager, atmosphereParameters.temperatureModelName,
				AtmosphericDatabaseStructure::getAtmosphericTableType(AtmosphericDatabaseStructure::TEMPERATURE), temperatureResults);
			double offSetTemp = atmosphereParameters.calculateOffSetDefTemperature();

			for (std::vector<std::vector<double> >::const_iterator it = temperatureResults.begin(); it != temperatureResults.end(); ++it) {
				const std::vector<double> & row = *it;
				if (atmosphereParameters.redefTemperature && row.at(0) < 11) {
					out << row.at(0) << _space_ << row.at(1) + offSetTemp << _space_ << row.at(2) << endl;
				} else {
					out << row.at(0) << _space_ << row.at(1) << _space_ << row.at(2) << endl;
				}
			}
			databaseManager.deconnectDatabase();
		}
	}

	static bool getWeightingOfAtmosphereParameters() {
		return _weightingOfAtmosphereParameters;
	}

	static bool getWeightingOfReflectanceParameters() {
		return _weightingOfReflectanceParameters;
	}

	static void setWeightingOfAtmosphereParameters(const bool weightingOfAtmosphereParameters) {
		_weightingOfAtmosphereParameters = weightingOfAtmosphereParameters;
	}

	static void setWeightingOfReflectanceParameters(const bool & weightingOfReflectanceParameters) {
		_weightingOfReflectanceParameters = weightingOfReflectanceParameters;
	}

	static double computeMoleculePhaseFunction(const double & cosinusPsy) {
		return (cosinusPsy * cosinusPsy + 1) * 0.719044332 + 0.041274225;
	}

	static double computeAerosolsPhaseFunction(	const double & cosinusPsy,
												const unsigned int & spectralBandIndex,
												int aerosoltype) {
		double firstTermNumerator = _parameterForAerosolPhaseFunctionA[aerosoltype][spectralBandIndex]
				* (1 - pow(_parameterForAerosolPhaseFunctionG1[aerosoltype][spectralBandIndex], 2));
		double secondTermNumerator = (1 - _parameterForAerosolPhaseFunctionA[aerosoltype][spectralBandIndex])
				* (1 - pow(_parameterForAerosolPhaseFunctionG2[aerosoltype][spectralBandIndex], 2));
		double firstTermDenominator = 1;
		double secondTermDenominator = 1;
		if (firstTermNumerator != 0) {
			firstTermDenominator = pow(
				1 + pow(_parameterForAerosolPhaseFunctionG1[aerosoltype][spectralBandIndex], 2)
						- 2 * _parameterForAerosolPhaseFunctionG1[aerosoltype][spectralBandIndex] * cosinusPsy, 1.5);
			if (firstTermDenominator == 0) {
				return 0;
			}
		}
		if (secondTermNumerator != 0) {
			secondTermDenominator = pow(
				1 + pow(_parameterForAerosolPhaseFunctionG2[aerosoltype][spectralBandIndex], 2)
						+ 2 * _parameterForAerosolPhaseFunctionG2[aerosoltype][spectralBandIndex] * cosinusPsy, 1.5);
			if (secondTermDenominator == 0) {
				return 0;
			}
		}
		return (firstTermNumerator / firstTermDenominator + secondTermNumerator / secondTermDenominator);
	}

	static unsigned int getIndiceDirectionDARTquiContientDirectionNonVirtual(	const double& zenithAngle,
																				const double& azimuthAngle,
																				const std::vector<DirectionCartesienne> & listDirections) {
		unsigned int dirOccupied = 0;
		bool ifOccupiedDirectionFound = false;
		double distancePhi = 0;
		for (unsigned int incidentDirection = 0; int(incidentDirection) < listDirections.size(); ++incidentDirection) {
			if (!listDirections[incidentDirection].isVirtual()) {
				if (zenithAngle <= listDirections[incidentDirection].getEndTheta()
						&& zenithAngle >= listDirections[incidentDirection].getBeginTheta()) {
					distancePhi = WrapTo2PI(fabs(azimuthAngle - listDirections[incidentDirection].getCenterPhi()));
					/* distancePhi==M_PI2 condition is included in WrapTo2PI method. */
					if (distancePhi > M_PI) {
						distancePhi = M_PI2 - distancePhi;
					}
					if (distancePhi <= listDirections[incidentDirection].getDeltaPhi() * 0.5 + 1e-10) {
						if (listDirections[incidentDirection].getProperty() != DirectionRoot::OCCUPIED) {
							return incidentDirection;
						} else {
							dirOccupied = incidentDirection;
							ifOccupiedDirectionFound = true;
						}
					}
				}
			}
		}
		if (ifOccupiedDirectionFound) {
			return dirOccupied;
		}
		cerr << "ERROR : No DART direction found. Input Theta/Phi: " << zenithAngle << "/" << azimuthAngle << ". DeltaPhi Calculated/Assumed: "
				<< endl;
		abort();
		return -1;
	}

	static void writeAtmospherePhaseFunction(const std::vector<DirectionCartesienne> & listDirections) {
		std::string folderPath = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator;
		createOutputDirectory(folderPath);
		std::string outputNameAtmospherePhaseFunction = folderPath + "AtmospherePhaseFunction";
		std::fstream OUT(outputNameAtmospherePhaseFunction.c_str(), std::fstream::out | std::fstream::binary);

		for (unsigned int indexMoleculeAerosol = 0; indexMoleculeAerosol < 1 + _numberOfAerosols; indexMoleculeAerosol++) {

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; spectralBandIndex++) {
				for (int incidentDirection = 0; incidentDirection < listDirections.size() + 1; incidentDirection++) {
					for (int outputDirection = 0; outputDirection < listDirections.size() + 1; outputDirection++) {
						OUT.write((char *) &_atmospherePhaseFunction[indexMoleculeAerosol][incidentDirection][outputDirection][spectralBandIndex],
							sizeof(double));
					}
				}
			}
		}
		OUT.close();
	}

	static void resizeAtmospherePhaseFunction(const std::vector<DirectionCartesienne> & listDirections) {
		_atmospherePhaseFunction.clear();
		_atmospherePhaseFunction.resize(1 + _numberOfAerosols);
		for (unsigned int i = 0; i < 1 + _numberOfAerosols; i++) {
			_atmospherePhaseFunction[i].resize(listDirections.size() + 1);
			for (int j = 0; j < listDirections.size() + 1; j++) {
				_atmospherePhaseFunction[i][j].resize(listDirections.size() + 1);
				for (int k = 0; k < listDirections.size() + 1; k++) {
					_atmospherePhaseFunction[i][j][k].resize(_numberOfSpectralBands);
				}
			}
		}
	}

	static unsigned int getNumberOfDivisions(const unsigned int & minimumNumberOfDivisions, const std::vector<DirectionCartesienne> & listDirections) {
		//the number of sub-direction is set to 3600 in order to guarantee the phase function value
		unsigned int numberOfDivisions = round(sqrt(3600.0 / listDirections.size()));
		if (numberOfDivisions < 4) {
			numberOfDivisions = 4;
		}
		if (numberOfDivisions < minimumNumberOfDivisions) {
			numberOfDivisions = minimumNumberOfDivisions;
		}

		cout << "    Number of divisions for aerosol phase function for each incident and outgoing directions: " << numberOfDivisions << "." << endl;
		return numberOfDivisions;
	}

	static void initializeSpectralTable(std::vector<double> & vectorToInitialize) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] = 0;
		}
	}

	static void multiplySpectralTable(	std::vector<double> & vectorToInitialize,
										const std::vector<double> & factorVector) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] *= factorVector[spectralBandIndex];
		}
	}
	static void multiplySpectralTable(	std::vector<double> & vectorToInitialize,
										const double & factor) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] *= factor;
		}
	}

	static void divideSpectralTable(std::vector<double> & vectorToInitialize,
									const std::vector<double> & factorVector) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] /= factorVector[spectralBandIndex];
		}
	}
	static void divideSpectralTable(std::vector<double> & vectorToInitialize,
									const double & factor) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] /= factor;
		}
	}

	static void addSpectralTable(	std::vector<double> & vectorToInitialize,
									const std::vector<double> & factorVector) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] += factorVector[spectralBandIndex];
		}
	}
	static void addSpectralTable(	std::vector<double> & vectorToInitialize,
									const double & factor) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] += factor;
		}
	}

	static void addDivideSpectralTable(	std::vector<double> & vectorToInitialize,
										const std::vector<double> & factorVector,
										const double & divide) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] += factorVector[spectralBandIndex] / divide;
		}
	}

	static void substractSpectralTable(	std::vector<double> & vectorToInitialize,
										const std::vector<double> & factorVector) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] -= factorVector[spectralBandIndex];
		}
	}

	static void substractSpectralTable(	std::vector<double> & vectorToInitialize,
										const double & factor) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] -= factor;
		}
	}

	static void equalSpectralTable(	std::vector<double> & vectorToInitialize,
									const std::vector<double> & factorVector) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] = factorVector[spectralBandIndex];
		}
	}
	static void equalSpectralTable(	std::vector<double> & vectorToInitialize,
									const double & factor) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			vectorToInitialize[spectralBandIndex] = factor;
		}
	}

	static void printMultiplicationSum(const std::vector<double> & multiplicationSumFfAllDirections) {
		cout
				<< "    Total multiplication of the sums of the phasefunctions from all incident discrete directions towards all outgoing discrete directions: ";
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			cout << multiplicationSumFfAllDirections[spectralBandIndex] << " ";
		}
		cout << "." << endl;
	}

	static double computePhaseFunctionValue(NewtonCotesIntegrationCalculator<double> & integration,
											const std::vector<std::vector<double> > & ffMap,
											const double & deltaX,
											const double & deltaY,
											const double & divideur) {
		integration.setInput(ffMap);
		integration.setDeltaX(deltaX);
		integration.setDeltaY(deltaY);
		integration.calculateIntegration2D();
		return integration.getOutput() / divideur;
	}

	static void computeFFMapMoleculesForDivisions(	const unsigned int & numberOfDivisions,
													std::vector<std::vector<double> > & ffMap_molecules,
													const DirectionCartesienne & inDir,
													const int & outputDirection,
													const std::vector<DirectionCartesienne> & listDirections,
													const PointsListsNCIntegration<DirectionCartesienne> & pointsList) {
		ffMap_molecules.clear();
		ffMap_molecules.resize(pointsList.getPointsMapCartesian(outputDirection).size(),
			std::vector<double>(pointsList.getPointsMapCartesian(outputDirection)[0].size(), 1));
		for (unsigned int firstDivisionIndex = 0; firstDivisionIndex < pointsList.getPointsMapCartesian(outputDirection).size();
				firstDivisionIndex++) {
			for (unsigned int secondDivisionIndex = 0; secondDivisionIndex < pointsList.getPointsMapCartesian(outputDirection)[0].size();
					secondDivisionIndex++) {
				ffMap_molecules[firstDivisionIndex][secondDivisionIndex] = computeMoleculePhaseFunction(
					inDir.getDirectionCartesienne().scalaire(
						pointsList.getPointsMapCartesian(outputDirection)[firstDivisionIndex][secondDivisionIndex]));
			}
		}
	}

	static void computeFFMapAerosolsIncidentForDivisions(	const unsigned int & numberOfDivisions,
															std::vector<std::vector<double> > & ffMap_aerosols_incident,
															const int & incidentDirection,
															const int & outputDirection,
															const std::vector<DirectionCartesienne> & listDirections,
															const PointsListsNCIntegration<DirectionCartesienne> & pointsList,
															const unsigned int & spectralBandIndex,
															const int & viewFirstDivisionIndex,
															const int & viewSecondDivisionIndex,
															bool & overlap,
															unsigned int typeindex) {

		overlap = false;
		ffMap_aerosols_incident.clear();
		ffMap_aerosols_incident.resize(pointsList.getPointsMapCartesian(incidentDirection).size(),
			std::vector<double>(pointsList.getPointsMapCartesian(incidentDirection)[0].size(), 1));
		for (unsigned int incidentDirectionFirstDivisionIndex = 0;
				incidentDirectionFirstDivisionIndex < pointsList.getPointsMapCartesian(incidentDirection).size();
				incidentDirectionFirstDivisionIndex++) {
			for (unsigned int incidentDirectionSecondDivisionIndex = 0;
					incidentDirectionSecondDivisionIndex < pointsList.getPointsMapCartesian(incidentDirection)[0].size();
					incidentDirectionSecondDivisionIndex++) {
				ffMap_aerosols_incident[incidentDirectionFirstDivisionIndex][incidentDirectionSecondDivisionIndex] =
						computeAerosolsPhaseFunction(
							pointsList.getPointsMapCartesian(incidentDirection)[incidentDirectionFirstDivisionIndex][incidentDirectionSecondDivisionIndex].scalaire(
								pointsList.getPointsMapCartesian(outputDirection)[viewFirstDivisionIndex][viewSecondDivisionIndex]),
							spectralBandIndex, typeindex);
				//if the phase angle of two sub-direction is less than 18 degree, overlap == true
				if (1
						- pointsList.getPointsMapCartesian(incidentDirection)[incidentDirectionFirstDivisionIndex][incidentDirectionSecondDivisionIndex].scalaire(
							pointsList.getPointsMapCartesian(outputDirection)[viewFirstDivisionIndex][viewSecondDivisionIndex]) < 0.05
						and numberOfDivisions * numberOfDivisions * listDirections.size() < 10000) {
					overlap = true;
				}
			}
		}
	}

	static void computeFFMapAerosols(	const unsigned int & numberOfDivisions,
										std::vector<std::vector<double> > & ffMapAerosolsIncident,
										const int & incidentDirection,
										const int & outputDirection,
										const std::vector<DirectionCartesienne> & listDirections,
										const DirectionCartesienne & sunDirection,
										const PointsListsNCIntegration<DirectionCartesienne> & pointsList,
										const PointsListsNCIntegration<DirectionCartesienne> & pointsListDense,
										const unsigned int & spectralBandIndex,
										std::vector<std::vector<double> > & ffMapAerosols,
										NewtonCotesIntegrationCalculator<double> & integration,
										bool & overlap,
										unsigned int typeindex) {
		overlap = false;
		ffMapAerosols.clear();
		ffMapAerosols.resize(pointsList.getPointsMapCartesian(outputDirection).size(),
			std::vector<double>(pointsList.getPointsMapCartesian(outputDirection)[0].size(), 1));
		//Iteration on view direction (_v)
		for (unsigned int viewFirstDivisionIndex = 0; viewFirstDivisionIndex < pointsList.getPointsMapCartesian(outputDirection).size();
				viewFirstDivisionIndex++) {
			if (overlap and numberOfDivisions * numberOfDivisions * listDirections.size() < 10000) {
				break;
			}
			for (unsigned int viewSecondDivisionIndex = 0; viewSecondDivisionIndex < pointsList.getPointsMapCartesian(outputDirection)[0].size();
					viewSecondDivisionIndex++) {

				if (incidentDirection != listDirections.size() && listDirections[incidentDirection].getSolidAngle() != 0) {
					/* Iteration on incident direction (_i) */
					computeFFMapAerosolsIncidentForDivisions(numberOfDivisions, ffMapAerosolsIncident, incidentDirection, outputDirection,
						listDirections, pointsList, spectralBandIndex, viewFirstDivisionIndex, viewSecondDivisionIndex, overlap, typeindex);
					//if the output direction is close to incident direction when the number of total sub-directions is less than 10000
					//overlap = true and break the loop
					if (overlap and numberOfDivisions * numberOfDivisions * listDirections.size() < 10000) {
						break;
					}
					ffMapAerosols[viewFirstDivisionIndex][viewSecondDivisionIndex] = computePhaseFunctionValue(integration, ffMapAerosolsIncident,
						pointsList.getDeltaCosTheta(incidentDirection), pointsList.getDeltaPhi(incidentDirection),
						listDirections[incidentDirection].getSolidAngle());
				} else {
					ffMapAerosols[viewFirstDivisionIndex][viewSecondDivisionIndex] = computeAerosolsPhaseFunction(
						sunDirection.getDirectionCartesienne().scalaire(
							pointsList.getPointsMapCartesian(outputDirection)[viewFirstDivisionIndex][viewSecondDivisionIndex]), spectralBandIndex,
						typeindex);
				}
			}
		}
		//if the output direction is close to incident direction or the incident direction is solar direction
		//when the number of total sub-directions is less than 10000
		//the pointlistDense is used
		if ((overlap or (incidentDirection == listDirections.size() && sunDirection.getSolidAngle() == 0))
				and numberOfDivisions * numberOfDivisions * listDirections.size() < 10000) {
			ffMapAerosols.clear();
			ffMapAerosols.resize(pointsListDense.getPointsMapCartesian(outputDirection).size(),
				std::vector<double>(pointsListDense.getPointsMapCartesian(outputDirection)[0].size(), 1));
			for (unsigned int viewFirstDivisionIndex = 0; viewFirstDivisionIndex < pointsListDense.getPointsMapCartesian(outputDirection).size();
					viewFirstDivisionIndex++) {
				for (unsigned int viewSecondDivisionIndex = 0;
						viewSecondDivisionIndex < pointsListDense.getPointsMapCartesian(outputDirection)[0].size(); viewSecondDivisionIndex++) {

					if (incidentDirection < listDirections.size() && listDirections[incidentDirection].getSolidAngle() != 0) {
						/* Iteration on incident direction (_i) */
						computeFFMapAerosolsIncidentForDivisions(numberOfDivisions, ffMapAerosolsIncident, incidentDirection, outputDirection,
							listDirections, pointsListDense, spectralBandIndex, viewFirstDivisionIndex, viewSecondDivisionIndex, overlap, typeindex);

						ffMapAerosols[viewFirstDivisionIndex][viewSecondDivisionIndex] = computePhaseFunctionValue(integration, ffMapAerosolsIncident,
							pointsListDense.getDeltaCosTheta(incidentDirection), pointsListDense.getDeltaPhi(incidentDirection),
							listDirections[incidentDirection].getSolidAngle());

					} else {
						ffMapAerosols[viewFirstDivisionIndex][viewSecondDivisionIndex] = computeAerosolsPhaseFunction(
							sunDirection.getDirectionCartesienne().scalaire(
								pointsListDense.getPointsMapCartesian(outputDirection)[viewFirstDivisionIndex][viewSecondDivisionIndex]),
							spectralBandIndex, typeindex);
					}
				}
			}
			overlap = true;
		}
	}

	static void findMinPhFWithinOutgoingDirAerosol(	const unsigned int & numberOfDivisions,
													std::vector<std::vector<double> > & ffMapAerosolsIncident,
													const int & incidentDirection,
													const int & outputDirection,
													const std::vector<DirectionCartesienne> & listDirections,
													const DirectionCartesienne & sunDirection,
													const PointsListsNCIntegration<DirectionCartesienne> & pointsList,
													const unsigned int & spectralBandIndex,
													NewtonCotesIntegrationCalculator<double> & integration,
													double & minPhF,
													unsigned int typeindex) {
		double tmpPhF = 9999;
		bool overlap = true;
		for (unsigned int viewFirstDivisionIndex = 0; viewFirstDivisionIndex < numberOfDivisions; viewFirstDivisionIndex++) {
			for (unsigned int viewSecondDivisionIndex = 0; viewSecondDivisionIndex < numberOfDivisions; viewSecondDivisionIndex++) {
				if (incidentDirection < listDirections.size() && listDirections[incidentDirection].getSolidAngle() != 0) {
					/* Iteration on incident direction (_i) */
					computeFFMapAerosolsIncidentForDivisions(numberOfDivisions, ffMapAerosolsIncident, incidentDirection, outputDirection,
						listDirections, pointsList, spectralBandIndex, viewFirstDivisionIndex, viewSecondDivisionIndex, overlap, typeindex);
					tmpPhF = computePhaseFunctionValue(integration, ffMapAerosolsIncident, pointsList.getDeltaCosTheta(incidentDirection),
						pointsList.getDeltaPhi(incidentDirection), listDirections[incidentDirection].getSolidAngle());

				} else {
					tmpPhF = computeAerosolsPhaseFunction(
						sunDirection.getDirectionCartesienne().scalaire(
							pointsList.getPointsMapCartesian(outputDirection)[viewFirstDivisionIndex][viewSecondDivisionIndex]), spectralBandIndex,
						typeindex);
				}
				if (tmpPhF < minPhF)
					minPhF = tmpPhF;
			}
		}
	}

	static void checkMoleculePhaseFunction(	const std::vector<double> & normalizationOfMoleculesPhaseFunction,
											const int & incidentDirection) {
		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
			/* Check the phase function of molecule, and give a warning if the integration is not 1. */
			if (fabs(normalizationOfMoleculesPhaseFunction[spectralBandIndex] - 1) > 0.01) {
				cerr << "    Warning: the molecule phase function over 4 pi space for incident direction " << incidentDirection << " and band "
						<< spectralBandIndex << " (" << normalizationOfMoleculesPhaseFunction[spectralBandIndex] << ")" << " is different from 1."
						<< endl;
			}
		}
	}

	/* Computation of atmosphere phase function */
	static void atmospherePhaseFunction(const std::vector<DirectionCartesienne> & listDirections,
										const DirectionCartesienne & sunDirection,
										const unsigned int & minimumNumberOfDivisions) {
		resizeAtmospherePhaseFunction(listDirections);
		/* number of points used for each dimension of newton-cotes integration. */
		unsigned int numberOfDivisions = getNumberOfDivisions(minimumNumberOfDivisions, listDirections);
		std::vector<unsigned int> virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd;
		std::vector<unsigned int> corresOutDiscreteDirInd;
		std::vector<unsigned int> corresInDiscreteDirInd;
		/* Pour toutes les directions d'entree du rayon */

		std::vector<std::vector<double> > max_aerosol_phasefunction_hg_corrDir_perSA(getNumberOfAerosol(), std::vector<double>(_numberOfSpectralBands, 0));

		for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; ++typeindex) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
				max_aerosol_phasefunction_hg_corrDir_perSA[typeindex][spectralBandIndex] = computeAerosolsPhaseFunction(1, spectralBandIndex,
					typeindex);
			}
		}

		boost::thread_group atmosphereThreadGroup;
		unsigned int incidentDirection = 0;
		for (unsigned int threadNumber = 0; threadNumber < _numberOfThreads; threadNumber++) {
			boost::thread *t = new boost::thread(processAtmospherePerThread, boost::ref(listDirections), boost::ref(sunDirection), numberOfDivisions,
				boost::ref(incidentDirection), boost::ref(max_aerosol_phasefunction_hg_corrDir_perSA));
			atmosphereThreadGroup.add_thread(t);
		}
		atmosphereThreadGroup.join_all();

		unsigned int nbVirtualDirections = 0;
		std::vector<double> multiplicationSumFfAllDirections(_numberOfSpectralBands, 1.0);
		std::vector<double> sumFFExceptForward(_numberOfSpectralBands, 0);

		//printMultiplicationSum(multiplicationSumFfAllDirections);
		unsigned int directionVcorrDiscrete = 0;
		bool ifCorresDirDiscreteAlreadyExist = false;
		/* Pour toutes les directions d'entree du rayon */
		for (int incidentDirection = 0; incidentDirection < listDirections.size(); incidentDirection++) {
			const DirectionCartesienne & inDir = listDirections[incidentDirection];
			if (!inDir.isVirtual()) {
				directionVcorrDiscrete = incidentDirection;
				for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
					const DirectionCartesienne & outDir = listDirections[outputDirection];
					/* The virtual direction through the corresponding forward scattering discrete direction is recalculated as the solid angle fraction. */
					if (outDir.isVirtual()) {
						/* If the virtual direction is inside the corresponding forward scattering discrete direction */
						if (listDirections[directionVcorrDiscrete].checkInclude(outDir.getCenterTheta(), outDir.getCenterPhi())) {
							ifCorresDirDiscreteAlreadyExist = (std::find(virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd.begin(),
								virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd.end(), outputDirection)
									!= virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd.end());
							if (!ifCorresDirDiscreteAlreadyExist) {
								/* summation of incoming phase functions except forward scattering incident direction */
								for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; typeindex++) {
									initializeSpectralTable(sumFFExceptForward);
									for (int incomingDirection = 0; incomingDirection < listDirections.size(); incomingDirection++) {
										if (!listDirections[incomingDirection].isVirtual() && incomingDirection != int(directionVcorrDiscrete)) {
											addDivideSpectralTable(sumFFExceptForward,
												_atmospherePhaseFunction[typeindex + 1][incomingDirection][outputDirection],
												outDir.getSolidAngle() / listDirections[incomingDirection].getSolidAngle());
										}
									}
									equalSpectralTable(_atmospherePhaseFunction[typeindex + 1][incidentDirection][outputDirection], 1);
									substractSpectralTable(_atmospherePhaseFunction[typeindex + 1][incidentDirection][outputDirection],
										sumFFExceptForward);
									divideSpectralTable(_atmospherePhaseFunction[typeindex + 1][incidentDirection][outputDirection],
														inDir.getSolidAngle());
									multiplySpectralTable(_atmospherePhaseFunction[typeindex + 1][incidentDirection][outputDirection],
														  outDir.getSolidAngle());
								}
								virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd.push_back(outputDirection);
								corresInDiscreteDirInd.push_back(incidentDirection);
								corresOutDiscreteDirInd.push_back(directionVcorrDiscrete);

							} else {
								cout
										<< "    Warning: More than 1 forward scattering discrete direction could be assigned for virtual direction index "
										<< outputDirection << "." << endl;
							}
						}
					}
				}
			} else {
				++nbVirtualDirections;
			}
		}
		unsigned int nbVirtualForwardScat = virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd.size();
		if (nbVirtualDirections == nbVirtualForwardScat) {
			if (nbVirtualForwardScat != 0) {
				cout << "      Virtual direction indices and their corresponding incident and outgoing discrete direction indices " << endl;
				cout << "    for forward-scattering aerosol phase function interpolation: " << endl;
				for (unsigned int i = 0; i < nbVirtualForwardScat; ++i) {
					cout << "      " << virtualDirIndListWhichHasBeenAssignForwardScatteringDiscreteInd[i] << "    " << corresInDiscreteDirInd[i]
							<< "    " << corresOutDiscreteDirInd[i] << endl;
				}
			}
		} else {
			cerr << "Error: Number of virtual direction (" << nbVirtualDirections
					<< ") is not consistant with number of forward scattering virtual directions" << nbVirtualForwardScat << endl;
			abort();
		}
	}

	static void processAtmospherePerThread(	const std::vector<DirectionCartesienne> & listDirections,
											const DirectionCartesienne & sunDirection,
											const unsigned int & numberOfDivisions,
											unsigned int & incidentDirection,
											const std::vector<std::vector<double> > & max_aerosol_phasefunction_hg_corrDir_perSA) {
		unsigned int localIncidentDirection;
		_atmosphereMutex.lock();
		localIncidentDirection = incidentDirection++;
		_atmosphereMutex.unlock();
		std::vector<std::vector<double> > ffMap_molecules;
		std::vector<std::vector<std::vector<double> > > ffMapAerosols;
		std::vector<std::vector<std::vector<double> > > ffMapAerosolsIncident;
		ffMap_molecules.resize(numberOfDivisions, std::vector<double>(numberOfDivisions, 1));
		ffMapAerosols.resize(_numberOfAerosols, std::vector<std::vector<double> >(numberOfDivisions, std::vector<double>(numberOfDivisions, 1)));
		ffMapAerosolsIncident.resize(_numberOfAerosols,
			std::vector<std::vector<double> >(numberOfDivisions, std::vector<double>(numberOfDivisions, 1)));
		std::vector<double> normalizationOfMoleculesPhaseFunction(_numberOfSpectralBands, 0); /* Phase function of molecules are normalized */
		std::vector<std::vector<double> > sumToBeSubtractedAerosols(_numberOfAerosols, vector<double>(_numberOfSpectralBands, 0)); /* Phase function of aerosol are subtracted for forward direction*/

		std::vector<double> minPhaseFunctionAerosolTowardCorrDir(_numberOfAerosols), maxPhaseFunctionAerosolTowardCorrDir(_numberOfAerosols);

		PointsListsNCIntegration<DirectionCartesienne> pointsList(listDirections, numberOfDivisions);
		//a second pointlist to calculate the aerosol phase function of sun direction incident and of the output directions near the incident direction
		//when the number of total sub-directions is less than 10000
		PointsListsNCIntegration<DirectionCartesienne> pointsListDense(listDirections, numberOfDivisions * 2);

		double moleculePhaseFunctionValue = 0;
		std::vector<double> aerosolPhaseFunctionValue(_numberOfAerosols, 0.);
		std::vector<std::vector<double> > min_aerosol_phasefunction_hg_corrDir_perSA(_numberOfAerosols,
			vector<double>(_numberOfSpectralBands, 9999.0));

		/* Since virtual direction may be in between 2 discrete direction, to avoid double assignment of forward scattering,it must be ensured that each virtual dir only has one corresponding discrete direction. */
		NewtonCotesIntegrationCalculator<double> integration;
		while (localIncidentDirection <= (unsigned int) listDirections.size()) {
			const DirectionCartesienne & inDir = localIncidentDirection == listDirections.size() ? sunDirection: listDirections[localIncidentDirection];

			/* Phase function of molecules are normalized */
			initializeSpectralTable(normalizationOfMoleculesPhaseFunction);
			/* Phase function of aerosol are subtracted for forward direction */
			for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; typeindex++) {
				initializeSpectralTable(sumToBeSubtractedAerosols[typeindex]);

				for (unsigned int bandIndex = 0; bandIndex < _numberOfSpectralBands; ++bandIndex) {
					min_aerosol_phasefunction_hg_corrDir_perSA[typeindex][bandIndex] = 9999.0;
				}
			}
			unsigned int discreteCorrespondingOutputDirection = 0;
			if (!inDir.isVirtual() || localIncidentDirection == (unsigned int) listDirections.size()) {
				discreteCorrespondingOutputDirection = localIncidentDirection;
				/*
				 The corresponding discrete direction which contains incident direction for aerosol phase function calculation
				 so the outgoing phase function of this special direction is not directly calculated (peak of a delta function),
				 it is calculatd by subtraction of 1 from the phase functions through all the other directions.
				 It is equal to the incident direction for normal discretedirections
				 For solar direction, it is found by function getIndiceDirectionDARTquiContientDirectionNonVirtual
				 */

				if (localIncidentDirection == (unsigned int) listDirections.size()) {
					/* Solar direction */
					discreteCorrespondingOutputDirection = getIndiceDirectionDARTquiContientDirectionNonVirtual(
						inDir.getCenterTheta(), inDir.getCenterPhi(),
						listDirections);
					cout << "    Discrete direction index assigned for solar aerosol forward scattering: " << discreteCorrespondingOutputDirection
							<< endl;
				}
				/* pour toute direction de sortie (exceptee la direction solaire (on ne diffuse pas dans la direction solaire) */
				for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
					const DirectionCartesienne & outDir = listDirections[outputDirection];
					computeFFMapMoleculesForDivisions(numberOfDivisions, ffMap_molecules, inDir, outputDirection, listDirections, pointsList);
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
						moleculePhaseFunctionValue = computePhaseFunctionValue(integration, ffMap_molecules,
							pointsList.getDeltaCosTheta(outputDirection), pointsList.getDeltaPhi(outputDirection),
							M_PI4);
						_atmospherePhaseFunction[phaseFunctionMolecule][localIncidentDirection][outputDirection][spectralBandIndex] =
								moleculePhaseFunctionValue;
						/* For aerosols, the phase function of directionVcorrDiscrete is not calculate directly */
						for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; typeindex++) {
							if (outputDirection != int(discreteCorrespondingOutputDirection)) {
								//overlap is a bool variable to judge if the output direction is close to incident direction when the number of total sub-directions is less than 10000
								//(if the incident direction is solar direction, overlap is set as true)
								bool overlap = false;

								computeFFMapAerosols(numberOfDivisions, ffMapAerosolsIncident[typeindex], localIncidentDirection, outputDirection,
									listDirections, sunDirection, pointsList, pointsListDense, spectralBandIndex, ffMapAerosols[typeindex], integration, overlap,
									typeindex);

								//pointlistDense is used if the output direction is close to incident direction or if the incident direction is solar direction
								if (overlap and numberOfDivisions * numberOfDivisions * listDirections.size() < 10000) {

									aerosolPhaseFunctionValue[typeindex] = computePhaseFunctionValue(integration, ffMapAerosols[typeindex],
										pointsListDense.getDeltaCosTheta(outputDirection), pointsListDense.getDeltaPhi(outputDirection),
										M_PI4);

								} else {

									aerosolPhaseFunctionValue[typeindex] = computePhaseFunctionValue(integration, ffMapAerosols[typeindex],
										pointsList.getDeltaCosTheta(outputDirection), pointsList.getDeltaPhi(outputDirection),
										M_PI4);

								}

								_atmospherePhaseFunction[typeindex + 1][localIncidentDirection][outputDirection][spectralBandIndex] =
										aerosolPhaseFunctionValue[typeindex];

							} else {

								aerosolPhaseFunctionValue[typeindex] = 0;
								findMinPhFWithinOutgoingDirAerosol(numberOfDivisions, ffMapAerosolsIncident[typeindex], localIncidentDirection,
									outputDirection, listDirections, sunDirection, pointsList, spectralBandIndex, integration,
									min_aerosol_phasefunction_hg_corrDir_perSA[typeindex][spectralBandIndex], typeindex);
							}
						}

						//if (localIncidentDirection==0){
						//	if(outputDirection >= 1 and outputDirection <=8){
						//	cout<<localIncidentDirection<<"\t"<<outputDirection<<"\t"<<setiosflags(ios::fixed)<<setprecision(6)<<aerosolPhaseFunctionValue
						//			<<"\t"<<moleculePhaseFunctionValue<<"\t"<<endl;
						//	}
						//}
						if (!outDir.isVirtual()/* || localIncidentDirection == (unsigned int) _numberOfDirections*/) {
//							cout<<"moleculePhaseFunctionValue: "<<moleculePhaseFunctionValue<<endl;

							normalizationOfMoleculesPhaseFunction[spectralBandIndex] += moleculePhaseFunctionValue;
							for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; typeindex++) {
								if (outputDirection != int(discreteCorrespondingOutputDirection)) {
									sumToBeSubtractedAerosols[typeindex][spectralBandIndex] += aerosolPhaseFunctionValue[typeindex];
								}
							}
						}
					}
				}

				checkMoleculePhaseFunction(normalizationOfMoleculesPhaseFunction, localIncidentDirection);
				/* Give the phase function of corresponding forward scattering discrete direction, which equals to 1 minus the sum of all the other discrete phase functions. */

//				std::vector<bool> ifNormalization(_numberOfSpectralBands, false);
				//Check the sum of the aerosol phase function to make sure the sum is less than 1.
				for (unsigned int typeindex = 0; typeindex < _numberOfAerosols; typeindex++) {
					for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralBands; ++spectralBandIndex) {
						//Check if the phase function range is outside the boundaries, so it has to be normalized.
						minPhaseFunctionAerosolTowardCorrDir[typeindex] = min_aerosol_phasefunction_hg_corrDir_perSA[typeindex][spectralBandIndex]
								* listDirections[discreteCorrespondingOutputDirection].getSolidAngle() / M_PI4;
						maxPhaseFunctionAerosolTowardCorrDir[typeindex] = max_aerosol_phasefunction_hg_corrDir_perSA[typeindex][spectralBandIndex]
								* listDirections[discreteCorrespondingOutputDirection].getSolidAngle() / M_PI4;

						if ((1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex] < minPhaseFunctionAerosolTowardCorrDir[typeindex])
								|| (1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex] > maxPhaseFunctionAerosolTowardCorrDir[typeindex])) {
							if (localIncidentDirection == (unsigned int) listDirections.size()) {
								static bool ifPrintForSun = true;

								if (ifPrintForSun) {
									cerr
											<< "Warning: For the sun direction, the integral of the aerosol phase function over all scattering directions except forward direction is not coherent "
											<< "(e.g., band:" << spectralBandIndex << ". Incident direction:" << localIncidentDirection
											<< ". Computed value: " << 1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex]
											<< ". Expected minimum:" << minPhaseFunctionAerosolTowardCorrDir[typeindex] << ". Expected maximum: "
											<< maxPhaseFunctionAerosolTowardCorrDir[typeindex] << ".) " << endl;

									cerr << "Hence, scattering function is normalized. "
											<< "For removing the warning (we STRONGLY suggest for more accurate results), increase the integral step (present step is "
											<< numberOfDivisions << " that is used for computing aerosol phase function." << endl;

									ifPrintForSun = false;
								}
							} else {
								static bool ifPrintForOtherThanSun = true;
								if (ifPrintForOtherThanSun) {
									cerr
											<< "Warning: For at least one direction other than incident sun direction, the integral of the aerosol phase function over all scattering directions except forward direction is not coherent "
											<< "(e.g., band:" << spectralBandIndex << ". Incident direction:" << localIncidentDirection
											<< ". Computed value: " << 1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex]
											<< ". Expected minimum:" << minPhaseFunctionAerosolTowardCorrDir[typeindex] << ". Expected maximum: "
											<< maxPhaseFunctionAerosolTowardCorrDir[typeindex] << ".) " << endl;

									cerr << "Hence, scattering function is normalized. "
											<< "For removing the warning (more accurate results), we would STRONGLY recommend you to increase the integral step (present step is "
											<< numberOfDivisions << " that is used for computing aerosol phase function." << endl;

									ifPrintForOtherThanSun = false;
								}
							}

							//Normalization:

							double calculatedSum = sumToBeSubtractedAerosols[typeindex][spectralBandIndex];

							if (1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex] < minPhaseFunctionAerosolTowardCorrDir[typeindex]) {
								sumToBeSubtractedAerosols[typeindex][spectralBandIndex] = 1 - minPhaseFunctionAerosolTowardCorrDir[typeindex];
							} else if (1 - sumToBeSubtractedAerosols[typeindex][spectralBandIndex]
									> maxPhaseFunctionAerosolTowardCorrDir[typeindex]) {
								sumToBeSubtractedAerosols[typeindex][spectralBandIndex] = 1 - maxPhaseFunctionAerosolTowardCorrDir[typeindex];
							}
							for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
								/* phase function of molecules are normalized. (usually it is not necessary since (normalizationOfMoleculesPhaseFunction=1), this is done just in case) */
								_atmospherePhaseFunction[typeindex + 1][localIncidentDirection][outputDirection][spectralBandIndex] /= (calculatedSum
										/ sumToBeSubtractedAerosols[typeindex][spectralBandIndex]);
							}
						}
					}

					equalSpectralTable(_atmospherePhaseFunction[typeindex + 1][localIncidentDirection][discreteCorrespondingOutputDirection], 1);
					substractSpectralTable(_atmospherePhaseFunction[typeindex + 1][localIncidentDirection][discreteCorrespondingOutputDirection],
						sumToBeSubtractedAerosols[typeindex]);
				}
				/* pour toute direction de sortie (exceptee la direction solaire (on ne diffuse pas dans la direction solaire) */
				for (int outputDirection = 0; outputDirection < listDirections.size(); outputDirection++) {
					/* phase function of molecules are normalized. (usually it is not necessary since (normalizationOfMoleculesPhaseFunction=1), this is done just in case) */
					divideSpectralTable(_atmospherePhaseFunction[phaseFunctionMolecule][localIncidentDirection][outputDirection],
						normalizationOfMoleculesPhaseFunction);
				}
			}
			_atmosphereMutex.lock();
			localIncidentDirection = incidentDirection++;
			_atmosphereMutex.unlock();
		}
	}
};

#endif /* PHASEPROPERTY_HPP_ */
