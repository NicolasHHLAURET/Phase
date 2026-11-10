/*
 * ALSSinglePulse.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSSINGLEPULSE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSSINGLEPULSE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif
#include "yaml-cpp/yaml.h"
#include "../ILaserScanningMode.hpp"
#include "../ISinglePulseFunction.hpp"
#include "ALS.hpp"
#include "../../../../../../object/geometry/AngleDirection.hpp"
#include "../../../../../../object/genericObject/DoubletDouble.hpp"
namespace DartInputs {
	class ALSSinglePulse: public ALS, public ISinglePulseFunction  {
	private:
		shared_ptr<AngleDirection> fovDirection;
		shared_ptr<DoubletDouble> centerPositionOfIllumination;

	public:
		ALSSinglePulse() {}

		const shared_ptr<AngleDirection> & getFovDirection() const {
			return fovDirection;
		}

		const shared_ptr<DoubletDouble> & getCenterPositionOfIllumination() const {
			return centerPositionOfIllumination;
		}

		void operator =(const YAML::Node& node) {
			ALS::operator=(node);
			if (!node.IsNull()) {
				if (!node.get("centerPositionOfIllumination").IsNull()) {
					centerPositionOfIllumination = shared_ptr<DoubletDouble>(new DoubletDouble());
					(*centerPositionOfIllumination) = node.get("centerPositionOfIllumination");
				}

				if (!node.get("fovDirection").IsNull()) {
					fovDirection = shared_ptr<AngleDirection>(new AngleDirection());
					(*fovDirection) = node.get("fovDirection");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ALS_ALSSINGLEPULSE_HPP_ */
