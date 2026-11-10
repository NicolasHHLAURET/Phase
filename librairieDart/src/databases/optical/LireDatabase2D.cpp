/*
 * LireDatabase2D.cpp
 *
 *  Created on: 25 sept. 2014
 *      Author: guilleuxj
 */


#include "../include/databases/optical/LireDatabase2D.hpp"
using namespace std;

	std::map<LireDatabase2D::Key,double,LireDatabase2D::compare> LireDatabase2D::_reflectances = std::map<LireDatabase2D::Key,double,LireDatabase2D::compare>();

	bool LireDatabase2D::hasReflectanceInMemory(const Key &key) {
		return _reflectances.find(key) != _reflectances.end();
	}

	double LireDatabase2D::getReflectanceInMemory(const Key &key) {
		return _reflectances.find(key)->second;
	}

	double LireDatabase2D::setReflectanceInMemory(const Key &key, const double &value) {
		return _reflectances[key] = value;
	}
