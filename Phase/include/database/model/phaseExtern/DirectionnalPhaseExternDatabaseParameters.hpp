/*
 * DirectionnalPhaseExternDatabaseParameters.hpp
 *
 *  Created on: 6 nov. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_DATABASE_MODEL_PHASEEXTERN_DIRECTIONNALPHASEEXTERNDATABASEPARAMETERS_HPP_
#define INCLUDE_DATABASE_MODEL_PHASEEXTERN_DIRECTIONNALPHASEEXTERNDATABASEPARAMETERS_HPP_

class DirectionnalPhaseExternDatabaseParameters: public PhaseDatabaseParameters {
private:
	double _thetaView;
	double _phiView;
	double _thetaIncident;
	double _phiIncident;
	int _idAngle;

public:
	DirectionnalPhaseExternDatabaseParameters(	const string & databaseName,
												const string & databasePath,
												const string & modelName,
												const unsigned int numberOfBands,
												const int & idAngle,
												const double & thetaView,
												const double &phiView,
												const double & thetaIncident,
												const double & phiIncident) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands) {
		_idAngle = idAngle;
		_thetaView = thetaView;
		_phiView = phiView;
		_thetaIncident = thetaIncident;
		_phiIncident = phiIncident;
		_databaseColumChoices.push_back("reflectance");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return PHASE_EXTERN_DIRECTIONNAL_DATABASE;
	}

	void setIdAngle(const unsigned int & idAngle) {
		_idAngle = idAngle;
	}

	const double getIdAngle() {
		return _idAngle;
	}
	const double getThetaView() {
		return _thetaView;
	}

	const double getPhiView() {
		return _phiView;
	}

	const double getThetaIncident() {
		return _thetaIncident;
	}

	const double getPhiIncident() {
		return _phiIncident;
	}

	const double & getReflectanceForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}
};

#endif /* INCLUDE_DATABASE_MODEL_PHASEEXTERN_DIRECTIONNALPHASEEXTERNDATABASEPARAMETERS_HPP_ */
