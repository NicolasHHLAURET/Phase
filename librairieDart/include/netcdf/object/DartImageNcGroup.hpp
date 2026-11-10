/*
 * DartImageNcGroup.hpp
 *
 *  Created on: 16 mayo 2024
 *      Author: guilleuxj
 */

#ifndef DARTIMAGENCGROUP_HPP_
#define DARTIMAGENCGROUP_HPP_

//Netcdf
#include <netcdf>
#include <string>
#include <vector>
#include <sstream>
#include "../DartNetcdf.hpp"

using namespace std;
using namespace netCDF;

class DartImageNcGroup {
private:
	std::vector<std::string> _majorGroups; // majorimages, orthorectified, persceneelement
	std::string _imageName; // mandatory
	std::string _level; // boa, moa, toa
	std::string _iteration; // iter1, iterX, coupl (can be empty ?)
	std::string _unit; // radiance, reflectance, temperature
	std::string _component; // total, psi, psii, q, u, v
	std::string _bandName; // computed band name (can be empty)
	std::string _lidarGroup; // lidar group, maybe lidar images can have different dimensions ???

	std::string _perTypeType; // per type of subproduct, persceneelement, perlightsource ...
	std::string _perTypeName; // if product per type are defined (perTypeType must be defined), this contains affected type, empty otherwise

	// This is for retrocompatibility from old folder hierarchy
	std::string _legacyFolderPath;

	// attributes to attach
	std::map<std::string, std::string> _imageAttributes;

	// When groups are defined, must enable netcdf writing
	bool _writeNetcdf;

public:

	// DEBUG CONSTRICTOR
//	DartImageNcGroup() : _imageName("UNDEFINED_IMAGE_NAME"), _level("UNDEFINED_LEVEL"),
//	_iteration("UNDEFINED_ITERATION"), _unit("UNDEFINED_UNIT"), _component("UNDEFINED_COMPONENT"),
//	_bandName("UNDEFINED_BAND"), _legacyFolderPath("UNDEFINED_LEGACY_PATH"),_perTypeName("ERROR_TYPE_NAME"), _writeNetcdf(false)
//	{
//	}

	DartImageNcGroup() : _imageName(""), _level(""),
	_iteration(""), _unit(""), _component(""),
	_bandName(""), _legacyFolderPath(""), _perTypeType(""), _perTypeName(""), _writeNetcdf(false), _lidarGroup("")
	{
	}

	DartImageNcGroup & operator =(const DartImageNcGroup & ncGroup) {
		if (this != &ncGroup) {
			_imageName = ncGroup._imageName;
			_level = ncGroup._level;
			_iteration = ncGroup._iteration;
			_unit = ncGroup._unit;
			_component = ncGroup._component;
			_bandName = ncGroup._bandName;
			_legacyFolderPath = ncGroup._legacyFolderPath;
			_majorGroups = ncGroup._majorGroups;
			_imageAttributes = ncGroup._imageAttributes;
			_perTypeType = ncGroup._perTypeType;
			_perTypeName = ncGroup._perTypeName;
			_writeNetcdf = ncGroup._writeNetcdf;
			_lidarGroup = ncGroup._lidarGroup;
		}
		return *this;
	}

	friend std::ostream & operator<<(	std::ostream &stream,
										const DartImageNcGroup & ncGroups) {
		stream << "ncGroups{";
		stream << "\t" << "_imageName:" << ncGroups._imageName << endl;
		stream << "\t" << "_level:" << ncGroups._level << endl;
		stream << "\t" << "_iteration: " << ncGroups._iteration << endl;
		stream << "\t" << "_unit: " << ncGroups._unit << endl;
		stream << "\t" << "_component: " << ncGroups._component << endl;
		stream << "\t" << "_bandName: " << ncGroups._bandName << endl;
		stream << "\t" << "_perTypeType: " << ncGroups._perTypeType << endl;
		stream << "\t" << "_perTypeName: " << ncGroups._perTypeName << endl;
		stream << "\t" << "_legacyFolderPath: " << ncGroups._legacyFolderPath << endl;
		stream << "\t" << "_writeNetcdf: " << ncGroups._writeNetcdf << endl;
		stream << "\t" << "_lidarGroup: " << ncGroups._lidarGroup << endl;
		for (std::string majorGroup : ncGroups._majorGroups)
		{
			stream << "\t" << "majorGroup: " << majorGroup << endl;
		}
		for (auto const& attribute : ncGroups._imageAttributes)
		{
			stream << "\t" << "attribute: " << attribute.first << "="<< attribute.second << endl;
		}
		stream << "}" << endl;
		return stream;
	}

