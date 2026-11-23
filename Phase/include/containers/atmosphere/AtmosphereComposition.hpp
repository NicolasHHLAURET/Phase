/*
 * AtmosphereComponents.hpp
 *
 *  Created on: 3 mars 2016
 *      Author: guilleuxj
 */

#ifndef AtmosphereComponents_HPP_
#define AtmosphereComponents_HPP_

#include <vector>
#include "../../PhaseStructure.hpp"
using namespace std;

class AtmosphereComposition {
//Contenu du vecteur des composants de l'atmosphere
//[0]  =	double gasTransmittanceForScattering;
//[1]  =	double aerosolOpticalDepth;
//[2]  =	double gasTransmittanceAbsUpDown;
//[3]  =	double aerosolSingleScatteringAlbedo;
//[4]  =	double co2TransmittanceAbs;
//[5]  =	double coTransmittanceAbs;
//[6]  =	double ch4TransmittanceAbs;
//[7]  =	double n2oTransmittanceAbs;
//[8]  =	double o2TransmittanceAbs;
//[9]  =	double n2TransmittanceAbs;
//[10] =	double o3TransmittanceAbs;
//[11] =	double otherGasTransmittanceAbs;
//[12] =	double h2oTransmittanceAbs;

//Ceci a ete supprime du vecteur, car c'est pas des trucs interpoles normalement, ils sont differents ceux la.
//[13] =	double gasTransmittanceForAbsoption; // ??
//[14] =	double gasFactorForAnalyticalTransmittance; // ??
//[15] =	double gasOpticalDepthForScattering; // ??
//[16] =	double aerosolFactorForAnalyticalTransmittance; // ??
//[17] =	double aerosolTransmittanceForAbsoption; // ??

private:
	// Composition de l'atmosphere
	vector<double> _atmosphereMoleculeComponents;
	vector<double> _aerosol_tau; // per aerosol
	vector<double> _aerosol_beta;// per aerosol
	vector< vector <double> > _gasCroSec;//equivalent gas cross section per absorbing gas per altitude
	// Variables de calcul
	int _numberOfAerosol;
	int _numberOfAbsorbingGas;
	double _gasTransmittanceForAbsoption;
	double _gasFactorForAnalyticalTransmittance;
	double _gasOpticalDepthForScattering;
	vector <double> _aerosolFactorForAnalyticalTransmittance;
	vector <double> _aerosolTransmittanceForAbsoption;

public:
	static const int NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS = 16;

	static const int T_TOTAL_UD_ABS = 0,
			T_MOL_SCAT = 1,
			T_CO2_ABS =2,
			T_CO_ABS = 3,
			T_CH4_ABS = 4,
			T_N2O_ABS = 5,
			T_O2_ABS = 6,
			T_N2_ABS = 7,
			T_O3_ABS = 8,
			T_H2O_ABS = 9,
			T_NH3_ABS = 10,
			T_NO_ABS = 11,
			T_NO2_ABS = 12,
			T_SO2_ABS = 13,
			T_HNO3_ABS = 14,
			T_OTHERS_ABS = 15;
			//TAU_AER = 16,
			//BETA_AER = 17;

	AtmosphereComposition() {
		_atmosphereMoleculeComponents = vector<double>(NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS);
		//_aerosol_tau;
		//_aerosol_beta;
		_numberOfAerosol = 1;
		_numberOfAbsorbingGas = 3;//co2 + h2o + o3
		_gasTransmittanceForAbsoption = 0;
		_gasFactorForAnalyticalTransmittance = 0;
		_gasOpticalDepthForScattering = 0;
		//_aerosolFactorForAnalyticalTransmittance;
		//_aerosolTransmittanceForAbsoption;
	}

	AtmosphereComposition(const AtmosphereComposition & ac2) :
			_atmosphereMoleculeComponents(ac2._atmosphereMoleculeComponents), _aerosol_tau(ac2._aerosol_tau), _aerosol_beta(ac2._aerosol_beta),_gasCroSec(ac2._gasCroSec),
			_numberOfAerosol(ac2._numberOfAerosol), _numberOfAbsorbingGas(ac2._numberOfAbsorbingGas),_gasTransmittanceForAbsoption(ac2._gasTransmittanceForAbsoption),
			_gasFactorForAnalyticalTransmittance(ac2._gasFactorForAnalyticalTransmittance), _gasOpticalDepthForScattering(ac2._gasOpticalDepthForScattering),
			_aerosolFactorForAnalyticalTransmittance(ac2._aerosolFactorForAnalyticalTransmittance), _aerosolTransmittanceForAbsoption(ac2._aerosolTransmittanceForAbsoption) {

	}

