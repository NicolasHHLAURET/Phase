/*
 * NetcdfFile.hpp
 *
 *  Created on: 17 apr. 2024
 *      Author: guilleuxj
 */

#ifndef DARTNETCDFFILE_HPP_
#define DARTNETCDFFILE_HPP_

#include <netcdf>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "../../../libDartBase/include/GeneralDef.hpp"
#include "../../../libDartBase/include/Lib_fileManager.h"
#include "../../../libBoost/boost/thread/mutex.hpp"

#include "DartNetcdf.hpp"

using namespace std;
using namespace netCDF;

/**
 * Netcdf file container, in order to count number of group in a netcdf file
 */
class DartNetcdfFile {
private:
	NcFile 				_ncFile;
	string 				_absoluteParentPath;
	string 				_originalFileName;
	unsigned int 		_maxNumberOfBands;
	unsigned int 		_fileIndex;
	unsigned int		_groupIndex;
	boost::mutex 		_netcdfMutex;
	map<string, string>	_ncGlobalAttributes; // Keep global attribute in memory to transpose them on new splitted nc files

	unsigned int numDigits(unsigned int number)	const
	{
		if (number == 0)
		{
			return 1;
		}
	    unsigned int digits = 0;
	    while (number) {
	        number /= 10;
	        ++digits;
	    }
	    return digits;
	}

	void openNc(string fileName, bool replace)
	{
		ostringstream netcdfAbsolutePath;
		netcdfAbsolutePath << _absoluteParentPath << pathSeparator << fileName;
		NcFile::FileMode mode = boost::filesystem::exists(netcdfAbsolutePath.str()) ? NcFile::write : NcFile::replace;
		mode = replace ? NcFile::replace : mode;
		_ncFile.open(netcdfAbsolutePath.str(), mode);
		if (mode == NcFile::write)
		{
			// Already exists, need to check groups
			NcGroupAtt nbGroupsAttribute = _ncFile.getAtt(DartNetcdf::NC_NB_GROUPS_ATTRIBUTE);
			if (nbGroupsAttribute.isNull())
			{
				openNextFile();
			} else
			{
				if (nbGroupsAttribute.getType() == NC_UINT)
				{
					nbGroupsAttribute.getValues(&_groupIndex);
					if (_groupIndex > DartNetcdf::NC_MAX_NUMBER_OF_ALLOWED_GROUPS)
					{
						openNextFile();
					}
				} else
				{
					// Unable to read nb group, open next file ...
					openNextFile();
				}
			}
		} else {
			// Not exists, set nb group to 0
			_groupIndex = 0;
			saveNbGroup();
		}
		if (_maxNumberOfBands > 0)
		{
			setBandPrefixAttribute(_maxNumberOfBands);
		}
	}

	void incrementGroupNb()
	{
		_groupIndex++;
		saveNbGroup();
	}

	void saveNbGroup()	const
	{
		_ncFile.putAtt(DartNetcdf::NC_NB_GROUPS_ATTRIBUTE, NC_UINT, _groupIndex);
	}

	// open next netcdf file
	void openNextFile()
	{
		_ncFile.close(); // close previous file, no effect if no file openned
		_fileIndex++;
		_groupIndex = 0; // reset group index
		ostringstream ossFileName;
		ossFileName << _originalFileName << "_" << _fileIndex << ".nc";
		openNc(ossFileName.str(), false);
		// copy global attributes to new splitted file
		for (auto const& ncGlobalAttribute : _ncGlobalAttributes)
		{
			this->putAtt(ncGlobalAttribute.first, ncGlobalAttribute.second);
		}
	}

public:
	DartNetcdfFile():
		_maxNumberOfBands(0), _ncFile(), _absoluteParentPath(""), _originalFileName(""), _fileIndex(0), _groupIndex(0)
	{
		// Need to call init function before using clazz
	}

	/**
	 * @param absoluteParentPath : absolute path to the folder where the netcdf is stored
	 * @param fileNameWithoutExt : netcdf file name without the ".nc" extension
	 * @param replace : replace previous file, by default append results to previous file, DART should delete files at the begining of the execution
	 */
	DartNetcdfFile(string absoluteParentPath, string fileNameWithoutExt, bool replace = false) :
		_maxNumberOfBands(0), _ncFile(), _absoluteParentPath(absoluteParentPath), _originalFileName(fileNameWithoutExt), _fileIndex(0), _groupIndex(0)
	{
		init(absoluteParentPath, fileNameWithoutExt, replace);
	}

	// delayed constructor, don't use to reinit as behavior is not tested.
	void init(string absoluteParentPath, string fileNameWithoutExt, bool replace = false)
	{
		_absoluteParentPath = absoluteParentPath;
		_originalFileName = fileNameWithoutExt;
		_ncGlobalAttributes.clear();
		if (!isDir(absoluteParentPath))
		{
			createDirectory(absoluteParentPath);
		}
		ostringstream ossFileName;
		ossFileName << _originalFileName << ".nc";
		openNc(ossFileName.str(), replace);
	}

	boost::mutex & getNetcdfFileMutex()
	{
		return _netcdfMutex;
	}

