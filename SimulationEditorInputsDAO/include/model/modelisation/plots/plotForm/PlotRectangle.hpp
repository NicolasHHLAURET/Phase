/*
 * PlotRectangle.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_PLOTRECTANGLE_HPP_
#define INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_PLOTRECTANGLE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IPlotForm.hpp"
#include "../../../object/geometry/SubScene.hpp"
namespace DartInputs {
	class PlotRectangle: public IPlotForm {
	private:
		shared_ptr<SubScene> plotForm;
		double rotationAroundCenter;
	public:
		PlotRectangle() :
				rotationAroundCenter(0) {}

		const shared_ptr<SubScene> & getPlotForm() const {
			return plotForm;
		}

		const double & getRotationAroundCenter() const {
			return rotationAroundCenter;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				rotationAroundCenter = node.get("rotationAroundCenter").as<double>();
				if (!node.get("plotForm").IsNull()) {
					plotForm = shared_ptr<SubScene>(new SubScene());
					(*plotForm) = node.get("plotForm");
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_PLOTS_PLOTFORM_PLOTRECTANGLE_HPP_ */
