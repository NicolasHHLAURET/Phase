/*
 * ALSMultiPulseUserDefined.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEUSERDEFINED_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEUSERDEFINED_HPP_
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"
#include "controlPoint/IControlPoint.hpp"
#include "controlPoint/ControlPointGroundGCP.hpp"
#include "../../../../../../object/genericObject/DoubletDouble.hpp"
#include "ALS.hpp"
namespace DartInputs {
	class ALSMultiPulseUserDefined: public ALS {
	public:
		enum RangeResolutionDefinition {
			ANGLE, DIMENSION
		};
	private:
		shared_ptr<DoubletDouble> startPoint;
		shared_ptr<DoubletDouble> endPoint;
		double platformAzimuth;
		double swthWidth;
		shared_ptr<IControlPoint> controlPoint;
		RangeResolutionDefinition rangeResolutionDefintion;
		double azimuthalResolution;
		double rangeResolution;

	public:
		ALSMultiPulseUserDefined() :
				platformAzimuth(0), swthWidth(30), rangeResolutionDefintion(DIMENSION), azimuthalResolution(3), rangeResolution(3) {}

		const shared_ptr<DoubletDouble> & getStartPoint() const {
			return startPoint;
		}

		const shared_ptr<DoubletDouble> & getEndPoint() const {
			return endPoint;
		}

		const double & getPlatformAzimuth() const {
			return platformAzimuth;
		}

		const double & getSwthWidth() const {
			return swthWidth;
		}

		const shared_ptr<IControlPoint> & getControlPoint() const {
			return controlPoint;
		}

		const RangeResolutionDefinition & getRangeResolutionDefintion() const {
			return rangeResolutionDefintion;
		}

		const double & getAzimuthalResolution() const {
			return azimuthalResolution;
		}

		const double & getRangeResolution() const {
			return rangeResolution;
		}

		void operator =(const YAML::Node& node) {
			ALS::operator=(node);
			if (!node.IsNull()) {
				platformAzimuth = node.get("platformAzimuth").as<double>();
				swthWidth = node.get("swthWidth").as<double>();
				azimuthalResolution = node.get("azimuthalResolution").as<double>();
				rangeResolution = node.get("rangeResolution").as<double>();

				if (node.get("rangeResolutionDefintion").as<std::string>() == "ANGLE") {
					rangeResolutionDefintion = ANGLE;
				} else {
					rangeResolutionDefintion = DIMENSION;
				}
				if (!node.get("startPoint").IsNull()) {
					startPoint = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*startPoint) = node.get("startPoint");
				}
				if (!node.get("endPoint").IsNull()) {
					endPoint = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*endPoint) = node.get("endPoint");
				}
				if (!node.get("controlPoint").IsNull()) {
					controlPoint = shared_ptr<IControlPoint>(new ControlPointGroundGCP());
					static_cast<ControlPointGroundGCP &>(*controlPoint) = node.get("controlPoint");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSMULTIPULSEUSERDEFINED_HPP_ */
