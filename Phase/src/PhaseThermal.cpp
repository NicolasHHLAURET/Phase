/*
 * PhaseThermal.cpp
 *
 *  Created on: 23 sept. 2026
 *      Author: lauretn
 */

#include "Phase.hpp"
#include "Matrix4.hpp"

// Helper function to clear and initialize thetas/phis
void initializeAngles(std::vector<float> & angles, unsigned int nbAngles, float defaultValue) {
    angles.clear();
    if (nbAngles != 1) {
        angles.resize(nbAngles);
    } else {
        angles = {defaultValue};
    }
}

void writeScopeRadianceFile(PhaseGeneralParametersDTO & genDTO,
                          unsigned int & nbLayers,
                          std::vector<float> & thetas,
                          std::vector<float> & phis,
                          const std::string & temperatureInFile,
                          const std::string & outputFolder,
                          const std::string & outputFileName) {
    std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, temperatureInFile);
    std::ifstream influx(inputFilePath.c_str(), std::ios::in);
    if (!influx) {
        std::cerr << "Warning: Failed to open temperature file: " << inputFilePath << std::endl;
        return;
    }

    unsigned int nbTheta, nbPhi;
    if (!(influx >> nbLayers >> nbTheta >> nbPhi)) {
        std::cerr << "Warning: Failed to read header from temperature file: " << inputFilePath << std::endl;
        return;
    }

    // Initialize thetas and phis
    initializeAngles(thetas, nbTheta, 180.f);
    initializeAngles(phis, nbPhi, 360.f);

    // Read theta/phi values if nbTheta/nbPhi != 1
    if (nbTheta != 1) {
        for (unsigned int i = 0; i < nbTheta; ++i) {
            if (!(influx >> thetas[i])) {
                std::cerr << "Warning: Failed to read theta value " << i << " from file: " << inputFilePath << std::endl;
                return;
            }
        }
    }
    if (nbPhi != 1) {
        for (unsigned int i = 0; i < nbPhi; ++i) {
            if (!(influx >> phis[i])) {
                std::cerr << "Warning: Failed to read phi value " << i << " from file: " << inputFilePath << std::endl;
                return;
            }
        }
    }

    createDirectory(outputFolder);
    boost::filesystem::path outputPath(outputFolder);
    std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);

    const double precision = 0.001;
    const unsigned int maxIter = 1000;
    float radiance;
    double currentTemperature;

    for (unsigned int l = 0; l < nbLayers; ++l) {
        for (unsigned int t = 0; t < nbTheta; ++t) {
            for (unsigned int p = 0; p < nbPhi; ++p) {
                if (!(influx >> currentTemperature)) {
                    std::cerr << "Warning: Failed to read temperature at layer " << l
                              << ", theta " << t << ", phi " << p << " from file: " << inputFilePath << std::endl;
                    return;
                }
                for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
                    radiance = (genDTO.spectralDomain[bandIndex] == 0) ? 0.0f :
                               static_cast<float>(temperatureToLuminanceMeanOnSpectralBand(
                                   currentTemperature,
                                   genDTO.emissionLaw[bandIndex],
                                   genDTO.lowerBandWavelength[bandIndex] * 1E-6,
                                   genDTO.upperBandWavelength[bandIndex] * 1E-6,
                                   precision,
                                   maxIter));
                    outflux.write((const char *)&radiance, sizeof(float));
                }
            }
        }
    }

    outflux.close();
    influx.close();
}

