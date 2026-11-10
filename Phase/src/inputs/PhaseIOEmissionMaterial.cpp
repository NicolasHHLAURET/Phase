/*
 * PhaseIOEmissionMaterial.cpp
 *
 *  Created on: 22 sept. 2026
 *      Author: lauretn
 */
#include "PhaseDAO.hpp"

#include <string>
#include <stdexcept>
#include <sstream>

namespace {
    // Helper: Convert string to EmissionUnit
    phsdata::EmissionUnit stringToEmissionUnit(const std::string& str) {
        if (str == "0") return phsdata::EmissionUnit::Temperature;
        if (str == "1") return phsdata::EmissionUnit::Radiance;
        throw std::runtime_error("Invalid EmissionUnit value: " + str);
    }

    // Helper: Convert string to FrontIlluminationOnly
    phsdata::FrontIlluminationOnly stringToFrontIlluminationOnly(const std::string& str) {
        if (str == "0") return phsdata::FrontIlluminationOnly::No;
        if (str == "1") return phsdata::FrontIlluminationOnly::Yes;
        throw std::runtime_error("Invalid FrontIlluminationOnly value: " + str);
    }

    // Helper: Convert string to Precomputed
    phsdata::Precomputed stringToPrecomputed(const std::string& str) {
        if (str == "0") return phsdata::Precomputed::No;
        if (str == "1") return phsdata::Precomputed::Yes;
        throw std::runtime_error("Invalid Precomputed value: " + str);
    }

    // Helper: Convert string to Profile
    phsdata::Profile stringToProfile(const std::string& str) {
        if (str == "0") return phsdata::Profile::No;
        if (str == "1") return phsdata::Profile::Yes;
        throw std::runtime_error("Invalid Profile value: " + str);
    }
}

// ------------------------------------------------------------
// PARSER FUNCTIONS
// ------------------------------------------------------------

// Parse a single DirectEmission at index `idx`
phsdata::DirectEmission PhaseDAO::parseDirectEmission(const string & header, int idx) {
	phsdata::DirectEmission de;
    const std::string base = header + "DirectEmission[" + std::to_string(idx) + "].";

    de.emissionName = DartSAParameters::get<std::string>(base + "emissionName");
    de.emissionUnit = stringToEmissionUnit(DartSAParameters::get<std::string>(base + "emissionUnit"));
    de.indexEmission = DartSAParameters::get<int>(base + "indexEmission");

    if (DartSAParameters::exists(base + "Temperature.emissionValue")) {
        double value = DartSAParameters::get<double>(base + "Temperature.emissionValue");
        de.data = std::unique_ptr<phsdata::EmissionData>(new phsdata::TemperatureData(value));
    }
    else if (DartSAParameters::exists(base + "RadianceList.Radiance[0].emissionValue")) {
        int radCount = DartSAParameters::getNbOf(base + "RadianceList.Radiance");
        std::vector<double> radValues;
        for (int i = 0; i < radCount; ++i) {
            std::string radKey = base + "RadianceList.Radiance[" + std::to_string(i) + "].emissionValue";
            radValues.push_back(DartSAParameters::get<double>(radKey));
        }
        de.data = std::unique_ptr<phsdata::EmissionData>(new phsdata::RadianceData(std::move(radValues)));
    }
    else {
        throw std::runtime_error("DirectEmission[" + std::to_string(idx) + "]: No Temperature or RadianceList found.");
    }

    return de;
}

