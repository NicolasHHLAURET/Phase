/*
 * LUdcmp.h
 *
 *  Created on: Oct 3, 2012
 *      Author: yint
 *      From Numerical Recipes: LUDecomposition to solve a matrix.
 */

#ifndef LUDCMP_H_
#define LUDCMP_H_

#include<iostream>
#include<vector>
#include "math.h"

using namespace std;

class LUdcmp {
	unsigned int n;
	vector<vector<double> > lu; // Stores the decomposition.
	vector<vector<double> > &aref; // *******not implemented.
	vector<int> indx; //Stores the permutation.
	double d; //Used by det.
public:
	LUdcmp(vector<vector<double> > &a) :
		n(a.size()), lu(a), aref(a), indx(n) {
		//	Given a matrix a[0..n-1][0..n-1], this routine replaces it by the LU decomposition of a
		//	rowwise permutation of itself. a is input. On output, it is arranged as in equation (2.3.14)
		//	above; indx[0..n-1] is an output vector that records the row permutation eﬀected by the
		//	partial pivoting; d is output as ˙1 depending on whether the number of row interchanges
		//	was even or odd, respectively. This routine is used in combination with solve to solve linear
		//	equations or invert a matrix.

		const double TINY = 1.0e-40; //A small number.
		unsigned int i, j;
		int imax = 0;
		int k = 0;
		double big;
		double temp;
		vector<double> vv(n, 0); //vv stores the implicit scaling of each row.
		d = 1.0; //No row interchanges yet.
		for (i = 0; i < n; i++) { //Loop over rows to get the implicit scaling information.
			big = 0.0;
			for (j = 0; j < n; j++)
				if ((temp = fabs(lu[i][j])) > big)
					big = temp;
			if (big == 0.0)
				throw("Singular matrix in LUdcmp"); //No nonzero largest element.
			vv[i] = 1.0 / big; //Save the scaling.
		}
		for (k = 0; k < int(n); k++) { //This is the outermost kij loop.
			big = 0.0; //Initialize for the search for largest pivot element.
			for (i = k; i < n; i++) {
				temp = vv[i] * fabs(lu[i][k]);
				if (temp > big) { //Is the ﬁgure of merit for the pivot better than
					big = temp; //the best so far?
					imax = i;
				}
			}
			if (k != imax) { //Do we need to interchange rows?
				for (j = 0; j < n; j++) { //Yes, do so...
					temp = lu[imax][j];
					lu[imax][j] = lu[k][j];
					lu[k][j] = temp;
				}
				d = -d; //...and change the parity of d.
				vv[imax] = vv[k]; //Also interchange the scale factor.
			}
			indx[k] = imax;
			if (lu[k][k] == 0.0)
				lu[k][k] = TINY;
			//	If the pivot element is zero, the matrix is singular (at least to the precision of the
			//	algorithm). For some applications on singular matrices, it is desirable to substitute
			//	TINY for zero.
			for (i = k + 1; i < n; i++) {
				temp = lu[i][k] /= lu[k][k]; //Divide by the pivot element.
				for (j = k + 1; j < n; j++) //Innermost loop: reduce remaining submatrix.
					lu[i][j] -= temp * lu[k][j];
			}
		}

	}
	void solve(	vector<double> &b,
				vector<double> &x) //Solve for a single right-hand side.
	//Solves the set of n linear equations A x D b using the stored LU decomposition of A.
	//b[0..n-1] is input as the right-hand side vector b, while x returns the solution vector x; b and
	//x may reference the same vector, in which case the solution overwrites the input. This routine
	//takes into account the possibility that b will begin with many zero elements, so it is eﬃcient for
	//use in matrix inversion.
	{
		int i, ii = 0, ip, j;
		double sum;
		if (b.size() != n || x.size() != n)
			throw("LUdcmp::solve bad sizes");
		for (i = 0; i < int(n); i++)
			x[i] = b[i];
		for (i = 0; i < int(n); i++) {
			//When ii is set to a positive value, it will become the
			//index of the ﬁrst nonvanishing element of b. We now
			//do the forward substitution, equation (2.3.6). The
			//only new wrinkle is to unscramble the permutation as we go.
			ip = indx[i];
			sum = x[ip];
			x[ip] = x[i];
			if (ii != 0)
				for (j = ii - 1; j < i; j++)
					sum -= lu[i][j] * x[j];
			else if (sum != 0.0) //A nonzero element was encountered, so from now on we
				ii = i + 1; //will have to do the sums in the loop above.
			x[i] = sum;
		}
		for (i = n - 1; i >= 0; i--) {
			//Now we do the backsubstitution, equation (2.3.7).
			sum = x[i];
			for (j = i + 1; j < int(n); j++)
				sum -= lu[i][j] * x[j];
			x[i] = sum / lu[i][i]; //Store a component of the solution vector X .
		} //All done!
	}

	void solve(	vector<vector<double> > &b,
				vector<vector<double> > &x) //Solve for multiple right-hand sides.
	{
		//Solves m sets of n linear equations A X D B using the stored LU decomposition of A. The
		//matrix b[0..n-1][0..m-1] inputs the right-hand sides, while x[0..n-1][0..m-1] returns the
		//solution A 1 B. b and x may reference the same matrix, in which case the solution overwrites
		//the input.

		int i, j, m = b[0].size();
		if (b.size() != n || x.size() != n || b[0].size() != x[0].size())
			throw("LUdcmp::solve bad sizes");
		vector<double> xx(n, 0);
		for (j = 0; j < m; j++) {
			//Copy and solve each column in turn.
			for (i = 0; i < int(n); i++)
				xx[i] = b[i][j];
			solve(xx, xx);
			for (i = 0; i < int(n); i++)
				x[i][j] = xx[i];
		}
	}
	//	void inverse(vector<vector<double> > &ainv); //inverse of matrix.
	double det() //determinant.
	//Using the stored LU decomposition, return the determinant of the matrix A.
	{
		double dd = d;
		for (unsigned int i = 0; i < n; i++)
			dd *= lu[i][i];
		return dd;
	}
	void mprove(vector<double> &b,
				vector<double> &x); // *****not implemented.
};

#endif /* LUDCMP_H_ */