void writeScopeRadianceFile(PhaseGeneralParametersDTO & genDTO,
                          unsigned int & nbLayers,
                          std::vector<float> & thetas,
                          std::vector<float> & phis,
                          const std::vector<std::string> & radianceFiles,
                          const std::string & outputFolder,
                          const std::string & outputFileName) {
    if (radianceFiles.empty()) {
        throw std::runtime_error("radianceFiles is empty");
    }

    std::vector<float> interleavedFlatMatrix;
    unsigned int nbTheta = 0, nbPhi = 0, previousNbTheta = 0, previousNbPhi = 0, previousNbLayers = 0;

    for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands && bandIndex < radianceFiles.size(); bandIndex++) {
        std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, radianceFiles[bandIndex]);
        std::ifstream influx(inputFilePath.c_str(), std::ios::in);
        if (influx.fail()) {
            std::cerr << "Warning: Failed to open SCOPE radiance file: " << inputFilePath
                      << ". Band " << bandIndex << " will be zero." << std::endl;
            continue;
        }

        if (!(influx >> nbLayers >> nbTheta >> nbPhi)) {
            std::cerr << "Warning: Failed to read header from radiance file: " << inputFilePath << std::endl;
            continue;
        }

        // Validate dimensions
        if (nbLayers == 0 || nbTheta == 0 || nbPhi == 0) {
            std::cerr << "Warning: Invalid dimensions in radiance file: " << inputFilePath << std::endl;
            continue;
        }

        // Initialize thetas and phis for the first band only
        if (bandIndex == 0) {
            initializeAngles(thetas, nbTheta, 180.f);
            initializeAngles(phis, nbPhi, 360.f);

            // Read theta/phi values if nbTheta/nbPhi != 1
            if (nbTheta != 1) {
                for (unsigned int i = 0; i < nbTheta; ++i) {
                    if (!(influx >> thetas[i])) {
                        std::cerr << "Warning: Failed to read theta value " << i << " from file: " << inputFilePath << std::endl;
                        continue;
                    }
                }
            }
            if (nbPhi != 1) {
                for (unsigned int i = 0; i < nbPhi; ++i) {
                    if (!(influx >> phis[i])) {
                        std::cerr << "Warning: Failed to read phi value " << i << " from file: " << inputFilePath << std::endl;
                        continue;
                    }
                }
            }
            interleavedFlatMatrix.resize(nbLayers * nbTheta * nbPhi * genDTO.numberOfSpectralBands, 0.f);
        } else {
            // Validate consistency with previous files
            if (previousNbTheta != nbTheta || previousNbPhi != nbPhi || previousNbLayers != nbLayers) {
                throw std::runtime_error("Irrecoverable Error: Inconsistent number of values between SCOPE radiance files " +
                                          radianceFiles[bandIndex - 1] + " and " + radianceFiles[bandIndex]);
            }
            float trash;
            // Skip all the thetas and phis
            for (unsigned int i = 0; i < nbTheta; ++i) {
                if (!(influx >> trash)) {
                    std::cerr << "Warning: Failed to read theta value " << i << " from file: " << inputFilePath << std::endl;
                    continue;
                }
            }
            for (unsigned int i = 0; i < nbPhi; ++i) {
                if (!(influx >> trash)) {
                    std::cerr << "Warning: Failed to read phi value " << i << " from file: " << inputFilePath << std::endl;
                    continue;
                }
            }
        }

        // Read radiance values
        float value;
        for (unsigned int l = 0; l < nbLayers; ++l) {
            for (unsigned int t = 0; t < nbTheta; ++t) {
                for (unsigned int p = 0; p < nbPhi; ++p) {
                    if (!(influx >> value)) {
                        std::cerr << "Warning: Failed to read radiance value at layer " << l
                                  << ", theta " << t << ", phi " << p << " from file: " << inputFilePath << std::endl;
                        continue;
                    }
                    interleavedFlatMatrix[((l * nbTheta + t) * nbPhi + p) * genDTO.numberOfSpectralBands + bandIndex] = value;
                }
            }
        }
        influx.close();

        previousNbTheta = nbTheta;
        previousNbPhi = nbPhi;
        previousNbLayers = nbLayers;
    }

    createDirectory(outputFolder);
    boost::filesystem::path outputPath(outputFolder);
    std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);
    outflux.write((const char *)interleavedFlatMatrix.data(), sizeof(float) * interleavedFlatMatrix.size());
    outflux.close();
}

std::string getOrCreateScopeTexture(PhaseGeneralParametersDTO & genDTO,
                                    const std::vector<std::string> & scopeRadianceFileNames,
                                    int nbLayers,
                                    double layerSize,
                                    double altitude) {
    if (scopeRadianceFileNames.empty()) {
        throw std::runtime_error("scopeRadianceFileNames is empty");
    }

    std::string combinedFileNames;
    for (const std::string & fileName : scopeRadianceFileNames) {
        combinedFileNames += DartEnv::getFilePathForRead(INPUT_DIR, fileName) + "_";
    }

    auto it = genDTO._fileNameToTextureMap.find(combinedFileNames);
    if (it == genDTO._fileNameToTextureMap.end()) {
        std::string textureName = "scopeProfileTex_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
        std::string textureKeyRoot = "scene.textures." + textureName;

        unsigned int nbLayersActual;
        std::vector<float> thetas, phis;

        boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
        if (outputDir.empty()) {
            throw std::runtime_error("Output directory path is empty");
        }
        std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();

        writeScopeRadianceFile(genDTO, nbLayersActual, thetas, phis, scopeRadianceFileNames, outputDir.string(), textureName + ".bin");

        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", outputTextureFile);
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".nbLayers", boost::lexical_cast<std::string>(nbLayersActual));
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "scopetemperature");
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");

        std::ostringstream valueStream;
        for (const auto value : thetas) {
            valueStream << value << " ";
        }
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".thetas", valueStream.str());

        valueStream.str("");
        for (const auto value : phis) {
            valueStream << value << " ";
        }
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".phis", valueStream.str());

        genDTO._fileNameToTextureMap[combinedFileNames] = textureName;
        return textureName;
    } else {
        return it->second;
    }
}