// Parse a single IlluminationRatio at index `idx`
phsdata::IlluminationRatio PhaseDAO::parseIlluminationRatio(const string & header, int idx) {
	phsdata::IlluminationRatio ir;
    const std::string base = header + "IlluminationRatio[" + std::to_string(idx) + "].";

    ir.emissionName = DartSAParameters::get<std::string>(base + "emissionName");
    ir.emissionUnit = stringToEmissionUnit(DartSAParameters::get<std::string>(base + "emissionUnit"));
    ir.indexEmission = DartSAParameters::get<int>(base + "indexEmission");
    ir.frontIlluminationOnly = stringToFrontIlluminationOnly(DartSAParameters::get<std::string>(base + "frontIlluminationOnly"));
    ir.precomputed = stringToPrecomputed(DartSAParameters::get<std::string>(base + "precomputed"));
    ir.profile = stringToProfile(DartSAParameters::get<std::string>(base + "profile"));

    if (ir.profile == phsdata::Profile::No) {
        if (ir.emissionUnit == phsdata::EmissionUnit::Temperature) {
            auto data = std::unique_ptr<phsdata::TemperatureRangeData>(new phsdata::TemperatureRangeData());
            data->data.minK = DartSAParameters::get<double>(base + "TemperatureRange.minK");
            data->data.maxK = DartSAParameters::get<double>(base + "TemperatureRange.maxK");
            ir.data = std::move(data);
        }
        else { // Radiance
            auto data = std::unique_ptr<phsdata::RadianceRangeListData>(new phsdata::RadianceRangeListData());
            int rangeCount = DartSAParameters::getNbOf(base + "RadianceRangeList.RadianceRange");
            for (int i = 0; i < rangeCount; ++i) {
                std::string rangeKey = base + "RadianceRangeList.RadianceRange[" + std::to_string(i) + "].";
                phsdata::RadianceRange rr;
                rr.minR = DartSAParameters::get<double>(rangeKey + "minR");
                rr.maxR = DartSAParameters::get<double>(rangeKey + "maxR");
                data->data.push_back(rr);
            }
            ir.data = std::move(data);
        }
    }
    else { // Profile::Yes
        if (ir.emissionUnit == phsdata::EmissionUnit::Temperature) {
            auto data = std::unique_ptr<phsdata::TemperatureRangeProfileData>(new phsdata::TemperatureRangeProfileData());
            data->data.altitude = DartSAParameters::get<double>(base + "TemperatureRangeProfile.altitude");
            data->data.layerNumber = DartSAParameters::get<int>(base + "TemperatureRangeProfile.layerNumber");
            data->data.layerSize = DartSAParameters::get<double>(base + "TemperatureRangeProfile.layerSize");
            data->data.maxFile = DartSAParameters::get<std::string>(base + "TemperatureRangeProfile.maxFile");
            data->data.minFile = DartSAParameters::get<std::string>(base + "TemperatureRangeProfile.minFile");
            ir.data = std::move(data);
        }
        else { // Radiance
            auto data = std::unique_ptr<phsdata::RadianceRangeProfileListData>(new phsdata::RadianceRangeProfileListData());
            data->altitude = DartSAParameters::get<double>(base + "RadianceRangeProfileList.altitude");
            data->layerNumber = DartSAParameters::get<int>(base + "RadianceRangeProfileList.layerNumber");
            data->layerSize = DartSAParameters::get<double>(base + "RadianceRangeProfileList.layerSize");

            int profileCount = DartSAParameters::getNbOf(base + "RadianceRangeProfileList.RadianceRangeProfile");
            for (int i = 0; i < profileCount; ++i) {
                std::string profileKey = base + "RadianceRangeProfileList.RadianceRangeProfile[" + std::to_string(i) + "].";
                phsdata::RadianceRangeProfile rrp;
                rrp.maxFile = DartSAParameters::get<std::string>(profileKey + "maxFile");
                rrp.minFile = DartSAParameters::get<std::string>(profileKey + "minFile");
                data->data.push_back(rrp);
            }
            ir.data = std::move(data);
        }
    }

    return ir;
}

// Parse a single EmissionMatrix at index `idx`
phsdata::EmissionMatrix PhaseDAO::parseEmissionMatrix(const string & header, int idx) {
	phsdata::EmissionMatrix em;
    const std::string base = "EmissionFunction.EmissionMatrix[" + std::to_string(idx) + "].";

    em.emissionName = DartSAParameters::get<std::string>(base + "emissionName");
    em.emissionUnit = stringToEmissionUnit(DartSAParameters::get<std::string>(base + "emissionUnit"));
    em.indexEmission = DartSAParameters::get<int>(base + "indexEmission");

    // Parse file data
	if (em.emissionUnit == phsdata::EmissionUnit::Temperature) {
		if (DartSAParameters::exists(base + "TemperatureFile.file")) {
			em.fileData.temperatureFile = std::unique_ptr<std::string>(new std::string(DartSAParameters::get<std::string>(base + "TemperatureFile.file")));
		}
	}
    else { // Radiance
        int fileCount = DartSAParameters::getNbOf(base + "RadianceFileList.RadianceFile");
        for (int i = 0; i < fileCount; ++i) {
            std::string fileKey = base + "RadianceFileList.RadianceFile[" + std::to_string(i) + "].file";
            em.fileData.radianceFiles.push_back(DartSAParameters::get<std::string>(fileKey));
        }
    }

    // Parse Dimensions
    {
        const std::string dimBase = base + "Dimensions.";
        em.dimensions.voxelSize.sizeX = DartSAParameters::get<double>(dimBase + "VoxelSize.sizeX");
        em.dimensions.voxelSize.sizeY = DartSAParameters::get<double>(dimBase + "VoxelSize.sizeY");
        em.dimensions.voxelSize.sizeZ = DartSAParameters::get<double>(dimBase + "VoxelSize.sizeZ");

        em.dimensions.voxelNumber.numberX = DartSAParameters::get<int>(dimBase + "VoxelNumber.numberX");
        em.dimensions.voxelNumber.numberY = DartSAParameters::get<int>(dimBase + "VoxelNumber.numberY");
        em.dimensions.voxelNumber.numberZ = DartSAParameters::get<int>(dimBase + "VoxelNumber.numberZ");
    }

    // Parse Positioning
    {
        const std::string posBase = base + "Positioning.";
        em.positioning.lowerCorner.cornerX = DartSAParameters::get<double>(posBase + "LowerCorner.cornerX");
        em.positioning.lowerCorner.cornerY = DartSAParameters::get<double>(posBase + "LowerCorner.cornerY");
        em.positioning.lowerCorner.cornerZ = DartSAParameters::get<double>(posBase + "LowerCorner.cornerZ");

        em.positioning.rotation.intrinsicZ = DartSAParameters::get<double>(posBase + "Rotation.intrinsicZ");
        em.positioning.rotation.nutationY = DartSAParameters::get<double>(posBase + "Rotation.nutationY");
        em.positioning.rotation.precessionZ = DartSAParameters::get<double>(posBase + "Rotation.precessionZ");
    }

    // Parse BorderManagement
    {
        const std::string borderBase = base + "BorderManagement.";
        em.borderManagement.borderX = DartSAParameters::get<int>(borderBase + "borderX");
        em.borderManagement.borderY = DartSAParameters::get<int>(borderBase + "borderY");
        em.borderManagement.borderZ = DartSAParameters::get<int>(borderBase + "borderZ");
    }

    return em;
}

