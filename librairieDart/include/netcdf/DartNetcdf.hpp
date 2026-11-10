/*
 * DartNetcdf.hpp
 *
 *  Created on: 03 oct. 2023
 *      Author: guilleuxj
 */

#ifndef DARTNETCDF_HPP_
#define DARTNETCDF_HPP_

//Netcdf
#include <netcdf>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
using namespace netCDF;

class DartNetcdf {
public:
	static const string 		NC_MAX_BAND_ATTRIBUTE;
	static const string 		NC_NB_GROUPS_ATTRIBUTE;
	static const string 		NC_BAND_PREFIX;
	static const unsigned int 	NC_MAX_NUMBER_OF_ALLOWED_GROUPS;

	static void putNetcdfDataFrom2DVector(NcVar &ncdfData,const std::vector<std::vector<double> > &vector2d);
	static void putNetcdfDataFrom1DVector(NcVar &ncdfData,const std::vector<double> &vector1d);
	static NcGroup getOrCreateGroup(NcFile &ncdfFile, const string &groupName);
	static NcGroup getOrCreateGroup(NcGroup &ncdfGroup, const string &groupName);
	static NcDim getOrCreateDim(NcFile &ncdfFile, const string &dimName, size_t dimSize);
	static NcDim getOrCreateDim(NcGroup &ncdfGroup, const string &dimName, size_t dimSize);
};


#endif /* DARTNETCDF_HPP_ */
