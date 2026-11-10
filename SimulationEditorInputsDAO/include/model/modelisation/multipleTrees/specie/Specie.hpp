/*
 * Specie.hpp
 *
 *  Created on: 5 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIE_HPP_
#define INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "yaml-cpp/yaml.h"
#include "BranchAndTwig.hpp"

#include "specieDimensions/ISpecieDimensions.hpp"
#include "specieDimensions/ExactSpecieDimensions.hpp"
#include "specieDimensions/RandomSpecieDimensions.hpp"

#include "crownLevel/CrownLevel.hpp"
namespace DartInputs {
	class Specie {
	private:
		double laiSpecies;
		shared_ptr<BranchAndTwig> branchAndTwig;
		shared_ptr<ISpecieDimensions> specieDimensions;
		int crownOpticalPropertyId;
		int crownThermalPropertyId;
		std::vector<shared_ptr<CrownLevel> > crownLevels;

	public:
		Specie() :
				laiSpecies(4), crownOpticalPropertyId(0), crownThermalPropertyId(0) {}


		const double & getLaiSpecies() const {
			return laiSpecies;
		}
		const shared_ptr<BranchAndTwig> & getBranchAndTwig() const {
			return branchAndTwig;
		}
		const shared_ptr<ISpecieDimensions> & getSpecieDimensions() const {
			return specieDimensions;
		}
		const int & getCrownOpticalPropertyId() const {
			return crownOpticalPropertyId;
		}
		const int & getCrownThermalPropertyId() const {
			return crownThermalPropertyId;
		}
		const std::vector<shared_ptr<CrownLevel> > & getCrownLevels() const {
			return crownLevels;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				laiSpecies = node.get("laiSpecies").as<double>();
				crownOpticalPropertyId = node.get("crownOpticalPropertyId").as<int>();
				crownThermalPropertyId = node.get("crownThermalPropertyId").as<int>();

				if (!node.get("branchAndTwig").IsNull()) {
					branchAndTwig = shared_ptr<BranchAndTwig>(new BranchAndTwig());
					(*branchAndTwig) = node.get("branchAndTwig");
				}

				if (!node.get("specieDimensions").IsNull()) {
					if (node.get("specieDimensions").Tag() == "!modelisation.multipleTrees.specie.specieDimensions.ExactSpecieDimensions") {
						specieDimensions = shared_ptr<ISpecieDimensions>(new ExactSpecieDimensions());
						static_cast<ExactSpecieDimensions &>(*specieDimensions) = node.get("specieDimensions");
					} else if (node.get("specieDimensions").Tag() == "!modelisation.multipleTrees.specie.specieDimensions.RandomSpecieDimensions") {
						specieDimensions = shared_ptr<ISpecieDimensions>(new RandomSpecieDimensions());
						static_cast<RandomSpecieDimensions &>(*specieDimensions) = node.get("specieDimensions");
					}
				}

				crownLevels.clear();
				if (!node.get("crownLevels").IsNull() && node.get("crownLevels").IsSequence()) {
					for (YAML::const_iterator it = node.get("crownLevels").begin(); it != node.get("crownLevels").end(); ++it) {
						shared_ptr<CrownLevel> crownLevel = shared_ptr<CrownLevel>(new CrownLevel());
						(*crownLevel) = (*it);
						crownLevels.push_back(crownLevel);
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_MULTIPLETREES_SPECIE_SPECIE_HPP_ */
