/*
 * Base_interp.hpp
 *
 *  Created on: Mar 7, 2013
 *      Author: yint
 */

#ifndef BASE_INTERP_HPP_
#define BASE_INTERP_HPP_

#include<iostream>
#include "math.h"
#include<vector>

struct Base_interp
//Abstract base class used by all interpolation routines in this chapter. Only the routine interp
//is called directly by the user.
{
	int n, mm, jsav, cor, dj;
	std::vector<double> xx, yy;
	Base_interp() {}
	Base_interp(const Base_interp & tmp) :
			xx(tmp.xx), yy(tmp.yy) {
		n = tmp.n;
		mm = tmp.mm;
		jsav = tmp.jsav;
		cor = tmp.cor;
		dj = tmp.dj;
	}
	
	Base_interp & operator =(const Base_interp & tmp) {
		n = tmp.n;
		mm = tmp.mm;
		jsav = tmp.jsav;
		cor = tmp.cor;
		dj = tmp.dj;
		xx = tmp.xx;
		yy = tmp.yy;
		return *this;
	}
	
	Base_interp(std::vector<double> &x,
				std::vector<double> &y,
				int m) :
			n(x.size()), mm(m), jsav(0), cor(0), xx(x), yy(y) {
		dj = min(1, (int) pow((double) n, 0.25));
	} //Constructor: Set up for interpolating on a table of x’s and y’s of length m. Normally called
	  //by a derived class, not by the user.
	
	double interp(double x) {
		//Given a value x, return an interpolated value, using data pointed to by xx and yy.
		int jlo = cor ? hunt(x) : locate(x);
		return rawinterp(jlo, x);
	}
	int locate(const double x);
	int hunt(const double x);
	//See deﬁnitions below.
	double virtual rawinterp(	int jlo,
								double x) = 0;
	//Derived classes provide this as the actual interpolation method.
};

inline int Base_interp::locate(const double x)
//Given a value x, return a value j such that x is (insofar as possible) centered in the subrange
//xx[j..j+mm-1], where xx is the stored pointer. The values in xx must be monotonic, either
//increasing or decreasing. The returned value is not less than 0, nor greater than n-1.
{
	int ju, jm, jl;
	if (n < 2 || mm < 2 || mm > n)
		throw("locate size error");
	bool ascnd = (xx[n - 1] >= xx[0]);
	//True if ascending order of table, false otherwise.
	jl = 0;
	//Initialize lower
	ju = n - 1;
	//and upper limits.
	while (ju - jl > 1) {
		//If we are not yet done,
		jm = (ju + jl) >> 1;
		//compute a midpoint,
		if ((x >= xx[jm]) == ascnd)
			jl = jm;
		//and replace either the lower limit
		else
			ju = jm;
		//or the upper limit, as appropriate.
	}
	//Repeat until the test condition is satisﬁed.
	cor = abs(jl - jsav) > dj ? 0 : 1;
	//Decide whether to use hunt or locate next time.
	jsav = jl;
	return max(0, min(n - mm, jl - ((mm - 2) >> 1)));
}

inline int Base_interp::hunt(const double x)
//Given a value x, return a value j such that x is (insofar as possible) centered in the subrange
//xx[j..j+mm-1], where xx is the stored pointer. The values in xx must be monotonic, either
//increasing or decreasing. The returned value is not less than 0, nor greater than n-1.
{
	int jl = jsav, jm, ju, inc = 1;
	if (n < 2 || mm < 2 || mm > n)
		throw("hunt size error");
	bool ascnd = (xx[n - 1] >= xx[0]);
	//True if ascending order of table, false otherwise.
	if (jl < 0 || jl > n - 1) {
		//Input guess not useful. Go immediately to bisec-tion.
		jl = 0;
		ju = n - 1;
	} else {
		if ((x >= xx[jl]) == ascnd) {
			//Hunt up:
			for (;;) {
				ju = jl + inc;
				if (ju >= n - 1) {
					ju = n - 1;
					break;
				}
				//Oﬀ end of table.
				else if ((x < xx[ju]) == ascnd)
					break;
				//Found bracket.
				else {
					//Not done, so double the increment and try again.
					jl = ju;
					inc += inc;
				}
			}
		} else {
			//Hunt down:
			ju = jl;
			for (;;) {
				jl = jl - inc;
				if (jl <= 0) {
					jl = 0;
					break;
				}
				//Oﬀ end of table.
				else if ((x >= xx[jl]) == ascnd)
					break;
				//Found bracket.
				else {
					//Not done, so double the increment and try again.
					ju = jl;
					inc += inc;
				}
			}
		}
	}
	while (ju - jl > 1) {
		//Hunt is done, so begin the ﬁnal bisection phase:
		jm = (ju + jl) >> 1;
		if ((x >= xx[jm]) == ascnd)
			jl = jm;
		else
			ju = jm;
	}
	cor = abs(jl - jsav) > dj ? 0 : 1;
	//Decide whether to use hunt or locate next time.
	jsav = jl;
	return max(0, min(n - mm, jl - ((mm - 2) >> 1)));
}

#endif /* BASE_INTERP_HPP_ */
