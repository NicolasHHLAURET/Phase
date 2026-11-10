/*
 * ImageFileNcKeys.hpp
 *
 *  Created on: 9 mai 2023
 *      Author: guilleuxj
 */

#ifndef INCLUDE_NAMESPACES_NETCDF_IMAGEFILENCKEYS_HPP_
#define INCLUDE_NAMESPACES_NETCDF_IMAGEFILENCKEYS_HPP_

#include <string>

/**
 * Namming: FILE_TYPE_KEY
 * 			FILE: BRF for brf.nc (many namespaces can be ysed)
 * 			TYPE: ATT for attributes, DIM for dimensions, GRP for group, VAR for variables
 * 			KEY: Used defined key
 * 			Example ATM_GROUP_DATABASE: file Atmosphere.nc, database group
 */
namespace ImageFileNcKeys
{
	// Dimensions
	const std::string IMG_DIM_X = "X";
	const std::string IMG_DIM_Y = "Y";

	// Groups
	const std::string IMG_GRP_Major = "MajorImages";
	const std::string IMG_GRP_Lidar = "Lidar";
	const std::string IMG_GRP_MonteCarlo = "MonteCarlo";
	const std::string IMG_GRP_RadiativeBudget = "RadiativeBudget";
	const std::string IMG_GRP_Fluorescence = "Fluorescence";
	const std::string IMG_GRP_Polarisation = "Polarisation";
	const std::string IMG_GRP_PerSceneElements = "PerSceneElements";
	const std::string IMG_GRP_Orthorectified = "Orthorectified";
	const std::string IMG_GRP_PerLightSources = "PerLightSources";

	const std::string IMG_GRP_LIDAR_GroundSensor = "Ground_sensor";
	const std::string IMG_GRP_LIDAR_SolarNoise = "Solar_noise";
	const std::string IMG_GRP_LIDAR_PhotonInformation = "Photon_information";


	const std::string IMG_GRP_LEVEL_MonteCarlo = "MC";
	const std::string IMG_GRP_LEVEL_Lidar = "Lidar";

	const std::string IMG_GRP_LEVEL_BOA = "BOA";
	const std::string IMG_GRP_LEVEL_TOA = "TOA";
	const std::string IMG_GRP_LEVEL_MOA = "MOA";
	const std::string IMG_GRP_LEVEL_MOA_PureAtmos = "MOA_PUREATMOS";
	const std::string IMG_GRP_LEVEL_MOA_Direct = "MOA_DIRECT";
	const std::string IMG_GRP_LEVEL_MOA_Diffus = "MOA_DIFFUS";

	const std::string IMG_GRP_UNIT_Radiance = "Radiance";
	const std::string IMG_GRP_UNIT_Brf = "Reflectance";
	const std::string IMG_GRP_UNIT_Tapp = "Temperature";

	const std::string IMG_GRP_ITER_Iter = "ITER";
	const std::string IMG_GRP_ITER_Coupl = "COUPL";
	const std::string IMG_GRP_ITER_IterX = "ITERX";
	const std::string IMG_GRP_ITER_Illudir = "ILLUDIR";
	const std::string IMG_GRP_ITER_Illudiff = "ILLUDIFF";
	const std::string IMG_GRP_ITER_Order1 = "ORDER1";

	const std::string IMG_GRP_COMPONENT_Total = "Total";
	const std::string IMG_GRP_COMPONENT_Q = "Q";
	const std::string IMG_GRP_COMPONENT_U = "U";
	const std::string IMG_GRP_COMPONENT_V = "V";
	const std::string IMG_GRP_COMPONENT_PSI = "PSI";
	const std::string IMG_GRP_COMPONENT_PSII = "PSII";

	// Variables
	const std::string IMG_VAR_Image = "image";

	// Attribute
	const std::string IMG_ATT_ImageFile = "_IMAGES";
	const std::string IMG_ATT_SensorType = "Sensor_type";

	// Labels
	const std::string IMG_LAB_BrfCamera = "BRFCAMERA";
	const std::string IMG_LAB_MinTheta = "Min_theta";
	const std::string IMG_LAB_MaxTheta = "Max_theta";
	const std::string IMG_LAB_MinPhi = "Min_phi";
	const std::string IMG_LAB_MaxPhi = "Max_phi";
	const std::string IMG_LAB_Resolution = "Resolution";

	// Errors
	const std::string IMG_ERR_MajorGroup = "ERROR_MAJOR_GROUP";
	const std::string IMG_ERR_Level = "UNKNOWN_LEVEL";
	const std::string IMG_ERR_Iter = "UNKNOWN_ITERATION";
	const std::string IMG_ERR_Unit = "UNKNOWN_UNIT";
	const std::string IMG_ERR_Component = "UNKNOWN_COMPONENT";
}

#endif /* INCLUDE_NAMESPACES_NETCDF_IMAGEFILENCKEYS_HPP_ */
