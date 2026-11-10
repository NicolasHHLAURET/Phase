/*
 * TemperatureFunction.hpp
 *
 *  Created on: 21 août 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_TEMPERATUREFUNCTION_HPP_
#define INCLUDE_CONTAINERS_TEMPERATUREFUNCTION_HPP_

#include <string>

class TemperatureFunction {
private:
	double meanTemperature;
	double deltaTemperature;
	std::string idTemperature;

	bool overrideTemperatureProfile;
	unsigned int useOpticalFactorMatrix;
	bool singleTemperatureSurface;
	bool usePrecomputedIPARs;

	double minIPAR;
	double maxIPAR;

	bool duplicateFirstOpticalMatrixLayer;
	std::string opticalFactorMatrixFile;

	double sunlitTemperature, shadedTemperature;
	std::string sunlitScopeFile, shadedScopeFile;

public:
	double getDeltaTemperature() const {
		return deltaTemperature;
	}
	void setDeltaTemperature(double deltaTemperature) {
		this->deltaTemperature = deltaTemperature;
	}
	
	bool isDuplicateFirstOpticalMatrixLayer() const {
		return duplicateFirstOpticalMatrixLayer;
	}
	void setDuplicateFirstOpticalMatrixLayer(bool duplicateFirstOpticalMatrixLayer) {
		this->duplicateFirstOpticalMatrixLayer = duplicateFirstOpticalMatrixLayer;
	}
	
	const std::string& getIdTemperature() const {
		return idTemperature;
	}
	void setIdTemperature(const std::string& idTemperature) {
		this->idTemperature = idTemperature;
	}
	
	double getMaxIpar() const {
		return maxIPAR;
	}
	void setMaxIpar(double maxIpar) {
		maxIPAR = maxIpar;
	}
	
	double getMeanTemperature() const {
		return meanTemperature;
	}
	void setMeanTemprature(double meanTemperature) {
		this->meanTemperature = meanTemperature;
	}
	
	double getMinIpar() const {
		return minIPAR;
	}
	void setMinIpar(double minIpar) {
		minIPAR = minIpar;
	}
	
	const std::string& getOpticalFactorMatrixFile() const {
		return opticalFactorMatrixFile;
	}
	void setOpticalFactorMatrixFile(const std::string& opticalFactorMatrixFile) {
		this->opticalFactorMatrixFile = opticalFactorMatrixFile;
	}
	
	bool isOverrideTemperatureProfile() const {
		return overrideTemperatureProfile;
	}
	void setOverrideTemperatureProfile(bool overrideTemperatureProfile) {
		this->overrideTemperatureProfile = overrideTemperatureProfile;
	}
	
	bool isSingleTemperatureSurface() const {
		return singleTemperatureSurface;
	}
	void setSingleTemperatureSurface(bool singleTemperatureSurface) {
		this->singleTemperatureSurface = singleTemperatureSurface;
	}
	
	bool isUsingOpticalFactorMatrix() const {
		return useOpticalFactorMatrix == 1;
	}
	bool isUsingSunlitShadedTemperatures() const {
		return useOpticalFactorMatrix == 2;
	}
	bool isUsingScopeFiles() const {
		return useOpticalFactorMatrix == 3;
	}
	void setUseOpticalFactorMatrix(unsigned int useOpticalFactorMatrix) {
		this->useOpticalFactorMatrix = useOpticalFactorMatrix;
	}
	unsigned int getUseOpticalFactorMatrix() const {
		return this->useOpticalFactorMatrix;
	}
	void setSunlitTemperature(double sunlitTemperature) {
		this->sunlitTemperature = sunlitTemperature;
	}

	double getSunlitTemperature() const {
		return this->sunlitTemperature;
	}

	void setShadedTemperature(double shadedTemperature) {
		this->shadedTemperature = shadedTemperature;
	}

	double getShadedTemperature() const {
		return this->shadedTemperature;
	}

	void setSunlitScopeFile(const std::string &scopeSunlitFile) {
		this->sunlitScopeFile = scopeSunlitFile;
	}

	const std::string &getSunlitScopeFile() const {
		return this->sunlitScopeFile;
	}

	void setShadedScopeFile(const std::string &scopeShadedFile) {
		this->shadedScopeFile = scopeShadedFile;
	}

	const std::string &getShadedScopeFile() const {
		return this->shadedScopeFile;
	}

	bool isUsingPrecomputedIpaRs() const {
		return usePrecomputedIPARs;
	}
	void setUsePrecomputedIpaRs(bool usePrecomputedIpaRs) {
		usePrecomputedIPARs = usePrecomputedIpaRs;
	}
};

#endif /* INCLUDE_CONTAINERS_TEMPERATUREFUNCTION_HPP_ */