std::string getOrCreateScopeTexture(PhaseGeneralParametersDTO & genDTO,
                                    const std::string & scopeFileName,
                                    int nbLayers,
                                    double layerSize,
                                    double altitude) {
    if (scopeFileName.empty()) {
        throw std::runtime_error("scopeFileName is empty");
    }

    auto it = genDTO._fileNameToTextureMap.find(scopeFileName);
    if (it == genDTO._fileNameToTextureMap.end()) {
        std::string textureName = "scopeProfileTex_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
        std::string textureKeyRoot = "scene.textures." + textureName;

        unsigned int nbLayersActual;
        std::vector<float> thetas, phis;

        boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
        if (outputDir.empty()) {
            throw std::runtime_error("Output directory path is empty");
        }
        std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();

        writeScopeRadianceFile(genDTO, nbLayersActual, thetas, phis, scopeFileName, outputDir.string(), textureName + ".bin");

        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", outputTextureFile);
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".nbLayers", boost::lexical_cast<std::string>(nbLayersActual));
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "scopetemperature");
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");

        std::ostringstream valueStream;
        for (const auto value : thetas) {
            valueStream << value << " ";
        }
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".thetas", valueStream.str());

        valueStream.str("");
        for (const auto value : phis) {
            valueStream << value << " ";
        }
        genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".phis", valueStream.str());

        genDTO._fileNameToTextureMap[scopeFileName] = textureName;
        return textureName;
    } else {
        return it->second;
    }
}

void writeRadianceFile(PhaseGeneralParametersDTO & genDTO,
                      const std::string & temperatureInFile,
                      const std::string & outputFolder,
                      const std::string & outputFileName) {
    std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, temperatureInFile);
    std::ifstream influx(inputFilePath.c_str(), std::ios::in);
    if (influx.fail()) {
        std::cerr << "Warning: Failed to open temperature file: " << inputFilePath
                  << " (relative path: " << temperatureInFile << ")" << std::endl;
        return;
    }

    createDirectory(outputFolder);
    boost::filesystem::path outputPath(outputFolder);
    std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);

    const double precision = 0.001;
    const unsigned int maxIter = 1000;
    double currentTemperature;
    float radiance; // LuxCore expects float

    while (true) {
        if (!(influx >> currentTemperature)) {
            if (influx.eof()) break; // EOF reached
            // Skip non-numeric lines
            std::string line;
            if (!std::getline(influx, line)) break; // EOF or error
            continue;
        }

        for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
            radiance = (genDTO.spectralDomain[bandIndex] == 0) ? 0.0f :
                       static_cast<float>(temperatureToLuminanceMeanOnSpectralBand(
                           currentTemperature,
                           genDTO.emissionLaw[bandIndex],
                           genDTO.lowerBandWavelength[bandIndex] * 1E-6,
                           genDTO.upperBandWavelength[bandIndex] * 1E-6,
                           precision,
                           maxIter));
            outflux.write((const char *)&radiance, sizeof(float));
        }
    }

    outflux.close();
    influx.close();
}

void writeRadianceFile(PhaseGeneralParametersDTO & genDTO, const std::string & temperatureInFile, const phsdata::VoxelNumber & voxelNumber, const std::string & outputFolder, const std::string & outputFileName) {
    std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, temperatureInFile);
    std::ifstream influx(inputFilePath.c_str(), std::ios::in);
    if (influx.fail()) {
        std::cerr << "Warning: Failed to open temperature file: " << inputFilePath << " (relative path: " << temperatureInFile << ")" << std::endl;
        return;
    }
    // Ensure output directory exists
	createDirectory(outputFolder);
	boost::filesystem::path outputPath(outputFolder);
	std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);
	const double precision = 0.001;
	const unsigned int maxIter = 1000;
	const unsigned int expectedNumberOfVoxels = voxelNumber.numberX * voxelNumber.numberY * voxelNumber.numberZ;
	double currentTemperature;
	float radiance; // LuxCore expects float and not double
	unsigned int currentIndex = 0;
	// Ignore excedent values coming from the file, if any
	while (currentIndex < expectedNumberOfVoxels) {
        if (!(influx >> currentTemperature)) {
            if (influx.eof()) break; // EOF reached
            // Skip non-numeric lines
            std::string line;
            if (!std::getline(influx, line)) break; // EOF or error
            continue;
        }
		for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
			radiance = genDTO.spectralDomain[bandIndex] == 0 ? 0.0f : static_cast<float>(temperatureToLuminanceMeanOnSpectralBand(
				currentTemperature,
				genDTO.emissionLaw[bandIndex],
				genDTO.lowerBandWavelength[bandIndex] * 1E-6,
				genDTO.upperBandWavelength[bandIndex] * 1E-6,
				precision,
				maxIter));
			outflux.write((const char *)&radiance, sizeof(float));
		}
		++currentIndex;
	}
	// Pad with zeros if necessary
	radiance = 0.f;
	while (currentIndex < expectedNumberOfVoxels) {
		for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
			outflux.write((const char *)&radiance, sizeof(float));
		}
		++currentIndex;
	}

	outflux.close();
	influx.close();
}

