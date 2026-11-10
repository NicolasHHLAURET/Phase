/*
 * RadiativeBudgetFileKeys.hpp
 *
 *      Author: guilleuxj
 */

#ifndef INCLUDE_NAMESPACES_NETCDF_RADIATIVEBUDGETFILENCKEYS_HPP_
#define INCLUDE_NAMESPACES_NETCDF_RADIATIVEBUDGETFILENCKEYS_HPP_

#include <string>

/**
 * Namming: FILE_TYPE_KEY
 * 			FILE: RB for radiativeBudget.nc (many namespaces can be ysed)
 * 			TYPE: ATT for attributes, DIM for dimensions, GRP for group, VAR for variables
 * 			KEY: Used defined key
 * 			Example ATM_GROUP_DATABASE: file Atmosphere.nc, database group
 */
namespace RadiativeBudgetFileNcKeys
{
	// Dimensions
	const std::string RB_DIM_CellZ = "Max_z"; // 3D
	const std::string RB_DIM_CellX = "Max_x"; // 3D
	const std::string RB_DIM_CellY = "Max_y"; // 3D
	const std::string RB_DIM_ProfileX = "N_layers"; // 1D
	const std::string RB_DIM_ProfileY = "N_data"; // 1D
	const std::string RB_DIM_FigureX = "N_triangles"; // Figures (label triangles)
	const std::string RB_DIM_FigureY = "N_data"; // Figures (label triangles)
	const std::string RB_DIM_FiguresList = "Object_list";
	const std::string RB_DIM_SSList = "Object_list";
	const std::string RB_DIM_SSX = "N_triangles"; // Sunlit & shaded
	const std::string RB_DIM_SSY = "N_data"; // Sunlit & shaded
	const std::string RB_DIM_TriangleX = "N_types"; // Triangles (label type)

	// Groups
	const std::string RB_GRP_3D = "3D";
	const std::string RB_GRP_Profile = "1D";
	const std::string RB_GRP_Figures = "Triangles";
	const std::string RB_GRP_SS = "Sunlit_and_shaded";
	const std::string RB_GRP_Triangles = "Type";

	// Variables
	const std::string RB_VAR_All = "All";
	const std::string RB_VAR_FiguresList = "Objects_list";
	const std::string RB_VAR_SSList = "Objects_list";

	// Attribute
	const std::string RB_ATT_Header = "_tableHeader";
	const std::string RB_ATT_ObjectListSeparator = "_separator";
	const std::string RB_ATT_ObjectList = "Object_list";
	const std::string RB_ATT_Description = "Description";
	const std::string RB_ATT_Unit = "Unit";


	// Labels
	const std::string RB_LAB_Product = "Product_";
	const std::string RB_LAB_TypeNum = "Type=";
	const std::string RB_LAB_ObjectListSeparator = ";";

	const std::string RB_LAB_Fraction = "Fraction";
	const std::string RB_LAB_Watt = "W";
	const std::string RB_LAB_WattPerSquareMeter = "W/m²";
	const std::string RB_LAB_Unknown = "Unknown";

	const std::string RB_LAB_Height = "Height";
	const std::string RB_LAB_ObjectIndex = "ObjectIndex";
	const std::string RB_LAB_TriangleIndex = "TriangleIndex";
	const std::string RB_LAB_Ground = "Ground";
	const std::string RB_LAB_Turbid = "Turbid";
	const std::string RB_LAB_Fluid = "Fluid";
	const std::string RB_LAB_SurfaceArea = "surface_area_[m2]";
	const std::string RB_LAB_Iter = "ITER";
	const std::string RB_LAB_X = "X";
	const std::string RB_LAB_IterX = "ITERX";
	const std::string RB_LAB_Coupl = "COUPL";
	const std::string RB_LAB_Illudir = "ILLUDIR";
	const std::string RB_LAB_Illudiff = "ILLUDIFF";
	const std::string RB_LAB_PSI = "_PSI";
	const std::string RB_LAB_PSII = "_PSII";
	const std::string RB_LAB_Back = "_back";

	const std::string RB_LAB_Absorbed = "Absorbed";
	const std::string RB_LAB_Intercepted = "Intercepted";
	const std::string RB_LAB_Emitted = "Emitted";
	const std::string RB_LAB_Scattered = "Scattered";

	const std::string RB_LAB_TotalExit = "Total_Exit";
	const std::string RB_LAB_TotalEntry = "Total_Entry";

	const std::string RB_LAB_FaceMZExit = "Face_-Z_Exit";
	const std::string RB_LAB_FaceMZEntry = "Face_-Z_Entry";
	const std::string RB_LAB_FacePZExit = "Face_+Z_Exit";
	const std::string RB_LAB_FacePZEntry = "Face_+Z_Entry";

	const std::string RB_LAB_FaceMXExit = "Face_-X_Exit";
	const std::string RB_LAB_FaceMXEntry = "Face_-X_Entry";
	const std::string RB_LAB_FacePXExit = "Face_+X_Exit";
	const std::string RB_LAB_FacePXEntry = "Face_+X_Entry";

	const std::string RB_LAB_FaceMYExit = "Face_-Y_Exit";
	const std::string RB_LAB_FaceMYEntry = "Face_-Y_Entry";
	const std::string RB_LAB_FacePYExit = "Face_+Y_Exit";
	const std::string RB_LAB_FacePYEntry = "Face_+Y_Entry";

	// Description
	const std::string RB_DESC_SunlitShaded = "\"RB terms and areas of sunlit and shaded parts of triangle\" per triangle (line), from band 0 to... \nN_triangles = (N_objects x N_triangles/object). \nN_data = Object index, Triangle index per object, RB terms, Triangle area";
	const std::string RB_DESC_3D = "3D RB[z][x][y] per \"user-selected RB term\", from band 0 to.... Number of terms = (Max_x).(Max_y).(Max_z)";
	const std::string RB_DESC_1D = "Vertical profile (depth ∆z of each layer z = DART cell ∆z) of N_data (height, RB terms) of the \"user-selected RB terms\", from band 0 to...";
	const std::string RB_DESC_Triangles = "\"RB of front and back triangle faces, and triangle area\" per triangle (line), from band 0 to... \nN_triangles = (N_objects x N_triangles/object). \nN_data = Object index, Triangle index per object, RB terms, Triangle area";
	const std::string RB_DESC_Type = "\"RB terms\" per user-defined type (i.e., user-specified color of imported 3D models), from band 0 to ....";
	const std::string RB_DESC_FiguresList = "Insert here description for objects list";

}

#endif /* INCLUDE_NAMESPACES_NETCDF_RADIATIVEBUDGETFILENCKEYS_HPP_ */