	void setLegacyFolderPath(const std::string& legacyFolderPath) {
		_legacyFolderPath = legacyFolderPath;
	}

	bool isWriteNetcdf() {
		return _writeNetcdf;
	}

	void setWriteNetcdf(const bool & writeNercdf) {
		_writeNetcdf = writeNercdf;
	}

	const std::string & getLegacyFolderPath() const {
		return _legacyFolderPath;
	}

	void addMajorGroup(const std::string& groupName) {
		_majorGroups.push_back(groupName);
	}
	void removeLastMajorGroup() {
		_majorGroups.pop_back();
	}

	void setImageName(const std::string& imageName) {
		_imageName = imageName;
	}

	void setLevelName(const std::string& levelName) {
		_level = levelName;
	}

	void setIterationName(const std::string& iterationName) {
		_iteration = iterationName;
	}

	void setUnitName(const std::string& unitName) {
		_unit = unitName;
	}

	void setComponentName(const std::string& componentName) {
		_component = componentName;
	}

	void setBandName(const std::string& bandName) {
		_bandName = bandName;
	}

	void setPerTypeType(const std::string& perTypeType) {
		_perTypeType = perTypeType;
	}

	void setPerTypeName(const std::string& perTypeName) {
		_perTypeName = perTypeName;
	}

	void setLidarGroup(const std::string lidarGroup)
	{
		_lidarGroup = lidarGroup;
	}

	void removeLidarGroup()
	{
		_lidarGroup = "";
	}

	void putAttribute(const std::string& attributeKey, const std::string& attributeValue) {
		_imageAttributes[attributeKey] = attributeValue;
	}

	void clearAttributes() {
		_imageAttributes.clear();
	}

	const std::map<std::string, std::string> & getAttributes() {
		return _imageAttributes;
	}

	void setBandName(const unsigned int & bandNumber, const unsigned int & nbMaxBands) {
		ostringstream bandPrefix;
		bandPrefix << DartNetcdf::NC_BAND_PREFIX;
		unsigned int bandDigits = numDigits(bandNumber);
		unsigned int maxBandDigits = numDigits(nbMaxBands - 1);  // -1 bacause first band index is 0
		for (unsigned int i = 0; i < maxBandDigits - bandDigits; ++i)
		{
			bandPrefix << "0";
		}
		bandPrefix << bandNumber;
		_bandName = bandPrefix.str();
	}

	void removeBandName() {
		_bandName = "";
	}

	unsigned int numDigits(unsigned int number)
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

	std::vector<std::string> getGroupList()
	{
		std::vector<std::string> otherMajorGroups;
		return getGroupList(otherMajorGroups);
	}

	std::vector<std::string> getGroupList(const std::vector<std::string> otherMajorGroups)
	{
		std::vector<std::string> groupList;
		if (!_perTypeType.empty() && !_perTypeName.empty())
		{
			groupList.push_back(_perTypeType);
			groupList.push_back(_perTypeName);
		}
		for (std::string majorGroup : _majorGroups)
		{
			groupList.push_back(majorGroup);
		}
		for (std::string otherMajorGroup : otherMajorGroups)
		{
			groupList.push_back(otherMajorGroup);
		}
		if (_majorGroups.empty() && otherMajorGroups.empty() && _perTypeType.empty())
		{
			groupList.push_back("MajorImages");
		}
		if (!_lidarGroup.empty())
		{
			groupList.push_back(_lidarGroup);
		}
		if (!_imageName.empty())
		{
			groupList.push_back(_imageName);
		}
		if (!_level.empty())
		{
			groupList.push_back(_level);
		}
		if (!_iteration.empty())
		{
			groupList.push_back(_iteration);
		}
		if (!_unit.empty())
		{
			groupList.push_back(_unit);
		}
		if (!_component.empty())
		{
			groupList.push_back(_component);
		}
		if (!_bandName.empty())
		{
			groupList.push_back(_bandName);
		}
		return groupList;
	}
};

#endif /* DARTIMAGENCGROUP_HPP_ */
