/*
 * EarthCoordinate.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_OBJECT_GEOMETRY_EARTHCOORDINATE_HPP_
#define INCLUDE_MODEL_OBJECT_GEOMETRY_EARTHCOORDINATE_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class EarthCoordinate {
	private:
		double longitude;
		double latitude;
		double altitude;

	public:
		EarthCoordinate() :
				longitude(0), latitude(0), altitude(0) {}

		const double & getLongitude() const {
			return longitude;
		}
		const double & getLatitude() const {
			return latitude;
		}
		const double & getAltitude() const {
			return altitude;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				longitude = node.get("longitude").as<double>();
				latitude = node.get("latitude").as<double>();
				altitude = node.get("altitude").as<double>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_OBJECT_GEOMETRY_EARTHCOORDINATE_HPP_ */
