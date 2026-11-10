/*
 * OversampledThetaRange.hpp
 *
 *  Created on: 14 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OBJECT_OVERSAMPLEDTHETARANGE_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OBJECT_OVERSAMPLEDTHETARANGE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class OversampledThetaRange {
	private:
		double theta__0;
		double theta__1;

	public:
		OversampledThetaRange() :
			theta__0(0), theta__1(0) {}

		const double & getTheta__0() const {
			return theta__0;
		}

		const double & getTheta__1() const {
			return theta__1;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				theta__0 = node.get("theta__0").as<double>();
				theta__1 = node.get("theta__1").as<double>();
			}
		}
	};
}



#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_OBJECT_OVERSAMPLEDTHETARANGE_HPP_ */
