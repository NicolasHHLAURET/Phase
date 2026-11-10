/*
 * FluorescenceMatrices.cpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#include <iostream>

// Generation an unique id for databases
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "PhaseProperty.hpp"
#include "fluorescence/FluorescenceMatrices.hpp"

namespace phase {
void FluorescenceService::computeMatrices(	FluorescenceResults & fluorescenceResults,
											InterpolatedMatrices & interpolatedMatrices,
											int typeOfReflectance,
											unsigned int modelIndex,
											const phsdata::ProspectExternParameters & prospectParams,
											const phsdata::FluorescenceExternParameters & fluorescence,
											const PhaseGeneralParametersDTO & genDTO,
											unsigned int face) const {
	std::cout << " - Computation of fluorescence conversion matrices" << std::endl;
	const std::string fileName = "fluorescence_" + std::to_string(typeOfReflectance) + "_" + std::to_string(modelIndex) + "_" + std::to_string(face) + ".txt";

	createOutputDirectory(DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase());
	const std::string absoluteMatricePath = DartEnv::getCurrentSimulationPath() + pathSeparator + DartEnv::getRelativePathToLibPhase() + pathSeparator + fileName;

	std::ofstream ofstreamFluorescenceMatrices(absoluteMatricePath.c_str(), std::fstream::out);
	if (!ofstreamFluorescenceMatrices) {
		disable(fluorescenceResults, typeOfReflectance, modelIndex);
		return;
	}

	/* Compute the Cm used by the script, if it's the new one, need to be set to 0 if Cbc+cp is used, for the old one it's the sum of Cbc+Cp if the new cm definition is used */
	const double scriptCm = fluorescence.useCombinedYields ? (prospectParams.useCm ? 0 : prospectParams.cm) : (prospectParams.useCm ? prospectParams.cbc + prospectParams.cp : prospectParams.cm);

	TmpDirectoryGuard tmpDirGuard;
	tmpDirGuard.path = DartEnv::getCurrentSimulationOutputPath() + "fluspectTMP" + boost::lexical_cast<std::string>(boost::uuids::random_generator()()) + pathSeparator;
	createOutputDirectory(tmpDirGuard.path);

	std::ostringstream commandLineRoot;
	commandLineRoot << DartEnv::getPythonExecutable() << " ";
	commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "Fluspect" << pathSeparator << "src" << pathSeparator
			<< (fluorescence.useCombinedYields ? "Fluspect_B_CX_PRO.py" : "Fluspect_B_CX_P6.py") << " ";
	commandLineRoot << DartEnv::getFilePath(INPUT_DIR, prospectParams.inputProspectFile) << " ";
	commandLineRoot << tmpDirGuard.path << " ";
	commandLineRoot << "-fluspect" << " ";
	commandLineRoot << prospectParams.cab << " " << prospectParams.car << " " << prospectParams.cBrown << " " << prospectParams.cw << " " << scriptCm << " " << prospectParams.n << " ";

	// Suffix appended after the yields: combined mode ignores PSII; separated mode passes both.
	const std::string yieldArgsSuffix =
			fluorescence.useCombinedYields ? (boost::lexical_cast<std::string>(prospectParams.v2z) + " " + boost::lexical_cast<std::string>(prospectParams.anthocyanin) + " "
					+ boost::lexical_cast<std::string>(prospectParams.cbc) + " " + boost::lexical_cast<std::string>(prospectParams.cp)) : (boost::lexical_cast<std::string>(prospectParams.v2z) + " "
					+ boost::lexical_cast<std::string>(prospectParams.anthocyanin));

	const unsigned int wlEMin = 400, wlEMax = 750, wlFMin = 640, wlFMax = 850;
	FluspectMatrices fluspectMatrices(wlEMax - wlEMin + 1, wlFMax - wlFMin + 1);

	// Select the yield pairs to process: the global combinations, or the single forced one.
	std::vector<std::pair<double, double>> yieldPairs;
	bool forceYields = true;
	if (!fluorescence.forceYields && !PhaseProperty::getFluorescenceYields().empty()) {
		forceYields = false;
		yieldPairs = PhaseProperty::getFluorescenceYields();
		std::cout << "   -> Nb of yields combinations: " << yieldPairs.size() << std::endl;
	} else {
		// Forced mode. In combined mode the pair's second value is only recorded in the
		// results (unused by fluspect); in separated mode it is passed on the command line.
		yieldPairs.push_back(std::make_pair(fluorescence.yieldPSI, fluorescence.yieldPSII));
	}

	for (const auto & pairPSI_PSII : yieldPairs) {
		std::ostringstream commandLine;
		commandLine << commandLineRoot.str() << pairPSI_PSII.first;
		if (fluorescence.useCombinedYields)
			commandLine << " " << pairPSI_PSII.second;
		commandLine << " " << yieldArgsSuffix;

		boost::optional<std::string> fluspectOutputFile = runFluspect(commandLine, tmpDirGuard.path);
		if (!fluspectOutputFile) {
			disable(fluorescenceResults, typeOfReflectance, modelIndex);
			return;
		}

		interpolateAndWrite(
			ofstreamFluorescenceMatrices,
			*fluspectOutputFile,
			fluspectMatrices,
			interpolatedMatrices,
			genDTO,
			wlEMin,
			wlEMax,
			wlFMin,
			wlFMax,
			fluorescence.useCombinedYields);

		fluorescenceResults.yieldPSI.push_back(pairPSI_PSII.first);
		fluorescenceResults.yieldPSII.push_back(pairPSI_PSII.second);
	}

	fluorescenceResults.forceYields = forceYields;
	fluorescenceResults.fileName = fileName;
	fluorescenceResults.isFluorescent = true;
	fluorescenceResults.useCombinedYields = fluorescence.useCombinedYields;
	fluorescenceResults.useBioCLimaticProfileEta = false;
}

