/*
 * EmissionMaterials.hpp
 *
 *  Created on: 21 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_EMISSIONMATERIALS_HPP_
#define INCLUDE_INPUTS_STORAGE_EMISSIONMATERIALS_HPP_

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace phsdata {

// ------------------------------------------------------------
// ENUMS
// ------------------------------------------------------------
enum class EmissionUnit : uint8_t { Temperature = 0, Radiance = 1 };
enum class FrontIlluminationOnly : uint8_t { No = 0, Yes = 1 };
enum class Precomputed : uint8_t { No = 0, Yes = 1 };
enum class Profile : uint8_t { No = 0, Yes = 1 };

// ------------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------------
class EmissionData;
class TemperatureData;
class RadianceData;
class IlluminationRatioData;
class TemperatureRange;
class RadianceRange;
class TemperatureRangeProfile;
class RadianceRangeProfile;
class VoxelSize;
class VoxelNumber;
class Dimensions;
class LowerCorner;
class Rotation;
class Positioning;
class BorderManagement;
class EmissionMatrixFileData;
class EmissionBase;

// ------------------------------------------------------------
// BASE CLASS FOR EMISSION TYPES (DirectEmission, IlluminationRatio, etc.)
// ------------------------------------------------------------
class EmissionBase {
public:
    std::string emissionName;
    EmissionUnit emissionUnit;
    int indexEmission;
    virtual ~EmissionBase() = default;
};

// ------------------------------------------------------------
// EMISSION DATA (Polymorphic Base Class)
// ------------------------------------------------------------
class EmissionData {
public:
    virtual ~EmissionData() = default;
    virtual EmissionUnit getUnit() const = 0;

    // New virtual methods for type-safe access
    virtual double getTemperatureValue() const {
        throw std::logic_error("Not a TemperatureData");
    }
    virtual const std::vector<double>& getRadianceValues() const {
        throw std::logic_error("Not a RadianceData");
    }
};

class TemperatureData : public EmissionData {
public:
    double value;
    explicit TemperatureData(double v) : value(v) {}
    EmissionUnit getUnit() const override { return EmissionUnit::Temperature; }
    double getTemperatureValue() const override { return value; }
};

class RadianceData : public EmissionData {
public:
    std::vector<double> values;
    explicit RadianceData(std::vector<double> v) : values(std::move(v)) {}
    EmissionUnit getUnit() const override { return EmissionUnit::Radiance; }
    const std::vector<double>& getRadianceValues() const override { return values; }
};

// ------------------------------------------------------------
// RANGES (for IlluminationRatio)
// ------------------------------------------------------------
class TemperatureRange {
public:
    double minK;
    double maxK;
};

class RadianceRange {
public:
    double minR;
    double maxR;
};

class TemperatureRangeProfile {
public:
    double altitude;
    int layerNumber;
    double layerSize;
    std::string maxFile;
    std::string minFile;
};

class RadianceRangeProfile {
public:
    std::string maxFile;
    std::string minFile;
};

// ------------------------------------------------------------
// ILLUMINATION RATIO DATA (Polymorphic Base Class)
// ------------------------------------------------------------
class IlluminationRatioData {
public:
    virtual ~IlluminationRatioData() = default;
    virtual bool isTemperatureRange() const { return false; }
    virtual bool isRadianceRangeList() const { return false; }
    virtual bool isTemperatureRangeProfile() const { return false; }
    virtual bool isRadianceRangeProfileList() const { return false; }

    // Downcast helpers
    virtual TemperatureRange* asTemperatureRange() { return nullptr; }
    virtual std::vector<RadianceRange>* asRadianceRangeList() { return nullptr; }
    virtual TemperatureRangeProfile* asTemperatureRangeProfile() { return nullptr; }
    virtual std::vector<RadianceRangeProfile>* asRadianceRangeProfileList() { return nullptr; }

    // Const versions
    virtual const TemperatureRange* asTemperatureRange() const { return nullptr; }
    virtual const std::vector<RadianceRange>* asRadianceRangeList() const { return nullptr; }
    virtual const TemperatureRangeProfile* asTemperatureRangeProfile() const { return nullptr; }
    virtual const std::vector<RadianceRangeProfile>* asRadianceRangeProfileList() const { return nullptr; }
};

class TemperatureRangeData : public IlluminationRatioData {
public:
    TemperatureRange data;
    bool isTemperatureRange() const override { return true; }
    TemperatureRange* asTemperatureRange() override { return &data; }
    const TemperatureRange* asTemperatureRange() const override { return &data; }
};

class RadianceRangeListData : public IlluminationRatioData {
public:
    std::vector<RadianceRange> data;
    bool isRadianceRangeList() const override { return true; }
    std::vector<RadianceRange>* asRadianceRangeList() override { return &data; }
    const std::vector<RadianceRange>* asRadianceRangeList() const override { return &data; }
};

class TemperatureRangeProfileData : public IlluminationRatioData {
public:
    TemperatureRangeProfile data;
    bool isTemperatureRangeProfile() const override { return true; }
    TemperatureRangeProfile* asTemperatureRangeProfile() override { return &data; }
    const TemperatureRangeProfile* asTemperatureRangeProfile() const override { return &data; }
};

class RadianceRangeProfileListData : public IlluminationRatioData {
public:
    double altitude;
    int layerNumber;
    int layerSize;
    std::vector<RadianceRangeProfile> data; // List of profiles (each with maxFile/minFile)
    bool isRadianceRangeProfileList() const override { return true; }
    std::vector<RadianceRangeProfile>* asRadianceRangeProfileList() { return &data; }
    const std::vector<RadianceRangeProfile>* asRadianceRangeProfileList() const { return &data; }
    // Add getters for altitude/layerNumber/layerSize if needed
};

// ------------------------------------------------------------
// DIMENSIONS & POSITIONING
// ------------------------------------------------------------
class VoxelSize {
public:
    double sizeX;
    double sizeY;
    double sizeZ;
};

class VoxelNumber {
public:
    int numberX;
    int numberY;
    int numberZ;
};

class Dimensions {
public:
    VoxelSize voxelSize;
    VoxelNumber voxelNumber;
};

class LowerCorner {
public:
    double cornerX;
    double cornerY;
    double cornerZ;
};

class Rotation {
public:
    double intrinsicZ;
    double nutationY;
    double precessionZ;
};

class Positioning {
public:
    LowerCorner lowerCorner;
    Rotation rotation;
};

class BorderManagement {
public:
    int borderX;
    int borderY;
    int borderZ;
};

// ------------------------------------------------------------
// FILE DATA (for EmissionMatrix and EmissionPerTriangle)
// ------------------------------------------------------------
class EmissionMatrixFileData {
public:
    std::unique_ptr<std::string> temperatureFile; // nullptr if not set
    std::vector<std::string> radianceFiles;
};

// ------------------------------------------------------------
// MAIN CLASSES (now inherit from EmissionBase)
// ------------------------------------------------------------
class DirectEmission : public EmissionBase {
public:
    std::unique_ptr<EmissionData> data; // TemperatureData or RadianceData
};

class IlluminationRatio : public EmissionBase {
public:
    FrontIlluminationOnly frontIlluminationOnly;
    Precomputed precomputed;
    Profile profile;
    std::unique_ptr<IlluminationRatioData> data; // One of the 4 derived types
};

class EmissionMatrix : public EmissionBase {
public:
    EmissionMatrixFileData fileData;
    Dimensions dimensions;
    Positioning positioning;
    BorderManagement borderManagement;
};

class EmissionPerTriangle : public EmissionBase {
public:
    EmissionMatrixFileData fileData;
};

// ------------------------------------------------------------
// TOP-LEVEL CLASS
// ------------------------------------------------------------
class EmissionMaterials {
public:
    double skylForIlluminatioRatio;
    std::vector<DirectEmission> directEmissions;
    std::vector<IlluminationRatio> illuminationRatios;
    std::vector<EmissionMatrix> emissionMatrices;
    std::vector<EmissionPerTriangle> emissionPerTriangles;
};

} // namespace phsdata

#endif /* INCLUDE_INPUTS_STORAGE_EMISSIONMATERIALS_HPP_ */
