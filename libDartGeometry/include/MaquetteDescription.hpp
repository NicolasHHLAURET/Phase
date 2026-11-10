/*
 * MaquetteDescription.hpp
 *
 *  Created on: 25 juin 2010
 *      Author: lauretn
 */

#ifndef MAQUETTEDESCRIPTION_HPP_
#define MAQUETTEDESCRIPTION_HPP_

#include "Point3.hpp"
#include "Point2.hpp"

class MaquetteDescription {
private:
	Triplet<double> _dimensionsCellules;
	Triplet<unsigned int> _nbsCellules;
	Triplet<double> _sceneDimensions;

public:
	MaquetteDescription(const Triplet<double> & dimensionsCellules = Triplet<double>(1, 1, 1),
						const Triplet<unsigned int> & nbsCellules = Triplet<unsigned int>(1, 1, 1)) :
			_dimensionsCellules(dimensionsCellules), _nbsCellules(nbsCellules) {
		updateSceneDimensions();
	}
	
	// Accesseurs
	const Triplet<double> & getDimensionsCellules() const {
		return _dimensionsCellules;
	}
	const Triplet<unsigned int> & getNbsCellule() const {
		return _nbsCellules;
	}
	const unsigned int getNbLignes() const {
		return _nbsCellules.getX();
	}
	const unsigned int getNbColonnes() const {
		return _nbsCellules.getY();
	}
	const unsigned int getNbAltitudes() const {
		return _nbsCellules.getZ();
	}
	const Triplet<double> & getSceneDimensions() const {
		return _sceneDimensions;
	}
	
	// Modifieurs
	void setDimensionsCellules(const Triplet<double> & dimensionsCellules) {
		_dimensionsCellules = dimensionsCellules;
	}
	void setNbsCellule(const Triplet<unsigned int> & nbsCellules) {
		_nbsCellules = nbsCellules;
	}
	void setSceneDimensions(const Triplet<double> & sceneDimensions) {
		_sceneDimensions = sceneDimensions;
	}
	void setSceneHeight(const double & sceneHeight) {
		_sceneDimensions.setZ(sceneHeight);
	}
	void subdivise(unsigned int subdivision) {
		_dimensionsCellules /= subdivision;
		_nbsCellules *= subdivision;
	}

	void setDimensionsAndNumberOfCells(const Triplet<double> & dimensionsCellules, const Triplet<unsigned int> & nbsCellules) {
		setNbsCellule(nbsCellules);
		setDimensionsCellules(dimensionsCellules);
		updateSceneDimensions();
	}
	void setSceneDimensionsAndNumberOfCells(const Triplet<double> & sceneDimensions, const Triplet<unsigned int> & nbsCellules) {
		setNbsCellule(nbsCellules);
		setSceneDimensions(sceneDimensions);
		updateCellDimensions();
	}
	void setSceneAndCellDimensions(const Triplet<double> & sceneDimensions, const Triplet<double> & dimensionsCellules) {
		setDimensionsCellules(dimensionsCellules);
		setSceneDimensions(sceneDimensions);
		updateCellNumber();
	}
	void updateSceneHeight(const double & sceneHeight) {
		_nbsCellules.setZ(ceil(sceneHeight / _dimensionsCellules.getZ()));
		_sceneDimensions.setZ(_nbsCellules.getZ() * _dimensionsCellules.getZ());
	}
	void updateNbCellHeight(const unsigned & nbCellZ) {
		_nbsCellules.setZ(nbCellZ);
		_sceneDimensions.setZ(nbCellZ * _dimensionsCellules.getZ());
	}
	
	// Methodes
	const Point3<int> getIndiceCelluleDepuisCoordonneesAbsolues(const Point3<double> & positionPoint) const {
		Point3<int> indices = positionPoint / _dimensionsCellules;
		if (positionPoint.getX() < 0) {
			indices.setX(indices.getX() - 1);
		}
		if (positionPoint.getY() < 0) {
			indices.setY(indices.getY() - 1);
		}
		if (positionPoint.getZ() < 0) {
			indices.setZ(indices.getZ() - 1);
		}
		return indices;
	}
	
	void updateSceneDimensions() {
		_sceneDimensions = _dimensionsCellules * _nbsCellules;
	}
	void updateCellDimensions() {
		_dimensionsCellules = _sceneDimensions / _nbsCellules;
	}
	void updateCellNumber() {
		_nbsCellules.setX(ceil(_sceneDimensions.getX() / _dimensionsCellules.getX()));
		_nbsCellules.setY(ceil(_sceneDimensions.getY() / _dimensionsCellules.getY()));
		_nbsCellules.setZ(ceil(_sceneDimensions.getZ() / _dimensionsCellules.getZ()));
		updateSceneDimensions();
	}

	const string toString() const {
		string res = "Maquette:{dimension:";
		res.append(_dimensionsCellules.toString());
		res.append(" ; NbCells:");
		res.append(_nbsCellules.toString());
		res.append("}");
		return res;
	}
};

#endif /* MAQUETTEDESCRIPTION_HPP_ */
