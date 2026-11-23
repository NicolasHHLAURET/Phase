/*
 * DirectionCartesienne.hpp
 *
 *  Created on: 6 fevr. 2012
 *      Author: lauretn
 */

#ifndef DIRECTIONCARTESIENNE_HPP_
#define DIRECTIONCARTESIENNE_HPP_

#include "DirectionRoot.hpp"
#include "Vecteur.hpp"

class DirectionCartesienne: public DirectionRoot {
private:
	Vecteur<double> _Cartesien;

public:
	typedef enum ModeTaitBryan {
		XZY = 0,
		XYZ = 1,
		YXZ = 2,
		YZX = 3,
		ZYX = 4,
		ZXY = 5,
		OTHER = 6
	} ModeTaitBryan;

	DirectionCartesienne() :
			DirectionRoot() {}
	
	DirectionCartesienne(	const double& theta_b,
							const double& theta_e,
							const double& phi_b,
							const double& phi_e) :
			DirectionRoot(theta_b, theta_e, phi_b, phi_e) {
		calculerCoordonneesCartesiennes();
	}
	
	DirectionCartesienne(	ModeTaitBryan rotationMode,
							double angle1,
							double angle2,
							double angle3,
							bool invertDirection) {
		_Cartesien = convertTaitBryanToCartesian(rotationMode, angle1, angle2, angle3);
		if (invertDirection) {
			_Cartesien = _Cartesien.negate();
		}
	}

	DirectionCartesienne & calculerCoordonneesCartesiennes() {
		double Epsilon = 1E-10;
		double ct, cp, st, sp;
		
		ct = cos(this->getCenterTheta());
		if (fabs(ct) < Epsilon) {
			ct = 0.0;
		}
		
		cp = cos(this->getCenterPhi());
		if (fabs(cp) < Epsilon) {
			cp = 0.0;
		}
		
		st = sin(this->getCenterTheta());
		if (fabs(st) < Epsilon) {
			st = 0.0;
		}
		
		sp = sin(this->getCenterPhi());
		if (fabs(sp) < Epsilon) {
			sp = 0.0;
		}
		
		_Cartesien.setZ(ct);
		_Cartesien.setX(st * cp);
		_Cartesien.setY(st * sp);
		_Cartesien.normaliserEgal();
		return *this;
	}
	
	static Vecteur<double> convertiDirectionPolaireEnCartesien(	const double &theta,
																const double &phi,
																const DirectionRoot::Unit & unit = DirectionRoot::RADIANS) {
		Vecteur<double> resultsVector;
		double epsilon = 1E-10;
		double thetaInRad = theta;
		double phiInRad = phi;
		if (unit == DirectionRoot::DEGREES) {
			thetaInRad *= M_PI / 180.;
			phiInRad *= M_PI / 180.;
		}
		double cosTheta = cos(thetaInRad);
		if (fabs(cosTheta) < epsilon) {
			cosTheta = 0.0;
		}
		double cosPhi = cos(phiInRad);
		if (fabs(cosPhi) < epsilon) {
			cosPhi = 0.0;
		}

		double sinTheta = sin(thetaInRad);
		if (fabs(sinTheta) < epsilon) {
			sinTheta = 0.0;
		}

		double sinPhi = sin(phiInRad);
		if (fabs(sinPhi) < epsilon) {
			sinPhi = 0.0;
		}

		resultsVector.setZ(cosTheta);
		resultsVector.setX(sinTheta * cosPhi);
		resultsVector.setY(sinTheta * sinPhi);
		return resultsVector.normaliser();
	}