	AtmosphereComposition & operator=(const AtmosphereComposition & ac2) {
		if (this != &ac2) {
			_atmosphereMoleculeComponents = ac2._atmosphereMoleculeComponents;
			_aerosol_tau = ac2._aerosol_tau;
			_aerosol_beta = ac2._aerosol_beta;
			_gasCroSec = ac2._gasCroSec;
			_numberOfAerosol = ac2._numberOfAerosol;
			_numberOfAbsorbingGas = ac2._numberOfAbsorbingGas;
			_gasTransmittanceForAbsoption = ac2._gasTransmittanceForAbsoption;
			_gasFactorForAnalyticalTransmittance = ac2._gasFactorForAnalyticalTransmittance;
			_gasOpticalDepthForScattering = ac2._gasOpticalDepthForScattering;
			_aerosolFactorForAnalyticalTransmittance = ac2._aerosolFactorForAnalyticalTransmittance;
			_aerosolTransmittanceForAbsoption = ac2._aerosolTransmittanceForAbsoption;
		}
		return *this;
	}

	void setAtmosphericComponentsFromDatabase(	const vector<vector<double> > & rowAerosols,
												const vector<double> & rowGas,
												const vector<vector<double> > & rowCroSec,
												const AtmosphereParameters & atmosPara) {
		initializeAerosolPropertyVector(atmosPara.numberOfAerosol);
		initializeCrossSectionVector(atmosPara.numberOfAbsorbingGas);

		for(int i = 0; i < atmosPara.numberOfAerosol; i++){
			_aerosol_tau[i] = rowAerosols[i].at(1) * atmosPara.aerosolOpticalDepthFactor.at(i);
			_aerosol_beta[i] = rowAerosols[i].at(2);
		}

		for(int i = 0; i < atmosPara.numberOfAbsorbingGas; i++){
			for(int j = 0; j < 36; j++){//36 altitudes
				_gasCroSec[i][j] = rowCroSec[i][j+1];
			}
		}

		_atmosphereMoleculeComponents.at(T_TOTAL_UD_ABS) = rowGas.at(1);
		double multi_factorUpdown_abs = 1.0;

		_atmosphereMoleculeComponents.at(T_CO2_ABS) = pow(rowGas.at(3),atmosPara.co2MixRate/365.0);
		multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_CO2_ABS)/rowGas.at(3);

