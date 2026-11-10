/*
 * TLSSinglePulse.hpp
 *
 *  Created on: 22 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSSINGLEPULSE_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSSINGLEPULSE_HPP_

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
#include "TLS.hpp"
#include "../../../../../../object/geometry/AngleDirection.hpp"
namespace DartInputs {
	class TLSSinglePulse: public ISinglePulseFunction, public TLS {
	private:
		shared_ptr<AngleDirection> fovDirection;

	public:
		TLSSinglePulse() {}

		const shared_ptr<AngleDirection> & getFovDirection() const {
			return fovDirection;
		}

		void operator =(const YAML::Node& node) {
			TLS::operator=(node);
			if (!node.IsNull()) {
				if (!node.get("fovDirection").IsNull()) {
					fovDirection = shared_ptr<AngleDirection>(new AngleDirection());
					(*fovDirection) = node.get("fovDirection");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_TLS_TLSSINGLEPULSE_HPP_ */
