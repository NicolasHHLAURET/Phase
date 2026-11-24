/*
 * AirPhaseFunction.cpp
 *
 *  Created on: 19 févr. 2016
 *      Author: lauretn
 */

#include "../../../include/containers/air/AirPhaseFunction.hpp"

void AirPhaseFunction::readFileData(	const vector<double> & lambmin,
					const vector<double> & lambmax) {
	_numberOfSpectralValues = lambmin.size();
	if (!_readingDone) {
		if (_airLaw) {
			_airLaw.reset();
		}
		AirPhaseFunctionFileLoader fileData;
		string databaseFilePath = DartEnv::getDatabasePath(_database);
		if (databaseFilePath.compare("") != 0) {
			databaseFilePath += _database;
		}
		cout << "   -> Processing : " << _parametersFilePath << endl;
		if (fileData.loadFile(databaseFilePath, _parametersFilePath)) {
			vector<vector<double> > integratedValuesPerUserSpectralBand(_numberOfSpectralValues);
			vector<vector<double> > interpolationTransferObject(fileData.getNumberOfValues() + 2, vector<double>(fileData.getNumberOfColumns(), 0));

			_albedoPerBand.resize(_numberOfSpectralValues);
			_sigmaPerBand.resize(_numberOfSpectralValues);
			_albedoMultiplicativeFactorPerBand.resize(_numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
			_sigmaMultiplicativeFactorPerBand.resize(_numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
			_parameter1MultiplicativeFactorPerBand.resize(_numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
			_parameter2MultiplicativeFactorPerBand.resize(_numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
			_parameter3MultiplicativeFactorPerBand.resize(_numberOfSpectralValues, 1.); // Security resize: should already be at the correct one

			for (unsigned int spectralBandIndex = 0; spectralBandIndex < _numberOfSpectralValues; spectralBandIndex++) {
				integratedValuesPerUserSpectralBand[spectralBandIndex].resize(fileData.getNumberOfColumns());
				this->computeTrapezeInterpolation(lambmin[spectralBandIndex], lambmax[spectralBandIndex], fileData.getNumberOfValues(),
					fileData.getNumberOfColumns(), fileData.getValuesPerBand(), interpolationTransferObject);
				_sigmaPerBand[spectralBandIndex] = interpolationTransferObject[fileData.getNumberOfValues() + 1][1];
				_albedoPerBand[spectralBandIndex] = interpolationTransferObject[fileData.getNumberOfValues() + 1][2];
				for (unsigned int i = 0; i < interpolationTransferObject.back().size(); ++i) {
					integratedValuesPerUserSpectralBand[spectralBandIndex][i] = interpolationTransferObject.back()[i];
				}
			}
			if (fileData.getLaw().compare("Rayleigh") == 0) {
				// Instance of a Rayleigh Law
				_airLaw = shared_ptr<AirLaw>(new AirRayleigh());
				AirRayleigh & airRayleigh = static_cast<AirRayleigh &>(*_airLaw);
				for (unsigned int spectralBandIndex = 0; spectralBandIndex < integratedValuesPerUserSpectralBand.size(); spectralBandIndex++) {
					airRayleigh.addCoefficient(
						CoefficientRayleigh(
							integratedValuesPerUserSpectralBand[spectralBandIndex][3] * _parameter1MultiplicativeFactorPerBand[spectralBandIndex],
							integratedValuesPerUserSpectralBand[spectralBandIndex][4] * _parameter2MultiplicativeFactorPerBand[spectralBandIndex]));
				}
			} else if (fileData.getLaw().compare("HenyeyGreenstein") == 0) {
				// Instance of a HenleyGreenstein Law
				_airLaw = shared_ptr<AirLaw>(new AirHenleyGreenstein());
				AirHenleyGreenstein & airHenleyGreenstein = static_cast<AirHenleyGreenstein &>(*_airLaw);
				for (unsigned int spectralBandIndex = 0; spectralBandIndex < integratedValuesPerUserSpectralBand.size(); spectralBandIndex++) {
					airHenleyGreenstein.addCoefficient(
						CoefficientHenleyGreenstein(
							integratedValuesPerUserSpectralBand[spectralBandIndex][3] * _parameter1MultiplicativeFactorPerBand[spectralBandIndex],
							integratedValuesPerUserSpectralBand[spectralBandIndex][4] * _parameter2MultiplicativeFactorPerBand[spectralBandIndex],
							integratedValuesPerUserSpectralBand[spectralBandIndex][5] * _parameter3MultiplicativeFactorPerBand[spectralBandIndex]));
				}
			}
		} else {
			cerr << "An error occurred during the reading of " << databaseFilePath << endl;
		}
		_readingDone = true;
	}
}

void AirPhaseFunction::computeScatteringFunction(	const vector<double> & lambmin,
								const vector<double> & lambmax,
								const vector<DirectionCartesienne> & listDirections,
								const unsigned int & nbSector) {
	if (!_airLaw) {
		if (_readingDone) {
			cerr << "Air Phase Function: Error in object creation. No air law defined, or phase file provided." << endl;
			return;
		}
		this->readFileData(lambmin, lambmax);
	}
	_airLaw->computeScatteringFunction(listDirections, lambmin.size(), nbSector);
}

void AirPhaseFunction::writeData(	PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties) {
	if (_airLaw) {
		ostringstream filePathStream;
		filePathStream << DartEnv::getCurrentSimulationOutputPath() << DartEnv::getSeparator() << PhaseUtils::getOutputFolder()
				<< DartEnv::getSeparator() << _index << "." << _parametersFilePath << "_ScatteringFunction";
		string filePath = filePathStream.str();
		ofstream outputData(filePath.c_str(), ios::out | ios::binary);
		if (outputData) {
			const vector<vector<vector<double> > > & scatteringfunction = _airLaw->getScatteringFunction();
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < scatteringfunction.size(); ++spectralBandIndex) {
				for (unsigned int sector = 0; sector < scatteringfunction[spectralBandIndex].size(); ++sector) {
					for (unsigned int dirVis = 0; dirVis < scatteringfunction[spectralBandIndex][sector].size(); ++dirVis) {
						outputData.write((char *) &scatteringfunction[spectralBandIndex][sector][dirVis], sizeof(double));
					}
				}
			}
			outputData.close();
		}
		pkmg.setTypeFonctionDePhase(globalReferences::AIR_FP);

		for (unsigned int spectralBandIndex = 0; spectralBandIndex < _albedoPerBand.size(); ++spectralBandIndex) {
			pkmg.setBandeSpectrale(spectralBandIndex);
			pkmg.setNumeroFonctionDePhase(_index);
			outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
			outCoeffSpecProperties << pkmg.cleNomFichier() << _parametersFilePath << endl;
			outCoeffSpecProperties << pkmg.cleAlbedo() << (_albedoPerBand[spectralBandIndex] * _albedoMultiplicativeFactorPerBand[spectralBandIndex]) << endl;
			outCoeffSpecProperties << pkmg.cleSigma() << (_sigmaPerBand[spectralBandIndex] * _sigmaMultiplicativeFactorPerBand[spectralBandIndex]) << endl;
		}
	}
}

void AirPhaseFunction::computeTrapezeInterpolation(	const double & lowerBound,
									const double & upperBound,
									const unsigned int & numberOfValues,
									const unsigned int & numberOfColumns,
									const vector<vector<double> > & dataVector,
									vector<vector<double> > & savedData) {
	double factorLower;
	double factorUpper;
	double distanceFromLowerToUpper;
	double interpolatedValue;

	if (dataVector[numberOfValues - 1][0] <= lowerBound) {
		cout << "Maximum wavelength in table is inferior to the lower bound" << endl;
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] = dataVector[numberOfValues - 1][column];
		}
		return;
	}
	if (dataVector[0][0] >= upperBound) {
		cout << "Minimum wavelength in table is superior to the upper bound" << endl;
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] = dataVector[0][column];
		}
		return;
	}
	if (numberOfValues == 2) {
		double meanWavelength = 0.5 * (upperBound + lowerBound);
		distanceFromLowerToUpper = dataVector[1][0] - dataVector[0][0];
		factorLower = (dataVector[1][0] - meanWavelength) / distanceFromLowerToUpper;
		factorUpper = (meanWavelength - dataVector[0][0]) / distanceFromLowerToUpper;
		for (unsigned int column = 0; column < numberOfColumns; column++) {
			interpolatedValue = factorLower * dataVector[0][column] + factorUpper * dataVector[1][column];
			savedData[numberOfValues + 1][column] = interpolatedValue;
		}
		return;
	}
	unsigned int tableBegining = 0;
	while (dataVector[tableBegining + 1][0] <= lowerBound) {
		tableBegining++;
	}
	unsigned int tableEnding = tableBegining;
	while (tableEnding < numberOfValues && dataVector[tableEnding][0] <= upperBound) {
		for (unsigned int column = 0; column < numberOfColumns; column++) {
			savedData[tableEnding - tableBegining][column] = dataVector[tableEnding][column];
		}
		tableEnding++;
	}
    if(tableEnding >= numberOfValues) tableEnding = numberOfValues - 1;
	if (tableBegining == tableEnding) {
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] = dataVector[tableEnding][column];
		}
		return;
	}
	if (tableEnding - tableBegining == 1) {
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] = savedData[0][column];
		}
		return;
	}
	unsigned int lastSavedValue = tableEnding - tableBegining;
	for (unsigned int column = 0; column < numberOfColumns; column++) {
		savedData[lastSavedValue][column] = dataVector[tableEnding][column];
	}

	// Interpolation lowerBound upperBound
	distanceFromLowerToUpper = savedData[1][0] - savedData[0][0];
	factorLower = (savedData[1][0] - lowerBound) / distanceFromLowerToUpper;
	factorUpper = (lowerBound - savedData[0][0]) / distanceFromLowerToUpper;
	for (unsigned int column = 0; column < numberOfColumns; column++) {
		interpolatedValue = factorLower * savedData[0][column] + factorUpper * savedData[1][column];
		savedData[0][column] = interpolatedValue;
	}
	distanceFromLowerToUpper = savedData[lastSavedValue][0] - savedData[lastSavedValue - 1][0];
	factorLower = (savedData[lastSavedValue][0] - upperBound) / distanceFromLowerToUpper;
	factorUpper = (upperBound - savedData[lastSavedValue - 1][0]) / distanceFromLowerToUpper;
	for (unsigned int column = 0; column < numberOfColumns; column++) {
		interpolatedValue = factorLower * savedData[lastSavedValue - 1][column] + factorUpper * savedData[lastSavedValue][column];
		savedData[lastSavedValue][column] = interpolatedValue;
	}
	//compute mean value
	for (unsigned int column = 1; column < numberOfColumns; column++) {
		savedData[numberOfValues + 1][column] = 0;
	}
	for (unsigned int value = 0; value < lastSavedValue; value++) {
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] += 0.5 * (savedData[value][column] + savedData[value + 1][column])
					* (savedData[value + 1][0] - savedData[value][0]);
		}
	}
	if (upperBound > lowerBound) {
		for (unsigned int column = 1; column < numberOfColumns; column++) {
			savedData[numberOfValues + 1][column] /= (upperBound - lowerBound);
		}
	}
}
