/*
 * EquSystemsBase.h
 *
 *  Created on: Oct 9, 2012
 *      Author: yint
 *
 *      The base class for a system of equations
 */

#ifndef EQUSYSTEMSBASE_H_
#define EQUSYSTEMSBASE_H_

#include <vector>
#include <limits>
#include "math.h"
#include "LUdcmp.h"

struct EquSystemsBase {
	virtual vector<double> fvec(vector<double> const& x) { //Calculate the values of the equation systems
		vector<double> res(x.size(), 0);
		return res;
	}
	virtual vector<vector<double> > fjac_f(vector<double> const& x) { //Calculate the jacobian matrix using analytical expression.
		vector<double> res_seg(x.size(), 0);
		vector < vector<double> > res(x.size(), res_seg);
		return res;
	}
	
	vector<double> fvecNRfmin;

	vector<vector<double> > nrfdjac(vector<double> const& x,
									vector<double> const& fvecNRfdjac,
									double const& EPS = 1e-8) {
		int n = x.size();
		vector<double> df_seg(n, 0);
		vector < vector<double> > df(n, df_seg);
		vector<double> xh = x;
		for (int j = 0; j < n; j++) {
			double temp = xh[j];
			double h = EPS * fabs(temp);
			if (h == 0.0)
				h = EPS;
			xh[j] = temp + h;
			//Trick to reduce ﬁnite-precision error.
			h = xh[j] - temp;
			vector<double> f = fvec(xh);
			xh[j] = temp;
			for (int i = 0; i < n; i++)
				//Forward difference formula.
				df[i][j] = (f[i] - fvecNRfdjac[i]) / h;
		}
		return df;
	} //Calculate the jacobian matrix by approximation.
	
	double nrfmin(vector<double> const& x) { //Returns f=1/2*F*F. Also stores value of F in fvec.
		int n = x.size();
		double sum = 0;
		fvecNRfmin = fvec(x);
		for (int i = 0; i < n; i++)
			sum += pow(fvecNRfmin[i], 2);
		return 0.5 * sum;
	}
	
	void usrfun(vector<double> const& x,
				vector<double> & vec,
				vector<vector<double> > & jac) {
		vec = fvec(x);
		jac = fjac_f(x);
	}
	
	void lnsrch(vector<double> & xold,
				double & fold,
				vector<double> & g,
				vector<double> & p,
				vector<double> &x,
				double &f,
				double stpmax,
				bool &check) {
		//	Given an n-dimensional point xold[0..n-1], the value of the function and gradient there, fold
		//	and g[0..n-1], and a direction p[0..n-1], ﬁnds a new point x[0..n-1] along the direction
		//	p from xold where the function or functor func has decreased “suﬃciently.” The new function
		//	value is returned in f. stpmax is an input quantity that limits the length of the steps so that you
		//	do not try to evaluate the function in regions where it is undeﬁned or subject to overﬂow. p is
		//	usually the Newton direction. The output quantity check is false on a normal exit. It is true
		//	when x is too close to xold. In a minimization algorithm, this usually signals convergence and
		//	can be ignored. However, in a zero-ﬁnding algorithm the calling program should check whether
		//	the convergence is spurious.
		const double ALF = 1.0e-4, TOLX = numeric_limits<double>::epsilon();
		//	ALF ensures sufficient decrease in function value; TOLX is the convergence criterion on x.
		double a, alam, alam2 = 0.0, alamin, b, disc, f2 = 0.0;
		double rhs1, rhs2, slope = 0.0, sum = 0.0, temp, test, tmplam;
		int i, n = xold.size();
		check = false;
		for (i = 0; i < n; i++)
			sum += p[i] * p[i];
		sum = sqrt(sum);
		if (sum > stpmax)
			for (i = 0; i < n; i++)
				//	roots multidim.h
				p[i] *= stpmax / sum;
		//	Scale if attempted step is too big.
		for (i = 0; i < n; i++)
			slope += g[i] * p[i];
		if (slope >= 0.0)
			throw("Roundoff problem in lnsrch.");
		test = 0.0;
		//	Compute min .
		for (i = 0; i < n; i++) {
			temp = fabs(p[i]) / max(fabs(xold[i]), 1.0);
			if (temp > test)
				test = temp;
		}
		alamin = TOLX / test;
		alam = 1.0;
		//	Always try full Newton step ﬁrst.
		for (;;) {
			//	Start of iteration loop.
			for (i = 0; i < n; i++)
				x[i] = xold[i] + alam * p[i];
			f = nrfmin(x);
			if (alam < alamin) {
				//	Convergence on x. For zero ﬁnd-
				for (i = 0; i < n; i++)
					x[i] = xold[i];
				//	ing, the calling program should
				check = true;
				//	verify the convergence.
				return;
			} else if (f <= fold + ALF * alam * slope)
				return;
			//	Suﬃcient function decrease.
			else {
				//	Backtrack.
				if (alam == 1.0)
					tmplam = -slope / (2.0 * (f - fold - slope));
				//	First time.
				else {
					//	Subsequent backtracks.
					rhs1 = f - fold - alam * slope;
					rhs2 = f2 - fold - alam2 * slope;
					a = (rhs1 / (alam * alam) - rhs2 / (alam2 * alam2)) / (alam - alam2);
					b = (-alam2 * rhs1 / (alam * alam) + alam * rhs2 / (alam2 * alam2)) / (alam - alam2);
					if (a == 0.0)
						tmplam = -slope / (2.0 * b);
					else {
						disc = b * b - 3.0 * a * slope;
						if (disc < 0.0)
							tmplam = 0.5 * alam;
						else if (b <= 0.0)
							tmplam = (-b + sqrt(disc)) / (3.0 * a);
						else
							tmplam = -slope / (b + sqrt(disc));
					}
					if (tmplam > 0.5 * alam)
						tmplam = 0.5 * alam;
				}
			}
			alam2 = alam;
			f2 = f;
			alam = max(tmplam, 0.1 * alam);
		}
		//	Try again.
	}
	
