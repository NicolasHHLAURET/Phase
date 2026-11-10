/*
 * Spline_interp.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: yint
 */

#ifndef SPLINE_INTERP_HPP_
#define SPLINE_INTERP_HPP_
#include "Base_interp.hpp"

struct Spline_interp: Base_interp
//Cubic spline interpolation object. Construct with x and y vectors, and (optionally) values of
//the ﬁrst derivative at the endpoints, then call interp for interpolated values.
{
	std::vector<double> y2;
	Spline_interp() {}
	
	Spline_interp(const Spline_interp & tmp) :
			Base_interp(tmp) {
		y2 = tmp.y2;
	}
	
	Spline_interp & operator =(const Spline_interp & tmp) {
		Base_interp::operator=(tmp);
		y2 = tmp.y2;
		return *this;
	}
	
	Spline_interp(	std::vector<double> &xv,
					std::vector<double> &yv,
					double yp1 = 1.e99,
					double ypn = 1.e99) :
			Base_interp(xv, yv, 2), y2(xv.size()) {
		sety2(xv, yv, yp1, ypn);
	}
	void sety2(	std::vector<double> &xv,
				std::vector<double> &yv,
				double yp1,
				double ypn);
	double rawinterp(	int jl,
						double xv);
};

inline void Spline_interp::sety2(	std::vector<double> &xv,
									std::vector<double> &yv,
									double yp1,
									double ypn)
//This routine stores an array y2[0..n-1] with second derivatives of the interpolating function
//at the tabulated points pointed to by xv, using function values pointed to by yv. If yp1 and/or
//ypn are equal to 1 1099 or larger, the routine is signaled to set the corresponding boundary
//condition for a natural spline, with zero second derivative on that boundary; otherwise, they are
//the values of the ﬁrst derivatives at the endpoints.
									{
	int i, k;
	double p, qn, sig, un;
	int n = y2.size();
	std::vector<double> u(n - 1);
	if (yp1 > 0.99e99)
		//The lower boundary condition is set either to be “nat-ural” or else to have a speciﬁed ﬁrst derivative.
		y2[0] = u[0] = 0.0;
	else {
		y2[0] = -0.5;
		u[0] = (3.0 / (xv[1] - xv[0])) * ((yv[1] - yv[0]) / (xv[1] - xv[0]) - yp1);
	}
	for (i = 1; i < n - 1; i++) {
		//This is the decomposition loop of the tridiagonal al-gorithm. y2 and u are used for tem-
		//	porary storage of the decomposed factors.
		sig = (xv[i] - xv[i - 1]) / (xv[i + 1] - xv[i - 1]);
		
		p = sig * y2[i - 1] + 2.0;
		
		y2[i] = (sig - 1.0) / p;
		
		u[i] = (yv[i + 1] - yv[i]) / (xv[i + 1] - xv[i]) - (yv[i] - yv[i - 1]) / (xv[i] - xv[i - 1]);
		u[i] = (6.0 * u[i] / (xv[i + 1] - xv[i - 1]) - sig * u[i - 1]) / p;
	}
	if (ypn > 0.99e99)
		//The upper boundary condition is set either to be “natural” or else to have a speciﬁed ﬁrst derivative.
		qn = un = 0.0;
	
	else {
		
		qn = 0.5;
		un = (3.0 / (xv[n - 1] - xv[n - 2])) * (ypn - (yv[n - 1] - yv[n - 2]) / (xv[n - 1] - xv[n - 2]));
	}
	y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0);
	for (k = n - 2; k >= 0; k--)
		//This is the backsubstitution loop of the tridiagonal algorithm.
		y2[k] = y2[k] * y2[k + 1] + u[k];
	
}

inline double Spline_interp::rawinterp(	int jl,
										double x)
//Given a value x, and using pointers to data xx and yy, and the stored vector of second derivatives
//y2, this routine returns the cubic spline interpolated value y.
										{
	int klo = jl, khi = jl + 1;
	double y, h, b, a;
	h = xx[khi] - xx[klo];
	if (h == 0.0)
		throw("Bad input to routine splint");
	//The xa’s must be distinct.
	a = (xx[khi] - x) / h;
	b = (x - xx[klo]) / h;
	//Cubic spline polynomial is now evaluated.
	y = a * yy[klo] + b * yy[khi] + ((a * a * a - a) * y2[klo] + (b * b * b - b) * y2[khi]) * (h * h) / 6.0;
	return y;
}

#endif /* SPLINE_INTERP_HPP_ */
