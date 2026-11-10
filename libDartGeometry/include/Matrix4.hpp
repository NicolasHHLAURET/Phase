/*! Time-stamp: <@(#) Matrix4.hpp   22/09/2009   LAURET Nicolas CESBIO>
 *********************************************************************
 *  @file    : Matrix4.hpp
 *
 *  Projet   : DART
 *
 *  Societe  : CESBIO
 *
 *  Auteur   : LAURET Nicolas                        Date: 22/09/2009
 *
 *  Description : Definit une matrice 4x4 de nombre du type defini
 *  par le developpeur, et destinee aux transformationx 3D.
 *
 *********************************************************************
 * Historique :
 *
 * v 1.1 02/10/2009  LAURET Nicolas : Conversion en classe generique (template)
 * v 1.0 22/09/2009  LAURET Nicolas : Creation
 *
 *********************************************************************
 */

#ifndef MATRIX4_H_
#define MATRIX4_H_

#include "Point3.hpp"
#include "Vecteur.hpp"

#include <algorithm>
#include <string>
#include <cstring>
#include <sstream>
#include <cmath>

using namespace std;

template<typename T>
class Matrix4 {
protected:
	// Attributs
	T m[4][4];

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

	// Constructeurs
	/*
	 * v 1.0 :
	 * Note : Les 4 valeurs de la derniere ligne d'une matrice de transformation
	 * sont toujours (0,0,0,1). cette ligne est obligatoire lors des multiplications
	 * de matrices de transformations entre elles.
	 * Dans cette version, la multiplication realise est celle avec un point.
	 * Elles n'ont donc aucune utilitees en l'etat.
	 */
	Matrix4<T>(	const T & m00 = 1,
				const T & m01 = 0,
				const T & m02 = 0,
				const T & m03 = 0,
				const T & m10 = 0,
				const T & m11 = 1,
				const T & m12 = 0,
				const T & m13 = 0,
				const T & m20 = 0,
				const T & m21 = 0,
				const T & m22 = 1,
				const T & m23 = 0,
				const T & m30 = 0,
				const T & m31 = 0,
				const T & m32 = 0,
				const T & m33 = 1) {
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;
		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;
		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;
		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}
	
	// Constructeur par copie
	Matrix4<T>(const Matrix4<T> & mat) {
		memmove(m, mat.m, 16 * sizeof (T));
	}
	
	Matrix4<T>(const T matrix[4][4]) {
		memmove(m, matrix, 16 * sizeof (T));
	}

	// Constructeur de conversion de type (Implique que les types T et U doivent pouvoir
	// etre castes de l'un vers l'autre)
	template<typename U>
	Matrix4<T>(const Matrix4<U> & mat) {
		m[0][0] = mat.getM00();
		m[0][1] = mat.getM01();
		m[0][2] = mat.getM02();
		m[0][3] = mat.getM03();
		m[1][0] = mat.getM10();
		m[1][1] = mat.getM11();
		m[1][2] = mat.getM12();
		m[1][3] = mat.getM13();
		m[2][0] = mat.getM20();
		m[2][1] = mat.getM21();
		m[2][2] = mat.getM22();
		m[2][3] = mat.getM23();
		m[3][0] = mat.getM30();
		m[3][1] = mat.getM31();
		m[3][2] = mat.getM32();
		m[3][3] = mat.getM33();
	}
	
	// Destructeurs
	virtual ~Matrix4() {}
	
	// Accesseurs
	const T & getM00() const;
	const T & getM01() const;
	const T & getM02() const;
	const T & getM03() const;
	const T & getM10() const;
	const T & getM11() const;
	const T & getM12() const;
	const T & getM13() const;
	const T & getM20() const;
	const T & getM21() const;
	const T & getM22() const;
	const T & getM23() const;
	const T & getM30() const;
	const T & getM31() const;
	const T & getM32() const;
	const T & getM33() const;

	// Modifieurs
	Matrix4<T> & setM00(const T & m00);
	Matrix4<T> & setM01(const T & m01);
	Matrix4<T> & setM02(const T & m02);
	Matrix4<T> & setM03(const T & m03);
	Matrix4<T> & setM10(const T & m10);
	Matrix4<T> & setM11(const T & m11);
	Matrix4<T> & setM12(const T & m12);
	Matrix4<T> & setM13(const T & m13);
	Matrix4<T> & setM20(const T & m20);
	Matrix4<T> & setM21(const T & m21);
	Matrix4<T> & setM22(const T & m22);
	Matrix4<T> & setM23(const T & m23);
	Matrix4<T> & setM30(const T & m30);
	Matrix4<T> & setM31(const T & m31);
	Matrix4<T> & setM32(const T & m32);
	Matrix4<T> & setM33(const T & m33);

