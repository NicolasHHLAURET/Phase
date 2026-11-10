/*
 * RBFinterp.h
 *
 *  Created on: Oct 3, 2012
 *      Author: yint
 *      From Numerical Recipes: Radial Basis Function Interpolation
 */

#ifndef RBFINTERP_H_
#define RBFINTERP_H_

#include "LUdcmp.h"
#include <vector>

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

class RBFunction {
	//	Abstract base class template for any particular radial basis function. See speciﬁc examples below.
public:
	virtual double rbf(double r)  const = 0;
	virtual shared_ptr<RBFunction> clone() const = 0;
	virtual ~RBFunction() {}
	;
};

class RBFinterp {
private:
	//	Object for radial basis function interpolation using n points in dim dimensions. Call constructor
	//	once, then interp as many times as desired.
	int dim;
	int n;
	vector<vector<double> > pts;
	vector<double> vals;
	vector<double> w;
	shared_ptr<RBFunction> fn; //not implemented, the default is multi_quadratic
	bool norm;

public:
	RBFinterp() {
		dim = 0;
		n = 0;
		norm = false;
	}
	virtual ~RBFinterp() {}

	RBFinterp(	const vector<vector<double> > &ptss,
				const vector<double> &valss,
				const RBFunction & func,
				bool nrbf = true) :
			dim(ptss[0].size()), n(ptss.size()), pts(ptss), vals(valss), w(n), /*fn(func),*/norm(nrbf) {
		//	Constructor. The n dim matrix ptss inputs the data points, the vector valss the function
		//	values. func contains the chosen radial basis function, derived from the class RBF_fn. The
		//	default value of nrbf gives RBF interpolation; set it to 1 for NRBF.
		fn = func.clone();
		double sum;
		vector < vector<double> > rbf(n, vector<double>(n, 0.0));
		vector<double> rhs(n, 0.0);
		for (int i = 0; i < n; i++) { //	Fill the matrix .jr i rj j/ and the r.h.s. vector.
			sum = 0.;
			for (int j = 0; j < n; j++) {
				sum += (rbf[i][j] = fn->rbf(rad(pts[i], pts[j])));
			}
			if (norm)
				rhs[i] = sum * vals[i];
			else
				rhs[i] = vals[i];
		}
		LUdcmp lu(rbf);
		//	Solve the set of linear equations.
		lu.solve(rhs, w);
	}
	RBFinterp(const RBFinterp & brfInterp) {
		dim = brfInterp.dim;
		n = brfInterp.n;
		pts = brfInterp.pts;
		vals = brfInterp.vals;
		w = brfInterp.w;
		fn = brfInterp.fn;
		norm = brfInterp.norm;
	}
	RBFinterp & operator=(const RBFinterp & brfInterp) {
		dim = brfInterp.dim;
		n = brfInterp.n;
		pts = brfInterp.pts;
		vals = brfInterp.vals;
		w = brfInterp.w;
		fn = brfInterp.fn;
		norm = brfInterp.norm;
		return *this;
	}

	int const& getDimension() const {
		return dim;
	}

	vector<double> const & getWeights() const{
		return w;
	}

	int const& getNbElements() const {
		return n;
	}

	vector<vector<double> > const& getPts() const {
		return pts;
	}

	vector<double> const& getVals() const {
		return vals;
	}
	double interp(const vector<double> &pt) const {
		//	Return the interpolated function value at a dim-dimensional point pt.
		double fval, sum = 0., sumw = 0.;
		if ((int) pt.size() != dim)
			throw("RBF_interp bad pt size");
		for (int i = 0; i < n; i++) {
			//	Sum over all tabulated points.
			fval = fn->rbf(rad(pt, pts[i]));
			sumw += w[i] * fval;
			sum += fval;
		}
		return norm ? sumw / sum : sumw;
	}
	double rad(	const vector<double> & p1,
				const vector<double> & p2)  const { //Euclidean distance.
		double sum = 0.;
		for (int i = 0; i < dim; i++)
			sum += pow((p1[i] - p2[i]), 2);
		return sqrt(sum);
	}
	double rbf_multiquadric(double r) const {
		return sqrt(1 + pow(r, 2));
	}
};

class RBF_multiquadric: public RBFunction {
	//Instantiate this and send to RBF_interp to get multiquadric interpolation.
	double r02;
public:
	RBF_multiquadric(double scale = 1.) :
			r02(pow(scale, 2)) {}
	//Constructor argument is the scale factor. See text.
	double rbf(double r)  const {
		return sqrt(pow(r, 2) + r02);
	}
	virtual ~RBF_multiquadric() {}

	shared_ptr<RBFunction> clone() const {
		return shared_ptr<RBFunction>(new RBF_multiquadric(*this));
	}
};

class RBF_thinplate: public RBFunction {
	//Same as above, but for thin-plate spline.
	double r0;
public:
	RBF_thinplate(double scale = 1.) :
			r0(scale) {}
	double rbf(double r) const {
		return r <= 0. ? 0. : pow(r, 2) * log(r / r0);
	}
	shared_ptr<RBFunction> clone() const {
		return shared_ptr<RBFunction>(new RBF_thinplate(*this));
	}
	virtual ~RBF_thinplate() {}
};

class RBF_gauss: public RBFunction {
	//Same as above, but for Gaussian.
	double r0;
public:
	RBF_gauss(double scale = 1.) :
			r0(scale) {}
	double rbf(double r) const {
		return exp(-0.5 * pow(r / r0, 2));
	}
	shared_ptr<RBFunction> clone() const {
		return shared_ptr<RBFunction>(new RBF_gauss(*this));
	}
	virtual ~RBF_gauss() {}

};

class RBF_inversemultiquadric: public RBFunction {
	//Same as above, but for inverse multiquadric.
	double r02;
public:
	RBF_inversemultiquadric(double scale = 1.) :
			r02(scale * scale) {}
	double rbf(double r) const {
		return 1. / sqrt(r * r + r02);
	}
	shared_ptr<RBFunction> clone() const {
		return shared_ptr<RBFunction>(new RBF_inversemultiquadric(*this));
	}
	virtual ~RBF_inversemultiquadric() {}

};

class RBF_power: public RBFunction {
	//Instantiate this and send to RBF_interp to get multiquadric interpolation.
	double r02;
public:
	RBF_power(double scale = 1.) :
			r02(scale) {}
	//Constructor argument is the scale factor. See text.
	double rbf(double r) const {
		return pow(r, r02);
	}
	virtual ~RBF_power() {}
	shared_ptr<RBFunction> clone() const {
		return shared_ptr<RBFunction>(new RBF_power(*this));
	}

};

#endif /* RBFINTERP_H_ */
