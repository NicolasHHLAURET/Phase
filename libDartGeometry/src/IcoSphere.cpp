/*
 * IcoSphere.cpp
 *
 *  Created on: 17 avr. 2015
 *      Author: lauretn
 */

#include "IcoSphere.hpp"

IcoSphere::IcoSphere() {
	double t = (1.0 + sqrt(5.0)) / 2.0;
	addVertex(Point3<double>(-1, t, 0));
	addVertex(Point3<double>(1, t, 0));
	addVertex(Point3<double>(-1, -t, 0));
	addVertex(Point3<double>(1, -t, 0));
	addVertex(Point3<double>(0, -1, t));
	addVertex(Point3<double>(0, 1, t));
	addVertex(Point3<double>(0, -1, -t));
	addVertex(Point3<double>(0, 1, -t));
	addVertex(Point3<double>(t, 0, -1));
	addVertex(Point3<double>(t, 0, 1));
	addVertex(Point3<double>(-t, 0, -1));
	addVertex(Point3<double>(-t, 0, 1));

	// 5 faces around point 0
	addTriangle(0, 11, 5);
	addTriangle(0, 5, 1);
	addTriangle(0, 1, 7);
	addTriangle(0, 7, 10);
	addTriangle(0, 10, 11);

	// 5 adjacent faces
	addTriangle(1, 5, 9);
	addTriangle(5, 11, 4);
	addTriangle(11, 10, 2);
	addTriangle(10, 7, 6);
	addTriangle(7, 1, 8);

	// 5 faces around point 3
	addTriangle(3, 9, 4);
	addTriangle(3, 4, 2);
	addTriangle(3, 2, 6);
	addTriangle(3, 6, 8);
	addTriangle(3, 8, 9);

	// 5 adjacent faces
	addTriangle(4, 9, 5);
	addTriangle(2, 4, 11);
	addTriangle(6, 2, 10);
	addTriangle(8, 6, 7);
	addTriangle(9, 8, 1);

	_indicesList = _temporaryIndicesList;
	_temporaryIndicesList.clear();
}

unsigned int IcoSphere::getMiddlePoint(	const unsigned int & indexP1,
										const unsigned int & indexP2) {
	// first check if we have it already
	bool firstIsSmaller = indexP1 < indexP2;
	uint64_t smallerIndex = firstIsSmaller ? indexP1 : indexP2;
	uint64_t greaterIndex = firstIsSmaller ? indexP2 : indexP1;
	uint64_t key = (smallerIndex << 32) + greaterIndex;

	map<uint64_t, unsigned int>::const_iterator it = _middlePointsIndexCache.find(key);
	if (it != _middlePointsIndexCache.end()) {
		return it->second;
	}
	// not in cache, calculate it
	Point3<double> middlePoint = (_vertexList[indexP1] + _vertexList[indexP2]) / 2.;
	addVertex(middlePoint);
	_middlePointsIndexCache[key] = _vertexList.size() - 1;
	return _vertexList.size() - 1;
}

void IcoSphere::sudivide() {
	for (unsigned int triangleIndex = 0; triangleIndex < _indicesList.size(); triangleIndex += 3) {
		unsigned int a = getMiddlePoint(_indicesList[triangleIndex], _indicesList[triangleIndex + 1]);
		unsigned int b = getMiddlePoint(_indicesList[triangleIndex + 1], _indicesList[triangleIndex + 2]);
		unsigned int c = getMiddlePoint(_indicesList[triangleIndex + 2], _indicesList[triangleIndex]);

		addTriangle(_indicesList[triangleIndex], a, c);
		addTriangle(_indicesList[triangleIndex + 1], b, a);
		addTriangle(_indicesList[triangleIndex + 2], c, b);
		addTriangle(a, b, c);
	}
	_indicesList = _temporaryIndicesList;
	_temporaryIndicesList.clear();
}
