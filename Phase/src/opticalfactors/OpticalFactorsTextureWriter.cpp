/*
 * OpticalFactorsTextureWriter.cpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#include "libDartEnv.h"
#include "opticalfactors/OpticalFactorsTextureWriter.hpp"

namespace phase {
bool OpticalFactorsTextureWriter::writeTexture(const PhaseGeneralParametersDTO & genDTO,
								const std::string & textureName,
								const phsdata::MultiplicativeFactorList & mfl,
								const std::string & outputFolder) const {
	if (!writeBinaryFile(genDTO, textureName, mfl, outputFolder))
		return false;

	const std::string textureKeyRoot = "scene.textures." + textureName;
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "matrix3d");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", textureName + ".bin");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".border.z", "repeatlast");

	const phsdata::OpticalFactorMatrix * referenceOFM = referenceMatrix(mfl);
	if (referenceOFM != nullptr)  // always true here since the file write succeeded
		writeVoxelKeys(genDTO, textureKeyRoot, *referenceOFM);

	return true;
}

bool OpticalFactorsTextureWriter::writeBinaryFile(const PhaseGeneralParametersDTO & genDTO,
							 const std::string & textureName,
							 const phsdata::MultiplicativeFactorList & mfl,
							 const std::string & outputFolder) const {
	const unsigned int nbBands = genDTO.numberOfSpectralBands;
	const unsigned int nbVoxels = referenceNbVoxels(mfl);   // add next to getReferenceOpticalFactorMatrix
	if (nbVoxels == 0)
		return false;

	// Layout: voxel-major, bands interleaved. Defaults to 1. (neutral factor).
	std::vector<float> interleavedFlatMatrix(nbVoxels * nbBands, 1.f);

	if (mfl.useSameOpticalFactorMatrixForAllBands) {
		if (!fillFromOpticalFactorFile(interleavedFlatMatrix, *mfl.bandCommonOpticalFactors, nbBands, 0, true))
			return false;
	} else {
		// The current XML organization mistakenly allows the user to define different voxelNumber and
		// cellDimensions per spectral band. This is unintended; the reference (first) OFM's dimensions win.
		// TODO: Reorganize the XML to make voxelNumber and cellDimensions unique and common to all bands
		for (unsigned int bandIndex = 0; bandIndex < nbBands && bandIndex < mfl.factors.size(); bandIndex++) {
			const phsdata::MultiplicativeFactor & currentMF = *(mfl.factors[bandIndex]);
			if (currentMF.useOpticalFactorMatrix && currentMF.opticalFactorMatrix.is_initialized())
				if (!fillFromOpticalFactorFile(interleavedFlatMatrix, *currentMF.opticalFactorMatrix, nbBands, bandIndex, false))
					return false;
		}
	}

	createDirectory(outputFolder);
	boost::filesystem::path outputPath(outputFolder);
	std::ofstream outflux((outputPath / (textureName + ".bin")).string().c_str(), std::ios::binary | std::ios::out);
	outflux.write((const char *)interleavedFlatMatrix.data(), sizeof(float) * interleavedFlatMatrix.size());
	outflux.close();
	return true;
}

// Reads one OFM's file into interleavedFlatMatrix (layout: voxel-major, bands interleaved).
// bandMask: for the common matrix, all bands get the same value; otherwise only bandOffset's slot is filled.
bool OpticalFactorsTextureWriter::fillFromOpticalFactorFile(std::vector<float> & interleavedFlatMatrix,
							   const phsdata::OpticalFactorMatrix & ofm,
							   unsigned int numberOfSpectralBands,
							   unsigned int bandOffset,
							   bool replicateAcrossBands) const {
	const std::string inputFilePath = DartEnv::getFilePathForRead(INPUT_DIR, ofm.opticalFactorMatrixFile);
	std::ifstream influx(inputFilePath.c_str(), std::ios::in);
	if (influx.fail()) {
		std::cerr << "Warning: Failed to open optical factor file: " << inputFilePath << std::endl;
		return false;
	}

	float value;
	unsigned int voxelIndex = 0;
	const unsigned int nbVoxels = static_cast<unsigned int>(interleavedFlatMatrix.size()) / numberOfSpectralBands;
	// Ignore values from the file outside the matrix range.
	while (voxelIndex < nbVoxels && influx >> value) {
		if (replicateAcrossBands) {
			for (unsigned int bandIndex = 0; bandIndex < numberOfSpectralBands; bandIndex++)
				interleavedFlatMatrix[voxelIndex * numberOfSpectralBands + bandIndex] = value;
		} else {
			interleavedFlatMatrix[voxelIndex * numberOfSpectralBands + bandOffset] = value;
		}
		++voxelIndex;
	}
	return true;
}

void OpticalFactorsTextureWriter::writeVoxelKeys(	const PhaseGeneralParametersDTO & genDTO,
													const std::string & textureKeyRoot,
													const phsdata::OpticalFactorMatrix & ofm) const {
	const phsdata::VoxelNumber & vn = ofm.voxelNumber;
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".voxel.number", std::to_string(vn.numberX) + " " + std::to_string(vn.numberY) + " " + std::to_string(vn.numberZ));

	double cellXY, cellZ;
	if (ofm.useDifferentCellDimensions && ofm.cellDimensions.is_initialized()) {
		cellXY = ofm.cellDimensions->cellXY;
		cellZ = ofm.cellDimensions->cellZ;
	} else {
		cellXY = genDTO.globalCellDimensions.cellXY;
		cellZ = genDTO.globalCellDimensions.cellZ;
	}
	genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".voxel.size", std::to_string(cellXY) + " " + std::to_string(cellXY) + " " + std::to_string(cellZ));
}

const phsdata::OpticalFactorMatrix * OpticalFactorsTextureWriter::referenceMatrix(const phsdata::MultiplicativeFactorList & mfl) const {
    if (mfl.useSameOpticalFactorMatrixForAllBands)
        return mfl.bandCommonOpticalFactors.is_initialized() ? mfl.bandCommonOpticalFactors.get_ptr() : nullptr;
    for (const auto & factor : mfl.factors) {
        if (factor->useOpticalFactorMatrix && factor->opticalFactorMatrix.is_initialized())
            return factor->opticalFactorMatrix.get_ptr();
    }
    return nullptr;
}

// Returns the number of voxels declared by the reference OFM, or 0 if there is none.
unsigned int OpticalFactorsTextureWriter::referenceNbVoxels(const phsdata::MultiplicativeFactorList & mfl) const {
    const phsdata::OpticalFactorMatrix * ofm = referenceMatrix(mfl);
    if (ofm == nullptr)
        return 0;
    return ofm->voxelNumber.numberX * ofm->voxelNumber.numberY * ofm->voxelNumber.numberZ;
}
}