void writeRadianceFile(PhaseGeneralParametersDTO & genDTO, const vector<std::string> & radianceFiles, const std::string & outputFolder, const std::string & outputFileName) {
	// The total number of values is unknown at this point and, while not recommended to the user, may vary from one file to another
	// Note 2: LuxCore expects float and not double
	std::vector<float> interleavedFlatMatrix;

	// Read each spectral band file and interleave
    for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands && bandIndex < radianceFiles.size(); bandIndex++) {
        std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, radianceFiles[bandIndex]);
        std::ifstream influx(inputFilePath.c_str(), std::ios::in);
        if (influx.fail()) {
            std::cerr << "Warning: Failed to open radiance file: " << inputFilePath << ". Band " << bandIndex << " will be zero." << std::endl;
            continue;
        }

        unsigned int matrixIndex = bandIndex; // Start at bandIndex (interleaved)
        float value;
        while (influx >> value) {
        	if (matrixIndex >= interleavedFlatMatrix.size())
        		interleavedFlatMatrix.resize(interleavedFlatMatrix.size() + genDTO.numberOfSpectralBands, 0.f); // Repeated calls to resize are inefficient, but is there a better way ?
        	interleavedFlatMatrix[matrixIndex] = value;
            matrixIndex += genDTO.numberOfSpectralBands;
        }
        influx.close();
    }

	// Ensure output directory exists
	createDirectory(outputFolder);
    boost::filesystem::path outputPath(outputFolder);
	std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);
	outflux.write((const char *)interleavedFlatMatrix.data(), sizeof(float) * interleavedFlatMatrix.size());
	outflux.close();
}

void writeRadianceFile(PhaseGeneralParametersDTO & genDTO, const vector<std::string> & radianceFiles, const phsdata::VoxelNumber & voxelNumber, const std::string & outputFolder, const std::string & outputFileName) {
	// Note: This allocates a large buffer (voxels × bands × sizeof(float)).
	// 		 If this fails, the next module (e.g., LuxCore) would also fail to load the matrix.
	// Note 2: LuxCore expects float and not double
	std::vector<float> interleavedFlatMatrix(voxelNumber.numberX * voxelNumber.numberY * voxelNumber.numberZ * genDTO.numberOfSpectralBands, 0.f);

	// Read each spectral band file and interleave
    for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands && bandIndex < radianceFiles.size(); bandIndex++) {
        std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, radianceFiles[bandIndex]);
        std::ifstream influx(inputFilePath.c_str(), std::ios::in);
        if (influx.fail()) {
            std::cerr << "Warning: Failed to open radiance file: " << inputFilePath << ". Band " << bandIndex << " will be zero." << std::endl;
            continue;
        }

        unsigned int matrixIndex = bandIndex; // Start at bandIndex (interleaved)
        float value;
        while (matrixIndex < interleavedFlatMatrix.size() && (influx >> value)) {
            interleavedFlatMatrix[matrixIndex] = value;
            matrixIndex += genDTO.numberOfSpectralBands;
        }
        influx.close();
    }

	// Ensure output directory exists
	createDirectory(outputFolder);
    boost::filesystem::path outputPath(outputFolder);
	std::ofstream outflux((outputPath / outputFileName).string().c_str(), std::ios::binary | std::ios::out);
	outflux.write((const char *)interleavedFlatMatrix.data(), sizeof(float) * interleavedFlatMatrix.size());
	outflux.close();
}

void writeDirectEmission(PhaseGeneralParametersDTO & genDTO,
                        const phsdata::DirectEmission & de,
                        const std::string & key,
                        double precision,
                        unsigned int maxIter) {
    const double epsilon = 1e-10;
    std::string emissionKey = key + de.emissionName + "_" + std::to_string(de.indexEmission) + ".";
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "index", std::to_string(de.indexEmission));

    if (de.data->getUnit() == phsdata::EmissionUnit::Temperature) {
        double value = de.data->getTemperatureValue();
        if (std::abs(value) < epsilon) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
        } else {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
            std::ostringstream emission;
            for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
                if (bandIndex > 0) emission << " ";
                emission << (genDTO.spectralDomain[bandIndex] == PhaseGeneralParametersDTO::Visible ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
                            value,
                            genDTO.emissionLaw[bandIndex],
                            genDTO.lowerBandWavelength[bandIndex] * 1E-6,
                            genDTO.upperBandWavelength[bandIndex] * 1E-6,
                            precision,
                            maxIter));
            }
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", emission.str());
        }
    } else {
        const std::vector<double>& values = de.data->getRadianceValues();
        bool allZero = true;
        for (double val : values) {
            if (val >= epsilon) {
                allZero = false;
                break;
            }
        }
        if (allZero) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
        } else {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
            std::ostringstream emission;
            for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
                if (bandIndex > 0) emission << " ";
                emission << (bandIndex < values.size()) ? values[bandIndex] : 0.0;
            }
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", emission.str());
        }
    }
}

