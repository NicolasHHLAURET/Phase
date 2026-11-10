/*
 * Spline2D_interp.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: yint
 */

#ifndef SPLINE2D_INTERP_HPP_
#define SPLINE2D_INTERP_HPP_

#include "Spline_interp.hpp"

struct Spline2D_interp {
	//Object for two-dimensional cubic spline interpolation on a matrix. Construct with a vector of x1
	//values, a vector of x2 values, and a matrix of tabulated function values yij . Then call interp
	//for interpolated values.
	int m, n;
	std::vector<std::vector<double> > y;
	std::vector<double> x1;
	std::vector<double> x2;
	std::vector<double> yv;
	std::vector<Spline_interp> srp;
	Spline2D_interp() {

	}
	
	Spline2D_interp(const Spline2D_interp & tmp) {
		m = tmp.m;
		n = tmp.n;
		y = tmp.y;
		x1 = tmp.x1;
		x2 = tmp.x2;
		yv = tmp.yv;
		srp = tmp.srp;
	}
	
	Spline2D_interp & operator =(const Spline2D_interp & tmp) {
		m = tmp.m;
		n = tmp.n;
		y = tmp.y;
		x1 = tmp.x1;
		x2 = tmp.x2;
		yv = tmp.yv;
		srp = tmp.srp;
		return *this;
	}
	
	Spline2D_interp(std::vector<double> &x1v,
					std::vector<double> &x2v,
					std::vector<std::vector<double> > &ym) :
			m(x1v.size()), n(x2v.size()), y(ym), x1(x1v), x2(x2v), yv(m), srp(m) {
		for (int i = 0; i < m; i++)
			srp[i] = Spline_interp(x2, y[i]);
		//Save an array of pointers to 1-dim row splines.
	}
	~Spline2D_interp() {

	}
	//We need a destructor to clean up.
	double interp(	double x1p,
					double x2p) {
		for (int i = 0; i < m; i++)
			yv[i] = (srp[i]).interp(x2p);
		//Interpolate on each row.
		Spline_interp scol(x1, yv);
		//Construct the column spline, and evaluate it.
		return scol.interp(x1p);
	}
};

#endif /* SPLINE2D_INTERP_HPP_ */
