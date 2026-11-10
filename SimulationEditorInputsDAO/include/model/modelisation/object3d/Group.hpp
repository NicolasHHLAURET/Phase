/*
 * Group.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUP_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUP_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class Group {
	private:
		std::string groupName;
		int groupId;

	public:
		Group() :
				groupId(-42) {}
		virtual ~Group() {}

		const std::string & getGroupName() const {
			return groupName;
		}

		const int & getGroupId() const {
			return groupId;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				groupId = node.get("groupId").as<int>();
				groupName = node.get("groupName").as<std::string>();
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUP_HPP_ */
