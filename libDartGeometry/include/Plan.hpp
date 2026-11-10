/*
 * Plan.hpp
 *
 *  Created on: 2 mai 2011
 *      Author: graue
 */

#ifndef PLAN_HPP_
#define PLAN_HPP_

#include <iostream>
#include "Droite3d.hpp"
#include <cmath>

class Plan {
private:
	Vecteur<double> _normale; // unit normal vector
	Point3<double> _originePlan; // unit normal vector
	double _d; // shortest distance from origin
	double _eps; // point/plane distance epsilon
	
public:
	// Computes the plane equation using Newell's averaging algorithm.
	Plan(const std::vector<Point3<double> > &points) :
			_normale(0.0, 0.0, 0.0), _originePlan(points[0]), _d(0.0), _eps(0.0) {
		unsigned int nPoints = points.size();
//		assert(points.size() > 2);
		
		Vecteur<double> avrPnt(0, 0, 0);
		for (unsigned int i = 1; i < nPoints; ++i) {
			avrPnt += points[i - 1];
			_normale += Vecteur<double>(points[i - 1]).vectoriel(Vecteur<double>(points[i]));
			updateEpsilon(points[i]);
		}
		avrPnt += points[nPoints - 1];
		_normale += Vecteur<double>(points[nPoints - 1]).vectoriel(Vecteur<double>(points[0]));
		_normale = _normale.normaliser();
		_d = _normale.scalaire(avrPnt * (-1.0 / nPoints));
	}
	
	Plan(	const Point3<double> &point1,
			const Point3<double> &point2,
			const Point3<double> &point3) :
			_normale(0.0, 0.0, 0.0), _originePlan(point1), _d(0.0), _eps(0.0) {
		Vecteur<double> AB(point2);
		AB -= point1;
		
		Vecteur<double> AC(point3);
		AC -= point1;
		
		_normale = AB.vectoriel(AC).normaliser();
		
		if (std::isnan(_normale.getX())) {
			std::cout << " Plan : NAAAN pour points : " << point1 << " " << point2 << " " << point3 << std::endl;
		}
	}
	Plan(	const Vecteur<double> &normale,
			const Point3<double> &origine) :
			_normale(normale), _originePlan(origine), _d(0.0), _eps(0.0) {}
	Plan(	const Droite3d &droitevecteur,
			const Point3<double> &origine) :
			_normale(droitevecteur.getDirection()), _originePlan(origine), _d(0.0), _eps(0.0) {}
	Plan() :
			_normale(0.0, 0.0, 0.0), _originePlan(0.0, 0.0, 0.0), _d(0.0), _eps(0.0) {}
	Plan(	const Vecteur<double>& v,
			double x) :
			_normale(v), _d(x), _eps(0.0) {}
	Plan(const Plan& pl) :
			_normale(pl._normale), _originePlan(pl._originePlan), _d(pl._d), _eps(pl._eps) {}
	
	const Vecteur<double>& getNormale() const {
		return _normale;
	}
	const Point3<double>& getOrigine() const {
		return _originePlan;
	}
	
	// The point is topologically known to lie in the plane, so check
	// and update epsilon accordingly:
	void updateEpsilon(const Point3<double>& point);

	// Signed distance from the point to the plane.
	double sDistance(const Point3<double>& p) const {
		return Vecteur<double>(p).scalaire(_normale) + _d;
	}
	
	// Intersection point between this Plane and transversal line.
	PointIntersection<double> calculPointIntersection(	const Vecteur<double> & vecteurDirecteur,
														const Point3<double> & origine) const;
	PointIntersection<double> calculPointIntersection(const Droite3d & droite) const;
	PointIntersection<double> calculPointIntersection(	const Plan & plan1,
														const Plan & plan2) const;
	Droite3d calculPointIntersection(const Plan &plan2) const;

	static PointIntersection<double> solutionPivotGauss(std::vector<std::vector<double> > &e) {
		int n = 3;
		double s[4];
		double temp;
		int a, t;
		
		for (int x = 0; x < n - 1; x++) {
			for (a = 1 + x; a < n; a++) {
				temp = e[x][a];
				for (t = x; t < n + 1; t++) {
					e[t][a] = e[t][a] * e[x][x] - e[t][x] * temp; /// triangulation du systeme.
				}
			}
		}
		
		//Passons au remplacage..
		s[n - 1] = e[n][n - 1] / e[n - 1][n - 1];
		e[n][n - 1] = 0;
		e[n - 1][n - 1] = 0;
		
		for (int ligne = 1; ligne < n; ligne++) {
			for (int sol = 2; sol <= n; sol++) {
				e[n - ligne][n - sol] *= s[n - ligne];
				e[n][n - sol] -= e[n - ligne][n - sol];
				e[n - ligne][n - sol] = 0;
			}
			s[n - (ligne + 1)] = e[n][n - (ligne + 1)] / e[n - (ligne + 1)][n - (ligne + 1)];
		}
		
		return PointIntersection<double>(s[0], s[1], s[2]);
	}
	
	friend std::ostream & operator<<(std::ostream &stream,
								const Plan & plan);
	// Which side of the plane is this point on?
	//  Where      whichSide( const Point& ) const;
	
};

