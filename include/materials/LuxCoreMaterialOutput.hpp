/*
 * LuxCoreMaterialOutput.hpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_LUXCOREMATERIALOUTPUT_HPP_
#define INCLUDE_MATERIALS_LUXCOREMATERIALOUTPUT_HPP_

#include <iostream>
#include "PhaseProperty.hpp"

// LuxCoreMaterialOutput.h  (header-only, shared by all material writers)
namespace phase::luxcore {
// Number of extra simulated bands appended after the real spectral bands:
// 3 Stokes components (Q,U,V) when polarization is enabled, 2 photosystems (PSI,PSII)
// when fluorescence is enabled.
unsigned int nbExtraSimulatedBands() {
    unsigned int nb = 0;
    if (PhaseProperty::getPolarization())
        nb += 3;
    if (PhaseProperty::getFluorescence())
        nb += 2;
    return nb;
}

// Appends `bandValue(sbi)` for each spectral band, then repeats the same values for
// each extra simulated band (reflectance / transmittance are duplicated).
template<typename BandValue>
void appendPerBandDuplicated(std::ostringstream & os, unsigned int nbBands, const BandValue & v) {
    for (unsigned int sbi = 0; sbi < nbBands; ++sbi)
        os << " " << v(sbi);
    for (unsigned int repeat = 0; repeat < nbExtraSimulatedBands(); ++repeat)
        for (unsigned int sbi = 0; sbi < nbBands; ++sbi)
            os << " " << v(sbi);
}

// Appends `bandValue(sbi)` for each spectral band, then pads the extra simulated
// bands with zeros (emissivity: the extra bands don't emit).
template<typename BandValue>
void appendPerBandZeroPadded(std::ostringstream & os, unsigned int nbBands, const BandValue & v) {
    for (unsigned int sbi = 0; sbi < nbBands; ++sbi)
        os << " " << v(sbi);
    for (unsigned int repeat = 0; repeat < nbExtraSimulatedBands() * nbBands; ++repeat)
        os << " 0";
}

std::string writeLuxCoreDirectTransmittance(const vector<double> & directTransmittance) {
	static atomic<unsigned int> fileIndex = 0;
	std::string fileName = "transmittance_" + std::to_string(fileIndex++) + ".txt";
	createOutputDirectory(DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase());
	std::string absoluteFilePath = DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase() + pathSeparator + fileName;

	std::ofstream stream(absoluteFilePath.c_str(), ios::out);
	for (auto value : directTransmittance) {
		stream << value << std::endl;
	}
	stream.close();

	return DartEnv::getRelativePathToLibPhase() + pathSeparator + fileName;
}
}

#endif /* INCLUDE_MATERIALS_LUXCOREMATERIALOUTPUT_HPP_ */