void writeIlluminationRatio(PhaseGeneralParametersDTO & genDTO,
							const phsdata::IlluminationRatio & ir,
							const std::string & key,
							double precision,
							unsigned int maxIter) {
    const double epsilon = 1e-10;
    std::string emissionKey = key + ir.emissionName + "_" + std::to_string(ir.indexEmission) + ".";
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "index", std::to_string(ir.indexEmission));

    if (ir.data->isTemperatureRange()) {
    	const phsdata::TemperatureRange * tr = ir.data->asTemperatureRange();

        if (tr->maxK < epsilon || tr->minK > tr->maxK) {
        	// minK can be == 0 as long as maxK > 0.
        	// minK can't be > maxK. It's an error from the user. Disable the emission function in this case. Write a warning to the user ?
            // maxK (and thus minK) can't be negative. Also disable the emission function. Write a warning to the user ?
        	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
        } else {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(ir.frontIlluminationOnly == phsdata::FrontIlluminationOnly::Yes));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(ir.precomputed == phsdata::Precomputed::Yes));

        	// Write min radiance, derived from minK
        	{
				std::ostringstream emission;
				for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
					if (bandIndex > 0) emission << " ";
					emission << (genDTO.spectralDomain[bandIndex] == PhaseGeneralParametersDTO::Visible ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
								tr->minK,
								genDTO.emissionLaw[bandIndex],
								genDTO.lowerBandWavelength[bandIndex] * 1E-6,
								genDTO.upperBandWavelength[bandIndex] * 1E-6,
								precision,
								maxIter));
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", emission.str());
        	}
        	// Write max radiance, derived from maxK
        	{
				std::ostringstream emission;
				for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
					if (bandIndex > 0) emission << " ";
					emission << (genDTO.spectralDomain[bandIndex] == PhaseGeneralParametersDTO::Visible ? 0.0 : temperatureToLuminanceMeanOnSpectralBand(
								tr->maxK,
								genDTO.emissionLaw[bandIndex],
								genDTO.lowerBandWavelength[bandIndex] * 1E-6,
								genDTO.upperBandWavelength[bandIndex] * 1E-6,
								precision,
								maxIter));
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", emission.str());
        	}
        }
    } else if (ir.data->isRadianceRangeList()) {
    	const std::vector<phsdata::RadianceRange> & rrl = *(ir.data->asRadianceRangeList());

        bool maxGreaterThanZero = false;
        bool minGreaterThanMax = false;
        for (const phsdata::RadianceRange & rr : rrl) {
        	// All the minR in the list can be equal to zero (or negative), as long as one maxR > 0
            if (rr.maxR >= epsilon) {
            	// Also prevents negative values
            	maxGreaterThanZero = true;
                break;
            }
            if (rr.minR > rr.maxR) {
                minGreaterThanMax = true;
                break;
            }
        }
        if (!maxGreaterThanZero || minGreaterThanMax) {
        	// Disable the emission function if:
        	// All maxK are == 0 and/or negative
        	// At least one minK is greater than maxK
        	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
        } else {
        	// At least 1 maxK is greater than zero
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(ir.frontIlluminationOnly == phsdata::FrontIlluminationOnly::Yes));
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(ir.precomputed == phsdata::Precomputed::Yes));

        	// Write min radiance
        	{
				std::ostringstream emission;
				for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
	                if (bandIndex > 0) emission << " ";
	                // Pad with zero if not enough stored values. Ignore values beyond genDTO.numberOfSpectralBands.
	                // Negative values (if any) are set to zero
	                emission << (bandIndex < rrl.size() && rrl[bandIndex].minR > 0.0) ? rrl[bandIndex].minR : 0.0;
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", emission.str());
        	}
        	// Write max radiance
        	{
				std::ostringstream emission;
				for (unsigned int bandIndex = 0; bandIndex < genDTO.numberOfSpectralBands; bandIndex++) {
	                if (bandIndex > 0) emission << " ";
	                emission << (bandIndex < rrl.size() && rrl[bandIndex].maxR > 0.0) ? rrl[bandIndex].maxR : 0.0;
				}
				genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", emission.str());
        	}
        }
    } else if (ir.data->isTemperatureRangeProfile()) {
    	const phsdata::TemperatureRangeProfile* trp = ir.data->asTemperatureRangeProfile();

		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(ir.frontIlluminationOnly == phsdata::FrontIlluminationOnly::Yes));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(ir.precomputed == phsdata::Precomputed::Yes));

		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", getOrCreateScopeTexture(genDTO, trp->minFile, trp->layerNumber, trp->layerSize, trp->altitude));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", getOrCreateScopeTexture(genDTO, trp->maxFile, trp->layerNumber, trp->layerSize, trp->altitude));
    } else { // ir.data->isRadianceRangeProfileList()
    	// We are guaranteed that ir.data is a phsdata::RadianceRangeProfileListData
    	const phsdata::RadianceRangeProfileListData & rrpld = static_cast<const phsdata::RadianceRangeProfileListData &>(*(ir.data.get()));
    	const std::vector<phsdata::RadianceRangeProfile> & rrpl = rrpld.data;

        // Validate files
        for (const phsdata::RadianceRangeProfile & rrp : rrpl) {
            if (rrp.minFile.empty() || rrp.maxFile.empty()) {
            	cerr << "Warning: " << "RadianceRangeProfile: minFile or maxFile is empty for emission function " << ir.emissionName << endl;
        		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
        		return;
            }
        }

		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "ratio");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "frontIlluminationOnly", std::to_string(ir.frontIlluminationOnly == phsdata::FrontIlluminationOnly::Yes));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "precomputed", std::to_string(ir.precomputed == phsdata::Precomputed::Yes));

		{
			std::vector<std::string> minFiles;
			for (const phsdata::RadianceRangeProfile & rrp : rrpl)
				minFiles.push_back(rrp.minFile);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.min", getOrCreateScopeTexture(genDTO, minFiles, rrpld.layerNumber, rrpld.layerSize, rrpld.altitude));
		}
		{
			std::vector<std::string> maxFiles;
			for (const phsdata::RadianceRangeProfile & rrp : rrpl)
				maxFiles.push_back(rrp.maxFile);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission.max", getOrCreateScopeTexture(genDTO, maxFiles, rrpld.layerNumber, rrpld.layerSize, rrpld.altitude));
		}
	}
}