	void newt(	vector<double> & x,
				bool &check) {
		//	Given an initial guess x[0..n-1] for a root in n dimensions, ﬁnd the root by a globally convergent
		//	Newton’s method. The vector of functions to be zeroed, called fvec[0..n-1] in the routine
		//	below, is returned by the user-supplied function or functor vecfunc (see text). The output
		//	quantity check is false on a normal return and true if the routine has converged to a local
		//	minimum of the function fmin deﬁned below. In this case try restarting from a diﬀerent initial
		//	guess.
		const int MAXITS = 200;
		const double TOLF = 1.0e-8, TOLMIN = 1.0e-12, STPMX = 100.0;
		const double TOLX = numeric_limits<double>::epsilon();
		//	Here MAXITS is the maximum number of iterations; TOLF sets the convergence criterion on
		//	function values; TOLMIN sets the criterion for deciding whether spurious convergence to a
		//	minimum of fmin has occurred; STPMX is the scaled maximum step length allowed in line
		//	searches; and TOLX is the convergence criterion on ıx.
		int i, j, its, n = x.size();
		double den, f, fold, stpmax, sum, temp, test;
		vector<double> g(n, 0), p(n, 0), xold(n, 0);
		vector<double> fjac_seg(n, 0);
		vector < vector<double> > fjac(n, fjac_seg);
		f = nrfmin(x); //fvecNRfmin is also computed by this call.
		test = 0.0; //Test for initial guess being a root. Use
		for (i = 0; i < n; i++) { //more stringent test than simply TOLF.
			if (fabs(fvecNRfmin[i]) > test)
				test = fabs(fvecNRfmin[i]);
		}
		if (test < 0.01 * TOLF) {
			check = false;
			return;
		}
		
		sum = 0.0;
		for (i = 0; i < n; i++)
			sum += pow(x[i], 2);
		//	Calculate stpmax for line searches.
		stpmax = STPMX * max(sqrt(sum), double(n));
		
		for (its = 0; its < MAXITS; its++) {
			
			//	Start of iteration loop.
			fjac = nrfdjac(x, fvecNRfmin);
//			fjac = fjac_f(x);
			
//			for (unsigned int i=0; i<3; i++ ) {
//				for (unsigned int j=0; j<3; j++ ) {
//					cout<<fjac[i][j]<<" ";
//				}
//				cout<<endl;
//			}
			
			//	If analytic Jacobian is available, you can replace the struct NRfdjac below with your
			//	own struct in fjac.
			for (i = 0; i < n; i++) {
				//	Compute rf for the line search.
				sum = 0.0;
				for (j = 0; j < n; j++)
					sum += fjac[j][i] * fvecNRfmin[j];
				g[i] = sum;
			}
			
			for (i = 0; i < n; i++) {
				xold[i] = x[i]; //	Store x,
			}
			fold = f; //	and f .
			for (i = 0; i < n; i++) {
				p[i] = -fvecNRfmin[i]; //	Right-hand side for linear equations.
			}
			
			LUdcmp alu(fjac); //	Solve linear equations by LU decomposition.
			alu.solve(p, p);
			lnsrch(xold, fold, g, p, x, f, stpmax, check);
			//	lnsrch returns new x and f . It also calculates fvecNRfmin at the new x when it calls fmin.
			test = 0.0;
			//	Test for convergence on function values.
			for (i = 0; i < n; i++)
				if (fabs(fvecNRfmin[i]) > test)
					test = fabs(fvecNRfmin[i]);
			if (test < TOLF) {
				check = false;
				return;
			}
			if (check) {
				//	Check for gradient of f zero, i.e., spurious convergence.
				test = 0.0;
				
				den = max(f, 0.5 * n);
				for (i = 0; i < n; i++) {
					temp = fabs(g[i]) * max(fabs(x[i]), 1.0) / den;
					if (temp > test)
						test = temp;
				}
				check = (test < TOLMIN);
				return;
			}
			test = 0.0;
			//	Test for convergence on ıx.
			for (i = 0; i < n; i++) {
				temp = (fabs(x[i] - xold[i])) / max(fabs(x[i]), 1.0);
				if (temp > test)
					test = temp;
			}
			if (test < TOLX)
				return;
		}
		cout << "aaaaaaaaaaaaaaaaaaa" << endl;
		throw("MAXITS exceeded in newt");
	}
	
};

#endif /* EQUSYSTEMSBASE_H_ */
