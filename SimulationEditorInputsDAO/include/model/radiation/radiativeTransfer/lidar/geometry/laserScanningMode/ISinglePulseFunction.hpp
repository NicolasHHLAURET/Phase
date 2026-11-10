/*
 * ISinglePulseFunction.hpp
 *
 *  Created on: 14 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ISINGLEPULSEFUNCTION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ISINGLEPULSEFUNCTION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "../../../../../object/geometry/AngleDirection.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class ISinglePulseFunction {
	public:
		virtual ~ISinglePulseFunction() {}
		virtual const shared_ptr<AngleDirection> & getFovDirection() const = 0;
		virtual void operator =(const YAML::Node& node) = 0;
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_LIDAR_GEOMETRY_LASERSCANNINGMODE_ISINGLEPULSEFUNCTION_HPP_ */
