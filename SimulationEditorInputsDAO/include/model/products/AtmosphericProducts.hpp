/*
 * AtmosphericProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_ATMOSPHERICPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_ATMOSPHERICPRODUCTS_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class AtmosphericProducts {
	private:
		bool order1Atmosphere;
		bool brfBtfRadianceTOA;
		bool radianceBOAAfterCoupling;
		bool radianceBOABeforeCoupling;

	public:
		AtmosphericProducts() :
				order1Atmosphere(false), brfBtfRadianceTOA(false), radianceBOAAfterCoupling(false), radianceBOABeforeCoupling(false) {}

		const bool & getOrder1Atmosphere() const {
			return order1Atmosphere;
		}

		const bool & getBrfBtfRadianceTOA() const {
			return brfBtfRadianceTOA;
		}

		const bool & getRadianceBOAAfterCoupling() const {
			return radianceBOAAfterCoupling;
		}

		const bool & getRadianceBOABeforeCoupling() const {
			return radianceBOABeforeCoupling;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				order1Atmosphere = node.get("order1Atmosphere").as<bool>();
				brfBtfRadianceTOA = node.get("brfBtfRadianceTOA").as<bool>();
				radianceBOAAfterCoupling = node.get("radianceBOAAfterCoupling").as<bool>();
				radianceBOABeforeCoupling = node.get("radianceBOABeforeCoupling").as<bool>();
			}
		}

	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_ATMOSPHERICPRODUCTS_HPP_ */
