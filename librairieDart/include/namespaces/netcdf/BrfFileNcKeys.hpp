/*
 * BrfFileNcKeys.hpp
 *
 *  Created on: 9 mai 2023
 *      Author: guilleuxj
 */

#ifndef INCLUDE_NAMESPACES_NETCDF_BRFFILENCKEYS_HPP_
#define INCLUDE_NAMESPACES_NETCDF_BRFFILENCKEYS_HPP_

#include <string>

/**
 * Namming: FILE_TYPE_KEY
 * 			FILE: BRF for brf.nc (many namespaces can be ysed)
 * 			TYPE: ATT for attributes, DIM for dimensions, GRP for group, VAR for variables
 * 			KEY: Used defined key
 * 			Example ATM_GROUP_DATABASE: file Atmosphere.nc, database group
 */
namespace BrfFileNcKeys
{
	// Dimensions
	const std::string BRF_DIM_BAND = "Spectral_bands";
	const std::string BRF_DIM_DIRECTION = "Directions";
	const std::string BRF_DIM_THETAPHI = "Theta_phi";

	// Groups
	const std::string BRF_GRP_CENTRALIZED = "MajorResults";
	const std::string BRF_GRP_FLUORESCENCE = "Fluorescence";
	const std::string BRF_GRP_POLARISATION = "Polarisation";
	const std::string BRF_GRP_LEVEL_ALL = "All";
	const std::string BRF_GRP_RADIATION_Radiance = "Radiance";
	const std::string BRF_GRP_RADIATION_Brf = "Brf";
	const std::string BRF_GRP_RADIATION_Tapp = "Tapp";
	const std::string BRF_GRP_RADIATION_BrfTapp = "BrfTapp";

	// Variables
	const std::string BRF_VAR_SPECTRALBAND = "Spectral_bands";
	const std::string BRF_VAR_DIRECTION = "Directions";

	// Attribute
	const std::string BRF_ATT_Header = "Description";

	// Labels
	const std::string BRF_LAB_Centralizedbrf = "Lines correspond to spectral bands and columns to directions";
	const std::string BRF_LAB_Directions = "The first line corresponds to the thetas, the second to the phis. Each pair corresponds to a column in the BRFs";
	const std::string BRF_LAB_SpectralBands = "List of spectral bands, each band corresponds to a line in the BRFs";

	// Errors
	const std::string BRF_ERR_MAIN_PRODUCT = "UNKNOWN_MAIN_PRODUCT";
	const std::string BRF_ERR_ITERATION = "UNKNOWN_ITERATION";


}

#endif /* INCLUDE_NAMESPACES_NETCDF_BRFFILENCKEYS_HPP_ */