inline std::ostream & operator<<(std::ostream &stream,
							const Plan & plan) {
	stream << "Plan-{" << plan._normale << ", origine :" << plan._originePlan << "}";
	return stream;
}
// Compute the intersection point with the transversal line (p,q).
inline PointIntersection<double> Plan::calculPointIntersection(	const Vecteur<double> & vecteurDirecteur,
																const Point3<double> & origine) const {
	const double scalaireNormaleDirection = getNormale().scalaire(vecteurDirecteur);
	if (scalaireNormaleDirection != 0.0) {
		double distance = (getNormale().scalaire(_originePlan) - getNormale().scalaire(origine)) / scalaireNormaleDirection;
		return PointIntersection<double>(Point3<double>(origine + vecteurDirecteur * distance), NULL, distance, true);
	}
//	cout << " PLAN PARALLELLE : dir " << vecteurDirecteur << " point " << origine << " Pour PLan : " << this->toString() << endl;
	return PointIntersection<double>();
}

inline PointIntersection<double> Plan::calculPointIntersection(const Droite3d & droite) const {
	const double scalaireNormaleDirection = getNormale().scalaire(droite.getDirection());
	if (scalaireNormaleDirection != 0.0) {
		double distance = (getNormale().scalaire(_originePlan) - getNormale().scalaire(droite.getPoint1())) / scalaireNormaleDirection;
		return PointIntersection<double>(Point3<double>(droite.getPoint1() + droite.getDirection() * distance), NULL, distance, true);
	}
//	cout << " PLAN PARALLELLE" << droite.getDirection() << " " << droite.getPoint1() << " " << endl;
	return PointIntersection<double>();
}

inline PointIntersection<double> Plan::calculPointIntersection(	const Plan & plan1,
																const Plan & plan2) const {
	// resolution de A.x = B
	// avec A  les coefficients des normales des plans et B tel que ax + by + cz + d = 0
	std::vector < std::vector<double> > A(4, std::vector<double>(3));
	
	for (unsigned int i = 0; i < 3; ++i) {
		A[i][0] = this->getNormale()[i];
		A[i][1] = plan1.getNormale()[i];
		A[i][2] = plan2.getNormale()[i];
	}
	
	A[3][0] = this->getNormale().scalaire(this->getOrigine());
	A[3][1] = plan1.getNormale().scalaire(plan1.getOrigine());
	A[3][2] = plan2.getNormale().scalaire(plan2.getOrigine());
	
	//cout << "eq 1 :" << A[0][0] << " "<<A[1][0] << " "<<A[2][0] <<" = "<<A[3][0] << endl;
	//cout << "eq 2 :" << A[0][1] << " "<<A[1][1] << " "<<A[2][1] <<" = "<<A[3][1] << endl;
	//cout << "eq 3 :" << A[0][2] << " "<<A[1][2] << " "<<A[2][2] <<" = "<<A[3][2] << endl;
	//PointIntersection<double> point = solutionPivotGauss(A);
	//cout << "Solution : "<<point<<endl;
	
	return solutionPivotGauss(A);
	
}

inline void Plan::updateEpsilon(const Point3<double>& p) {
	double d = sDistance(p);
	if (d < 0.0)
		d = -d;
	if (d > _eps)
		_eps = d;
}

// intersect3D_2Planes(): the 3D intersect of two planes
inline Droite3d Plan::calculPointIntersection(const Plan &plan2) const {
	Vecteur<double> u = this->getNormale().vectoriel(plan2.getNormale()); // cross product
	double ax = (u[0] >= 0 ? u[0] : -u[0]);
	double ay = (u[1] >= 0 ? u[0] : -u[1]);
	double az = (u[2] >= 0 ? u[0] : -u[2]);
	
	// test if the two planes are parallel
	if ((ax + ay + az) < 0.000001) { // Pn1 and Pn2 are near parallel
		// test if disjoint or coincide
		Vecteur<double> v = plan2.getOrigine() - this->getOrigine();
		if (this->getNormale().scalaire(v) == 0) { // Pn2.V0 lies in Pn1
			std::cout << " Plans Coincidents" << std::endl;
		} else {
			std::cout << " Plans disjoints" << std::endl;
		}
		return Droite3d(Point3<double>(), Point3<double>());
	}
	
	// Pn1 and Pn2 intersect in a line
	// first determine max abs coordinate of cross product
	int maxc; // max coordinate
	if (ax > ay) {
		if (ax > az)
			maxc = 1;
		else
			maxc = 3;
	} else {
		if (ay > az)
			maxc = 2;
		else
			maxc = 3;
	}
	
	// next, to get a point on the intersect line
	// zero the max coord, and solve for the other two
	Point3<double> iP; // intersect point
	double d1, d2; // the constants in the 2 plane equations
	d1 = -this->getNormale().scalaire(this->getOrigine()); // note: could be pre-stored with plane
	d2 = -plan2.getNormale().scalaire(plan2.getOrigine()); // ditto
		
	switch (maxc) { // select max coordinate
		case 1: // intersect with x=0
			iP[0] = 0;
			iP[1] = (d2 * this->getNormale()[2] - d1 * plan2.getNormale()[2]) / u[0];
			iP[2] = (d1 * plan2.getNormale()[1] - d2 * this->getNormale()[1]) / u[0];
			break;
		case 2: // intersect with y=0
			iP[0] = (d1 * plan2.getNormale()[2] - d2 * this->getNormale()[2]) / u[1];
			iP[1] = 0;
			iP[2] = (d2 * this->getNormale()[0] - d1 * plan2.getNormale()[0]) / u[1];
			break;
		case 3: // intersect with z=0
			iP[0] = (d2 * this->getNormale()[1] - d1 * plan2.getNormale()[1]) / u[2];
			iP[1] = (d1 * plan2.getNormale()[0] - d2 * this->getNormale()[0]) / u[2];
			iP[2] = 0;
	}
	return Droite3d(iP, iP + u);
}

#endif /* PLAN_HPP_ */
