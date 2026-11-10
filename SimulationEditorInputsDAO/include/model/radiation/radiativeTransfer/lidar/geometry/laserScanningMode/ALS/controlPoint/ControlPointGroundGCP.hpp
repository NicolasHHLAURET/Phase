/*
 * ControlPointGroundGCP.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_CONTROLPOINT_CONTROLPOINTGROUNDGCP_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_CONTROLPOINT_CONTROLPOINTGROUNDGCP_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"
#include "IControlPoint.hpp"
#include "../../../../../../../object/genericObject/DoubletDouble.hpp"
namespace DartInputs {
	class ControlPointGroundGCP: public IControlPoint {
	private:
		shared_ptr<DoubletDouble> groundPosition;
		double lookAngle;

	public:
		ControlPointGroundGCP() :
				lookAngle(0) {}

		const shared_ptr<DoubletDouble> & getGroundPosition() const {
			return groundPosition;
		}

		const double & getLookAngle() const {
			return lookAngle;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				lookAngle = node.get("lookAngle").as<double>();

				if (!node.get("groundPosition").IsNull()) {
					groundPosition = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*groundPosition) = node.get("groundPosition");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_CONTROLPOINT_CONTROLPOINTGROUNDGCP_HPP_ */
