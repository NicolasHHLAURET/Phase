/*
 * PhaseProperty.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: yint
 */

#include "../include/PhaseProperty.hpp"

const int PhaseProperty::CONNEXION_MAX_ESSAI = 50; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
const int PhaseProperty::MAX_WARNING_MESSAGE = 10; // anti spam
const int PhaseProperty::CONNEXION_ATTENTE = 200; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)
const string PhaseProperty::CONNEXION_ERREUR_VERROU = "database is locked";
int PhaseProperty::_nbDisplayedWarningMessages = 0;

double PhaseProperty::_sunZenithAngleInRadians = 0;
double PhaseProperty::_Epulse = 0.001; // 1 mJ

vector<SolarIrradianceValues> PhaseProperty::_solarIrradianceValues = vector<SolarIrradianceValues>(0);

unsigned int PhaseProperty::_numberOfUserSpectralBands= 0;
vector<UserSpectralBand> PhaseProperty::_userSpectralBands = vector<UserSpectralBand>(0);

bool PhaseProperty::_isComputedIrradianceBottomOfAtmosphereCheck = false;
bool PhaseProperty::_ifSimuSolarNoise = false;
int PhaseProperty::_calculatorMethod = -1;
int PhaseProperty::_irradianceFromDatabaseComputationMethod = -1;
unsigned int PhaseProperty::_numberOfSpectralBands = 0;
unsigned int PhaseProperty::_numberOfThreads = 1;
string PhaseProperty::_tableSolarIrradiance;
string PhaseProperty::_columnSolarIrradiance;
string PhaseProperty::_solarConstantDatabaseName;
int PhaseProperty::_computationMethodeOfToaToBoa;
string PhaseProperty::_completeNameDatabase;

vector<MultiplicativeFactorForRpv> PhaseProperty::_multiplicativeFactorForRpv;
vector<MultiplicativeFactorForWaterInterface> PhaseProperty::_multiplicativeFactorForWaterInterface;
vector<MultiplicativeFactorForLambertien> PhaseProperty::_multiplicativeFactorForLambertien;
vector<MultiplicativeFactorForSpecular> PhaseProperty::_multiplicativeFactorForSpecular;
vector<MultiplicativeFactorForHapke> PhaseProperty::_multiplicativeFactorForHapke;
vector<MultiplicativeFactorForHapke2> PhaseProperty::_multiplicativeFactorForHapke2;
ReflectanceModelsParameters PhaseProperty::_reflectanceModelsParameters;
vector<unsigned int> PhaseProperty::_threadEnd;
vector<vector<vector<vector<double> > > > PhaseProperty::_atmospherePhaseFunction;
int PhaseProperty::_numberOfDirections;
int PhaseProperty::_numberOfAngularSectors;
int PhaseProperty::_numberOfAla;
int PhaseProperty::_nthAla;

bool PhaseProperty::_weightingOfAtmosphereParameters = true;
bool PhaseProperty::_weightingOfReflectanceParameters = true;
string PhaseProperty::_atmosphericDatabasePath;
time_t PhaseProperty::_tInit;
time_t PhaseProperty::_tMiddle;
unsigned int PhaseProperty::_tModulo;
double PhaseProperty::_tToPercent;
unsigned int PhaseProperty::_tIteration;
unsigned int PhaseProperty::_numberOfAerosols;
vector<vector<double> > PhaseProperty::_parameterForAerosolPhaseFunctionG1;
vector<vector<double> > PhaseProperty::_parameterForAerosolPhaseFunctionG2;
vector<vector<double> > PhaseProperty::_parameterForAerosolPhaseFunctionA;
bool PhaseProperty::_polarization = false;
bool PhaseProperty::_fluorescence = false;
bool PhaseProperty::_combinedPS = false;
PhaseProperty::listOfYields PhaseProperty::_fluorescenceYields;
string PhaseProperty::_fluorescenceFileName = "";
boost::mutex PhaseProperty::_atmosphereMutex;

boost::mutex PhaseProperty::_phaseExternMutex;
unsigned int PhaseProperty::_phaseExternMutexedIncrement;

