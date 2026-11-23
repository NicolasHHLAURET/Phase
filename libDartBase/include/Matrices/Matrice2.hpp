/*
 * Matrice2.hpp
 *
 *  Created on: 25 janv. 2010
 *      Author: goulierv
 */

#ifndef MATRICE2_HPP_
#define MATRICE2_HPP_

#include <vector>

using namespace std;

/**
 * Cette classe decrit une matrice a 2 dimensions de taille variable
 */
template<typename T>
class Matrice2 {
public:
	Matrice2() {}
	;
	inline const Matrice2<T>& redimensionner(	int x,
												int y) {
		contenu.resize(x, vector<T>(y));
		return *this;
	}
	inline const Matrice2<T>& initialiser(T val) {
		for (int i = 0; i < getSizeX(); ++i) {
			for (int j = 0; j < getSizeY(); ++j) {
				contenu[i][j] = val;
			}
		}
		return *this;
	}
	inline const Matrice2<T>& liberer() {
		contenu.resize(0);
		return *this;
	}
	inline T& get(	int x,
					int y) {
		return contenu[x][y];
	}
	inline const T& get(int x,
						int y) const {
		return contenu[x][y];
	}
	inline const Matrice2<T>& set(	int x,
									int y,
									T val) {
		contenu[x][y] = val;
		return *this;
	}
	inline const Matrice2<T>& incrementer(	int x,
											int y,
											T val) {
		contenu[x][y] += val;
		return *this;
	}
	inline const Matrice2<T>& decrementer(	int x,
											int y,
											T val) {
		contenu[x][y] -= val;
		return *this;
	}
	inline bool isEmpty() {
		return contenu.empty();
	}
	;
	inline const int getSizeX() const {
		return contenu.size();
	}
	inline const int getSizeY() const {
		return contenu[0].size();
	}
private:
	vector<vector<T> > contenu;
};

#endif /* MATRICE2_HPP_ */
