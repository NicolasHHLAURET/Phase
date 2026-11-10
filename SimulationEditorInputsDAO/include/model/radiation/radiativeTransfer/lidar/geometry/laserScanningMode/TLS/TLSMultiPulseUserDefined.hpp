/*
 * TLSMultiPulseUserDefined.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEUSERDEFINED_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEUSERDEFINED_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"
#include "TLS.hpp"
#include "../../../../../../object/geometry/AngleDirection.hpp"
namespace DartInputs {
	class TLSMultiPulseUserDefined: public TLS {
	private:
		shared_ptr<AngleDirection> angleCapture;
		double deltaZenith;
		double deltaAzimuth;
		double resolutionZenith;
		double resolutionAzimuth;
	public:
		TLSMultiPulseUserDefined() :
				deltaZenith(10), deltaAzimuth(10), resolutionZenith(1), resolutionAzimuth(1) {}

		const shared_ptr<AngleDirection> & getAngleCapture() const {
			return angleCapture;
		}

		const double & getDeltaZenith() const {
			return deltaZenith;
		}

		const double & getDeltaAzimuth() const {
			return deltaAzimuth;
		}

		const double & getResolutionZenith() const {
			return resolutionZenith;
		}

		const double & getResolutionAzimuth() const {
			return resolutionAzimuth;
		}

		void operator =(const YAML::Node& node) {
			TLS::operator=(node);
			if (!node.IsNull()) {
				deltaZenith = node.get("deltaZenith").as<double>();
				deltaAzimuth = node.get("deltaAzimuth").as<double>();
				resolutionZenith = node.get("resolutionZenith").as<double>();
				resolutionAzimuth = node.get("resolutionAzimuth").as<double>();

				if (!node.get("angleCapture").IsNull()) {
					angleCapture = shared_ptr<AngleDirection>(new AngleDirection());
					(*angleCapture) = node.get("angleCapture");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSMULTIPULSEUSERDEFINED_HPP_ */
