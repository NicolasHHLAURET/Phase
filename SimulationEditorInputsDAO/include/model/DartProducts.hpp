/*
 * DartProducts.hpp
 *
 *  Created on: 15 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTPRODUCTS_HPP_
#define INCLUDE_MODEL_DARTPRODUCTS_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "products/EarthSceneProducts.hpp"
#include "products/FluxTrackingProducts.hpp"
#include "products/LidarProducts.hpp"
#include "products/RadiativeBudgetProducts.hpp"
#include "products/AtmosphericProducts.hpp"
#include "products/LidarImageProducts.hpp"
#include "../yaml/YaMLReader.hpp"

/**
 shared_ptr<AirLaw> _airLaw;
 _airLaw = shared_ptr<AirLaw>(new AirRayleigh());
 AirRayleigh & airRayleigh = static_cast<AirRayleigh &>(*_airLaw);
 */
namespace DartInputs {
	class DartProducts {
	private:
		shared_ptr<EarthSceneProducts> earthSceneProducts;
		shared_ptr<FluxTrackingProducts> fluxTrackingProducts;
		shared_ptr<LidarProducts> lidarProducts;
		shared_ptr<RadiativeBudgetProducts> radiativeBudgetProducts;
		shared_ptr<AtmosphericProducts> atmosphericProducts;
		shared_ptr<LidarImageProducts> lidarImageProducts;

	public:
		DartProducts() {}

		const shared_ptr<EarthSceneProducts> & getEarthSceneProducts() const {
			return earthSceneProducts;
		}

		const shared_ptr<FluxTrackingProducts> & getFluxTrackingProducts() const {
			return fluxTrackingProducts;
		}

		const shared_ptr<LidarProducts> & getLidarProducts() const {
			return lidarProducts;
		}

		const shared_ptr<RadiativeBudgetProducts> & getRadiativeBudgetProducts() const {
			return radiativeBudgetProducts;
		}

		const shared_ptr<AtmosphericProducts> & getAtmosphericProducts() const {
			return atmosphericProducts;
		}

		const shared_ptr<LidarImageProducts> & getLidarImageProducts() const {
			return lidarImageProducts;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			if (!yamlReader.getFirstNode("earthSceneProducts").IsNull()) {
				earthSceneProducts = shared_ptr<EarthSceneProducts>(new EarthSceneProducts());
				(*earthSceneProducts) = yamlReader.getFirstNode("earthSceneProducts");
			}
			if (!yamlReader.getFirstNode("fluxTrackingProducts").IsNull()) {
				fluxTrackingProducts = shared_ptr<FluxTrackingProducts>(new FluxTrackingProducts());
				(*fluxTrackingProducts) = yamlReader.getFirstNode("fluxTrackingProducts");
			}
			if (!yamlReader.getFirstNode("lidarProducts").IsNull()) {
				lidarProducts = shared_ptr<LidarProducts>(new LidarProducts());
				(*lidarProducts) = yamlReader.getFirstNode("lidarProducts");
			}
			if (!yamlReader.getFirstNode("radiativeBudgetProducts").IsNull()) {
				radiativeBudgetProducts = shared_ptr<RadiativeBudgetProducts>(new RadiativeBudgetProducts());
				(*radiativeBudgetProducts) = yamlReader.getFirstNode("radiativeBudgetProducts");
			}
			if (!yamlReader.getFirstNode("atmosphericProducts").IsNull()) {
				atmosphericProducts = shared_ptr<AtmosphericProducts>(new AtmosphericProducts());
				(*atmosphericProducts) = yamlReader.getFirstNode("atmosphericProducts");
			}
			if (!yamlReader.getFirstNode("lidarImageProducts").IsNull()) {
				lidarImageProducts = shared_ptr<LidarImageProducts>(new LidarImageProducts());
				(*lidarImageProducts) = yamlReader.getFirstNode("lidarImageProducts");
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTPRODUCTS_HPP_ */
