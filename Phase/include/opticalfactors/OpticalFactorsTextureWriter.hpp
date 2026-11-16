/*
 * OpticalFactorsTextureWriter.hpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_OPTICALFACTORS_OPTICALFACTORSTEXTUREWRITER_HPP_
#define INCLUDE_OPTICALFACTORS_OPTICALFACTORSTEXTUREWRITER_HPP_

#include "PhaseDTOs.hpp"

namespace phase {

class OpticalFactorsTextureWriter {
public:
    // Writes the interleaved .bin matrix and declares the LuxCore texture keys.
    // Returns false if no usable optical factor matrix exists or the file could not be written.
    bool writeTexture(PhaseGeneralParametersDTO & genDTO,
                      const std::string & textureName,
                      const phsdata::MultiplicativeFactorList & mfl,
                      const std::string & outputFolder) const;

private:
    bool writeBinaryFile(PhaseGeneralParametersDTO & genDTO,
                        const std::string & textureName,
                        const phsdata::MultiplicativeFactorList & mfl,
                        const std::string & outputFolder) const;
    bool fillFromOpticalFactorFile(std::vector<float> & flatMatrix, const phsdata::OpticalFactorMatrix & ofm,
                                  unsigned int nbBands, unsigned int bandOffset, bool replicate) const;
    const phsdata::OpticalFactorMatrix * referenceMatrix(const phsdata::MultiplicativeFactorList & mfl) const;
    unsigned int referenceNbVoxels(const phsdata::MultiplicativeFactorList & mfl) const;
    void writeVoxelKeys(PhaseGeneralParametersDTO & genDTO, const std::string & keyRoot,
                        const phsdata::OpticalFactorMatrix & ofm) const;
};

} // namespace phase

#endif /* INCLUDE_OPTICALFACTORS_OPTICALFACTORSTEXTUREWRITER_HPP_ */
