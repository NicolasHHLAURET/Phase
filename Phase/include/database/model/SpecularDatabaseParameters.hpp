/*
 * SpecularDatabaseParameters.hpp
 *
 *  Created on: 7 oct. 2015
 *      Author: guilleuxj
 */

#ifndef SPECULARCOLUMNCHOICE_HPP_
#define SPECULARCOLUMNCHOICE_HPP_

using namespace std;

// Becarefull, this enum may to be consistant with coeff_diff.class_catalog.xml
enum RadianceDistribution {
	LIKE_GAUSSIAN = 0, PARABOLIC = 1, RADIANCE_CONSTANT = 2
};

class SpecularDatabaseParameters: public PhaseDatabaseParameters {
private:
	bool _useRefraction;
	RadianceDistribution _radianceDistribution;

public:
	SpecularDatabaseParameters(	const string & databaseName,
								const string & databasePath,
								const string & modelName,
								const unsigned int numberOfBands) :
			PhaseDatabaseParameters(databaseName, databasePath, modelName, numberOfBands), _useRefraction(false), _radianceDistribution(LIKE_GAUSSIAN) {
		_databaseColumChoices.push_back("refraction_index_down");
		_databaseColumChoices.push_back("intensity_factor_up_to_down");
		_databaseColumChoices.push_back("angular_divergence_up_to_down");
		_databaseColumChoices.push_back("k_power_up_to_down");
		_databaseColumChoices.push_back("refraction_index_up");
		_databaseColumChoices.push_back("intensity_factor_down_to_up");
		_databaseColumChoices.push_back("angular_divergence_down_to_up");
		_databaseColumChoices.push_back("k_power_down_to_up");
		resizeParametersVector();
	}

	const PropertyDatabase getDatabaseType() {
		return SPECULAR_DATABASE;
	}

	bool isUseRefraction() const {
		return _useRefraction;
	}

	void setUseRefraction(const bool & useRefraction) {
		_useRefraction = useRefraction;
	}

	RadianceDistribution getRadianceDistribution() const {
		return _radianceDistribution;
	}

	void setRadianceDistribution(RadianceDistribution radianceDistribution) {
		_radianceDistribution = radianceDistribution;
	}

	void setRadianceDistribution(int radianceDistribution) {
		switch (radianceDistribution) {
			case 0:
				_radianceDistribution = LIKE_GAUSSIAN;
				break;
			case 1:
				_radianceDistribution = PARABOLIC;
				break;
			case 2:
				_radianceDistribution = RADIANCE_CONSTANT;
				break;
			default:
				_radianceDistribution = LIKE_GAUSSIAN;
				break;
		}
	}

	const double & getRefractionIndexDownForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][0];
	}

	const double & getIntensityFactorUpToDownForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][1];
	}

	const double & getAngularDivergenceUpToDownForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][2];
	}

	const double & getKPowerUpToDownForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][3];
	}

	const double & getRefractionIndexUpForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][4];
	}

	const double & getIntensityFactorDownToUpForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][5];
	}

	const double & getAngularDivergenceDownToUpForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][6];
	}

	const double & getKPowerDownToUpForBand(const unsigned int & bandNumber) {
		return _parametersValues[bandNumber][7];
	}

};

#endif /* SPECULARCOLUMNCHOICE_HPP_ */
