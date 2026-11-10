/*
 * AtmosphereNcKeys.hpp
 *
 *  Created on: 1 sept. 2022
 *      Author: guilleuxj
 */

#ifndef INCLUDE_NAMESPACES_NETCDF_ATMOSPHERENCKEYS_HPP_
#define INCLUDE_NAMESPACES_NETCDF_ATMOSPHERENCKEYS_HPP_

#include <string>

/**
 * Namming: FILE_TYPE_KEY
 * 			FILE: ATM for atmosphere.nc (many namespaces can be ysed)
 * 			TYPE: ATT for attributes, DIM for dimensions, GRP for group, VAR for variables
 * 			KEY: Used defined key
 * 			Example ATM_GROUP_DATABASE: file Atmosphere.nc, database group
 */
namespace AtmosphereNcKeys {
	// Dimensions
	const std::string ATM_DIM_BAND = "Band_dimension";
	const std::string ATM_DIM_AEROSOL = "Aerosol_dimension";
	const std::string ATM_DIM_Gas = "Gas_dimension";
	const std::string ATM_DIM_ROWGASANDAEROSOL = "Row_gas_and_aerosol";
	const std::string ATM_DIM_LAYER = "Layer_dimension";
	const std::string ATM_DIM_ALTITUDEFORTEMP = "Altitude_for_temperature_pressure";
	const std::string ATM_DIM_ROWALTTEMPPRESS = "Row_altitude_temperature_pressure";
	const std::string ATM_DIM_CROSSSECTIONLAYER = "Cross_section_layer";
	const std::string ATM_DIM_CROSSSECTIONMOLECULE = "Cross_section_molecule";

	// Groups
	const std::string ATM_GRP_HG = "Henyey Greenstein";
	const std::string ATM_GRP_TRANSMITTANCE = "Transmittance";
	const std::string ATM_GRP_VERTICALDISTRIBUTION = "Vertical distribution";
	const std::string ATM_GRP_EQABSCROSSEC = "Equivalent absorption cross section";

	// Variables
	const std::string ATM_VAR_SPECTRALSAMPLING = "Spectral_bands";
	const std::string ATM_VAR_G1 = "g1";
	const std::string ATM_VAR_G2 = "g2";
	const std::string ATM_VAR_A = "a";
	const std::string ATM_VAR_TOTALAOD = "Aerosols optical_depth";
	const std::string ATM_VAR_AEROSOLSINGLESCAT = "Aerosol_single_scattering albedo";
	const std::string ATM_VAR_AllGas = "All_gas_transmittance";

	const std::string ATM_VAR_GASANDAEROSOLVERTICALDISTRIBUTION = "Gas_and_aerosols";
	const std::string ATM_VAR_TEMPERATUREANDPRESSUREVERTDISTRIB = "Temperature_and_pressure";
	const std::string ATM_VAR_EQABSCROSSECVERTDISTRIB = "Vertical_distribution";
	// Attributes
	const std::string ATM_ATT_PROCESSED = "Already_processed_by_phase";
	const std::string ATM_ATT_WITHDATABASE = "With_or_without_database";
	const std::string ATM_ATT_DATABASENAME = "Database_file_name";
	const std::string ATM_ATT_TYPEOFMODELGAS = "Type_of_model_gas";
	const std::string ATM_ATT_TYPEOFMODELGASCUM = "Type_of_model_gas_cumulative";
	const std::string ATM_ATT_TYPEOFMODELGASTEMP = "Type_of_model_gas_temperature";
	const std::string ATM_ATT_NBAEROSOLS = "Number_of_aerosols";
	const std::string ATM_ATT_TYPEOFAEROSOLS = "Type_of_model_aerosols";
	const std::string ATM_ATT_TYPEOFAEROSOLSCUM = "Type_of_model_aerosols_cumulative";
	const std::string ATM_ATT_TYPEOFHGPARAM = "Type_of_hg_parameters";
	const std::string ATM_ATT_PRECIPITABLEWATERCHECK = "Precipitable_water_amount_checkbox";
	const std::string ATM_ATT_WaterVapourScaleFactor = "Water vapour scale factor";
	const std::string ATM_ATT_AEROSOLDISTRIBUTION = "Aerosol_distribution";
	const std::string ATM_ATT_HEIGHTOFSENSOR = "Height_of_sensor";

	const std::string ATM_ATT_UNIT = "Unit";
	const std::string ATM_ATT_Header = "Header";
	const std::string ATM_ATT_Info = "Info";
	const std::string ATM_ATT_MOLECULELIST = "Molecule_list";

	// Without database attributres
	const std::string ATM_ATT_AerosolScaleFactor = "Aerosol_scale_factor";
	const std::string ATM_ATT_TotalAerosolOpticalDepth = "Total_aerosols_optical_depth";
	const std::string ATM_ATT_AerosolAlbedo = "Aerosol_albedo";

	const std::string ATM_ATT_HgG1 = "Henyey_Greenstein_g1";
	const std::string ATM_ATT_HgG2 = "Henyey_Greenstein_g2";
	const std::string ATM_ATT_HgA = "Henyey_Greenstein_a";

	const std::string ATM_ATT_GasOpticalDepth = "Gas_optical_depth_scattering";
	const std::string ATM_ATT_GasScaleFactor =  "Gas_scale_factor";
	const std::string ATM_ATT_TransmittanceOfGasses = "Transmittance_of_gases_absorb";
}



#endif /* INCLUDE_NAMESPACES_NETCDF_ATMOSPHERENCKEYS_HPP_ */
