#include "../../include/netcdf/DartNetcdf.hpp"

const string DartNetcdf::NC_BAND_PREFIX 		= "Band_";
const string DartNetcdf::NC_MAX_BAND_ATTRIBUTE 	= "_MAX_BAND_INDEX";
const string DartNetcdf::NC_NB_GROUPS_ATTRIBUTE = "_NB_GROUPS";
// Theorical max is 32767, but java viewer can not handle netcdf with more than ~10k groups (or 100MB ?)
// i need to do more inspection before increase this limit
const unsigned int DartNetcdf::NC_MAX_NUMBER_OF_ALLOWED_GROUPS = 10000;

/**
 * Convenient method to put data to a netcdf file from a 2d vector
 * Note that the NcVar MUST have the same dimension size defined as the 2dvector !!!
 */
void DartNetcdf::putNetcdfDataFrom2DVector(	NcVar &ncdfData,
											const std::vector<std::vector<double> > &vector2d)
{
	std::vector<double> builder1d;
	builder1d.reserve(vector2d.size() * vector2d[0].size());
	for (const auto& row : vector2d)
		builder1d.insert(builder1d.end(), row.begin(), row.end());
	ncdfData.putVar(&builder1d[0]);
}
void DartNetcdf::putNetcdfDataFrom1DVector(	NcVar &ncdfData,
											const std::vector<double> &vector1d)
{
	ncdfData.putVar(&vector1d[0]);
}

NcGroup DartNetcdf::getOrCreateGroup(	NcFile &ncdfFile,
										const string &groupName)
{
	if (ncdfFile.getGroup(groupName).isNull())
	{
		ncdfFile.addGroup(groupName);
	}
	return ncdfFile.getGroup(groupName);
}

NcGroup DartNetcdf::getOrCreateGroup(	NcGroup &ncdfGroup,
										const string &groupName)
{
	if (ncdfGroup.getGroup(groupName).isNull())
	{
		ncdfGroup.addGroup(groupName);
	}
	return ncdfGroup.getGroup(groupName);
}

NcDim DartNetcdf::getOrCreateDim(	NcFile &ncdfFile,
									const string &dimName,
									size_t dimSize)
{
	if (ncdfFile.getDim(dimName).isNull())
	{
		ncdfFile.addDim(dimName, dimSize);
	}
	return ncdfFile.getDim(dimName);
}

NcDim DartNetcdf::getOrCreateDim(	NcGroup &ncdfGroup,
									const string &dimName,
									size_t dimSize)
{
	if (ncdfGroup.getDim(dimName).isNull())
	{
		ncdfGroup.addDim(dimName, dimSize);
	}
	return ncdfGroup.getDim(dimName);
}