		if (atmosPara.ifDefAmountOtherGases) {

			_atmosphereMoleculeComponents.at(T_H2O_ABS) = rowGas.at(10)*rowGas.at(11);
			//H2O is scaled somewhere else since the line broadening is not simulated by DART

			_atmosphereMoleculeComponents.at(T_MOL_SCAT) = pow(rowGas.at(2),atmosPara.mulScatMulFactor);
			//T_MOL_SCAT does not matter since only updown absorption is rescaled.

			_atmosphereMoleculeComponents.at(T_CO_ABS) = pow(rowGas.at(4),atmosPara.coMulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_CO_ABS)/rowGas.at(4);

			_atmosphereMoleculeComponents.at(T_CH4_ABS) = pow(rowGas.at(5),atmosPara.ch4MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_CH4_ABS)/rowGas.at(5);

			_atmosphereMoleculeComponents.at(T_N2O_ABS) = pow(rowGas.at(6),atmosPara.n2oMulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_N2O_ABS)/rowGas.at(6);

			_atmosphereMoleculeComponents.at(T_O2_ABS) = pow(rowGas.at(7),atmosPara.o2MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_O2_ABS)/rowGas.at(7);

			_atmosphereMoleculeComponents.at(T_N2_ABS) = pow(rowGas.at(8),atmosPara.n2MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_N2_ABS)/rowGas.at(8);

			_atmosphereMoleculeComponents.at(T_O3_ABS) = pow(rowGas.at(9),atmosPara.o3MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_O3_ABS)/rowGas.at(9);

			_atmosphereMoleculeComponents.at(T_NH3_ABS) = pow(rowGas.at(12),atmosPara.nh3MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_NH3_ABS)/rowGas.at(12);

			_atmosphereMoleculeComponents.at(T_NO_ABS) = pow(rowGas.at(13),atmosPara.noMulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_NO_ABS)/rowGas.at(13);

			_atmosphereMoleculeComponents.at(T_NO2_ABS) = pow(rowGas.at(14),atmosPara.no2MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_NO2_ABS)/rowGas.at(14);

			_atmosphereMoleculeComponents.at(T_SO2_ABS) = pow(rowGas.at(15),atmosPara.so2MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_SO2_ABS)/rowGas.at(15);

			_atmosphereMoleculeComponents.at(T_HNO3_ABS) = pow(rowGas.at(16),atmosPara.hno3MulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_HNO3_ABS)/rowGas.at(16);

			_atmosphereMoleculeComponents.at(T_OTHERS_ABS) = pow(rowGas.at(17),atmosPara.othersMulFactor);
			multi_factorUpdown_abs*=_atmosphereMoleculeComponents.at(T_OTHERS_ABS)/rowGas.at(17);

			this->scaleGasTransmittanceAbsUpDown(multi_factorUpdown_abs);

		} else {
			_atmosphereMoleculeComponents.at(T_MOL_SCAT) = rowGas.at(2);
			_atmosphereMoleculeComponents.at(T_CO_ABS) = rowGas.at(4);
			_atmosphereMoleculeComponents.at(T_CH4_ABS) = rowGas.at(5);
			_atmosphereMoleculeComponents.at(T_N2O_ABS) = rowGas.at(6);
			_atmosphereMoleculeComponents.at(T_O2_ABS) = rowGas.at(7);
			_atmosphereMoleculeComponents.at(T_N2_ABS) = rowGas.at(8);
			_atmosphereMoleculeComponents.at(T_O3_ABS) = rowGas.at(9);
			_atmosphereMoleculeComponents.at(T_H2O_ABS) = rowGas.at(10)*rowGas.at(11);
			_atmosphereMoleculeComponents.at(T_NH3_ABS) = rowGas.at(12);
			_atmosphereMoleculeComponents.at(T_NO_ABS) = rowGas.at(13);
			_atmosphereMoleculeComponents.at(T_NO2_ABS) = rowGas.at(14);
			_atmosphereMoleculeComponents.at(T_SO2_ABS) = rowGas.at(15);
			_atmosphereMoleculeComponents.at(T_HNO3_ABS) = rowGas.at(16);
			_atmosphereMoleculeComponents.at(T_OTHERS_ABS) = rowGas.at(17);
		}
	}
	const vector<double> & getAtmosphereComponents() const {
		return _atmosphereMoleculeComponents;
	}

	void setAtmosphereComponents(const vector<double> & atmosphereComponents) {
		if (atmosphereComponents.size() != NUMBER_OF_ATMOSPHERE_MOLECULE_COMPONENTS) {
			cerr << "Warning: calculation with atmospheric database can be wrong: too many or missing atmospheric components";
		}
		_atmosphereMoleculeComponents = atmosphereComponents;
	}
	void initializeAerosolPropertyVector(const int & numberOfAerosol){
		setNbOfAerosol(numberOfAerosol);
		_aerosol_tau.resize(getnumberOfAerosol(),0.0);
		_aerosol_beta.resize(getnumberOfAerosol(),0.0);
		_aerosolFactorForAnalyticalTransmittance.resize(getnumberOfAerosol(),0.0);
		_aerosolTransmittanceForAbsoption.resize(getnumberOfAerosol(),0.0);
	}

	void initializeCrossSectionVector(const int & numberOfAbsorbingGas){
		setNbOfAbsorbingGas(numberOfAbsorbingGas);
		//the same veritcal distribution as the density profile
		_gasCroSec.resize(getNbOfAbsorbingGas(),vector<double>(36,0.0));//36 Modtran altitude from 0 to 100 km
	}

	/************************************************
	 * Manipulation des composants selon leur index *
	 ************************************************/
	const double & getAtmosphereComponentPerIndex(const int & index) const {
		return _atmosphereMoleculeComponents[index];
	}

	double & getModifiableAtmosphereComponentPerIndex(const int & index) {
		return _atmosphereMoleculeComponents[index];
	}

	const double & getAerosolSingleScatteringAlbedo(const int & index) const {
		return _aerosol_beta[index];
	}

	const vector<double> & getAerosolSingleScatteringAlbedo()const {
		return _aerosol_beta;
	}

	double & getModifiableAerosolSingleScatteringAlbedo(const int & index) {
		return _aerosol_beta[index];
	}

	const double & getAerosolOpticalDepth(const int & index) const {
		return _aerosol_tau[index];
	}

	const double & getCroSecPerIndex(const int & gas,
	                         const int & altitude) const {
		return _gasCroSec[gas][altitude];
	}

	const vector<vector<double> > & getCroSec() const {
		return _gasCroSec;
	}

	const vector<double> & getAerosolOpticalDepth() const {
		return _aerosol_tau;
	}

	double & getModifiableAerosolOpticalDepth(const int & index) {
		return _aerosol_tau[index];
	}

	double & getModifiableCroSecPerIndex(const int & gas,
	                             const int & altitude) {
		return _gasCroSec[gas][altitude];
	}

	void setAtmosphereComponentPerIndex(const int & index, const double & value) {
		_atmosphereMoleculeComponents[index] = value;
	}
	void setAerosolOpticalDepthPerIndex(const int & index, const double & value) {
		_aerosol_tau[index] = value;
	}
	void setCroSec(const int & gas, const int & altitude, const double & value) {
		_gasCroSec[gas][altitude] = value;
	}
	void setAerosolSingleScatteringAlbedoPerIndex(const int & index, const double & value) {
		_aerosol_beta[index] = value;
	}

	/***********************************
	 * Getters des variables de calcul *
	 ***********************************/
	const vector <double> & getAerosolTransmittanceForAbsoption() const {
		return _aerosolTransmittanceForAbsoption;
	}

	const vector <double> & getAerosolFactorForAnalyticalTransmittance() const {
		return _aerosolFactorForAnalyticalTransmittance;
	}

	const double & getGasOpticalDepthForScattering() const {
		return _gasOpticalDepthForScattering;
	}

	const double & getGasFactorForAnalyticalTransmittance() const {
		return _gasFactorForAnalyticalTransmittance;
	}

	const double & getGasTransmittanceForAbsoption() const {
		return _gasTransmittanceForAbsoption;
	}

	/**************************
	 * Getters des composants *
	 **************************/
	const double & getH2oTransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_H2O_ABS];
	}

	const double & getOtherGasTransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_OTHERS_ABS];
	}

	const double & getO3TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_O3_ABS];
	}

	const double & getN2TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_N2_ABS];
	}

	const double & getO2TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_O2_ABS];
	}

	const double & getN2oTransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_N2O_ABS];
	}

	const double & getCh4TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_CH4_ABS];
	}

	const double & getCoTransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_CO_ABS];
	}

	const double & getCo2TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_CO2_ABS];
	}

	const double & getNH3TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_NH3_ABS];
	}

	const double & getNOTransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_NO_ABS];
	}

	const double & getNO2TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_NO2_ABS];
	}

	const double & getSO2TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_SO2_ABS];
	}

	const double & getHNO3TransmittanceAbs() {
		return _atmosphereMoleculeComponents[T_HNO3_ABS];
	}

	const double & getGasTransmittanceAbsUpDown() {
		return _atmosphereMoleculeComponents[T_TOTAL_UD_ABS];
	}

	const double & getGasTransmittanceForScattering() const {
		return _atmosphereMoleculeComponents[T_MOL_SCAT];
	}

	void setGasTransmittanceForScattering(const double & t_mol_scat) {
		_atmosphereMoleculeComponents[T_MOL_SCAT] = t_mol_scat;
	}

	const int getnumberOfAerosol()const{
		return _numberOfAerosol;
	}

	const int getNbOfAbsorbingGas()const{
		return _numberOfAbsorbingGas;
	}
	/**************************
	 * Setters des composants *
	 **************************/
	void setGasTransmittanceAbsUpDown(double gasTransmittanceAbsUpDown) {
		_atmosphereMoleculeComponents[T_TOTAL_UD_ABS] = gasTransmittanceAbsUpDown;
	}

	void scaleGasTransmittanceAbsUpDown(double transmittanceRatio) {
		_atmosphereMoleculeComponents[T_TOTAL_UD_ABS] = _atmosphereMoleculeComponents[T_TOTAL_UD_ABS] * pow(transmittanceRatio,2);
	}

	void setH2oTransmittanceAbs(double h2oTransmittanceAbs) {
		_atmosphereMoleculeComponents[T_H2O_ABS] = h2oTransmittanceAbs;
	}

	/***********************************
	 * Setters des variables de calcul *
	 ***********************************/
	void setNbOfAerosol(int nbOfAerosol){
		_numberOfAerosol = nbOfAerosol;
	}

	void setNbOfAbsorbingGas(int numberOfAbsorbingGas){
		_numberOfAbsorbingGas = numberOfAbsorbingGas;
	}
	void setAerosolFactorForAnalyticalTransmittance(vector<double> aerosolFactorForAnalyticalTransmittance) {
		_aerosolFactorForAnalyticalTransmittance = aerosolFactorForAnalyticalTransmittance;
	}

	void setAerosolTransmittanceForAbsoption(vector<double> aerosolTransmittanceForAbsoption) {
		_aerosolTransmittanceForAbsoption = aerosolTransmittanceForAbsoption;
	}

	void setGasFactorForAnalyticalTransmittance(double gasFactorForAnalyticalTransmittance) {
		_gasFactorForAnalyticalTransmittance = gasFactorForAnalyticalTransmittance;
	}

	void setGasOpticalDepthForScattering(double gasOpticalDepthForScattering) {
		_gasOpticalDepthForScattering = gasOpticalDepthForScattering;
	}

	void setGasTransmittanceForAbsoption(double gasTransmittanceForAbsoption) {
		_gasTransmittanceForAbsoption = gasTransmittanceForAbsoption;
	}

};

#endif /* AIRRAYLEIGH_HPP_ */