Matrix4<double> computeTransformationMatrix(const phsdata::Dimensions & dimensions,
											const phsdata::Positioning & positioning) {
	auto centerMat = Matrix4<double>::Translate(
		-dimensions.voxelSize.sizeX * dimensions.voxelNumber.numberX / 2,
		-dimensions.voxelSize.sizeY * dimensions.voxelNumber.numberY / 2,
		-dimensions.voxelSize.sizeZ * dimensions.voxelNumber.numberZ / 2);
	auto intrinsicMat = Matrix4<double>::RotateZ(positioning.rotation.intrinsicZ);
	auto nutationMat = Matrix4<double>::RotateY(positioning.rotation.nutationY);
	auto precessionMat = Matrix4<double>::RotateZ(positioning.rotation.precessionZ);
	auto positionMat = Matrix4<double>::Translate(
		dimensions.voxelSize.sizeX * dimensions.voxelNumber.numberX / 2 + positioning.lowerCorner.cornerX,
		dimensions.voxelSize.sizeY * dimensions.voxelNumber.numberY / 2 + positioning.lowerCorner.cornerY,
		dimensions.voxelSize.sizeZ * dimensions.voxelNumber.numberZ / 2 + positioning.lowerCorner.cornerZ);// Transformation order (right-to-left):
	// 1. centerMat: Center the voxel grid at the origin.
	// 2. intrinsicMat: Apply intrinsic rotation (Z-axis).
	// 3. nutationMat: Apply nutation (Y-axis).
	// 4. precessionMat: Apply precession (Z-axis).
	// 5. positionMat: Translate to final position (lowerCorner + half dimensions).
	return positionMat * precessionMat * nutationMat * intrinsicMat * centerMat;
}

namespace {
    // Helper: Convert border type to string
    std::string borderTypeToString(int bt) {
        switch (bt) {
            case 0: return "zeros";
            case 1: return "repeatlast";
            case 2: return "modulo";
            default:
                std::cerr << "Warning: invalid matrix border-management value: " << bt << ". Defaulting to \"zeros\"." << std::endl;
                return "zeros";
        }
    }
}