	void setToIdentity() {
		setM00(1);
		setM11(1);
		setM22(1);
		setM33(1);
	}
	void setToZero() {
		std::fill(m, m + 16, 0);
	}

	// Methodes Constantes
	/**
	 * Cree un nouveau Tripletd contenant le resultat de la
	 * multiplication de cette matrice avec le Tripletd passe
	 * en parametre. Ce dernier n'est pas modifie.
	 */
	template<typename U>
	void multiplyEqual(const U & factor) {
		for (unsigned int i = 0; i < 4; ++i) {
			for (unsigned int j = 0; j < 4; ++j) {
				m[i][j] *= factor;
			}
		}
	}

	template<typename U>
	void addEqual(const U & additive) {
		for (unsigned int i = 0; i < 4; ++i) {
			for (unsigned int j = 0; j < 4; ++j) {
				m[i][j] += additive;
			}
		}
	}

	template<typename U>
	void addEqual(const Matrix4<U> & matrix2) {
		for (unsigned int i = 0; i < 4; ++i) {
			for (unsigned int j = 0; j < 4; ++j) {
				m[i][j] += matrix2.m[i][j];
			}
		}
	}

	template<typename U>
	Triplet<U> multiply(const Triplet<U> & point) const;

	template<typename U>
	Matrix4<U> operator*(const Matrix4<U> &m2) const {
		return multiply(m2);
	}
	template<typename U>
	Matrix4<U> multiply(const Matrix4<U> & m2) const {
		Matrix4<U> res;

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				res.m[i][j] = m[i][0] * m2.m[0][j] +
					m[i][1] * m2.m[1][j] +
					m[i][2] * m2.m[2][j] +
					m[i][3] * m2.m[3][j];
		return res;
	}

	template<typename U>
	void multiplyEqual(const Matrix4<U> & matrix2) {
		*this = multiply(matrix2);
	}

