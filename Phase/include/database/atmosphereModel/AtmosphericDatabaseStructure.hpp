/*
 * AtmosphericDatabaseStructure.hpp
 *
 *  Created on: 3 mars 2016
 *      Author: guilleuxj
 */

#ifndef ATMOSPHERICDATABASESTRUCTURE_HPP_
#define ATMOSPHERICDATABASESTRUCTURE_HPP_

#include <map>
#include <string>
#include <stdio.h>
#include "AtmosphericTableType.hpp"

using namespace std;

// Classe specialement reservee pour decrire la structure foireuse de dart_atmosphere.db C:
class AtmosphericDatabaseStructure {

public:
	enum AtmosphericType {
//		GAS_PARAMETERS,
		GAS_CUMLULATIVE, GAS_MODEL, AEROSOLS_MODEL, AEROSOLS_CUMULATIVE, TEMPERATURE, HG_PARAMETERS, WATER, WATER_AMOUNT_PARA
	};
private:
	static map<AtmosphericType, AtmosphericTableType> _atmosphericDatabaseStructure;
	static AtmosphericDatabaseStructure _singleton;

	const map<AtmosphericType, AtmosphericTableType> & getAtmosphericStructureMap() const {
		return _atmosphericDatabaseStructure;
	}

public:
	AtmosphericDatabaseStructure() {
		_atmosphericDatabaseStructure[AEROSOLS_CUMULATIVE] = AtmosphericTableType("_aECP", " altitude,aerosols", 2);
		_atmosphericDatabaseStructure[GAS_CUMLULATIVE] = AtmosphericTableType("_gECP",
			" altitude, gas_scat, h2o, ozone, n2, co2, co, ch4, n2o, o2, nh3, no, no2, so2, hno3, other_gases", 16);
//		_atmosphericDatabaseStructure[GAS_PARAMETERS] = AtmosphericTableType("_gHS", " h_m,h_h2o, h_co2, h_co, h_ch4, h_n2o, h_o2, h_n2 ", 8);
		_atmosphericDatabaseStructure[TEMPERATURE] = AtmosphericTableType("_gTEMP", " altitude, temperature, pressure", 3);

		_atmosphericDatabaseStructure[AEROSOLS_MODEL] = AtmosphericTableType("_aAOD", " wavelength,opt_depth,albedo ", 3);
		_atmosphericDatabaseStructure[HG_PARAMETERS] = AtmosphericTableType("_aHG", " wavelength,a,g1,g2 ", 4);
		_atmosphericDatabaseStructure[GAS_MODEL] = AtmosphericTableType("_gTRANS",
			" wavelength, t_total_ud_abs, t_mol_scat, t_co2_abs, t_co_abs, t_ch4_abs, t_n2o_abs, t_o2_abs, t_n2_abs, t_o3_abs, t_h2o_cont_abs, t_h2o_line_abs, t_nh3_abs, t_no_abs, t_no2_abs, t_so2_abs, t_hno3_abs, t_others_abs", 18);

		_atmosphericDatabaseStructure[WATER] = AtmosphericTableType("_gH2O", "", 0);
		_atmosphericDatabaseStructure[WATER_AMOUNT_PARA] = AtmosphericTableType("_pH2O", "amount_H2O_atm_cm, amount_H2O_g_per_cm2, density_H2O_atm_cm_per_km_0, density_H2O_atm_cm_per_km_1, temp_K_0, temp_K_1", 6);
	}

	static void setWaterAtmosphericTableStructure(	const string & tableComponent,
													const int & tableNumberObComponent) {
		_atmosphericDatabaseStructure[WATER] = AtmosphericTableType("_gH2O", " wavelength," + tableComponent + " ", tableNumberObComponent+1);
	}

	static const AtmosphericTableType & getAtmosphericTableType(const AtmosphericType & atmosphericType) {
		return _singleton.getAtmosphericStructureMap().find(atmosphericType)->second;
	}

};

#endif /* ATMOSPHERICDATABASESTRUCTURE_HPP_ */
