/*
 * IcoSphere.hpp
 *
 *  Created on: 17 avr. 2015
 *      Author: lauretn
 */

#ifndef ICOSPHERE_HPP_
#define ICOSPHERE_HPP_

#include <vector>
#include <map>
#include <cstdint>
#include "Point3.hpp"
using namespace std;

class IcoSphere {
public:
	typedef vector<unsigned int> TriangleList;
	typedef vector<Point3<double> > VertexList;

private:
	TriangleList _indicesList;
	VertexList _vertexList;

	TriangleList _temporaryIndicesList;
	map<uint64_t, unsigned int> _middlePointsIndexCache; // Stores the indices of the already created middle points

public:
	IcoSphere();

	virtual ~IcoSphere() {
		// TODO Auto-generated destructor stub
	}

	void sudivide(unsigned int nbSubdivision) {
		for (unsigned int s = 0; s < nbSubdivision; ++s) {
			 sudivide();
		}
	}

	void sudivide();

	const VertexList & getVertexList() {
		return _vertexList;
	}

	const TriangleList & getTriangleIndicesList() {
		return _indicesList;
	}

private:
	void addVertex(const Point3<double> & newVertex) {
		_vertexList.push_back(newVertex / newVertex.getDistanceToOrigin());
	}

	void addTriangle(const unsigned int & index0, const unsigned int & index1, const unsigned int & index2) {
		_temporaryIndicesList.push_back(index0);
		_temporaryIndicesList.push_back(index1);
		_temporaryIndicesList.push_back(index2);
	}

	unsigned int getMiddlePoint(const unsigned int & indexP1, const unsigned int & indexP2);
};

#endif /* ICOSPHERE_HPP_ */
