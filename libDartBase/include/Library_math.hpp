/*
 * Lib_math.hpp
 *
 *  Created on: 28 dec. 2009
 *      Author: dounaevae
 */

#ifndef LIBRARY_MATH_HPP_
#define LIBRARY_MATH_HPP_

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;

class Library_math {
public:
	Library_math();

	virtual ~Library_math();

	/**
	 * Computes the given matrix sum. The matrix need to have the same size.
	 * @param p_matrix1 the first operand
	 * @param p_matrix2 the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixSum(const vector<double> &p_matrix1,
									const vector<double> &p_matrix2) {
		int height;
		if ((p_matrix1.size() != p_matrix2.size()) || (p_matrix1.size() == 0)) {
			cerr << "Erreur dans le calcul des matrices (matrixSum (const vector<double>, const vector<double>)" << endl;
			exit(1);
		} else {
			height = p_matrix1.size();
		}
		vector<double> sum(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			if ((p_matrix1[i] == (-1.0)) || (p_matrix2[i] == (-1.0)))
				sum[i] = -1.0;
			else
				sum[i] = p_matrix1[i] + p_matrix2[i];
		}
		
		return sum;
	}
	
	/**
	 * Computes the sum of the given matrix and the given constant.
	 * @param p_matrix the first operand
	 * @param p_constant the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixSum(const vector<double> &p_matrix,
									const double &p_constant) {
		int height;
		if (p_matrix.size() == 0) {
			cerr << "Erreur dans le calcul des matrices (matrixSum(const vector<double>, const double))" << endl;
			exit(1);
		} else {
			height = p_matrix.size();
		}
		vector<double> sum(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			if (p_matrix[i] == (-1.0))
				sum[i] = -1.0;
			else
				sum[i] = p_matrix[i] + p_constant;
		}
		
		return sum;
	}
	/**
	 * Computes the given matrix product. The matrix need to have the same size.
	 * @param p_matrix1 the first operand
	 * @param p_matrix2 the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixProduct(const vector<double> &p_matrix1,
										const vector<double> &p_matrix2) {
		int height;
		if ((p_matrix1.size() != p_matrix2.size()) || (p_matrix1.size() == 0)) {
			cerr << "Erreur dans le calcul des matrices (matrixProduct(const vector<double>, const vector<double>))" << endl;
			exit(1);
		} else {
			height = p_matrix1.size();
		}
		vector<double> product(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			if ((p_matrix1[i] == (-1.0) || (p_matrix2[i] == (-1.0))))
				product[i] = -1.0;
			else
				product[i] = p_matrix1[i] * p_matrix2[i];
		}
		
		return product;
	}
	
	/**
	 * Computes the product of the given matrix by the given constant.
	 * @param p_matrix the first operand
	 * @param p_constant the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixProduct(const vector<double> &p_matrix,
										const double &p_constant) {
		int height;
		if (p_matrix.size() == 0) {
			cerr << "Erreur dans le calcul des matrices (matrixProduct(const vector<double>, const double))" << endl;
			exit(1);
		} else {
			height = p_matrix.size();
		}
		vector<double> product(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			if (p_matrix[i] == (-1.0))
				product[i] = -1.0;
			else
				product[i] = p_matrix[i] * p_constant;
		}
		
		return product;
	}
	
	/**
	 * Computes the given matrix division. The matrix need to have the same size.
	 * @param p_matrix1 the first operand
	 * @param p_matrix2 the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixDivision(	const vector<double> &p_matrix1,
											const vector<double> &p_matrix2) {
		int height;
		if ((p_matrix1.size() != p_matrix2.size()) || (p_matrix1.size() == 0)) {
			cerr << "Erreur dans le calcul des matrices (matrixDivision(const vector<double>, const vector<double>))" << endl;
			exit(1);
		} else {
			height = p_matrix1.size();
		}
		vector<double> division(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			
			// Be careful to the division by zero !
			if (p_matrix2[i] == 0) {
				division[i] = 0;
			} else {
				if ((p_matrix1[i] == (-1.0) || (p_matrix2[i] == (-1.0))))
					division[i] = -1.0;
				else
					division[i] = p_matrix1[i] / p_matrix2[i];
			}
			
		}
		
		return division;
	}
	/**
	 * Computes the division of the given constant by the given matrix.
	 * @param p_constant the first operand
	 * @param p_matrix the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixDivision(	const double &p_constant,
											const vector<double> &p_matrix) {
		int height;
		if (p_matrix.size() == 0) {
			cerr << "Erreur dans le calcul des matrices (matrixDivision(const double, const vector<double>))" << endl;
			exit(1);
		} else {
			height = p_matrix.size();
		}
		vector<double> division(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			// Be careful to the division by zero !
			if (p_matrix[i] == 0) {
				division[i] = 0;
			} else {
				if (p_matrix[i] == (-1.0))
					division[i] = -1.0;
				else
					division[i] = p_constant / p_matrix[i];
			}
		}
		
		return division;
	}
	/**
	 * Computes the division of the given matrix by the given constant.
	 * @param p_matrix the first operand
	 * @param p_constant the second operand
	 * @return the result matrix
	 */
	static vector<double> matrixDivision(	const vector<double> &p_matrix,
											const double &p_constant) {
		int height;
		if (p_matrix.size() == 0) {
			cerr << "Erreur dans le calcul des matrices (matrixDivision(const vector<double>, const double))" << endl;
			exit(1);
		} else {
			height = p_matrix.size();
		}
		vector<double> division(height);	// The returned matrix
			
		for (int i = 0; i < height; i++) {
			// Be careful to the division by zero !
			if (p_constant == 0) {
				division[i] = 0;
			} else {
				if (p_matrix[i] == (-1.0))
					division[i] = -1.0;
				else
					division[i] = p_matrix[i] / p_constant;
			}
		}
		
		return division;
	}
	