	Matrix4<T> getInvertMatrix() const {
		Matrix4<T> res;
		T s0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
		T s1 = m[0][0] * m[1][2] - m[1][0] * m[0][2];
		T s2 = m[0][0] * m[1][3] - m[1][0] * m[0][3];
		T s3 = m[0][1] * m[1][2] - m[1][1] * m[0][2];
		T s4 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
		T s5 = m[0][2] * m[1][3] - m[1][2] * m[0][3];

		T c5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		T c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		T c3 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		T c2 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		T c1 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		T c0 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

		// Should check for 0 determinant
		T invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

		res.setM00((m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invdet);
		res.setM01((-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invdet);
		res.setM02((m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invdet);
		res.setM03((-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invdet);

		res.setM10((-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invdet);
		res.setM11((m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invdet);
		res.setM12((-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invdet);
		res.setM13((m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invdet);

		res.setM20((m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invdet);
		res.setM21((-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invdet);
		res.setM22((m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invdet);
		res.setM23((-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invdet);

		res.setM30((-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invdet);
		res.setM31((m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invdet);
		res.setM32((-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invdet);
		res.setM33((m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invdet);
		return res;
	}

	Matrix4<T> inverse() const {
		int indxc[4], indxr[4];
		int ipiv[4] = {0, 0, 0, 0};
		double minv[4][4];
		memcpy(minv, m, 4 * 4 * sizeof (double));
		for (int i = 0; i < 4; ++i) {
			int irow = -1, icol = -1;
			double big = 0.;
			// Choose pivot
			for (int j = 0; j < 4; ++j) {
				if (ipiv[j] != 1) {
					for (int k = 0; k < 4; ++k) {
						if (ipiv[k] == 0) {
							if (std::abs(minv[j][k]) >= big) {
								big = std::abs(minv[j][k]);
								irow = j;
								icol = k;
							}
						} else if (ipiv[k] > 1)
							throw std::runtime_error("Singular matrix in MatrixInvert: " + this->toString());
					}
				}
			}
			++ipiv[icol];
			// Swap rows _irow_ and _icol_ for pivot
			if (irow != icol) {
				for (int k = 0; k < 4; ++k)
					std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;
			if (minv[icol][icol] == 0.f)
				throw std::runtime_error("Singular matrix in MatrixInvert: " + this->toString());
			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			float pivinv = 1.f / minv[icol][icol];
			minv[icol][icol] = 1.f;
			for (int j = 0; j < 4; ++j)
				minv[icol][j] *= pivinv;
			// Subtract this row from others to zero out their columns
			for (int j = 0; j < 4; ++j) {
				if (j != icol) {
					double save = minv[j][icol];
					minv[j][icol] = 0;
					for (int k = 0; k < 4; ++k)
						minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (int j = 3; j >= 0; --j) {
			if (indxr[j] != indxc[j]) {
				for (int k = 0; k < 4; ++k)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}

		return Matrix4<T>(minv);
	}

	static Matrix4<double> Translate(const Vecteur<double> &delta) {
		return Translate(delta._x, delta._y, delta._z);
	}

	// Note: All rotation angles are in degrees.
	static Matrix4<double> RotateX(double deg) {
		double cosAngle = std::cos(deg * M_PI / 180);
		double sinAngle = std::sin(deg * M_PI / 180);
		return Matrix4<double>(	1, 0, 0, 0,
								0, cosAngle, -sinAngle, 0,
								0, sinAngle, cosAngle, 0,
								0, 0, 0, 1);
	}
	static Matrix4<double> RotateY(double deg) {
		double cosAngle = std::cos(deg * M_PI / 180);
		double sinAngle = std::sin(deg * M_PI / 180);
		return Matrix4<double>(	cosAngle, 0, sinAngle, 0,
								0, 1, 0, 0,
								-sinAngle, 0, cosAngle, 0,
								0, 0, 0, 1);
	}
	static Matrix4<double> RotateZ(double deg) {
		double cosAngle = std::cos(deg * M_PI / 180);
		double sinAngle = std::sin(deg * M_PI / 180);
		return Matrix4<double>(	cosAngle, -sinAngle, 0, 0,
								sinAngle, cosAngle, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1);
	}
	static Matrix4<double> Translate(double x, double y, double z) {
		return Matrix4<double> (1, 0, 0, x,
								0, 1, 0, y,
								0, 0, 1, z,
								0, 0, 0, 1);
	}
	static Matrix4<double> Scale(double x, double y, double z) {
		return Matrix4<double> (x, 0, 0, 0,
								0, y, 0, 0,
								0, 0, z, 0,
								0, 0, 0, 1);
	}

	static Matrix4<double> Perspective(double fov, double nearClipping, double farClipping) {
		double eps = 1e-4;
		nearClipping = max(0.0, nearClipping);
		farClipping = max(farClipping, nearClipping + eps);

		// Perform projective divide
		const double invDenom = 1.0 / (1.0 - nearClipping / farClipping);
		const Matrix4<double> persp(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, invDenom, -nearClipping * invDenom,
				0, 0, 1, 0);
		// Scale to canonical viewing volume
		const double invTanAng = 1.0 / std::tan(fov/180.0 * M_PI/2.0);

		return Scale(invTanAng, invTanAng, 1).multiply(persp);
	}

	// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	// Rotation angle is provided in degree [0-360[
	static void computeRotationMatrixAroundCustomAxis(Matrix4<double> & res, const Vecteur<double> & customAxis, const double & rotationAngle) {
		Matrix4<double> K(
			0., -customAxis.getZ(), customAxis.getY(), 0.,
			customAxis.getZ(), 0., -customAxis.getX(), 0.,
			-customAxis.getY(), customAxis.getX(), 0., 0.
		);
		Matrix4<double> KSquared = K;
		KSquared.multiplyEqual(K);

		K.multiplyEqual(std::sin(rotationAngle * M_PI / 180));
		KSquared.multiplyEqual(1. - std::cos(rotationAngle * M_PI / 180));

		res.setToIdentity();
		res.addEqual(K);
		res.addEqual(KSquared);
	}

	// Rotation angle is provided in degree [0-360[
	static Matrix4<double> computeRotationMatrixAroundCustomAxis(	const Vecteur<double> & customAxis,
																	const double & rotationAngle) {
		Matrix4<double> res;
		computeRotationMatrixAroundCustomAxis(res, customAxis, rotationAngle);
		return res;
	}

	// The LuxCore Image defines the Z axis as going toward the scene, the Y axis going downward and the X axis going leftward
	static void computeLuxCoreSceneToCameraTransformationMatrix(	Matrix4<double> & transformationMatrix,
															const Point3<double> & cameraPosition,
															const Vecteur<double> & cameraDirection,
															const Vecteur<double> & cameraUp) {
		// Code from "luxrays/core/geometry/transform.cpp": to be directly used later when direct call to LuxCore will be performed
		transformationMatrix.setM03(cameraPosition.getX());
		transformationMatrix.setM13(cameraPosition.getY());
		transformationMatrix.setM23(cameraPosition.getZ());
		transformationMatrix.setM33(1);
		const Vecteur<double> right = cameraDirection.vectoriel(cameraUp).normaliser();
		const Vecteur<double> newUp = right.vectoriel(cameraDirection);
		transformationMatrix.setM00(right.getX());
		transformationMatrix.setM10(right.getY());
		transformationMatrix.setM20(right.getZ());
		transformationMatrix.setM30(0.);
		transformationMatrix.setM01(newUp.getX());
		transformationMatrix.setM11(newUp.getY());
		transformationMatrix.setM21(newUp.getZ());
		transformationMatrix.setM31(0.);
		transformationMatrix.setM02(cameraDirection.getX());
		transformationMatrix.setM12(cameraDirection.getY());
		transformationMatrix.setM22(cameraDirection.getZ());
		transformationMatrix.setM32(0.);
		transformationMatrix = transformationMatrix.getInvertMatrix();
	}

	static Matrix4<double> computeLuxCoreSceneToCameraTransformationMatrix(const Point3<double> & cameraPosition,
																	const Vecteur<double> & cameraDirection,
																	const Vecteur<double> & cameraUp) {
		Matrix4<double> res;
		computeLuxCoreSceneToCameraTransformationMatrix(res, cameraPosition, cameraDirection, cameraUp);
		return res;
	}

	static void computeDartSceneToCameraTransformationMatrix(	Matrix4<double> & transformationMatrix,
															const Point3<double> & cameraPosition,
															const Vecteur<double> & cameraDirection,
															const Vecteur<double> & cameraUp) {
		// Code from "luxrays/core/geometry/transform.cpp": to be directly used later when direct call to LuxCore will be performed
		transformationMatrix.setM03(cameraPosition.getX());
		transformationMatrix.setM13(cameraPosition.getY());
		transformationMatrix.setM23(cameraPosition.getZ());
		transformationMatrix.setM33(1);
		const Vecteur<double> right = cameraDirection.vectoriel(cameraUp).normaliser();
		const Vecteur<double> newUp = right.vectoriel(cameraDirection);
		transformationMatrix.setM00(-newUp.getX());
		transformationMatrix.setM10(-newUp.getY());
		transformationMatrix.setM20(-newUp.getZ());
		transformationMatrix.setM30(0.);
		transformationMatrix.setM01(right.getX());
		transformationMatrix.setM11(right.getY());
		transformationMatrix.setM21(right.getZ());
		transformationMatrix.setM31(0.);
		transformationMatrix.setM02(-cameraDirection.getX());
		transformationMatrix.setM12(-cameraDirection.getY());
		transformationMatrix.setM22(-cameraDirection.getZ());
		transformationMatrix.setM32(0.);
		transformationMatrix = transformationMatrix.getInvertMatrix();
	}

	static Matrix4<double> computeDartSceneToCameraTransformationMatrix(const Point3<double> & cameraPosition,
																	const Vecteur<double> & cameraDirection,
																	const Vecteur<double> & cameraUp) {
		Matrix4<double> res;
		computeDartSceneToCameraTransformationMatrix(res, cameraPosition, cameraDirection, cameraUp);
		return res;
	}

	static void computeSceneToCameraTransformationMatrixPushbroom(	Matrix4<double> & transformationMatrix,
																	const Point3<double> & cameraPosition,
																	double directionAzimuth) {
		// Code from "luxrays/core/geometry/transform.cpp": to be directly used later when direct call to LuxCore will be performed
		transformationMatrix.setM03(cameraPosition.getX());
		transformationMatrix.setM13(cameraPosition.getY());
		transformationMatrix.setM23(cameraPosition.getZ());
		transformationMatrix.setM33(1);
		transformationMatrix.setM00(cosf(directionAzimuth));
		transformationMatrix.setM10(-sinf(directionAzimuth));
		transformationMatrix.setM20(0.);
		transformationMatrix.setM30(0.);
		transformationMatrix.setM01(sinf(directionAzimuth));
		transformationMatrix.setM11(cosf(directionAzimuth));
		transformationMatrix.setM21(0.);
		transformationMatrix.setM31(0.);
		transformationMatrix.setM02(0.);
		transformationMatrix.setM12(0.);
		transformationMatrix.setM22(-1.);
		transformationMatrix.setM32(0.);
		transformationMatrix = transformationMatrix.getInvertMatrix();
	}

	static Matrix4<double> computeSceneToCameraTransformationMatrixPushbroom(	const Point3<double> & cameraPosition,
																				double directionAzimuth) {
		Matrix4<double> res;
		computeSceneToCameraTransformationMatrixPushbroom(res, cameraPosition, directionAzimuth);
		return res;
	}

	// https://en.wikipedia.org/wiki/Euler_angles
	static void computeSceneToCameraMatrixYawPitchRoll(	Matrix4<double> & res,
														double yaw_Z,
														double pitch_Y,
														double roll_X,
														ModeTaitBryan modeRot) {
		double s1 = sin(yaw_Z);
		double c1 = cos(yaw_Z);
		double s2 = sin(pitch_Y);
		double c2 = cos(pitch_Y);
		double s3 = sin(roll_X);
		double c3 = cos(roll_X);

		res.setToIdentity();

		// If the original orientation of camera is assumed to be along -Z axis.
		//Reference: https://en.wikipedia.org/wiki/Euler_angles: Z1, Y2, X3
		//Note that the rotation is passive (reciprocal of Proper Euler angles   ->    Y1Z2Y3) where c3=1; s3=0; (https://en.wikipedia.org/wiki/Euler_angles)
		switch (modeRot) {
			case ModeTaitBryan::ZYX:
				//c1*c2; c2*s1; -s2;
				//c1*s2*s3-c3*s1; c1*c3+s1*s2*s3; c2*s3;
				//s1*s3+c1*c3*s2; c3*s1*s2-c1*s3; c2*c3;
				res.setM00(c1 * c2);
				res.setM10(c1 * s2 * s3 - c3 * s1);
				res.setM20(s1 * s3 + c1 * c3 * s2);
				res.setM01(c2 * s1);
				res.setM11(c1 * c3 + s1 * s2 * s3);
				res.setM21(c3 * s1 * s2 - c1 * s3);
				res.setM02(-s2);
				res.setM12(c2 * s3);
				res.setM22(c2 * c3);
				break;
			case ModeTaitBryan::XYZ:
				//c2*c3; c1*s3+c3*s1*s2; s1*s3-c1*c3*s2;
				//-c2*s3; c1*c3-s1*s2*s3; c3*s1+c1*s2*s3;
				//s2; -c2*s1; c1*c2;
				res.setM00(c2 * c3);
				res.setM10(-c2 * s3);
				res.setM20(s2);
				res.setM01(c1 * s3 + c3 * s1 * s2);
				res.setM11(c1 * c3 - s1 * s2 * s3);
				res.setM21(-c2 * s1);
				res.setM02(s1 * s3 - c1 * c3 * s2);
				res.setM12(c3 * s1 + c1 * s2 * s3);
				res.setM22(c1 * c2);
				break;
			case ModeTaitBryan::XZY:
				//c2*c3; s1*s3+c1*c3*s2; c3*s1*s2-c1*s3;
				//-s2; c1*c2; c2*s1;
				//c2*s3; c1*s2*s3-c3*s1; c1*c3+s1*s2*s3;
				res.setM00(c2 * c3);
				res.setM10(-s2);
				res.setM20(c2 * s3);
				res.setM01(s1 * s3 + c1 * c3 * s2);
				res.setM11(c1 * c2);
				res.setM21(c1 * s2 * s3 - c3 * s1);
				res.setM02(c3 * s1 * s2 - c1 * s3);
				res.setM12(c2 * s1);
				res.setM22(c1 * c3 + s1 * s2 * s3);
				break;
			case ModeTaitBryan::YZX:
				//c1*c2; s2; -c2*s1;
				//s1*s3-c1*c3*s2; c2*c3; c1*s3+c3*s1*s2;
				//c3*s1+c1*s2*s3; -c2*s3; c1*c3-s1*s2*s3;
				res.setM00(c1 * c2);
				res.setM10(s1 * s3 - c1 * c3 * s2);
				res.setM20(c3 * s1 + c1 * s2 * s3);
				res.setM01(s2);
				res.setM11(c2 * c3);
				res.setM21(-c2 * s3);
				res.setM02(-c2 * s1);
				res.setM12(c1 * s3 + c3 * s1 * s2);
				res.setM22(c1 * c3 - s1 * s2 * s3);
				break;
			case ModeTaitBryan::YXZ:
				//c1*c3+s1*s2*s3; c2*s3; c1*s2*s3-c3*s1;
				//c3*s1*s2-c1*s3; c2*c3; c1*c3*s2+s1*s3;
				//c2*s1; -s2; c1*c2;
				res.setM00(c1 * c3 + s1 * s2 * s3);
				res.setM10(c3 * s1 * s2 - c1 * s3);
				res.setM20(c2 * s1);
				res.setM01(c2 * s3);
				res.setM11(c2 * c3);
				res.setM21(-s2);
				res.setM02(c1 * s2 * s3 - c3 * s1);
				res.setM12(c1 * c3 * s2 + s1 * s3);
				res.setM22(c1 * c2);
				break;
			case ModeTaitBryan::ZXY:
				//c1*c3-s1*s2*s3; c3*s1+c1*s2*s3; -c2*s3;
				//-c2*s1; c1*c2; s2;
				//c1*s3+c3*s1*s2; s1*s3-c1*c3*s2; c2*c3;
				res.setM00(c1 * c3 - s1 * s2 * s3);
				res.setM10(-c2 * s1);
				res.setM20(c1 * s3 + c3 * s1 * s2);
				res.setM01(c3 * s1 + c1 * s2 * s3);
				res.setM11(c1 * c2);
				res.setM21(s1 * s3 - c1 * c3 * s2);
				res.setM02(-c2 * s3);
				res.setM12(s2);
				res.setM22(c2 * c3);
				break;
			default:
				cerr << "Error: Rotation Mode is not implemented" << endl;
				break;
		}
	}

	static Matrix4<double> computeSceneToCameraMatrixYawPitchRoll(	double yaw_Z,
																	double pitch_Y,
																	double roll_X,
																	ModeTaitBryan modeRot) {
		Matrix4<double> res;
		computeSceneToCameraMatrixYawPitchRoll(res, yaw_Z, pitch_Y, roll_X, modeRot);
		return res;
	}

	// https://en.wikipedia.org/wiki/Euler_angles
	static void computeCameraToSceneMatrixYawPitchRoll(Matrix4<double> & res,
												double yaw_Z,
												double pitch_Y,
												double roll_X,
												ModeTaitBryan modeRot) {
		//See detailed comments in the previous function:
		//The inverse is actually the same.

		//All the angles are reverted with reverted yaw, pitch, roll definition.
		double s1 = -sin(roll_X);
		double c1 = cos(roll_X);
		double s2 = -sin(pitch_Y);
		double c2 = cos(pitch_Y);
		double s3 = -sin(yaw_Z);
		double c3 = cos(yaw_Z);

		res.setToIdentity();

		switch (modeRot) {
			case ModeTaitBryan::ZYX:
				//c2*c3; c1*s3+c3*s1*s2; s1*s3-c1*c3*s2;
				//-c2*s3; c1*c3-s1*s2*s3; c3*s1+c1*s2*s3;
				//s2; -c2*s1; c1*c2;
				res.setM00(c2 * c3);
				res.setM10(-c2 * s3);
				res.setM20(s2);
				res.setM01(c1 * s3 + c3 * s1 * s2);
				res.setM11(c1 * c3 - s1 * s2 * s3);
				res.setM21(-c2 * s1);
				res.setM02(s1 * s3 - c1 * c3 * s2);
				res.setM12(c3 * s1 + c1 * s2 * s3);
				res.setM22(c1 * c2);
				break;
			case ModeTaitBryan::XYZ:
				//c1*c2; c2*s1; -s2;
				//c1*s2*s3-c3*s1; c1*c3+s1*s2*s3; c2*s3;
				//s1*s3+c1*c3*s2; c3*s1*s2-c1*s3; c2*c3;
				res.setM00(c1 * c2);
				res.setM10(c1 * s2 * s3 - c3 * s1);
				res.setM20(s1 * s3 + c1 * c3 * s2);
				res.setM01(c2 * s1);
				res.setM11(c1 * c3 + s1 * s2 * s3);
				res.setM21(c3 * s1 * s2 - c1 * s3);
				res.setM02(-s2);
				res.setM12(c2 * s3);
				res.setM22(c2 * c3);
				break;
			case ModeTaitBryan::XZY:
				//c1*c2; s2; -c2*s1;
				//s1*s3-c1*c3*s2; c2*c3; c1*s3+c3*s1*s2;
				//c3*s1+c1*s2*s3; -c2*s3; c1*c3-s1*s2*s3;
				res.setM00(c1 * c2);
				res.setM10(s1 * s3 - c1 * c3 * s2);
				res.setM20(c3 * s1 + c1 * s2 * s3);
				res.setM01(s2);
				res.setM11(c2 * c3);
				res.setM21(-c2 * s3);
				res.setM02(-c2 * s1);
				res.setM12(c1 * s3 + c3 * s1 * s2);
				res.setM22(c1 * c3 - s1 * s2 * s3);
				break;
			case ModeTaitBryan::YZX:
				//c2*c3; s1*s3+c1*c3*s2; c3*s1*s2-c1*s3;
				//-s2; c1*c2; c2*s1;
				//c2*s3; c1*s2*s3-c3*s1; c1*c3+s1*s2*s3;
				res.setM00(c2 * c3);
				res.setM10(-s2);
				res.setM20(c2 * s3);
				res.setM01(s1 * s3 + c1 * c3 * s2);
				res.setM11(c1 * c2);
				res.setM21(c1 * s2 * s3 - c3 * s1);
				res.setM02(c3 * s1 * s2 - c1 * s3);
				res.setM12(c2 * s1);
				res.setM22(c1 * c3 + s1 * s2 * s3);
				break;
			case ModeTaitBryan::YXZ:
				//c1*c3-s1*s2*s3; c3*s1+c1*s2*s3; -c2*s3;
				//-c2*s1; c1*c2; s2;
				//c1*s3+c3*s1*s2; s1*s3-c1*c3*s2; c2*c3;
				res.setM00(c1 * c3 - s1 * s2 * s3);
				res.setM10(-c2 * s1);
				res.setM20(c1 * s3 + c3 * s1 * s2);
				res.setM01(c3 * s1 + c1 * s2 * s3);
				res.setM11(c1 * c2);
				res.setM21(s1 * s3 - c1 * c3 * s2);
				res.setM02(-c2 * s3);
				res.setM12(s2);
				res.setM22(c2 * c3);
				break;
			case ModeTaitBryan::ZXY:
				//c1*c3+s1*s2*s3; c2*s3; c1*s2*s3-c3*s1;
				//c3*s1*s2-c1*s3; c2*c3; c1*c3*s2+s1*s3;
				//c2*s1; -s2; c1*c2;
				res.setM00(c1 * c3 + s1 * s2 * s3);
				res.setM10(c3 * s1 * s2 - c1 * s3);
				res.setM20(c2 * s1);
				res.setM01(c2 * s3);
				res.setM11(c2 * c3);
				res.setM21(-s2);
				res.setM02(c1 * s2 * s3 - c3 * s1);
				res.setM12(c1 * c3 * s2 + s1 * s3);
				res.setM22(c1 * c2);
				break;
			default:
				std::cerr << "Error: Rotation Mode is not implemented" << std::endl;
				break;
		}
	}

	static Matrix4<double> computeCameraToSceneMatrixYawPitchRoll(	double yaw_Z,
																	double pitch_Y,
																	double roll_X,
																	ModeTaitBryan modeRot) {
		Matrix4<double> res;
		computeCameraToSceneMatrixYawPitchRoll(res, yaw_Z, pitch_Y, roll_X, modeRot);
		return res;
	}

	std::string toFlatString() const;
	std::string toString() const;

	static ModeTaitBryan TaitBryanModeFromString(const std::string & code) {
		if (code == "XZY")
			return XZY;
		else if (code == "XYZ")
			return XYZ;
		else if (code == "YXZ")
			return YXZ;
		else if (code == "YZX")
			return YZX;
		else if (code == "ZYX")
			return ZYX;
		else if (code == "ZXY")
			return ZXY;
		else
			return XYZ;
	}

	static std::string TaitBryanModeToString(ModeTaitBryan mode) {
		switch (mode) {
			case XZY : return "XZY";
			case XYZ : return "XYZ";
			case YXZ : return "YXZ";
			case YZX : return "YZX";
			case ZYX : return "ZYX";
			case ZXY : return "ZXY";
			default: return "XYZ";
		}
	}
};

template<typename T>
inline const T & Matrix4<T>::getM00() const {
	return m[0][0];
}

template<typename T>
inline const T & Matrix4<T>::getM01() const {
	return m[0][1];
}

template<typename T>
inline const T & Matrix4<T>::getM02() const {
	return m[0][2];
}

template<typename T>
inline const T & Matrix4<T>::getM03() const {
	return m[0][3];
}

template<typename T>
inline const T & Matrix4<T>::getM10() const {
	return m[1][0];
}

template<typename T>
inline const T & Matrix4<T>::getM11() const {
	return m[1][1];
}

template<typename T>
inline const T & Matrix4<T>::getM12() const {
	return m[1][2];
}

template<typename T>
inline const T & Matrix4<T>::getM13() const {
	return m[1][3];
}

template<typename T>
inline const T & Matrix4<T>::getM20() const {
	return m[2][0];
}

template<typename T>
inline const T & Matrix4<T>::getM21() const {
	return m[2][1];
}

template<typename T>
inline const T & Matrix4<T>::getM22() const {
	return m[2][2];
}

template<typename T>
inline const T & Matrix4<T>::getM23() const {
	return m[2][3];
}

template<typename T>
inline const T & Matrix4<T>::getM30() const {
	return m[3][0];
}

template<typename T>
inline const T & Matrix4<T>::getM31() const {
	return m[3][1];
}

template<typename T>
inline const T & Matrix4<T>::getM32() const {
	return m[3][2];
}

template<typename T>
inline const T & Matrix4<T>::getM33() const {
	return m[3][3];
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM00(const T & m00) {
	m[0][0] = m00;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM01(const T & m01) {
	m[0][1] = m01;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM02(const T & m02) {
	m[0][2] = m02;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM03(const T & m03) {
	m[0][3] = m03;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM10(const T & m10) {
	m[1][0] = m10;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM11(const T & m11) {
	m[1][1] = m11;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM12(const T & m12) {
	m[1][2] = m12;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM13(const T & m13) {
	m[1][3] = m13;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM20(const T & m20) {
	m[2][0] = m20;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM21(const T & m21) {
	m[2][1] = m21;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM22(const T & m22) {
	m[2][2] = m22;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM23(const T & m23) {
	m[2][3] = m23;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM30(const T & m30) {
	m[3][0] = m30;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM31(const T & m31) {
	m[3][1] = m31;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM32(const T & m32) {
	m[3][2] = m32;
	return *this;
}

template<typename T>
inline Matrix4<T> & Matrix4<T>::setM33(const T & m33) {
	m[3][3] = m33;
	return *this;
}

template<typename T>
template<typename U>
inline Triplet<U> Matrix4<T>::multiply(const Triplet<U> & tuple) const {
	Triplet<U> res(tuple);
	res.setX(getM00() * tuple.getX() + getM01() * tuple.getY() + getM02() * tuple.getZ() + getM03() * tuple.getW());
	res.setY(getM10() * tuple.getX() + getM11() * tuple.getY() + getM12() * tuple.getZ() + getM13() * tuple.getW());
	res.setZ(getM20() * tuple.getX() + getM21() * tuple.getY() + getM22() * tuple.getZ() + getM23() * tuple.getW());
	// Perspective
	double sum = getM30() * tuple.getX() + getM31() * tuple.getY() + getM32() * tuple.getZ() + getM33() * tuple.getW();
	if(sum > 0.0 && sum != 1.0)
		res /= sum;

	return res;
}

template<typename T>
inline string Matrix4<T>::toFlatString() const {
	ostringstream os;
	os << getM00() << " " << getM01() << " ";
	os << getM02() << " " << getM03() << " ";
	os << getM10() << " " << getM11() << " ";
	os << getM12() << " " << getM13() << " ";
	os << getM20() << " " << getM21() << " ";
	os << getM22() << " " << getM23() << " ";
	os << getM30() << " " << getM31() << " ";
	os << getM32() << " " << getM33();

	return os.str();
}

template<typename T>
inline string Matrix4<T>::toString() const {
	ostringstream os;
	os << "|" << getM00() << "," << getM01() << ",";
	os << getM02() << "," << getM03() << "|" << endl;

	os << "|" << getM10() << "," << getM11() << ",";
	os << getM12() << "," << getM13() << "|" << endl;

	os << "|" << getM20() << "," << getM21() << ",";
	os << getM22() << "," << getM23() << "|" << endl;

	os << "|" << getM30() << "," << getM31() << ",";
	os << getM32() << "," << getM33() << "|";

	return os.str();
}

#endif /* MATRIX4_H_ */
