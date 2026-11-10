/*
 * RadiativeBudgetProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_PRODUCTS_RADIATIVEBUDGETPRODUCTS_HPP_
#define INCLUDE_MODEL_PRODUCTS_RADIATIVEBUDGETPRODUCTS_HPP_
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class RadiativeBudgetProducts {
	public:
		enum RadiativeBudgetUnit {
			FRACTION, WM2UM, WUM
		};
	private:
		RadiativeBudgetUnit radiativeBudgetUnit;
		bool extrapolation;
		bool profiles_f;
		bool d3_f;
		bool totalPerTypeOfTrianglesAndIteration_f;
		bool d3PerTypeOfTrianglesAndIteration_f;
		bool totalPerIndividualTrianglesAndPerIteration_f;

	public:
		RadiativeBudgetProducts() :
				radiativeBudgetUnit(FRACTION), extrapolation(true), profiles_f(true), d3_f(true), totalPerTypeOfTrianglesAndIteration_f(false), d3PerTypeOfTrianglesAndIteration_f(
					false), totalPerIndividualTrianglesAndPerIteration_f(false) {}

		const RadiativeBudgetUnit & getRadiativeBudgetUnit() const {
			return radiativeBudgetUnit;
		}

		const bool & getExtrapolation() const {
			return extrapolation;
		}

		const bool & getProfiles_f() const {
			return profiles_f;
		}

		const bool & getD3_f() const {
			return d3_f;
		}

		const bool & getTotalPerTypeOfTrianglesAndIteration_f() const {
			return totalPerTypeOfTrianglesAndIteration_f;
		}

		const bool & getD3PerTypeOfTrianglesAndIteration_f() const {
			return d3PerTypeOfTrianglesAndIteration_f;
		}

		const bool & getTotalPerIndividualTrianglesAndPerIteration_f() const {
			return totalPerIndividualTrianglesAndPerIteration_f;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("radiativeBudgetUnit").as<std::string>() == "FRACTION") {
					radiativeBudgetUnit = FRACTION;
				} else if (node.get("radiativeBudgetUnit").as<std::string>() == "WM2UM") {
					radiativeBudgetUnit = WM2UM;
				} else {
					radiativeBudgetUnit = WUM;
				}
				extrapolation = node.get("extrapolation");
				profiles_f = node.get("profiles_f");
				d3_f = node.get("d3_f");
				totalPerTypeOfTrianglesAndIteration_f = node.get("totalPerTypeOfTrianglesAndIteration_f");
				d3PerTypeOfTrianglesAndIteration_f = node.get("d3PerTypeOfTrianglesAndIteration_f");
				totalPerIndividualTrianglesAndPerIteration_f = node.get("totalPerIndividualTrianglesAndPerIteration_f");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_PRODUCTS_RADIATIVEBUDGETPRODUCTS_HPP_ */