void createMatrixTexture(	PhaseGeneralParametersDTO & genDTO,
								const string & textureKeyRoot,
								const phsdata::Dimensions & dimensions,
								const phsdata::Positioning & positioning,
								const phsdata::BorderManagement & borderManagement) {
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "type", "matrix3d");

	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.x", borderTypeToString(borderManagement.borderX));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.y", borderTypeToString(borderManagement.borderY));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "border.z", borderTypeToString(borderManagement.borderZ));

	genDTO.luxCoreRenderSceneProperties.ajouterCle(
						textureKeyRoot + ".voxel.number",
						std::to_string(dimensions.voxelNumber.numberX) + " " +
						std::to_string(dimensions.voxelNumber.numberY) + " " +
						std::to_string(dimensions.voxelNumber.numberZ));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(
						textureKeyRoot + ".voxel.size",
						std::to_string(dimensions.voxelSize.sizeX) + " " +
						std::to_string(dimensions.voxelSize.sizeY) + " " +
						std::to_string(dimensions.voxelSize.sizeZ));

	genDTO.luxCoreRenderSceneProperties.ajouterCle(
						textureKeyRoot + ".transformation",
						computeTransformationMatrix(dimensions, positioning).toFlatString()
						);
}

std::string getOrCreateMatrixTexture(	PhaseGeneralParametersDTO & genDTO,
										const std::string & temperatureFile,
										const phsdata::Dimensions & dimensions,
										const phsdata::Positioning & positioning,
										const phsdata::BorderManagement & borderManagement) {
	std::unordered_map<std::string, std::string>::const_iterator it = genDTO._fileNameToTextureMap.find(temperatureFile);
	if (it == genDTO._fileNameToTextureMap.end()) {
		std::string textureName = "radianceMatrix_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
		std::string textureKeyRoot = "scene.textures." + textureName + ".";

		createMatrixTexture(genDTO, textureKeyRoot, dimensions, positioning, borderManagement);

		boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
		std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();

		writeRadianceFile(genDTO, temperatureFile, dimensions.voxelNumber, outputDir.string(), textureName + ".bin");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", outputTextureFile);
		genDTO._fileNameToTextureMap[temperatureFile] = outputTextureFile;
		return textureName;
	} else {
		return it->second;
	}
}

std::string getOrCreateMatrixTexture(	PhaseGeneralParametersDTO & genDTO,
										const vector<std::string> & radianceFiles,
										const phsdata::Dimensions & dimensions,
										const phsdata::Positioning & positioning,
										const phsdata::BorderManagement & borderManagement) {
	std::string combinedFileNames = "";
	for (const std::string & fileName: radianceFiles) {
		combinedFileNames += DartEnv::getFilePathForRead(INPUT_DIR, fileName) + "_";
	}
	std::unordered_map<std::string, std::string>::const_iterator it = genDTO._fileNameToTextureMap.find(combinedFileNames);
	if (it == genDTO._fileNameToTextureMap.end()) {
		std::string textureName = "radianceMatrix_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
		std::string textureKeyRoot = "scene.textures." + textureName + ".";

		createMatrixTexture(genDTO, textureKeyRoot, dimensions, positioning, borderManagement);

		boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
		std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();
		writeRadianceFile(genDTO, radianceFiles, dimensions.voxelNumber, outputDir.string(), textureName + ".bin");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", outputTextureFile);
		genDTO._fileNameToTextureMap[combinedFileNames] = outputTextureFile;
		return textureName;
	} else {
		return it->second;
	}
}

void writeEmissionMatrix(PhaseGeneralParametersDTO & genDTO,
						const phsdata::EmissionMatrix & em,
						const std::string & key,
						double precision,
						unsigned int maxIter) {
    std::string emissionKey = key + em.emissionName + "_" + std::to_string(em.indexEmission) + ".";
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "index", std::to_string(em.indexEmission));

	if (em.emissionUnit == phsdata::EmissionUnit::Temperature) {
        if (!em.fileData.temperatureFile) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
            return;
        }
    	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", getOrCreateMatrixTexture(genDTO, *em.fileData.temperatureFile, em.dimensions, em.positioning, em.borderManagement));
	} else {
        if (em.fileData.radianceFiles.empty()) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
            return;
        }
    	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", getOrCreateMatrixTexture(genDTO, em.fileData.radianceFiles, em.dimensions, em.positioning, em.borderManagement));
	}
}

std::string getOrCreatePerTriangleTexture(	PhaseGeneralParametersDTO & genDTO,
											const std::string & temperatureFile) {
	std::unordered_map<std::string, std::string>::const_iterator it = genDTO._fileNameToTextureMap.find(temperatureFile);
	if (it == genDTO._fileNameToTextureMap.end()) {
		std::string textureName = "radianceMatrix_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
		std::string textureKeyRoot = "scene.textures." + textureName + ".";

		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "type", "pertriangle");
		boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
        if (outputDir.empty()) {
            throw std::runtime_error("Output directory path is empty");
        }
		std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();
		writeRadianceFile(genDTO, temperatureFile, outputDir.string(), textureName + ".bin");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", outputTextureFile);

		genDTO._fileNameToTextureMap[temperatureFile] = outputTextureFile;
		return textureName;
	} else {
		return it->second;
	}
}

