/*
 * DateSunAngle.hpp
 *
 *  Created on: 29 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DATESUNANGLE_HPP_
#define INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DATESUNANGLE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "ISunAngleDefinition.hpp"
#include "LocalTime.hpp"
namespace DartInputs {
	class DateSunAngle: public ISunAngleDefinition {
	private:
		int year;
		int month;
		int day;
		int hours;
		int minutes;
		int seconds;
		shared_ptr<LocalTime> localTime;

	public:
		DateSunAngle() :
				year(2012), month(1), day(1), hours(12), minutes(0), seconds(0) {}

		const int & getYear() const {
			return year;
		}
		const int & getMonth() const {
			return month;
		}
		const int & getDay() const {
			return day;
		}
		const int & getHours() const {
			return hours;
		}
		const int & getMinutes() const {
			return minutes;
		}
		const int & getSeconds() const {
			return seconds;
		}
		const shared_ptr<LocalTime> & getLocalTime() const {
			return localTime;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				year = node.get("year").as<int>();
				month = node.get("month").as<int>();
				day = node.get("day").as<int>();
				hours = node.get("hours").as<int>();
				minutes = node.get("minutes").as<int>();
				seconds = node.get("seconds").as<int>();

				if (!node.get("localTime").IsNull()) {
					localTime = shared_ptr<LocalTime>(new LocalTime());
					(*localTime) = node.get("localTime");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DIRECTION_SUNDIRECTION_DATESUNANGLE_HPP_ */
