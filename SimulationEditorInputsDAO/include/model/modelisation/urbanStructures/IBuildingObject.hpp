/*
 * IBuildingObject.hpp
 *
 *  Created on: 8 avr. 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_IBUILDINGOBJECT_HPP_
#define INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_IBUILDINGOBJECT_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include <vector>
#include "yaml-cpp/yaml.h"
#include "object/Vertex.hpp"
#include "object/Face.hpp"
namespace DartInputs {
	class IBuildingObject {
	private:
		std::vector<shared_ptr<Vertex> > objectPoints;
		std::vector<shared_ptr<Face> > faceProperties;

	public:
		IBuildingObject() {}
		virtual ~IBuildingObject() {}

		virtual const int getNbInputVertex() const = 0;
		virtual const int getNbFaceProperties() const = 0;

		const std::vector<shared_ptr<Vertex> > & getObjectPoints() const {
			return objectPoints;
		}

		const std::vector<shared_ptr<Face> > & getFaceProperties() const {
			return faceProperties;
		}

		virtual void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				objectPoints.clear();
				if (!node.get("objectPoints").IsNull() && node.get("objectPoints").IsSequence()) {
					for (YAML::const_iterator it = node.get("objectPoints").begin(); it != node.get("objectPoints").end(); ++it) {
						shared_ptr<Vertex> vertex = shared_ptr<Vertex>(new Vertex());
						(*vertex) = (*it);
						objectPoints.push_back(vertex);
					}
				}

				faceProperties.clear();
				if (!node.get("faceProperties").IsNull() && node.get("faceProperties").IsSequence()) {
					for (YAML::const_iterator it = node.get("faceProperties").begin(); it != node.get("faceProperties").end(); ++it) {
						shared_ptr<Face> face = shared_ptr<Face>(new Face());
						(*face) = (*it);
						faceProperties.push_back(face);
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_MODELISATION_URBANSTRUCTURES_IBUILDINGOBJECT_HPP_ */
