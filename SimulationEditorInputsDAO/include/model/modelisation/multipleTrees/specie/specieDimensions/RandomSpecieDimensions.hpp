/*
 * RandomSpecieDimensions.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_RANDOMSPECIEDIMENSIONS_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_RANDOMSPECIEDIMENSIONS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ISpecieDimensions.hpp"

#include "crownShape/ICrownShapeRandomDimension.hpp"
#include "crownShape/RandomComposedEllipsoidCrown.hpp"
#include "crownShape/RandomConiqueComposedCrown.hpp"
#include "crownShape/RandomEllipsoideCrown.hpp"
#include "crownShape/RandomTrapezoidCrown.hpp"
#include "crownShape/RandomTruncatedConeCrown.hpp"
namespace DartInputs {
	class RandomSpecieDimensions: public ISpecieDimensions {
	private:
		shared_ptr<ICrownShapeRandomDimension> crownShapeRandomDimensions;

		double meanCrownHeigth;
		double stdDevCrownHeight;

		double meanCrownIntrinsicRotation;
		double stdDevCrownIntrinsicRotation;

		double meanDiameterBelowCrown;
		double stdDevDiameterBelowCrown;

		double meanHeightBelowCrown;
		double stdDevHeightBelowCrown;

		double heightWithinCrown;
		double diameterWithinCrown;
	public:
		RandomSpecieDimensions() :
				meanCrownHeigth(6), stdDevCrownHeight(0), meanCrownIntrinsicRotation(0), stdDevCrownIntrinsicRotation(0), meanDiameterBelowCrown(0.3), stdDevDiameterBelowCrown(
					0), meanHeightBelowCrown(5), stdDevHeightBelowCrown(0), heightWithinCrown(3), diameterWithinCrown(0.7) {}

		const shared_ptr<ICrownShapeRandomDimension> & getCrownShapeRandomDimensions() const {
			return crownShapeRandomDimensions;
		}

		const double & getMeanCrownHeigth() const {
			return meanCrownHeigth;
		}
		const double & getStdDevCrownHeight() const {
			return stdDevCrownHeight;
		}

		const double & getMeanCrownIntrinsicRotation() const {
			return meanCrownIntrinsicRotation;
		}
		const double & getStdDevCrownIntrinsicRotation() const {
			return stdDevCrownIntrinsicRotation;
		}

		const double & getMeanDiameterBelowCrown() const {
			return meanDiameterBelowCrown;
		}
		const double & getStdDevDiameterBelowCrown() const {
			return stdDevDiameterBelowCrown;
		}

		const double & getMeanHeightBelowCrown() const {
			return meanHeightBelowCrown;
		}
		const double & getStdDevHeightBelowCrown() const {
			return stdDevHeightBelowCrown;
		}

		const double & getHeightWithinCrown() const {
			return heightWithinCrown;
		}
		const double & getDiameterWithinCrown() const {
			return diameterWithinCrown;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("crownShapeRandomDimensions").IsNull()) {
					if (node.get("crownShapeRandomDimensions").Tag()
							== "modelisation.multipleTrees.specie.specieDimensions.crownShape.RandomComposedEllipsoidCrown") {
						crownShapeRandomDimensions = shared_ptr<ICrownShapeRandomDimension>(new RandomComposedEllipsoidCrown());
						static_cast<RandomComposedEllipsoidCrown &>(*crownShapeRandomDimensions) = node.get("crownShapeRandomDimensions");
					} else if (node.get("crownShapeRandomDimensions").Tag()
							== "modelisation.multipleTrees.specie.specieDimensions.crownShape.RandomConiqueComposedCrown") {
						crownShapeRandomDimensions = shared_ptr<ICrownShapeRandomDimension>(new RandomConiqueComposedCrown());
						static_cast<RandomConiqueComposedCrown &>(*crownShapeRandomDimensions) = node.get("crownShapeRandomDimensions");
					} else if (node.get("crownShapeRandomDimensions").Tag()
							== "modelisation.multipleTrees.specie.specieDimensions.crownShape.RandomEllipsoideCrown") {
						crownShapeRandomDimensions = shared_ptr<ICrownShapeRandomDimension>(new RandomEllipsoideCrown());
						static_cast<RandomEllipsoideCrown &>(*crownShapeRandomDimensions) = node.get("crownShapeRandomDimensions");
					} else if (node.get("crownShapeRandomDimensions").Tag()
							== "modelisation.multipleTrees.specie.specieDimensions.crownShape.RandomTrapezoidCrown") {
						crownShapeRandomDimensions = shared_ptr<ICrownShapeRandomDimension>(new RandomTrapezoidCrown());
						static_cast<RandomTrapezoidCrown &>(*crownShapeRandomDimensions) = node.get("crownShapeRandomDimensions");
					} else if (node.get("crownShapeRandomDimensions").Tag()
							== "modelisation.multipleTrees.specie.specieDimensions.crownShape.RandomTruncatedConeCrown") {
						crownShapeRandomDimensions = shared_ptr<ICrownShapeRandomDimension>(new RandomTruncatedConeCrown());
						static_cast<RandomTruncatedConeCrown &>(*crownShapeRandomDimensions) = node.get("crownShapeRandomDimensions");
					}
				}

				meanCrownHeigth = node.get("meanCrownHeigth").as<double>();
				stdDevCrownHeight = node.get("stdDevCrownHeight").as<double>();

				meanCrownIntrinsicRotation = node.get("meanCrownIntrinsicRotation").as<double>();
				stdDevCrownIntrinsicRotation = node.get("stdDevCrownIntrinsicRotation").as<double>();

				meanDiameterBelowCrown = node.get("meanDiameterBelowCrown").as<double>();
				stdDevDiameterBelowCrown = node.get("stdDevDiameterBelowCrown").as<double>();

				meanHeightBelowCrown = node.get("meanHeightBelowCrown").as<double>();
				stdDevHeightBelowCrown = node.get("stdDevHeightBelowCrown").as<double>();

				heightWithinCrown = node.get("heightWithinCrown").as<double>();
				diameterWithinCrown = node.get("diameterWithinCrown").as<double>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIEDIMENSIONS_RANDOMSPECIEDIMENSIONS_HPP_ */
