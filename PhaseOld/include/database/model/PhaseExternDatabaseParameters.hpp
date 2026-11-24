/*
 * PhaseExternDatabaseParameters.hpp
 *
 *  Created on: 26 oct. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_DATABASE_MODEL_PHASEEXTERNDATABASEPARAMETERS_HPP_
#define INCLUDE_DATABASE_MODEL_PHASEEXTERNDATABASEPARAMETERS_HPP_

#include "phaseExtern/DirectionnalPhaseExternDatabaseParameters.hpp"

using namespace std;

/**
 * OK
 * C'est un subterfuge, cette classe étend PhaseDatabaseParameters, MAIS NE DOIT PAS ETRE LUE TEL QUEL !!!!
 * Les resultats retournés seront faux (ça prendra une direction au pif pour la bande)
 * Au pif = la première saisie par l'utilisateur
 */
class PhaseExternDatabaseParameters: public PhaseDatabaseParameters {
private:
	unsigned int _numberOfBands;
	vector<shared_ptr<PhaseDatabaseParameters> > _directionnalPhaseExtern;

public:
	PhaseExternDatabaseParameters(	const string & databaseName,
									const string & databasePath,
									const string & modelName,
									const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_numberOfBands = numberOfBands;
		_databaseColumChoices.push_back("idAngle");
		_databaseColumChoices.push_back("reflectance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return PHASE_EXTERN_DATABASE;
	}

	/**
	 * Ajoute une direction à la fonction de phase
	 * L'appellant s'assure qu'il n'y a pas de doublons, qu'il a mis un DISTINCT dans son SELECT
	 */
	void addDirectionnalPhaseExternDatabaseParameters(	const int & idAngle,
														const double & thetaView,
														const double & phiView,
														const double & thetaIncident,
														const double & phiIncident) {
		shared_ptr<DirectionnalPhaseExternDatabaseParameters> directionPhase(
			new DirectionnalPhaseExternDatabaseParameters(_databaseName, _databasePath, _databaseModel, _numberOfBands, idAngle, thetaView, phiView,
				thetaIncident, phiIncident));
		_directionnalPhaseExtern.push_back(directionPhase);
	}

	vector<shared_ptr<PhaseDatabaseParameters> > & getDirectionnalPhaseExternDatabaseParametersVector() {
		return _directionnalPhaseExtern;
	}

	/**
	 * Ces méthodes fonctionneront si la base à été lue non directinonement, mais les resultats seront faux !
	 * Il n'y a pas de @Deprecated en cpp mais voila l'idée !
	 */
	const double & getIdAngleForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getReflectanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}
};

#endif /* INCLUDE_DATABASE_MODEL_PHASEEXTERNDATABASEPARAMETERS_HPP_ */