// Parse a single EmissionPerTriangle at index `idx`
phsdata::EmissionPerTriangle PhaseDAO::parseEmissionPerTriangle(const string & header, int idx) {
	phsdata::EmissionPerTriangle ept;
    const std::string base = header + "EmissionPerTriangle[" + std::to_string(idx) + "].";

    ept.emissionName = DartSAParameters::get<std::string>(base + "emissionName");
    ept.emissionUnit = stringToEmissionUnit(DartSAParameters::get<std::string>(base + "emissionUnit"));
    ept.indexEmission = DartSAParameters::get<int>(base + "indexEmission");

    // Parse file data
    if (ept.emissionUnit == phsdata::EmissionUnit::Temperature) {
        if (DartSAParameters::exists(base + "TemperatureFile.file")) {
            ept.fileData.temperatureFile = std::unique_ptr<std::string>(new std::string(DartSAParameters::get<std::string>(base + "TemperatureFile.file")));
        }
    }
    else { // Radiance
        int fileCount = DartSAParameters::getNbOf(base + "RadianceFileList.RadianceFile");
        for (int i = 0; i < fileCount; ++i) {
            std::string fileKey = base + "RadianceFileList.RadianceFile[" + std::to_string(i) + "].file";
            ept.fileData.radianceFiles.push_back(DartSAParameters::get<std::string>(fileKey));
        }
    }

    return ept;
}

// ------------------------------------------------------------
// TOP-LEVEL PARSER
// ------------------------------------------------------------
void PhaseDAO::parseEmissionMaterials(PhaseGeneralParametersDTO & genDTO) {
	const std::string header = "Materials.EmissionFunction.";

    // Parse root attribute
	genDTO._emissionMaterials.skylForIlluminatioRatio = DartSAParameters::get<double>(header + "skylForIlluminatioRatio");

    // Parse DirectEmission list
    int deCount = DartSAParameters::getNbOf("EmissionFunction.DirectEmission");
    for (int i = 0; i < deCount; ++i) {
    	genDTO._emissionMaterials.directEmissions.push_back(parseDirectEmission(header, i));
    }

    // Parse IlluminationRatio list
    int irCount = DartSAParameters::getNbOf("EmissionFunction.IlluminationRatio");
    for (int i = 0; i < irCount; ++i) {
    	genDTO._emissionMaterials.illuminationRatios.push_back(parseIlluminationRatio(header, i));
    }

    // Parse EmissionMatrix list
    int emCount = DartSAParameters::getNbOf("EmissionFunction.EmissionMatrix");
    for (int i = 0; i < emCount; ++i) {
    	genDTO._emissionMaterials.emissionMatrices.push_back(parseEmissionMatrix(header, i));
    }

    // Parse EmissionPerTriangle list
    int eptCount = DartSAParameters::getNbOf("EmissionFunction.EmissionPerTriangle");
    for (int i = 0; i < eptCount; ++i) {
    	genDTO._emissionMaterials.emissionPerTriangles.push_back(parseEmissionPerTriangle(header, i));
    }
}