	/**
	 * Convenient method to put data to a netcdf file from a 2d vector
	 * Note that the NcVar MUST have the same dimension size defined as the 2dvector !!!
	 */
	void putNetcdfDataFrom2DVector(	NcVar &ncdfData,
									const std::vector<std::vector<double> > &vector2d)	const
	{
		std::vector<double> builder1d;
		builder1d.reserve(vector2d.size() * vector2d[0].size());
		for (const auto& row : vector2d)
			builder1d.insert(builder1d.end(), row.begin(), row.end());
		ncdfData.putVar(&builder1d[0]);
	}

	void putNetcdfDataFrom3DVector(	NcVar &ncdfData,
									const std::vector<std::vector<std::vector<double> > > &vector3d)	const
	{
		std::vector<double> builder1d;
		builder1d.reserve(vector3d.size() * vector3d[0].size() * vector3d[0][0].size());
		for (const auto& row2D : vector3d)
		{
			for (const auto& row1D : row2D)
			{
				builder1d.insert(builder1d.end(), row1D.begin(), row1D.end());
			}
		}
		ncdfData.putVar(&builder1d[0]);
	}
	void putNetcdfDataFrom1DVector(	NcVar &ncdfData,
									const std::vector<double> &vector1d)	const
	{
		ncdfData.putVar(&vector1d[0]);
	}

	NcGroup getOrCreateGroup(const string &groupName)
	{
		if (_ncFile.getGroup(groupName).isNull())
		{
			incrementGroupNb();
			if (_groupIndex > DartNetcdf::NC_MAX_NUMBER_OF_ALLOWED_GROUPS)
			{
				openNextFile();
			}
			_ncFile.addGroup(groupName);
		}
		return _ncFile.getGroup(groupName);
	}

	/**
	 * Get a variable localized at the root of the file
	 */
	NcVar getRootVar(const string &varName) const
	{
		return _ncFile.getVar(varName);
	}

	/**
	 * add a variable localized at the root of the file
	 */
	NcVar addRootVar(const string &varName, const NcType &ncType,const NcDim &ncDim) const
	{
		return _ncFile.addVar(varName, ncType, ncDim);
	}
	NcVar addRootVar(const string &varName, const NcType &ncType,const vector<NcDim> &ncDim) const
	{
		return _ncFile.addVar(varName, ncType, ncDim);
	}

	NcGroup getOrCreateGroups(	const vector<string> &groupList,
								const bool & fallback = true)
	{
		bool success = true;
		NcGroup childGroup;
		for(unsigned int i = 0; i < groupList.size(); i++)
		{
			if (i == 0)
			{
				if (_ncFile.getGroup(groupList[i]).isNull())
				{
					incrementGroupNb();
					if (_groupIndex > DartNetcdf::NC_MAX_NUMBER_OF_ALLOWED_GROUPS)
					{
						success = false;
						break; // break the for
					}
					_ncFile.addGroup(groupList[i]);
				}
				childGroup = _ncFile.getGroup(groupList[i]);
			} else
			{
				if (childGroup.getGroup(groupList[i]).isNull())
				{
					incrementGroupNb();
					if (_groupIndex > DartNetcdf::NC_MAX_NUMBER_OF_ALLOWED_GROUPS)
					{
						success = false;
						break; // break the for
					}
					childGroup.addGroup(groupList[i]);
				}
				childGroup = childGroup.getGroup(groupList[i]);
			}
		}

		if (!success)
		{
			if (fallback)
			{
				openNextFile();
				return getOrCreateGroups(groupList, false);
			} else
			{
				// child group will be a "netcdf null" object (group.isNull() == true) that need to be treated by the caller
			}
		}
		return childGroup;
	}

	NcDim getOrCreateDim(	const string &dimName,
											size_t dimSize)	const
	{
		if (_ncFile.getDim(dimName).isNull())
		{
			_ncFile.addDim(dimName, dimSize);
		}
		return _ncFile.getDim(dimName);
	}

	NcDim getOrCreateDim(	NcGroup &ncdfGroup,
											const string &dimName,
											size_t dimSize)	const
	{
		if (ncdfGroup.getDim(dimName).isNull())
		{
			ncdfGroup.addDim(dimName, dimSize);
		}
		return ncdfGroup.getDim(dimName);
	}

	void setBandPrefixAttribute(const unsigned int numberOfBands)
	{
		_maxNumberOfBands = numberOfBands;
		std::ostringstream bandName;
		bandName << numberOfBands - 1;
		this->putAtt(DartNetcdf::NC_MAX_BAND_ATTRIBUTE, bandName.str());
	}

	/**
	 * @precondition: setBandPrefixAttribute has been called once to initialize NC_MAX_BAND
	 */
	string computeBandName(const unsigned int bandNumber)	const
	{
		ostringstream bandPrefix;
		bandPrefix << DartNetcdf::NC_BAND_PREFIX;
		unsigned int bandDigits = numDigits(bandNumber);
		unsigned int maxBandDigits = numDigits(_maxNumberOfBands - 1);  // -1 bacause first band index is 0
		for (unsigned int i = 0; i < maxBandDigits - bandDigits; ++i)
		{
			bandPrefix << "0";
		}
		bandPrefix << bandNumber;
		return bandPrefix.str();
	}

	void putAtt(const std::string attributeName, const std::string attributeValue)
	{
		_ncFile.putAtt(attributeName, attributeValue);
		_ncGlobalAttributes[attributeName] = attributeValue;
	}
};


#endif /* DARTNETCDFFILE_HPP_ */
