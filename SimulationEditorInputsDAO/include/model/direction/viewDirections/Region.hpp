/*
 * Region.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_REGION_HPP_
#define INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_REGION_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IAddedViewDirections.hpp"

#include "IDirectionDescription.hpp"
#include "AngleDescription.hpp"
#include "CoordinateDescription.hpp"

#include "directionShape/IDirectionShape.hpp"
#include "directionShape/RectangleShape.hpp"
#include "directionShape/SquareShape.hpp"
namespace DartInputs {
	class Region: public IAddedViewDirections {
	private:
		bool generateImages;
		shared_ptr<IDirectionDescription> description;
		shared_ptr<IDirectionShape> shape;
		int nbDirectionInRegion;
		bool isVirtual;

	public:
		Region() :
				generateImages(false), nbDirectionInRegion(25), isVirtual(false) {}

		const bool & getGenerateImages() const {
			return generateImages;
		}
		const shared_ptr<IDirectionDescription> & getDescription() const {
			return description;
		}
		const shared_ptr<IDirectionShape> & getShape() const {
			return shape;
		}
		const int & getNbDirectionInRegion() const {
			return nbDirectionInRegion;
		}
		const bool & getIsVirtual() const {
			return isVirtual;
		}
		void operator =(const YAML::Node& node) {
			IAddedViewDirections::operator=(node);
			if (!node.IsNull()) {
				generateImages = node.get("generateImages").as<bool>();
				nbDirectionInRegion = node.get("nbDirectionInRegion").as<int>();
				isVirtual = node.get("isVirtual").as<bool>();

				if (!node.get("description").IsNull()) {
					if (node.get("description").Tag() == "!direction.viewDirections.AngleDescription") {
						description = shared_ptr<IDirectionDescription>(new AngleDescription());
						static_cast<AngleDescription &>(*description) = node.get("description");
					} else if (node.get("description").Tag() == "!direction.viewDirections.CoordinateDescription") {
						description = shared_ptr<IDirectionDescription>(new CoordinateDescription());
						static_cast<CoordinateDescription &>(*description) = node.get("description");
					}
				}
				if (!node.get("shape").IsNull()) {
					if (node.get("shape").Tag() == "!direction.viewDirections.directionShape.RectangleShape") {
						shape = shared_ptr<IDirectionShape>(new RectangleShape());
						static_cast<RectangleShape &>(*shape) = node.get("shape");
					} else if (node.get("shape").Tag() == "!direction.viewDirections.directionShape.SquareShape") {
						shape = shared_ptr<IDirectionShape>(new SquareShape());
						static_cast<SquareShape &>(*shape) = node.get("shape");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_VIEWDIRECTIONS_REGION_HPP_ */
