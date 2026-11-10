/*
 * AirPhaseFunction.hpp
 *
 *  Created on: 1 juin 2012
 *      Author: lauretn
 */
#ifndef AIRPHASEFUNCTION_HPP_
#define AIRPHASEFUNCTION_HPP_
#include "AirPhaseFunctionFileLoader.hpp"
#include "AirHenleyGreenstein.hpp"
#include "AirRayleigh.hpp"
#include "libDartBase.h"
#include "../../../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "PhaseProperty.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

class AirPhaseFunction {
private:
	unsigned int _numberOfSpectralValues;
	unsigned int _index;
	std::string _parametersFilePath;
	std::string _name;
	std::string _database;
	shared_ptr<AirLaw> _airLaw;
	std::vector<double> _albedoPerBand;
	std::vector<double> _sigmaPerBand;

	// Multiplicative factors
	std::vector<double> _albedoMultiplicativeFactorPerBand;
	std::vector<double> _sigmaMultiplicativeFactorPerBand;
	std::vector<double> _parameter1MultiplicativeFactorPerBand; // A (Henley) A (Rayleigh)
	std::vector<double> _parameter2MultiplicativeFactorPerBand; // G1 (Henley) C (Rayleigh)
	std::vector<double> _parameter3MultiplicativeFactorPerBand; // G2 (Henley) No use (Rayleigh)

	bool _readingDone;
	bool _useMultiplicativeFactorForLut;

public:
	AirPhaseFunction() :
		_numberOfSpectralValues(0), _index(0), _parametersFilePath("None"), _name(""), _database(""), _readingDone(true), _useMultiplicativeFactorForLut(false) {}

	AirPhaseFunction(int numberOfSpectralValues) :
		_numberOfSpectralValues(numberOfSpectralValues), _index(0), _parametersFilePath("None"), _name(""), _database(""), _readingDone(true), _useMultiplicativeFactorForLut(false) {
		_albedoPerBand.resize(numberOfSpectralValues);
		_sigmaPerBand.resize(numberOfSpectralValues);
		_albedoMultiplicativeFactorPerBand.resize(numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
		_sigmaMultiplicativeFactorPerBand.resize(numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
		_parameter1MultiplicativeFactorPerBand.resize(numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
		_parameter2MultiplicativeFactorPerBand.resize(numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
		_parameter3MultiplicativeFactorPerBand.resize(numberOfSpectralValues, 1.); // Security resize: should already be at the correct one
	}

	void readFileData(	const std::vector<double> & lambmin,
						const std::vector<double> & lambmax);

	void computeScatteringFunction(	const std::vector<double> & lambmin,
									const std::vector<double> & lambmax,
									const std::vector<DirectionCartesienne> & listDirections,
									const unsigned int & nbSector);

	void writeData(	PhaseKeyMappingGenerator & pkmg,
					std::ofstream & outCoeffSpecProperties);

	void computeTrapezeInterpolation(	const double & lowerBound,
										const double & upperBound,
										const unsigned int & numberOfValues,
										const unsigned int & numberOfColumns,
										const std::vector<std::vector<double> > & dataVector,
										std::vector<std::vector<double> > & savedData);

	void setParametersFilePath(	const std::string & parametersFilePath,
								const std::string & parametersDatabase) {
		_parametersFilePath = parametersFilePath;
		_database = parametersDatabase;
		_readingDone = false;
	}
	void setName(const std::string & nameAPF) {
		_name = nameAPF;
	}
	void setDatabase(const std::string & nameAPF) {
		_database = nameAPF;
	}
	void setIndex(const unsigned int & indexAPF) {
		_index = indexAPF;
	}
	void setAirLaw(const AirLaw & airLaw) {
		_airLaw = airLaw.clone();
	}
	void setSigmaPerBand(const std::vector<double> & sigmaParBand) {
		_sigmaPerBand = sigmaParBand;
	}
	void setAlbedoPerBand(const std::vector<double> & albedoPerBand) {
		_albedoPerBand = albedoPerBand;
	}
	const vector<double> getalbedomf(){
		return _albedoMultiplicativeFactorPerBand;
	}
	double getSigmaForBand(const unsigned int & sbIndex) {
		return _sigmaPerBand[sbIndex];
	}
	double getAlbedoForBand(const unsigned int & sbIndex) {
		return _albedoPerBand[sbIndex];
	}
	double getSigmaMultiplicativeFactorForBand(const unsigned int & sbIndex) {
		return _sigmaMultiplicativeFactorPerBand[sbIndex];
	}
	double getAlbedoMultiplicativeFactorForBand(const unsigned int & sbIndex) {
		return _albedoMultiplicativeFactorPerBand[sbIndex];
	}
	const std::string & getParametersFilePath() {
		return _parametersFilePath;
	}
	const std::string & getName() {
		return _name;
	}
	const std::string & getDatabase() {
		return _database;
	}
	const unsigned int & getIndex
	() {
		return _index;
	}

	// Matrix per incident direction, per viewing direction per spectral band : SF[dirInc][dirView][SpectBand]
	const std::vector<std::vector<std::vector<double> > > & getScatteringFunction() {
		if (_airLaw) {
			return _airLaw->getScatteringFunction();
		} else {
			return AirLaw::getEmptyScatteringFunction();
		}
	}
	const std::vector<std::vector<std::vector<double> > > & getScatteringFunctionInDirectionToOutDirection() {
		if (_airLaw) {
			return _airLaw->getScatteringFunctionInDirectionToOutDirection();
		} else {
			return AirLaw::getEmptyScatteringFunction();
		}
	}

	void setSigmaMultiplicativeFactor(const double& valueParameter, const unsigned int &spectralBandNumber) {
		_sigmaMultiplicativeFactorPerBand[spectralBandNumber] = valueParameter;
	}
	void setAlbedoMultiplicativeFactor(const double& valueParameter, const unsigned int &spectralBandNumber) {
		_albedoMultiplicativeFactorPerBand[spectralBandNumber] = valueParameter;
	}

	void setParameter1MultiplicativeFactor(const double& valueParameter, const unsigned int &spectralBandNumber) {
		_parameter1MultiplicativeFactorPerBand[spectralBandNumber] = valueParameter;
	}

	void setParameter2MultiplicativeFactor(const double& valueParameter, const unsigned int &spectralBandNumber) {
		_parameter2MultiplicativeFactorPerBand[spectralBandNumber] = valueParameter;
	}

	void setParameter3MultiplicativeFactor(const double& valueParameter, const unsigned  int &spectralBandNumber) {
		_parameter3MultiplicativeFactorPerBand[spectralBandNumber] = valueParameter;
	}

	void setUseMultiplicativeFactorForLut(bool useMultiplicativeFactorForLut) {
		_useMultiplicativeFactorForLut = useMultiplicativeFactorForLut;
	}
	const shared_ptr<AirLaw> getAirLaw() const {
		return _airLaw;
	}
};

#endif /* AIRPHASEFUNCTION_HPP_ */
