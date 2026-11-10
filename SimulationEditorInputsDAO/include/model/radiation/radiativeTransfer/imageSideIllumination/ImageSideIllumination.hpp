/*
 * ImageSideIllumination.hpp
 *
 *  Created on: 22 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_IMAGESIDEILLUMINATION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_IMAGESIDEILLUMINATION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "SideIllumination.hpp"

namespace DartInputs {
	class ImageSideIllumination {
		bool disableThermalEmission;
		bool disableSolarIllumination;

		shared_ptr<SideIllumination> backSideIllumination; // -X
		shared_ptr<SideIllumination> frontSideIllumination; // +X

		shared_ptr<SideIllumination> leftSideIllumination; // -Y
		shared_ptr<SideIllumination> rightSideIllumination; // +Y

		shared_ptr<SideIllumination> bottomSideIllumination; // -Z
		shared_ptr<SideIllumination> topSideIllumination; // +Z
	public:
		ImageSideIllumination() : disableThermalEmission(false), disableSolarIllumination(false){}

		const bool & getDisableThermalEmission() const {
			return disableThermalEmission;
		}

		const bool & getDisableSolarIllumination() const {
			return disableSolarIllumination;
		}

		const shared_ptr<SideIllumination> & getBackSideIllumination() const
		{
			return backSideIllumination;
		}
		const shared_ptr<SideIllumination> & getFrontSideIllumination() const
		{
			return frontSideIllumination;
		}

		const shared_ptr<SideIllumination> & getLeftSideIllumination() const
		{
			return leftSideIllumination;
		}
		const shared_ptr<SideIllumination> & getRightSideIllumination() const
		{
			return rightSideIllumination;
		}

		const shared_ptr<SideIllumination> & getBottomSideIllumination() const
		{
			return bottomSideIllumination;
		}
		const shared_ptr<SideIllumination> & getTopSideIllumination() const
		{
			return topSideIllumination;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				disableThermalEmission = node.get("disableThermalEmission").as<bool>();
				disableSolarIllumination = node.get("disableSolarIllumination").as<bool>();

				if (!node.get("backSideIllumination").IsNull()) {
					backSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*backSideIllumination) = node.get("backSideIllumination");
				}
				if (!node.get("frontSideIllumination").IsNull()) {
					frontSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*frontSideIllumination) = node.get("frontSideIllumination");
				}

				if (!node.get("leftSideIllumination").IsNull()) {
					leftSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*leftSideIllumination) = node.get("leftSideIllumination");
				}
				if (!node.get("rightSideIllumination").IsNull()) {
					rightSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*rightSideIllumination) = node.get("rightSideIllumination");
				}

				if (!node.get("bottomSideIllumination").IsNull()) {
					bottomSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*bottomSideIllumination) = node.get("bottomSideIllumination");
				}
				if (!node.get("topSideIllumination").IsNull()) {
					topSideIllumination = shared_ptr<SideIllumination>(new SideIllumination());
					(*topSideIllumination) = node.get("topSideIllumination");
				}
			}
		}
	};
}



#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_IMAGESIDEILLUMINATION_HPP_ */