void FluorescenceService::interpolateAndWrite(	std::ostream & ofstreamFluorescenceMatrices,
												const std::string & fluspectOutputFile,
												FluspectMatrices & fluspectMatrices,
												InterpolatedMatrices & interpolatedMatrices,
												const PhaseGeneralParametersDTO & genDTO,
												unsigned int wlEMin,
												unsigned int wlEMax,
												unsigned int wlFMin,
												unsigned int wlFMax,
												bool useCombinedYields) const {
	std::ifstream stream(fluspectOutputFile.c_str(), std::ifstream::in);
	if (!stream) {
		std::cerr << "Error reading the fluspect output file from " << fluspectOutputFile << std::endl;
		return;
	}

	const unsigned int nbBands = genDTO.numberOfSpectralBands;
	const unsigned int nE = fluspectMatrices.nE, nF = fluspectMatrices.nF;

	// Read the four matrices from the stream, applying the energy unit conversion in combined mode.
	// In combined mode only MBI/MFI are present in the file; MBII/MFII stay 0.
	for (unsigned int j = 0; j < nF; ++j) {
		for (unsigned int i = 0; i < nE; ++i) {
			fluspectMatrices.at(i, j, 0) = wlFMin + j;
		}
	}
	for (unsigned int k = 1; k <= 4; ++k) {
		if (useCombinedYields && (k == 2 || k == 4))
			continue;
		for (unsigned int j = 0; j < nF; ++j) {
			for (unsigned int i = 0; i < nE; ++i) {
				stream >> fluspectMatrices.at(i, j, k);
				if (useCombinedYields)
					fluspectMatrices.at(i, j, k) *= (double) (wlEMin + i) / (double) (wlFMin + j);
			}
		}
	}
	const bool streamOk = static_cast<bool>(stream);
	stream.close();
	if (!streamOk) {
		std::cerr << "Warning: fluspect output file " << fluspectOutputFile << " ended prematurely; missing values remain 0." << std::endl;
	}

	std::vector<std::vector<double>> interpolationInput(nF, std::vector<double>(5));
	std::vector<std::vector<double>> interpolationOutput(nE, std::vector<double>(1 + nbBands * 4));
	std::vector<std::vector<double>> interpolationResults;

	// Pass 1: interpolate on the fluorescence (emission) wavelength axis
	for (unsigned int i = 0; i < nE; ++i) {
		for (unsigned int j = 0; j < nF; ++j) {
			interpolationInput[j][0] = (wlFMin + j) / 1000.;
			interpolationInput[j][1] = fluspectMatrices.at(i, j, 1);
			interpolationInput[j][2] = fluspectMatrices.at(i, j, 2);
			interpolationInput[j][3] = fluspectMatrices.at(i, j, 3);
			interpolationInput[j][4] = fluspectMatrices.at(i, j, 4);
		}
		interpolationOutput[i][0] = (wlEMin + i) / 1000.;

		for (unsigned int sbi2 = 0; sbi2 < nbBands; sbi2++) {
			interpolationResults.assign(nF + 2, std::vector<double>(5, 0.));
			Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(sbi2), PhaseProperty::getUpperBandWavelength(sbi2), nF, 5, interpolationInput, interpolationResults);
			for (unsigned int k = 1; k <= 4; ++k)
				interpolationOutput[i][sbi2 * 4 + k] = interpolationResults[nF + 1][k];
		}
	}

	// Pass 2: interpolate on the excitation wavelength axis
	for (unsigned int sbi1 = 0; sbi1 < nbBands; sbi1++) {
		const bool excitationOutOfRange = PhaseProperty::getLowerBandWavelength(sbi1) * 1000. > wlEMax || PhaseProperty::getUpperBandWavelength(sbi1) * 1000. < wlEMin;
		if (excitationOutOfRange)
			continue;  // nothing in this band generates fluorescence; matrices remain 0

		interpolationResults.assign(nE + 2, std::vector<double>(1 + nbBands * 4, 0.));
		Interpolation_Trapeze(PhaseProperty::getLowerBandWavelength(sbi1), PhaseProperty::getUpperBandWavelength(sbi1), nE, 1 + nbBands * 4, interpolationOutput, interpolationResults);

		const double excitationWidthUm = (PhaseProperty::getUpperBandWavelength(sbi1) - PhaseProperty::getLowerBandWavelength(sbi1)) * 1000;
		for (unsigned int sbi2 = 0; sbi2 < nbBands; sbi2++) {
			const bool emissionOutOfRange = PhaseProperty::getLowerBandWavelength(sbi2) * 1000. > wlFMax || PhaseProperty::getUpperBandWavelength(sbi2) * 1000. < wlFMin;
			if (emissionOutOfRange)
				continue;  // this band emits nothing; matrix remains 0

			interpolatedMatrices.at(sbi1, sbi2, 0) = interpolationResults[nE + 1][sbi2 * 4 + 1] * excitationWidthUm;
			interpolatedMatrices.at(sbi1, sbi2, 2) = interpolationResults[nE + 1][sbi2 * 4 + 3] * excitationWidthUm;
			if (!useCombinedYields) {
				interpolatedMatrices.at(sbi1, sbi2, 1) = interpolationResults[nE + 1][sbi2 * 4 + 2] * excitationWidthUm;
				interpolatedMatrices.at(sbi1, sbi2, 3) = interpolationResults[nE + 1][sbi2 * 4 + 4] * excitationWidthUm;
			}
		}
	}

	// Write the four matrices unconditionally: in combined mode MBII/MFII are all zeroes.
	// This fixed 4-matrix format is simpler for the consumer than a variable one.
	ofstreamFluorescenceMatrices << std::setprecision(15);
	for (unsigned int k = 0; k < 4; ++k) {
		for (unsigned int sbi1 = 0; sbi1 < nbBands; sbi1++) {
			for (unsigned int sbi2 = 0; sbi2 < nbBands; sbi2++)
				ofstreamFluorescenceMatrices << interpolatedMatrices.at(sbi1, sbi2, k) << " ";
			ofstreamFluorescenceMatrices << std::endl;
		}
		ofstreamFluorescenceMatrices << std::endl;
	}
}