	/**
	 * Computes the given matrix exponential.
	 * @param p_matrix the matrix operand
	 * @return the result matrix
	 */
	static vector<double> matrixExp(vector<double> &p_matrix) {
		vector<double> expMatrix;	// The returned matrix
		
		for (unsigned int i = 0; i < p_matrix.size(); i++) {
			if (p_matrix[i] == (-1.0))
				expMatrix.push_back(-1.0);
			else
				expMatrix.push_back(exp(p_matrix[i]));
		}
		return expMatrix;
	}
	
	static void copyVectorToMatrix(	const vector<double> &vecteur,
									vector<vector<double> > &matrice,
									const int &hauteur,
									const int &largeur) {
		int index = 0;
		for (int i = 0; i < hauteur; ++i) {
			vector<double> sousMatrice;
			for (unsigned int j = 0; j < (vecteur.size() / hauteur); ++j) {
				sousMatrice.push_back(vecteur[index]);
				++index;
			}
			matrice.push_back(sousMatrice);
		}
	}
	
	static vector<double> convertMatrixToVector(const vector<vector<double> > &matrice) {
		vector<double> vecteur;
		for (unsigned int i = 0; i < matrice.size(); ++i) {
			vector<double> sousMatrice = matrice[i];
			for (unsigned int j = 0; j < sousMatrice.size(); ++j) {
				vecteur.push_back(sousMatrice[j]);
			}
		}
		return vecteur;
	}
	
	static vector<vector<double> > convertVectorToMatrix(	const vector<double> &vecteur,
															const int &hauteur) {
		vector<vector<double> > matrice;
		int index = 0;
		for (int i = 0; i < hauteur; ++i) {
			vector<double> sousMatrice;
			for (unsigned int j = 0; j < (vecteur.size() / hauteur); ++j) {
				sousMatrice.push_back(vecteur[index]);
				++index;
			}
			matrice.push_back(sousMatrice);
		}
		return matrice;
	}
	
	/**
	 * @return the minimum value of the given matrix
	 * @param p_matrix the matrix
	 */
	static double getMatrixMin(const vector<vector<double> > &p_matrix) {
		double min = p_matrix[0][0];		// The returned minimum value
		
		for (unsigned int i = 0; i < p_matrix.size(); i++) {
			for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
				if (p_matrix[i][j] < min) {
					min = p_matrix[i][j];
				}
			}
		}
		
		return min;
	}
	
	/**
	 * @return the maximum value of the given matrix
	 * @param p_matrix the matrix
	 */
	static double getMatrixMax(const vector<vector<double> > &p_matrix) {
		double max = p_matrix[0][0];		// The returned maximum value
		
		for (unsigned int i = 0; i < p_matrix.size(); i++) {
			for (unsigned int j = 0; j < p_matrix[i].size(); j++) {
				if (p_matrix[i][j] > max) {
					max = p_matrix[i][j];
				}
			}
		}
		
		return max;
	}
	
	static vector<double> matrixLog(const vector<double> &p_matrix) {
		vector<double> logMatrix;	// The returned matrix
		
		for (unsigned int i = 0; i < p_matrix.size(); ++i) {
			if (p_matrix[i] == (-1.0))
				logMatrix.push_back(-1.0);
			else
				logMatrix.push_back(log(p_matrix[i]));
		}
		
		return logMatrix;
	}
};

#endif /* LIBRARY_MATH_HPP_ */
