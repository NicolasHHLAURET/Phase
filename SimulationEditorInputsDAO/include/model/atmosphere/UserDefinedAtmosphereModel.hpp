/*
 * UserDefinedAtmosphereModel.hpp
 *
 *  Created on: 28 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_ATMOSPHERE_USERDEFINEDATMOSPHEREMODEL_HPP_
#define INCLUDE_MODEL_ATMOSPHERE_USERDEFINEDATMOSPHEREMODEL_HPP_

#include "IAtmosphereModel.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class UserDefinedAtmosphereModel: public IAtmosphereModel {
	private:
		std::string atmosphereTemperatureFile;

		bool eartCurvatureCorrection;
		bool midPointCorrection;

		double gasOpticalDepth;
		double gasTransmittance;
		double gasScaleFactor;

		double aerosolOpticalDepth;
		double aerosolAlbedo;
		double aerosolScaleFactor;

		double henyeyGreensteinParameterA;
		double henyeyGreensteinParameterG1;
		double henyeyGreensteinParameterG2;
	public:
		UserDefinedAtmosphereModel() :
				eartCurvatureCorrection(true), midPointCorrection(true), gasOpticalDepth(0.049), gasTransmittance(0.750), gasScaleFactor(8400), aerosolOpticalDepth(
					0.202), aerosolAlbedo(0.947), aerosolScaleFactor(1000), henyeyGreensteinParameterA(0.95), henyeyGreensteinParameterG1(0.790), henyeyGreensteinParameterG2(
					0.4) {}

		const std::string & getAtmosphereTemperatureFile() const {
			return atmosphereTemperatureFile;
		}
		const bool & getEartCurvatureCorrection() const {
			return eartCurvatureCorrection;
		}
		const bool & getMidPointCorrection() const {
			return midPointCorrection;
		}
		const double & getGasOpticalDepth() const {
			return gasOpticalDepth;
		}
		const double & getGasTransmittance() const {
			return gasTransmittance;
		}
		const double & getGasScaleFactor() const {
			return gasScaleFactor;
		}
		const double & getAerosolOpticalDepth() const {
			return aerosolOpticalDepth;
		}
		const double & getAerosolAlbedo() const {
			return aerosolAlbedo;
		}
		const double & getAerosolScaleFactor() const {
			return aerosolScaleFactor;
		}
		const double & getHenyeyGreensteinParameterA() const {
			return henyeyGreensteinParameterA;
		}
		const double & getHenyeyGreensteinParameterG1() const {
			return henyeyGreensteinParameterG1;
		}
		const double & getHenyeyGreensteinParameterG2() const {
			return henyeyGreensteinParameterG2;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				atmosphereTemperatureFile = node.get("atmosphereTemperatureFile").get("dartFilePath").get("relativePath").as<std::string>();
				eartCurvatureCorrection = node.get("eartCurvatureCorrection").as<bool>();
				midPointCorrection = node.get("midPointCorrection").as<bool>();

				gasOpticalDepth = node.get("gasOpticalDepth").as<double>();
				gasTransmittance = node.get("gasTransmittance").as<double>();
				gasScaleFactor = node.get("gasScaleFactor").as<double>();

				aerosolOpticalDepth = node.get("aerosolOpticalDepth").as<double>();
				aerosolAlbedo = node.get("aerosolAlbedo").as<double>();
				aerosolScaleFactor = node.get("aerosolScaleFactor").as<double>();

				henyeyGreensteinParameterA = node.get("henyeyGreensteinParameterA").as<double>();
				henyeyGreensteinParameterG1 = node.get("henyeyGreensteinParameterG1").as<double>();
				henyeyGreensteinParameterG2 = node.get("henyeyGreensteinParameterG2").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_ATMOSPHERE_USERDEFINEDATMOSPHEREMODEL_HPP_ */
