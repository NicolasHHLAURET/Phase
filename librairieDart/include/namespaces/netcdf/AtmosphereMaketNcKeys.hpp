/*
 * AtmosphereMaketNcKeys.hpp
 *
 *  Created on: 1 sept. 2022
 *      Author: guilleuxj
 */

#ifndef INCLUDE_NAMESPACES_NETCDF_ATMOSPHEREMAKETNCKEYS_HPP_
#define INCLUDE_NAMESPACES_NETCDF_ATMOSPHEREMAKETNCKEYS_HPP_

#include <string>

/**
 * Namming: FILE_TYPE_KEY
 * 			FILE: AMM for atmosphereMaket.nc (many namespaces can be ysed)
 * 			TYPE: ATT for attributes, DIM for dimensions, GRP for group, VAR for variables
 * 			KEY: Used defined key
 * 			Example AMM_GROUP_DATABASE: file AtmosphereMaket.nc, database group
 */
namespace AtmosphereMaketNcKeys {
	// Dimensions
	const std::string AMM_DIM_Geometry = "Geometry_dimension";
	const std::string AMM_DIM_Temperature = "Temperature_dimension";
	const std::string AMM_DIM_Layer = "Layer_dimension";
	const std::string AMM_DIM_Band = "Band_dimension";
	const std::string AMM_DIM_AtmosphereProperties = "AtmosphereProperties_dimension";
	const std::string AMM_DIM_BandLayer = "Band_Layer_dimension";
	const std::string AMM_DIM_OpticalProperties = "Optical_properties_dimension";

	// Groups
	const std::string AMM_GRP_AtmoshereMaket = "AtmosphereMaket";

	// Variables
	const std::string AMM_VAR_GeometryAtmosphere = "Geometry_of_the_atmosphere";
	const std::string AMM_VAR_TemperaturePerLayer = "Temperature_per_layer";
	const std::string AMM_VAR_AtmosphereProperties = "Atmosphere_General_Properties";
	const std::string AMM_VAR_OpticalProperties = "Optical_properties_per_layer";


	// Attributes
	const std::string AMM_ATT_Unit = "Unit";
	const std::string AMM_ATT_Header = "Header";
	const std::string AMM_ATT_TableHeader = "_tableHeader";
}



#endif /* INCLUDE_NAMESPACES_NETCDF_ATMOSPHEREMAKETNCKEYS_HPP_ */
