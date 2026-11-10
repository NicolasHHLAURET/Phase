/*
 * PushbroomScannerWithLinearPath.hpp
 *
 *  Created on: 26 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHLINEARPATH_HPP_
#define INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHLINEARPATH_HPP_

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
	class PushbroomScannerWithLinearPath: public IPusbroomScannerDefinition {
	private:
		shared_ptr<TripletDouble> pointInScannerPath;
		double flightDirectionAzimuth;
		double acquisitionAzimuthOrientation;

	public:
		PushbroomScannerWithLinearPath() :
				flightDirectionAzimuth(0), acquisitionAzimuthOrientation(0) {}

		const shared_ptr<TripletDouble> & getPointInScannerPath() const {
			return pointInScannerPath;
		}

		const double & getFlightDirectionAzimuth() const {
			return flightDirectionAzimuth;
		}

		const double & getAcquisitionAzimuthOrientation() const {
			return acquisitionAzimuthOrientation;
		}

		void operator=(const YAML::Node& node) {
			if (!node.IsNull()) {
				flightDirectionAzimuth = node.get("flightDirectionAzimuth").as<double>();
				acquisitionAzimuthOrientation = node.get("acquisitionAzimuthOrientation").as<double>();
				if (!node.get("pointInScannerPath").IsNull()) {
					if (node.get("pointInScannerPath").Tag() == "!object.genericObject.Triplet") {
						pointInScannerPath = shared_ptr<TripletDouble>(new TripletDouble());
						(*pointInScannerPath) = node.get("pointInScannerPath");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_AIRBORNESENSOR_PUSHBROOMDEFINITION_PUSHBROOMSCANNERWITHLINEARPATH_HPP_ */