	static Vecteur<double> convertTaitBryanToCartesian(unsigned int modeRot, double angleX, double angleY, double angleZ) {
		Vecteur<double> res;
		double s1 = sin(angleZ*M_PI / 180.);
		double c1 = cos(angleZ*M_PI / 180.);
		double s2 = sin(angleY*M_PI / 180.);
		double c2 = cos(angleY*M_PI / 180.);
		double s3 = sin(angleX*M_PI / 180.);
		double c3 = cos(angleX*M_PI / 180.);

		// If the original orientation of camera is assumed to be along -Z axis.
		//Reference: https://en.wikipedia.org/wiki/Euler_angles: Z1, Y2, X3
		//Note that the rotation is passive (reciprocal of Proper Euler angles   ->    Y1Z2Y3) where c3=1; s3=0; (https://en.wikipedia.org/wiki/Euler_angles)
		switch (modeRot) {
			case ModeTaitBryan::ZYX:
				//c1*c2; c2*s1; -s2;
				//c1*s2*s3-c3*s1; c1*c3+s1*s2*s3; c2*s3;
				//s1*s3+c1*c3*s2; c3*s1*s2-c1*s3; c2*c3;
//				res.setM00(c1 * c2);
//				res.setM10(c1 * s2 * s3 - c3 * s1);
//				res.setM20(s1 * s3 + c1 * c3 * s2);
//				res.setM01(c2 * s1);
//				res.setM11(c1 * c3 + s1 * s2 * s3);
//				res.setM21(c3 * s1 * s2 - c1 * s3);
//				res.setM02(-s2);
//				res.setM12(c2 * s3);
//				res.setM22(c2 * c3);
				res.setX(s1 * s3 + c1 * c3 * s2);
				res.setY(c3 * s1 * s2 - c1 * s3);
				res.setZ(c2 * c3);
				break;
			case ModeTaitBryan::XYZ:
				//c2*c3; c1*s3+c3*s1*s2; s1*s3-c1*c3*s2;
				//-c2*s3; c1*c3-s1*s2*s3; c3*s1+c1*s2*s3;
				//s2; -c2*s1; c1*c2;
//				res.setM00(c2 * c3);
//				res.setM10(-c2 * s3);
//				res.setM20(s2);
//				res.setM01(c1 * s3 + c3 * s1 * s2);
//				res.setM11(c1 * c3 - s1 * s2 * s3);
//				res.setM21(-c2 * s1);
//				res.setM02(s1 * s3 - c1 * c3 * s2);
//				res.setM12(c3 * s1 + c1 * s2 * s3);
//				res.setM22(c1 * c2);
				res.setX(s2);
				res.setY(-c2 * s1);
				res.setZ(c1 * c2);
				break;
			case ModeTaitBryan::XZY:
				//c2*c3; s1*s3+c1*c3*s2; c3*s1*s2-c1*s3;
				//-s2; c1*c2; c2*s1;
				//c2*s3; c1*s2*s3-c3*s1; c1*c3+s1*s2*s3;
//				res.setM00(c2 * c3);
//				res.setM10(-s2);
//				res.setM20(c2 * s3);
//				res.setM01(s1 * s3 + c1 * c3 * s2);
//				res.setM11(c1 * c2);
//				res.setM21(c1 * s2 * s3 - c3 * s1);
//				res.setM02(c3 * s1 * s2 - c1 * s3);
//				res.setM12(c2 * s1);
//				res.setM22(c1 * c3 + s1 * s2 * s3);
				res.setX(c2 * s3);
				res.setY(c1 * s2 * s3 - c3 * s1);
				res.setZ(c1 * c3 + s1 * s2 * s3);
				break;
			case ModeTaitBryan::YZX:
				//c1*c2; s2; -c2*s1;
				//s1*s3-c1*c3*s2; c2*c3; c1*s3+c3*s1*s2;
				//c3*s1+c1*s2*s3; -c2*s3; c1*c3-s1*s2*s3;
//				res.setM00(c1 * c2);
//				res.setM10(s1 * s3 - c1 * c3 * s2);
//				res.setM20(c3 * s1 + c1 * s2 * s3);
//				res.setM01(s2);
//				res.setM11(c2 * c3);
//				res.setM21(-c2 * s3);
//				res.setM02(-c2 * s1);
//				res.setM12(c1 * s3 + c3 * s1 * s2);
//				res.setM22(c1 * c3 - s1 * s2 * s3);
				res.setX(c3 * s1 + c1 * s2 * s3);
				res.setY(-c2 * s3);
				res.setZ(c1 * c3 - s1 * s2 * s3);
				break;
			case ModeTaitBryan::YXZ:
				//c1*c3+s1*s2*s3; c2*s3; c1*s2*s3-c3*s1;
				//c3*s1*s2-c1*s3; c2*c3; c1*c3*s2+s1*s3;
				//c2*s1; -s2; c1*c2;
//				res.setM00(c1 * c3 + s1 * s2 * s3);
//				res.setM10(c3 * s1 * s2 - c1 * s3);
//				res.setM20(c2 * s1);
//				res.setM01(c2 * s3);
//				res.setM11(c2 * c3);
//				res.setM21(-s2);
//				res.setM02(c1 * s2 * s3 - c3 * s1);
//				res.setM12(c1 * c3 * s2 + s1 * s3);
//				res.setM22(c1 * c2);
				res.setX(c2 * s1);
				res.setY(-s2);
				res.setZ(c1 * c2);
				break;
			case ModeTaitBryan::ZXY:
				//c1*c3-s1*s2*s3; c3*s1+c1*s2*s3; -c2*s3;
				//-c2*s1; c1*c2; s2;
				//c1*s3+c3*s1*s2; s1*s3-c1*c3*s2; c2*c3;
//				res.setM00(c1 * c3 - s1 * s2 * s3);
//				res.setM10(-c2 * s1);
//				res.setM20(c1 * s3 + c3 * s1 * s2);
//				res.setM01(c3 * s1 + c1 * s2 * s3);
//				res.setM11(c1 * c2);
//				res.setM21(s1 * s3 - c1 * c3 * s2);
//				res.setM02(-c2 * s3);
//				res.setM12(s2);
//				res.setM22(c2 * c3);
				res.setX(c1 * s3 + c3 * s1 * s2);
				res.setY(s1 * s3 - c1 * c3 * s2);
				res.setZ(c2 * c3);
				break;
			default:
				cerr << "Error: Rotation Mode is not implemented" << endl;
				break;
		}
		return res.normaliser().negate();
	}

	DirectionCartesienne & setDirectionCartesienne(const Vecteur<double> & vecteurCartesien) {
		_Cartesien = vecteurCartesien;
		return *this;
	}
	
	const Vecteur<double> & getDirectionCartesienne() const {
		return _Cartesien;
	}
	
	const double & getX() const {
		return _Cartesien.getX();
	}
	
	const double & getY() const {
		return _Cartesien.getY();
	}
	
	const double & getZ() const {
		return _Cartesien.getZ();
	}
};

#endif /* DIRECTIONCARTESIENNE_HPP_ */