std::string getOrCreatePerTriangleTexture(	PhaseGeneralParametersDTO & genDTO,
										const vector<std::string> & radianceFiles) {
	std::string combinedFileNames = "";
	// WARNING: The order of files in radianceFiles must match the spectral band order. [File1, File2] != [File2, File1]!
	for (const std::string & fileName: radianceFiles) {
		combinedFileNames += DartEnv::getFilePathForRead(INPUT_DIR, fileName) + "_";
	}
	std::unordered_map<std::string, std::string>::const_iterator it = genDTO._fileNameToTextureMap.find(combinedFileNames);
	if (it == genDTO._fileNameToTextureMap.end()) {
		std::string textureName = "radianceMatrix_" + boost::lexical_cast<std::string>(genDTO._fileNameToTextureMap.size());
		std::string textureKeyRoot = "scene.textures." + textureName + ".";

		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "type", "pertriangle");
		boost::filesystem::path outputDir = boost::filesystem::path(DartEnv::getCurrentSimulationOutputPath()) / PhaseUtils::getOutputFolder();
        if (outputDir.empty()) {
            throw std::runtime_error("Output directory path is empty");
        }
		std::string outputTextureFile = (outputDir / (textureName + ".bin")).string();
		writeRadianceFile(genDTO, radianceFiles, outputDir.string(), textureName + ".bin");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + "file", outputTextureFile);
		genDTO._fileNameToTextureMap[combinedFileNames] = outputTextureFile;
		return textureName;
	} else {
		return it->second;
	}
}

void writeEmissionPerTriangle(PhaseGeneralParametersDTO & genDTO,
						const phsdata::EmissionPerTriangle & ept,
						const std::string & key,
						double precision,
						unsigned int maxIter) {
    std::string emissionKey = key + ept.emissionName + "_" + std::to_string(ept.indexEmission) + ".";
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "index", std::to_string(ept.indexEmission));

	if (ept.emissionUnit == phsdata::EmissionUnit::Temperature) {
        if (!ept.fileData.temperatureFile) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
            return;
        }
    	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", getOrCreatePerTriangleTexture(genDTO, *ept.fileData.temperatureFile));
	} else {
        if (ept.fileData.radianceFiles.empty()) {
            genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
            return;
        }
    	genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "constant");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "emission", getOrCreatePerTriangleTexture(genDTO, ept.fileData.radianceFiles));
	}
}

void writeNullEmissionMaterial(PhaseGeneralParametersDTO & genDTO,
							   const phsdata::EmissionBase & eb,
								const std::string & key) {
    std::string emissionKey = key + eb.emissionName + "_" + std::to_string(eb.indexEmission) + ".";
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "generator", "phase");
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "index", std::to_string(eb.indexEmission));
    genDTO.luxCoreRenderSceneProperties.ajouterCle(emissionKey + "type", "null");
}

void processEmissionMaterials(PhaseGeneralParametersDTO & genDTO) {
	string key = "scene.radiations.";

	// Add default, fall-back, temperature property
	string defaultEmissionName = "defaultEmission";
	genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".type", "null");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(key + defaultEmissionName + ".generator", "phase");
	if (genDTO.isThermalEmissionEnabled) {
		double precision = 0.001;
		unsigned int maxIter = 1000;
		for (const phsdata::DirectEmission & de : genDTO._emissionMaterials.directEmissions) {
			writeDirectEmission(genDTO, de, key, precision, maxIter);
		}
		for (const phsdata::IlluminationRatio & ir : genDTO._emissionMaterials.illuminationRatios) {
			writeIlluminationRatio(genDTO, ir, key, precision, maxIter);
		}
		for (const phsdata::EmissionMatrix & em : genDTO._emissionMaterials.emissionMatrices) {
			writeEmissionMatrix(genDTO, em, key, precision, maxIter);
		}
		for (const phsdata::EmissionPerTriangle & em : genDTO._emissionMaterials.emissionPerTriangles) {
			writeEmissionPerTriangle(genDTO, em, key, precision, maxIter);
		}
	} else {
		// Write Null Emission materials (No thermal emission)
		for (const phsdata::DirectEmission & de : genDTO._emissionMaterials.directEmissions) {
			writeNullEmissionMaterial(genDTO, de, key);
		}
		for (const phsdata::IlluminationRatio & ir : genDTO._emissionMaterials.illuminationRatios) {
			writeNullEmissionMaterial(genDTO, ir, key);
		}
		for (const phsdata::EmissionMatrix & em : genDTO._emissionMaterials.emissionMatrices) {
			writeNullEmissionMaterial(genDTO, em, key);
		}
		for (const phsdata::EmissionPerTriangle & em : genDTO._emissionMaterials.emissionPerTriangles) {
			writeNullEmissionMaterial(genDTO, em, key);
		}
	}
}