void FluorescenceService::disable(	FluorescenceResults & fluorescenceResults,
									int typeOfReflectance,
									unsigned int modelIndex) const {
	std::cerr << " > fluorescence is disabled for material " << modelIndex << " of type " << typeOfReflectance << std::endl;
	fluorescenceResults.yieldPSI.clear();
	fluorescenceResults.yieldPSII.clear();
	fluorescenceResults.isFluorescent = false;
}

// Runs a fluspect command line and checks that the output file was produced.
// Returns the output file path on success, or an empty optional on failure.
boost::optional<std::string> FluorescenceService::runFluspect(	const std::ostringstream & commandLine,
																const std::string & fluspectTmpDirectory) const {
    std::cout << "    Run python cmd: " << commandLine.str() << std::endl;
    if (std::system(commandLine.str().c_str()) != 0) {
        std::cerr << commandLine.str() << " failed." << std::endl;
        return boost::none;
    }
    const std::string fluspectOutputFile = fluspectTmpDirectory + "Leaf_fluspect.txt";
    if (!boost::filesystem::exists(fluspectOutputFile)) {
        std::cerr << "Couldn't retrieve the fluspect output file." << std::endl;
        return boost::none;
    }
    return fluspectOutputFile;
}

void FluorescenceService::writeSimulationProperties(	const FluorescenceResults & fluorescenceResults,
												PhaseKeyMappingGenerator & pkmg,
												ofstream & outCoeffSpecProperties) const {
	if (fluorescenceResults.isFluorescent) {
		outCoeffSpecProperties << pkmg.cleUseFluorescence() << "1" << endl;
		outCoeffSpecProperties << pkmg.cleFluorescenceFileName() << fluorescenceResults.fileName << endl;
		outCoeffSpecProperties << pkmg.cleForceYields() << fluorescenceResults.forceYields << endl;
		outCoeffSpecProperties << pkmg.cleUseCombinedYields() << fluorescenceResults.useCombinedYields << endl;
		outCoeffSpecProperties << pkmg.cleFluorescenceNbCases() << fluorescenceResults.yieldPSI.size() << endl;
		for (unsigned int i = 0; i < fluorescenceResults.yieldPSI.size(); ++i) {
			outCoeffSpecProperties << pkmg.cleFluorescenceYieldPSI(i) << fluorescenceResults.yieldPSI[i] << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceYieldPSII(i) << fluorescenceResults.yieldPSII[i] << endl;
		}

		outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticUsage() << fluorescenceResults.useBioCLimaticProfileEta << endl;
		if (fluorescenceResults.useBioCLimaticProfileEta) {
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticFile() << fluorescenceResults.bioClimaticFileRelativePath << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticLinkedPropertyType() << fluorescenceResults.linkedOpticalPropertyType << endl;
			outCoeffSpecProperties << pkmg.cleFluorescenceBioClimaticLinkedPropertyIndex() << fluorescenceResults.linkedOpticalPropertyIndex << endl;
		}
	} else {
		outCoeffSpecProperties << pkmg.cleUseFluorescence() << "0" << endl;
	}
}

FluorescenceService::TmpDirectoryGuard::~TmpDirectoryGuard() {
    if (armed) {
        boost::system::error_code ec;
        boost::filesystem::remove_all(boost::filesystem::path(path), ec);
        // Best-effort cleanup; ignore errors (ec) rather than throwing from a destructor.
    }
}
}
