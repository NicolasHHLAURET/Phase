/*
 * MultipleTrees.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_MULTIPLETREES_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_MULTIPLETREES_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "../IDartObject.hpp"
#include "specie/Specie.hpp"
#include "specie/SpecieTreesTxt.hpp"
#include "position/ITreesPositions.hpp"
#include "position/ExactPosition.hpp"
#include "position/RandomPosition.hpp"
#include "../../object/geometry/SubScene.hpp"
namespace DartInputs {
	class MultipleTrees: public IDartObject {
	private:
		shared_ptr<Specie> specie;
		shared_ptr<ITreesPositions> treesPositions;
		shared_ptr<SubScene> subSceneLai;
		shared_ptr<SpecieTreesTxt> specieTreesTxt;

	public:
		MultipleTrees() {}

		const shared_ptr<Specie> & getSpecie() const {
			return specie;
		}
		const shared_ptr<ITreesPositions> & getTreesPositions() const {
			return treesPositions;
		}
		const shared_ptr<SubScene> & getSubSceneLai() const {
			return subSceneLai;
		}
		const shared_ptr<SpecieTreesTxt> & getSpecieTreesTxt() const {
			return specieTreesTxt;
		}

		void operator =(const YAML::Node& node) {
			IDartObject::operator =(node);
			if (!node.IsNull()) {
				if (!node.get("subSceneLai").IsNull()) {
					subSceneLai = shared_ptr<SubScene>(new SubScene());
					(*subSceneLai) = node.get("subSceneLai");
				}
				if (!node.get("specie").IsNull()) {
					specie = shared_ptr<Specie>(new Specie());
					(*specie) = node.get("specie");
				}
				if (!node.get("specieTreesTxt").IsNull()) {
					specieTreesTxt = shared_ptr<SpecieTreesTxt>(new SpecieTreesTxt());
					(*specieTreesTxt) = node.get("specieTreesTxt");
				}
				if (!node.get("treesPositions").IsNull()) {
					if (node.get("treesPositions").Tag() == "!modelisation.multipleTrees.position.ExactPosition") {
						treesPositions = shared_ptr<ITreesPositions>(new ExactPosition());
						static_cast<ExactPosition &>(*treesPositions) = node.get("treesPositions");
					} else if (node.get("treesPositions").Tag() == "!modelisation.multipleTrees.position.RandomPosition") {
						treesPositions = shared_ptr<ITreesPositions>(new RandomPosition());
						static_cast<RandomPosition &>(*treesPositions) = node.get("treesPositions");
					}
				}
			}
		}
	};
}

#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_MULTIPLETREES_HPP_ */
