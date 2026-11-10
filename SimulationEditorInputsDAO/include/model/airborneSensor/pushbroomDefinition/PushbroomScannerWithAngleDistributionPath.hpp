/*
 * PushbroomScannerWithAngleDistributionPath.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHANGLEDISTRIBUTIONPATH_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHANGLEDISTRIBUTIONPATH_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../../object/genericObject/TripletDouble.hpp"
#include "IPusbroomScannerDefinition.hpp"
namespace DartInputs {
	class PushbroomScannerWithAngleDistributionPath: public IPusbroomScannerDefinition {
	private:
		std::string sunThetaDistribution;
		std::string sunPhiThetaDistribution;
		shared_ptr<TripletDouble> scannerOffset;
		double sunAngleResolution;

	public:
		PushbroomScannerWithAngleDistributionPath() :
				sunAngleResolution(1) {}

		const std::string & getSunThetaDistribution() const {
			return sunThetaDistribution;
		}

		const std::string & getSunPhiThetaDistribution() const {
			return sunPhiThetaDistribution;
		}

		const shared_ptr<TripletDouble> & getScannerOffset() const {
			return scannerOffset;
		}

		const double & getSunAngleResolution() const {
			return sunAngleResolution;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				sunThetaDistribution = node.get("sunThetaDistribution").get("dartFilePath").get("relativePath").as<std::string>();
				sunPhiThetaDistribution = node.get("sunPhiThetaDistribution").get("dartFilePath").get("relativePath").as<std::string>();
				sunAngleResolution = node.get("sunAngleResolution").as<double>();

				if (!node.get("scannerOffset").IsNull()) {
					if (node.get("scannerOffset").Tag() == "!object.genericObject.Triplet") {
						scannerOffset = shared_ptr<TripletDouble>(new TripletDouble());
						(*scannerOffset) = node.get("scannerOffset");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHANGLEDISTRIBUTIONPATH_HPP_ */
