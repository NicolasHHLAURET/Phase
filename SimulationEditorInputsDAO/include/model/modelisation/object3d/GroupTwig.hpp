/*
 * GroupTwig.hpp
 *
 *  Created on: 15 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPTWIG_HPP_
#define INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPTWIG_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "Group.hpp"
#include "GroupLeaf.hpp"
#include "GroupTwig.hpp"
#include <vector>
namespace DartInputs {
	class GroupTwig: public Group {
	private:
		std::vector<shared_ptr<Group> > groups;
	public:
		GroupTwig() {}

		const std::vector<shared_ptr<Group> > & getGroups() const {
			return groups;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				groups.clear();
				if (!node.get("groups").IsNull() && node.get("groups").IsSequence()) {
					for (YAML::const_iterator it = node.get("groups").begin(); it != node.get("groups").end(); ++it) {
						shared_ptr<Group> group;
						if (it->Tag() == "!modelisation.object3d.GroupLeaf") {
							group = shared_ptr<Group>(new GroupLeaf());
							static_cast<GroupLeaf &>(*group) = (*it);
						} else if (it->Tag() == "!modelisation.object3d.GroupTwig") {
							group = shared_ptr<Group>(new GroupTwig());
							static_cast<GroupTwig &>(*group) = (*it);
						} else {
							break;
						}
						groups.push_back(group);
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_OBJECT3D_GROUPTWIG_HPP_ */
