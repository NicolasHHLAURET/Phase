/*
 * Matrice3.hpp
 *
 *  Created on: 20 janv. 2010
 *      Author: goulierv
 */

#ifndef MATRICE3_HPP_
#define MATRICE3_HPP_

#include <vector>

using namespace std;

/**
 * Cette classe décrit une matrice à 3 dimensions de taille variable
 */
template<typename T>
class Matrice3 {
public:
	Matrice3() {}
	;
	/** redimensionnement */
	inline const Matrice3<T>& redimensionner(	int x,
												int y,
												int z) {
		contenu.resize(x);
		for (int i = 0; i < x; ++i) {
			contenu[i].resize(y);
			for (int j = 0; j < y; ++j)
				contenu[i][j].resize(z);
		}
		return *this;
	}
	/** redimensionnement selon Z */
	inline const Matrice3<T>& resizeZ(int sizeZ) {
		const int x = getSizeX();
		const int y = getSizeY();
		const int z = getSizeZ();
		for (int i = 0; i < x; ++i) {
			for (int j = 0; j < y; ++j) {
				for (int k = 0; k < z; ++k) {
					contenu[x][y].resize(sizeZ);
				}
			}
		}
		return *this;
	}
	/** initialisation de la matrice */
	inline const Matrice3<T>& initialiser(T& val) {
		const int x = getSizeX();
		const int y = getSizeY();
		const int z = getSizeZ();
		
		for (int i = 0; i < x; ++i) {
			for (int j = 0; j < y; ++j) {
				for (int k = 0; k < z; ++k) {
					contenu[i][j][k] = val;
				}
			}
		}
		return *this;
	}
	/** liberation de la matrice */
	inline const Matrice3<T>& liberer() {
		contenu.clear();
		return *this;
	}
	/** acces par reference a un element */
	inline T& get(	int x,
					int y,
					int z) {
		return contenu[x][y][z];
	}
	inline const int getSizeX() {
		return contenu.size();
	}
	inline const int getSizeY() {
		return contenu[0].size();
	}
	inline const int getSizeZ() {
		return contenu[0][0].size();
	}
	inline const Matrice3<T>& set(	int x,
									int y,
									int z,
									T val) {
		if (contenu[x][y].size() <= (unsigned int) z) {
			resizeZ(z + 1);
		}
		contenu[x][y][z] = val;
		return *this;
	}
private:
	vector<vector<vector<T> > > contenu;
};

#endif /* MATRICE3_HPP_ */
