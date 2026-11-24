/*
 * PhaseSpecularComputation.hpp
 *
 *  Created on: 5 oct. 2015
 *      Author: guilleuxj
 */

#ifndef PHASESPECULARCOMPUTATION_HPP_
#define PHASESPECULARCOMPUTATION_HPP_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "database/model/SpecularDatabaseParameters.hpp"
#include "database/model/SpecularFoliarDatabaseParameters.hpp"

#include "PhaseUtils.hpp"

#include "foliar/LADComputation.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
using namespace boost;
#endif

/**
 * Fonctions de polarization & speculaire associées à phase.
 */
class PhasePolarization {
public:
	/* New functions to take into account the polarization */

	static void computeFresnelReflectancePerpendicular(	const double & deltaTheta,
														const double & refractiveIndex,
														const unsigned int & numberOfFresnelStep,
														std::vector<double> & perpendicularFresnelCoefficient) {
		double theta;
		double sineTheta;
		double sineThetaPlus;

		for (unsigned int index = 0; index <= numberOfFresnelStep; index++) {
			theta = index * deltaTheta;
			sineTheta = sin(theta);

			if (theta <= EPSILON) {
				perpendicularFresnelCoefficient[index] = (1 - refractiveIndex) / (1 + refractiveIndex);
				perpendicularFresnelCoefficient[index] *= perpendicularFresnelCoefficient[index];

			} else {
				sineThetaPlus = sin(theta + asin(sineTheta / refractiveIndex));

				if (sineThetaPlus == 0) {
					perpendicularFresnelCoefficient[index] = (1 - refractiveIndex) / (1 + refractiveIndex);
					perpendicularFresnelCoefficient[index] *= perpendicularFresnelCoefficient[index];

				} else {
					perpendicularFresnelCoefficient[index] = sin(theta - asin(sineTheta / refractiveIndex)) / sineThetaPlus;
					perpendicularFresnelCoefficient[index] *= perpendicularFresnelCoefficient[index];
				}
			}
		}
	}

	static void computeFresnelReflectanceParallel(	const double & deltaTheta,
													const double & refractiveIndex,
													const unsigned int & numberOfFresnelStep,
													std::vector<double> & parallelFresnelCoefficient) {
		double theta;
		double sineTheta;
		double tanThetaPlus;

		for (unsigned int index = 0; index <= numberOfFresnelStep; index++) {
			theta = index * deltaTheta;
			sineTheta = sin(theta);

			if (theta <= EPSILON) {
				parallelFresnelCoefficient[index] = (1 - refractiveIndex) / (1 + refractiveIndex);
				parallelFresnelCoefficient[index] *= parallelFresnelCoefficient[index];

			} else {
				tanThetaPlus = tan(theta + asin(sineTheta / refractiveIndex));

				if (tanThetaPlus == 0) {
					parallelFresnelCoefficient[index] = (1 - refractiveIndex) / (1 + refractiveIndex);
					parallelFresnelCoefficient[index] *= parallelFresnelCoefficient[index];
				} else {
					parallelFresnelCoefficient[index] = tan(theta - asin(sineTheta / refractiveIndex)) / tanThetaPlus;
					parallelFresnelCoefficient[index] *= parallelFresnelCoefficient[index];
				}

			}
		}
	}
	/* The precomputation of the Fresnel coefficients for the specular reflection on facets could not be used */
	static void computeAndWriteFresnel(	const unsigned int & modelIndex,
										const int & typeOfReflectance,
										shared_ptr<SpecularDatabaseParameters> & reflectanceMeanParameters) {
		unsigned int numberOfFresnelStep = 20;
		double deltaTheta = M_PI05 / numberOfFresnelStep;
		double refractionIndex;
		std::vector<double> perpendicularFresnelCoefficient;
		perpendicularFresnelCoefficient.resize(numberOfFresnelStep + 1);
		std::vector<double> parallelFresnelCoefficient;
		parallelFresnelCoefficient.resize(numberOfFresnelStep + 1);
		boost::numeric::ublas::matrix<double> fresnelPolarizationMatrix(4, 4);
		std::ostringstream val;
		val << modelIndex;
		std::string name;

		if (typeOfReflectance == globalReferences::LAMBERTIEN) {
			name = val.str() + "." + "Lambertian";
		}

		else if (typeOfReflectance == globalReferences::HAPKE_SPEC) {
			name = val.str() + "." + "Hapke1981";
		}

		else if (typeOfReflectance == globalReferences::HAPKE2_SPEC) {
			name = val.str() + "." + "Hapke2012";
		}

		else if (typeOfReflectance == globalReferences::RPV_PHASE) {
			name = val.str() + "." + "RPV";
		}

		std::string pathFresnelPolarizationMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
				+ "_FresnelPolarizationMatrices";
		std::fstream ofstreamFresnelPolarizationMatrices;
		ofstreamFresnelPolarizationMatrices.open(pathFresnelPolarizationMatrices.c_str(), std::fstream::out | std::fstream::binary);

		if (reflectanceMeanParameters == NULL) {
			fresnelPolarizationMatrix.clear();
			fresnelPolarizationMatrix(0, 0) = 1.0;
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
				for (unsigned int index = 0; index <= numberOfFresnelStep; index++) {
					writeFresnelPolarizationMatrix(fresnelPolarizationMatrix, ofstreamFresnelPolarizationMatrices);
				}
			}
		} else {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < PhaseProperty::getNumberOfSpectralBands(); spectralBandIndex++) {
				refractionIndex = reflectanceMeanParameters.get()->getRefractionIndexDownForBand(spectralBandIndex);
				computeFresnelReflectancePerpendicular(deltaTheta, refractionIndex, numberOfFresnelStep, perpendicularFresnelCoefficient);
				computeFresnelReflectanceParallel(deltaTheta, refractionIndex, numberOfFresnelStep, parallelFresnelCoefficient);

				for (unsigned int index = 0; index <= numberOfFresnelStep; index++) {
					computeReducedFresnelMatrix(perpendicularFresnelCoefficient[index], parallelFresnelCoefficient[index], fresnelPolarizationMatrix);
					fresnelPolarizationMatrix = 0.5 * (perpendicularFresnelCoefficient[index] + parallelFresnelCoefficient[index])
							* fresnelPolarizationMatrix;
					writeFresnelPolarizationMatrix(fresnelPolarizationMatrix, ofstreamFresnelPolarizationMatrices);
				}
			}
		}

		ofstreamFresnelPolarizationMatrices.close();
	}

	static double computeInterpolation(	const double & valueMin,
										const double & valueMax,
										const double & angleMax,
										const double & interpolatedAngle,
										const double & step) {
		return (interpolatedAngle - angleMax + step) / step * (valueMax - valueMin) + valueMin;
	}

	static double interpolateValue(	const std::vector<double> & array,
									const double & interpolatedAngle,
									const double & step) {
		for (unsigned int index = 0; index < array.size(); index++) {

			if (array[index] < 0.0) {
				cout << "ERROR in the sign of the value of the array for the calculation of the intersection" << index << " " << array[index] << endl;
//				std::getchar();
				return 0.;
			}

			if (step * double(index) == interpolatedAngle) {
				return array[index];

			} else if (step * double(index) > interpolatedAngle) {
				double interpolation = computeInterpolation(array[index - 1], array[index], step * double(index), interpolatedAngle, step);

				if (interpolation < 0.0) {
					cout << "ERROR in the calculation of the interpolation " << index << " " << interpolation << endl;
//					std::getchar();
					return 0.;
				}

				return interpolation;
			}
		}
		return array[array.size() - 1];
	}

	static void computeScatteringAngle(	const Vecteur<double> & omegaIn,
										const Vecteur<double> & omegaOut,
										double & scatteringAngle) {
		if (omegaIn.scalaire(omegaOut) >= 1.0) {
			scatteringAngle = 0.0;
		}

		else if (omegaIn.scalaire(omegaOut) <= -1.0) {
			scatteringAngle = M_PI;
		}

		else {
			scatteringAngle = acos(omegaIn.scalaire(omegaOut));
		}
	}

	static void computePolarizationRotationAngles(	const Vecteur<double> & omegaIn,
													const Vecteur<double> & omegaOut,
													double & polarizationRotationAngle1,
													double & polarizationRotationAngle2) {
		Vecteur<double> zVector;
		Vecteur<double> normalInputPlane;
		Vecteur<double> normalOutputPlane;
		Vecteur<double> normalScatteringPlane;
		zVector.setXYZ(0.0, 0.0, 1.0);

		polarizationRotationAngle1 = 0.0;
		polarizationRotationAngle2 = 0.0;

		normalInputPlane = zVector.vectoriel(omegaIn);
		normalInputPlane = normalInputPlane.normaliser();
		normalOutputPlane = zVector.vectoriel(omegaOut);
		normalOutputPlane = normalOutputPlane.normaliser();
		normalScatteringPlane = omegaIn.vectoriel(omegaOut);
		normalScatteringPlane = normalScatteringPlane.normaliser();

		if (normalScatteringPlane.getLongueur() > 0.0) {
			if (fabs(zVector.scalaire(normalScatteringPlane)) <= EPSILON) {
				polarizationRotationAngle1 = 0.0;
				polarizationRotationAngle2 = 0.0;
			} else if (zVector.scalaire(normalScatteringPlane) > 0.0) {
				polarizationRotationAngle1 = acos(normalInputPlane.scalaire(normalScatteringPlane));
				polarizationRotationAngle2 = acos(normalOutputPlane.scalaire(normalScatteringPlane));
			} else if (zVector.scalaire(normalScatteringPlane) < 0.0) {
				polarizationRotationAngle1 = -acos(normalInputPlane.scalaire(normalScatteringPlane));
				polarizationRotationAngle2 = -acos(normalOutputPlane.scalaire(normalScatteringPlane));
			}
			/* In the atmosphere, this case is like direct transmission or backscattering */
		} else {
			polarizationRotationAngle1 = 0.0;
			polarizationRotationAngle2 = 0.0;
		}
	}

	static void initializeMatrix(boost::numeric::ublas::matrix<double> & matrix) {
		for (unsigned int indexRow = 0; indexRow < matrix.size1(); indexRow++) {
			for (unsigned int indexColumn = 0; indexColumn < matrix.size2(); indexColumn++) {
				matrix(indexRow, indexColumn) = 0.0;
			}
		}
	}

	static void correctMatrix(boost::numeric::ublas::matrix<double> & matrix) {
		for (unsigned int indexRow = 0; indexRow < matrix.size1(); indexRow++) {
			for (unsigned int indexColumn = 0; indexColumn < matrix.size2(); indexColumn++) {
				if (fabs(matrix(indexRow, indexColumn)) <= EPSILON) {
					matrix(indexRow, indexColumn) = 0.0;
				}
			}
		}
	}

	static void computePolarizationRotationMatrix(	const double & polarizationRotationAngle,
													boost::numeric::ublas::matrix<double> & polarizationRotationMatrix) {
		initializeMatrix(polarizationRotationMatrix);

		polarizationRotationMatrix(0, 0) = 1.0;
		polarizationRotationMatrix(1, 1) = cos(2.0 * polarizationRotationAngle);
		polarizationRotationMatrix(1, 2) = sin(2.0 * polarizationRotationAngle);
		polarizationRotationMatrix(2, 1) = -sin(2.0 * polarizationRotationAngle);
		polarizationRotationMatrix(2, 2) = cos(2.0 * polarizationRotationAngle);
		polarizationRotationMatrix(3, 3) = 1.0;

		correctMatrix(polarizationRotationMatrix);
	}

	static void computeReducedFresnelMatrix(const double & perpendicularFresnelCoefficient,
											const double & parallelFresnelCoefficient,
											boost::numeric::ublas::matrix<double> & reducedFresnelMatrix) {
		initializeMatrix(reducedFresnelMatrix);

		reducedFresnelMatrix(0, 0) = 1.0;
		reducedFresnelMatrix(1, 1) = 1.0;
		reducedFresnelMatrix(0, 1) = (perpendicularFresnelCoefficient - parallelFresnelCoefficient)
				/ (perpendicularFresnelCoefficient + parallelFresnelCoefficient);
		reducedFresnelMatrix(1, 0) = reducedFresnelMatrix(0, 1);
		reducedFresnelMatrix(2, 2) = 2.0 * perpendicularFresnelCoefficient * parallelFresnelCoefficient
				/ (perpendicularFresnelCoefficient + parallelFresnelCoefficient);
		reducedFresnelMatrix(3, 3) = reducedFresnelMatrix(2, 2);

		correctMatrix(reducedFresnelMatrix);
	}

	static void writeFresnelPolarizationMatrix(	boost::numeric::ublas::matrix<double> & fresnelPolarizationMatrix,
												fstream & ofstreamFresnelPolarizationMatrices) {
		for (unsigned int indexRow = 0; indexRow < fresnelPolarizationMatrix.size1(); indexRow++) {
			for (unsigned int indexColumn = 0; indexColumn < fresnelPolarizationMatrix.size2(); indexColumn++) {
				ofstreamFresnelPolarizationMatrices.write((char *) &fresnelPolarizationMatrix(indexRow, indexColumn), sizeof(double));
			}
		}
	}

	static void writeFresnelPolarizationMatrixNotBinary(boost::numeric::ublas::matrix<double> & fresnelPolarizationMatrix,
														fstream & ofstreamFresnelPolarizationMatrices) {
		for (unsigned int indexRow = 0; indexRow < fresnelPolarizationMatrix.size1(); indexRow++) {
			for (unsigned int indexColumn = 0; indexColumn < fresnelPolarizationMatrix.size2(); indexColumn++) {
				ofstreamFresnelPolarizationMatrices << fresnelPolarizationMatrix(indexRow, indexColumn) << endl;
			}
		}
	}

	static double computeThetaSpecularFoliarDirection(	const DirectionCartesienne & directionIn,
														const DirectionCartesienne & directionOut) {
		double root = 2.0 * (1.0 - directionIn.getDirectionCartesienne().scalaire(directionOut.getDirectionCartesienne()));
		double denominator;

		if (root < 0.0) {
			return 0.0;
		} else {
			denominator = sqrt(root);
		}
		if (denominator == 0.0) {
			return 0.0;

		} else if (fabs((fabs(cos(directionIn.getCenterTheta()) - cos(directionOut.getCenterTheta())) / denominator) - 1.0) <= EPSILON) {
			return 0.0;
		} else {
			return acos(fabs(cos(directionIn.getCenterTheta()) - cos(directionOut.getCenterTheta())) / denominator);
		}
	}

	static double computePhiSpecularFoliarDirection(const DirectionCartesienne & directionIn,
													const DirectionCartesienne & directionOut) {
		double root1 = 1.0 - cos(directionIn.getCenterTheta()) * cos(directionIn.getCenterTheta());
		double root2 = 1.0 - cos(directionOut.getCenterTheta()) * cos(directionOut.getCenterTheta());
		double denominator;

		if (root1 < 0.0 || root2 < 0.0) {
			return 0.0;
		} else {
			denominator = sqrt(root1) * cos(directionIn.getCenterPhi()) - sqrt(root2) * cos(directionOut.getCenterPhi());
		}
		if (denominator == 0.0) {
			return 0.0;
		} else {
			return atan((sqrt(root1) * sin(directionIn.getCenterPhi()) - sqrt(root2) * sin(directionOut.getCenterPhi())) / denominator);
		}
	}

	static double computeSolidAngleSpecularFoliarDirection(	const DirectionCartesienne & directionIn,
															const DirectionCartesienne & directionOut,
															const DirectionCartesienne & specularFoliarDirection) {
		double sqRootIn;
		double sqRootOut;

		if (directionIn.getSolidAngle() < 0.0 || directionOut.getSolidAngle() < 0.0) {
			return 0.0;
		} else {
			sqRootIn = sqrt(directionIn.getSolidAngle());
			sqRootOut = sqrt(directionOut.getSolidAngle());
		}

		double denominator = fabs(directionIn.getDirectionCartesienne().scalaire(specularFoliarDirection.getDirectionCartesienne()));
		if (denominator == 0.0) {
			return 0.0;
		} else {
			return (sqRootIn + sqRootOut) * (sqRootIn + sqRootOut) * 0.25 / denominator;
		}
	}

	static void computeSpecularFoliarDirection(	const DirectionCartesienne & directionIn,
												const DirectionCartesienne & directionOut,
												DirectionCartesienne & specularFoliarDirection) {
		specularFoliarDirection.setCenterTheta(computeThetaSpecularFoliarDirection(directionIn, directionOut));
		specularFoliarDirection.setCenterPhi(computePhiSpecularFoliarDirection(directionIn, directionOut));
		specularFoliarDirection.calculerCoordonneesCartesiennes();
		specularFoliarDirection.setSolidAngle(computeSolidAngleSpecularFoliarDirection(directionIn, directionOut, specularFoliarDirection));
	}

	static void precomputeRotations(double & polarizationRotationAngle1,
									double & polarizationRotationAngle2,
									double & scatteringAngle,
									boost::numeric::ublas::matrix<double> & polarizationRotationMatrix1,
									boost::numeric::ublas::matrix<double> & polarizationRotationMatrix2,
									const Vecteur<double> & directionIn,
									const Vecteur<double> & directionOut) {

		computePolarizationRotationAngles(directionIn, directionOut, polarizationRotationAngle1, polarizationRotationAngle2);
		computePolarizationRotationMatrix(polarizationRotationAngle1, polarizationRotationMatrix1);
		computePolarizationRotationMatrix(-polarizationRotationAngle2, polarizationRotationMatrix2);
		computeScatteringAngle(directionIn, directionOut, scatteringAngle);
	}

	static void computeFoliarPolarizationMatrix(const double & roughness,
												const double & roughnessIndex,
												const std::vector<double> & functionG,
												const std::vector<double> & leafAngularDistribution,
												const double & stepLeafAngularDistribution,
												const std::vector<double> & perpendicularFresnelCoefficient,
												const std::vector<double> & parallelFresnelCoefficient,
												const double & stepFresnelCoefficient,
												const std::vector<DirectionCartesienne> & listDirections,
												std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferFoliarPolarizationMatrixPerDirection,
												std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferFoliarPolarizationMatrixPerAngularSector) {
		double scatteringAngle;
		double phaseAngle;
		double polarizationRotationAngle1;
		double polarizationRotationAngle2;
		double cosPSIfs;
		double PSIfs;
		double roughnessParameter;
		double roughnessParameterIndex;
		double solidAngleSum;
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix1(4, 4);
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix2(4, 4);
		boost::numeric::ublas::matrix<double> reducedFresnelMatrix(4, 4);
		boost::numeric::ublas::matrix<double> matrixProduct(4, 4);
		DirectionCartesienne specularFoliarDirection;

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				initializeMatrix(transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut]);
				if (directionIn != directionOut) {
					precomputeRotations(polarizationRotationAngle1, polarizationRotationAngle2, scatteringAngle, polarizationRotationMatrix1,
						polarizationRotationMatrix2, listDirections[directionIn].getDirectionCartesienne(),
						listDirections[directionOut].getDirectionCartesienne());
					phaseAngle = M_PI05 - scatteringAngle * 0.5;
					computeReducedFresnelMatrix(interpolateValue(perpendicularFresnelCoefficient, phaseAngle, stepFresnelCoefficient),
						interpolateValue(parallelFresnelCoefficient, phaseAngle, stepFresnelCoefficient), reducedFresnelMatrix);
					computeSpecularFoliarDirection(listDirections[directionIn], listDirections[directionOut], specularFoliarDirection);
					matrixProduct = boost::numeric::ublas::prod(reducedFresnelMatrix, polarizationRotationMatrix1);
					cosPSIfs = listDirections[directionIn].getDirectionCartesienne().scalaire(specularFoliarDirection.getDirectionCartesienne());
					if (fabs(cosPSIfs - 1.0) <= EPSILON) {
						PSIfs = 0.0;
					} else if (fabs(cosPSIfs + 1.0) <= EPSILON) {
						PSIfs = M_PI;
					} else {
						PSIfs = acos(cosPSIfs);
					}
					roughnessParameter = roughness;
					roughnessParameterIndex = roughnessIndex;
					if (specularFoliarDirection.getCenterTheta() <= EPSILON) {
						if (fabs(listDirections[directionIn].getCenterTheta() - M_PI) <= EPSILON) {
							transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut] = fabs(cosPSIfs)
									* interpolateValue(leafAngularDistribution,
										computeThetaSpecularFoliarDirection(listDirections[directionIn], listDirections[directionOut]),
										stepLeafAngularDistribution)
									* (listDirections[directionIn].getDeltaTheta() + listDirections[directionOut].getDeltaTheta()) / 4.0
									* PhaseUtils::computeRoughness(PSIfs, roughnessParameter, roughnessParameterIndex) * 0.5
									* (interpolateValue(perpendicularFresnelCoefficient, phaseAngle, stepFresnelCoefficient)
											+ interpolateValue(parallelFresnelCoefficient, phaseAngle, stepFresnelCoefficient))
									* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct) / functionG[directionIn];
						} else {
							transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut] = fabs(cosPSIfs)
									* interpolateValue(leafAngularDistribution,
										computeThetaSpecularFoliarDirection(listDirections[directionIn], listDirections[directionOut]),
										stepLeafAngularDistribution)
									* (listDirections[directionIn].getDeltaTheta() + listDirections[directionOut].getDeltaTheta()) / 2.0
									* PhaseUtils::computeRoughness(PSIfs, roughnessParameter, roughnessParameterIndex) * 0.5
									* (interpolateValue(perpendicularFresnelCoefficient, phaseAngle, stepFresnelCoefficient)
											+ interpolateValue(parallelFresnelCoefficient, phaseAngle, stepFresnelCoefficient))
									* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct) / functionG[directionIn];
						}
					} else {
						transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut] = fabs(cosPSIfs)
								* interpolateValue(leafAngularDistribution,
									computeThetaSpecularFoliarDirection(listDirections[directionIn], listDirections[directionOut]),
									stepLeafAngularDistribution) / (sin(specularFoliarDirection.getCenterTheta()) * M_PI)
								* specularFoliarDirection.getSolidAngle() * PhaseUtils::computeRoughness(PSIfs, roughnessParameter, roughnessParameterIndex) * 0.5
								* (interpolateValue(perpendicularFresnelCoefficient, phaseAngle, stepFresnelCoefficient)
										+ interpolateValue(parallelFresnelCoefficient, phaseAngle, stepFresnelCoefficient))
								* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct) / functionG[directionIn];
					}
				}
			}
		}
		for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
			initializeMatrix(transferFoliarPolarizationMatrixPerAngularSector[PhaseProperty::getNumberOfAngularSectors()][directionOut]);
		}
		transferFoliarPolarizationMatrixPerAngularSector[PhaseProperty::getNumberOfAngularSectors()] =
				transferFoliarPolarizationMatrixPerDirection[PhaseProperty::getNumberOfDirection()];
		for (int angularSector = 0; angularSector < PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				solidAngleSum = 0.0;
				initializeMatrix(transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut]);
				for (int directionIn = 0; directionIn < PhaseProperty::getNumberOfDirection(); directionIn++) {
					if ((listDirections[directionIn].getProperty() != DirectionRoot::VIRTUAL)
							&& (angularSector == int(listDirections[directionIn].getAngularSector()))) {
						solidAngleSum += listDirections[directionIn].getSolidAngle();
						transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut] +=
								transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut] * listDirections[directionIn].getSolidAngle();
					}
				}
				if (solidAngleSum > 0) {
					transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut] /= solidAngleSum;
				}
			}
		}
	}

	static void computeReducedRayleighMatrix(	const double & scatteringAngle,
												boost::numeric::ublas::matrix<double> & reducedRayleighMatrix) {
		initializeMatrix(reducedRayleighMatrix);

		reducedRayleighMatrix(0, 0) = 1.0;
		reducedRayleighMatrix(1, 1) = 1.0;
		reducedRayleighMatrix(0, 1) = (1.0 - cos(scatteringAngle) * cos(scatteringAngle)) / (1.0 + cos(scatteringAngle) * cos(scatteringAngle));
		reducedRayleighMatrix(1, 0) = reducedRayleighMatrix(0, 1);
		reducedRayleighMatrix(2, 2) = 2.0 * cos(scatteringAngle) / (1.0 + cos(scatteringAngle) * cos(scatteringAngle));
		reducedRayleighMatrix(3, 3) = reducedRayleighMatrix(2, 2);

		correctMatrix(reducedRayleighMatrix);
	}

	static void computePolarizationMatrixFluid(	AirPhaseFunction & airPhaseFunction,
												const std::vector<DirectionCartesienne> & listDirections,
												const unsigned int & spectralBandIndex,
												std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferRayleighPolarizationMatrix,
												std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferRayleighPolarizationMatrixPerAngularSector) {
		double scatteringAngle;
		double polarizationRotationAngle1;
		double polarizationRotationAngle2;
		double solidAngleSum;
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix1(4, 4);
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix2(4, 4);
		boost::numeric::ublas::matrix<double> reducedRayleighMatrix(4, 4);
		boost::numeric::ublas::matrix<double> matrixProduct(4, 4);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				initializeMatrix(transferRayleighPolarizationMatrix[directionIn][directionOut]);
				precomputeRotations(polarizationRotationAngle1, polarizationRotationAngle2, scatteringAngle, polarizationRotationMatrix1,
					polarizationRotationMatrix2, listDirections[directionIn].getDirectionCartesienne(),
					listDirections[directionOut].getDirectionCartesienne());
				computeReducedRayleighMatrix(scatteringAngle, reducedRayleighMatrix);
				matrixProduct = boost::numeric::ublas::prod(reducedRayleighMatrix, polarizationRotationMatrix1);
				transferRayleighPolarizationMatrix[directionIn][directionOut] =
						airPhaseFunction.getScatteringFunctionInDirectionToOutDirection()[spectralBandIndex][directionIn][directionOut]
								* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct);
			}
		}
		for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
			initializeMatrix(transferRayleighPolarizationMatrixPerAngularSector[PhaseProperty::getNumberOfAngularSectors()][directionOut]);
		}
		transferRayleighPolarizationMatrixPerAngularSector[PhaseProperty::getNumberOfAngularSectors()] =
				transferRayleighPolarizationMatrix[PhaseProperty::getNumberOfDirection()];
		for (int angularSector = 0; angularSector < PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				solidAngleSum = 0.0;
				initializeMatrix(transferRayleighPolarizationMatrixPerAngularSector[angularSector][directionOut]);
				for (int directionIn = 0; directionIn < PhaseProperty::getNumberOfDirection(); directionIn++) {
					if ((listDirections[directionIn].getProperty() != DirectionRoot::VIRTUAL)
							&& (angularSector == int(listDirections[directionIn].getAngularSector()))) {
						solidAngleSum += listDirections[directionIn].getSolidAngle();
						transferRayleighPolarizationMatrixPerAngularSector[angularSector][directionOut] +=
								transferRayleighPolarizationMatrix[directionIn][directionOut] * listDirections[directionIn].getSolidAngle();
					}
				}
				if (solidAngleSum > 0) {
					transferRayleighPolarizationMatrixPerAngularSector[angularSector][directionOut] /= solidAngleSum;
				}
			}
		}
	}

	static void computeRayleighPolarizationMatrixAtmosphere(const std::vector<DirectionCartesienne> & listDirections,
															const unsigned int & spectralBandIndex,
															std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferRayleighPolarizationMatrix) {
		double scatteringAngle;
		double polarizationRotationAngle1;
		double polarizationRotationAngle2;
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix1(4, 4);
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix2(4, 4);
		boost::numeric::ublas::matrix<double> reducedRayleighMatrix(4, 4);
		boost::numeric::ublas::matrix<double> matrixProduct(4, 4);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				initializeMatrix(transferRayleighPolarizationMatrix[directionIn][directionOut]);
				precomputeRotations(polarizationRotationAngle1, polarizationRotationAngle2, scatteringAngle, polarizationRotationMatrix1,
					polarizationRotationMatrix2, listDirections[directionIn].getDirectionCartesienne(),
					listDirections[directionOut].getDirectionCartesienne());
				computeReducedRayleighMatrix(scatteringAngle, reducedRayleighMatrix);
				matrixProduct = boost::numeric::ublas::prod(reducedRayleighMatrix, polarizationRotationMatrix1);
				transferRayleighPolarizationMatrix[directionIn][directionOut] =
						PhaseProperty::getAtmospherePhaseFunction()[0][directionIn][directionOut][spectralBandIndex]
								* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct);
			}
		}
	}

	static void computeAerosolPolarizationMatrixAtmosphere(	const std::vector<DirectionCartesienne> & listDirections,
															const unsigned int & spectralBandIndex,
															std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & transferRayleighPolarizationMatrix) {
		double scatteringAngle;
		double polarizationRotationAngle1;
		double polarizationRotationAngle2;
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix1(4, 4);
		boost::numeric::ublas::matrix<double> polarizationRotationMatrix2(4, 4);
		boost::numeric::ublas::matrix<double> reducedRayleighMatrix(4, 4);
		boost::numeric::ublas::matrix<double> matrixProduct(4, 4);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				initializeMatrix(transferRayleighPolarizationMatrix[directionIn][directionOut]);
				precomputeRotations(polarizationRotationAngle1, polarizationRotationAngle2, scatteringAngle, polarizationRotationMatrix1,
					polarizationRotationMatrix2, listDirections[directionIn].getDirectionCartesienne(),
					listDirections[directionOut].getDirectionCartesienne());
				computeReducedRayleighMatrix(scatteringAngle, reducedRayleighMatrix);
				matrixProduct = boost::numeric::ublas::prod(reducedRayleighMatrix, polarizationRotationMatrix1);
				transferRayleighPolarizationMatrix[directionIn][directionOut] =
						PhaseProperty::getAtmospherePhaseFunction()[1][directionIn][directionOut][spectralBandIndex]
								* boost::numeric::ublas::prod(polarizationRotationMatrix2, matrixProduct);
			}
		}
	}

	static void writeMatrix(fstream & ofstreamMatrix,
							const std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & polarizationMatrix) {
		for (unsigned int directionIn = 0; directionIn < polarizationMatrix.size(); directionIn++) {
			for (unsigned int directionOut = 0; directionOut < polarizationMatrix[directionIn].size(); directionOut++) {
				for (unsigned int indexRow = 0; indexRow < polarizationMatrix[directionIn][directionOut].size1(); indexRow++) {
					for (unsigned int indexColumn = 0; indexColumn < polarizationMatrix[directionIn][directionOut].size2(); indexColumn++) {
						ofstreamMatrix.write((char *) &polarizationMatrix[directionIn][directionOut](indexRow, indexColumn), sizeof(double));
					}
				}
			}
		}
	}

	static void writeMatrixNotBinary(	fstream & ofstreamMatrix,
										const std::vector<std::vector<boost::numeric::ublas::matrix<double> > > & polarizationMatrix) {
		for (unsigned int directionIn = 0; directionIn < polarizationMatrix.size(); directionIn++) {
			for (unsigned int directionOut = 0; directionOut < polarizationMatrix[directionIn].size(); directionOut++) {
				for (unsigned int indexRow = 0; indexRow < polarizationMatrix[directionIn][directionOut].size1(); indexRow++) {
					for (unsigned int indexColumn = 0; indexColumn < polarizationMatrix[directionIn][directionOut].size2(); indexColumn++) {
						ofstreamMatrix << directionIn << " " << directionOut << " " << indexRow << " " << indexColumn << " "
								<< polarizationMatrix[directionIn][directionOut](indexRow, indexColumn) << endl;
					}
				}
			}
		}
	}

	static void computeAndWriteFoliarPolarizationMatrix(const PhaseGeneralParametersDTO & genDTO,
														OpticalPropertyParameters & foliarPropertyParameters,
														const unsigned int & spectralBandIndex,
														const std::vector<DirectionCartesienne> & listDirections,
														const unsigned int & numberOfFresnelStep,
														FoliarModel & foliarModel,
														LADComputation & ladComputation) {
		unsigned int foliarModelIndice = foliarPropertyParameters.getModelIndex();
		shared_ptr<SpecularFoliarDatabaseParameters> foliarSpecularDatabaseParameters = ::static_pointer_cast<SpecularFoliarDatabaseParameters>(
			foliarPropertyParameters.getDatabaseParameters(SPECULAR_FOLIAR_DATABASE));

		double stepFresnelCoefficient = M_PI05 / numberOfFresnelStep;
		std::vector<double> perpendicularFresnelCoefficient;
		std::vector<double> parallelFresnelCoefficient;
		perpendicularFresnelCoefficient.resize(numberOfFresnelStep + 1);
		parallelFresnelCoefficient.resize(numberOfFresnelStep + 1);
		if (foliarSpecularDatabaseParameters != NULL) {
			computeFresnelReflectanceParallel(stepFresnelCoefficient,
				foliarSpecularDatabaseParameters->getTopRefractionIndexForBand(spectralBandIndex), numberOfFresnelStep, parallelFresnelCoefficient);
			computeFresnelReflectancePerpendicular(stepFresnelCoefficient,
				foliarSpecularDatabaseParameters->getTopRefractionIndexForBand(spectralBandIndex), numberOfFresnelStep,
				perpendicularFresnelCoefficient);
		}
		double ladMParameter = 0;
		double labB0Parameter = 0;
		double ladB1Parameter = 0;
		double ladB2Parameter = 0;
		std::string nameDatabaseLAD = "";
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferFoliarPolarizationMatrixPerDirection;
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferFoliarPolarizationMatrixPerAngularSector;
		transferFoliarPolarizationMatrixPerDirection.resize(PhaseProperty::getNumberOfDirection() + 1);
		transferFoliarPolarizationMatrixPerAngularSector.resize(PhaseProperty::getNumberOfAngularSectors() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferFoliarPolarizationMatrixPerDirection[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut].resize(4, 4);
			}
		}
		for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			transferFoliarPolarizationMatrixPerAngularSector[angularSector].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut].resize(4, 4);
			}
		}
		std::ostringstream val;
		val << foliarModelIndice;
		std::string name;
		name = val.str() + "." + foliarPropertyParameters.getModelName();
		std::string pathFoliarSpecularFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
				+ "_FoliarSpecularFunctionMatrices";
		std::fstream ofstreampathFoliarSpecularFunctionMatrices;
		ofstreampathFoliarSpecularFunctionMatrices.open(pathFoliarSpecularFunctionMatrices.c_str(), std::fstream::out | std::fstream::binary);

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForFoliar(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter,
				nameDatabaseLAD, foliarModel), "");
		if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPS
				&& foliarModel.getFoliarAverageLeafAngle(foliarModelIndice) != 0) {

			ladComputation.computeEllipsoidalLadDistribution(foliarModelIndice, foliarModel.getFoliarAverageLeafAngle(foliarModelIndice));

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPT) {

			ladComputation.computeEllipticalLadDistribution(foliarModelIndice, foliarModel.getFoliarEllipsePrincipalAngle(foliarModelIndice),
				foliarModel.getFoliarEllipseEccentricity(foliarModelIndice));

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::HORIZ
				|| (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPS
						&& foliarModel.getFoliarAverageLeafAngle(foliarModelIndice) < 1e-10)) {

			ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 0);

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::VERTI) {

			ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 90);

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::MANUAL) {

			ladComputation.computeClassicalLadDistribution(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter);
		}
		if (foliarSpecularDatabaseParameters == NULL) {
			for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
				for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
					transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut].clear();
					transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut](0, 0) = 1.0;
				}
			}
		} else {
			computeFoliarPolarizationMatrix(foliarSpecularDatabaseParameters->getTopForBand(spectralBandIndex),
				foliarSpecularDatabaseParameters->getTopIndexForBand(spectralBandIndex), foliarModel.getTransmTransfertFunctionVector(),
				foliarModel.getLeafAngleDistributionFunctionVector()[foliarModelIndice], 1.0 / 180.0 * M_PI, perpendicularFresnelCoefficient,
				parallelFresnelCoefficient, stepFresnelCoefficient, listDirections, transferFoliarPolarizationMatrixPerDirection,
				transferFoliarPolarizationMatrixPerAngularSector);
		}
		writeMatrix(ofstreampathFoliarSpecularFunctionMatrices, transferFoliarPolarizationMatrixPerAngularSector);
		ofstreampathFoliarSpecularFunctionMatrices.close();
	}

	static void computeAndWriteFoliarPolarizationMatrixNotBinary(	const PhaseGeneralParametersDTO & genDTO,
																	OpticalPropertyParameters & foliarPropertyParameters,
																	const unsigned int & spectralBandIndex,
																	const std::vector<DirectionCartesienne> & listDirections,
																	const unsigned int & numberOfFresnelStep,
																	FoliarModel & foliarModel,
																	LADComputation & ladComputation) {
		unsigned int foliarModelIndice = foliarPropertyParameters.getModelIndex();
		shared_ptr<SpecularFoliarDatabaseParameters> foliarSpecularDatabaseParameters = ::static_pointer_cast<SpecularFoliarDatabaseParameters>(
			foliarPropertyParameters.getDatabaseParameters(SPECULAR_FOLIAR_DATABASE));

		double stepFresnelCoefficient = M_PI05 / numberOfFresnelStep;
		std::vector<double> perpendicularFresnelCoefficient;
		std::vector<double> parallelFresnelCoefficient;
		perpendicularFresnelCoefficient.resize(numberOfFresnelStep + 1);
		parallelFresnelCoefficient.resize(numberOfFresnelStep + 1);
		if (foliarSpecularDatabaseParameters != NULL) {
			computeFresnelReflectanceParallel(stepFresnelCoefficient,
				foliarSpecularDatabaseParameters->getTopRefractionIndexForBand(spectralBandIndex), numberOfFresnelStep, parallelFresnelCoefficient);
			computeFresnelReflectancePerpendicular(stepFresnelCoefficient,
				foliarSpecularDatabaseParameters->getTopRefractionIndexForBand(spectralBandIndex), numberOfFresnelStep,
				perpendicularFresnelCoefficient);
		}
		double ladMParameter = 0;
		double labB0Parameter = 0;
		double ladB1Parameter = 0;
		double ladB2Parameter = 0;
		std::string nameDatabaseLAD = "";
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferFoliarPolarizationMatrixPerDirection;
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferFoliarPolarizationMatrixPerAngularSector;
		transferFoliarPolarizationMatrixPerDirection.resize(PhaseProperty::getNumberOfDirection() + 1);
		transferFoliarPolarizationMatrixPerAngularSector.resize(PhaseProperty::getNumberOfAngularSectors() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferFoliarPolarizationMatrixPerDirection[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferFoliarPolarizationMatrixPerDirection[directionIn][directionOut].resize(4, 4);
			}
		}
		for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			transferFoliarPolarizationMatrixPerAngularSector[angularSector].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut].resize(4, 4);
			}
		}
		std::ostringstream val;
		val << foliarModelIndice;
		std::string name;
		name = val.str() + "." + foliarPropertyParameters.getModelName();
		std::string pathFoliarSpecularFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
				+ "_FoliarSpecularFunctionMatrices.txt";
		std::fstream ofstreamFoliarSpecularFunctionMatrices;
		ofstreamFoliarSpecularFunctionMatrices.open(pathFoliarSpecularFunctionMatrices.c_str(), std::fstream::out);

		EXIT_IF(
			!PhaseDAO::readCoeffDiffParametersForFoliar(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter,
				nameDatabaseLAD, foliarModel), "");
		if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPS
				&& foliarModel.getFoliarAverageLeafAngle(foliarModelIndice) != 0) {

			ladComputation.computeEllipsoidalLadDistribution(foliarModelIndice, foliarModel.getFoliarAverageLeafAngle(foliarModelIndice));

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPT) {

			ladComputation.computeEllipticalLadDistribution(foliarModelIndice, foliarModel.getFoliarEllipsePrincipalAngle(foliarModelIndice),
				foliarModel.getFoliarEllipseEccentricity(foliarModelIndice));

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::HORIZ
				|| (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::ELLIPS
						&& foliarModel.getFoliarAverageLeafAngle(foliarModelIndice) < 1e-10)) {

			ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 0);

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::VERTI) {

			ladComputation.computeVerticalHorizontalLadDistribution(foliarModelIndice, 90);

		} else if (foliarModel.getFoliarTypeOfLeafAngleDistribution(foliarModelIndice) == globalReferences::MANUAL) {

			ladComputation.computeClassicalLadDistribution(foliarModelIndice, ladMParameter, labB0Parameter, ladB1Parameter, ladB2Parameter);
		}

		if (foliarSpecularDatabaseParameters == NULL) {
			for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
				for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
					transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut].clear();
					transferFoliarPolarizationMatrixPerAngularSector[angularSector][directionOut](0, 0) = 1.0;
				}
			}
		} else {
			computeFoliarPolarizationMatrix(foliarSpecularDatabaseParameters->getTopIndexForBand(spectralBandIndex),
				foliarSpecularDatabaseParameters->getTopIndexForBand(spectralBandIndex), foliarModel.getTransmTransfertFunctionVector(),
				foliarModel.getLeafAngleDistributionFunctionVector()[foliarModelIndice], 1.0 / 180.0 * M_PI, perpendicularFresnelCoefficient,
				parallelFresnelCoefficient, stepFresnelCoefficient, listDirections, transferFoliarPolarizationMatrixPerDirection,
				transferFoliarPolarizationMatrixPerAngularSector);
		}
		writeMatrixNotBinary(ofstreamFoliarSpecularFunctionMatrices, transferFoliarPolarizationMatrixPerAngularSector);
		ofstreamFoliarSpecularFunctionMatrices.close();
	}

	static void computeAndWritePolarizationMatrixFluid(	AirPhaseFunction & airPhaseFunction,
														const std::vector<DirectionCartesienne> & listDirections,
														const int & spectralBandIndex) {

		std::ostringstream val;
		val << airPhaseFunction.getIndex();
		std::string name;
		name = val.str() + "." + airPhaseFunction.getParametersFilePath();
		std::string pathRayleighFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
				+ "_ScatteringFunctionMatrices";
		std::fstream ofstreamRayleighFunctionMatrices;
		ofstreamRayleighFunctionMatrices.open(pathRayleighFunctionMatrices.c_str(), std::fstream::out | std::fstream::binary);

		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrix;
		transferRayleighPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrixPerAngularSector;
		transferRayleighPolarizationMatrixPerAngularSector.resize(PhaseProperty::getNumberOfAngularSectors() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferRayleighPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrix[directionIn][directionOut].resize(4, 4);
			}
		}
		for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			transferRayleighPolarizationMatrixPerAngularSector[angularSector].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrixPerAngularSector[angularSector][directionOut].resize(4, 4);
			}
		}
		computePolarizationMatrixFluid(airPhaseFunction, listDirections, spectralBandIndex, transferRayleighPolarizationMatrix,
			transferRayleighPolarizationMatrixPerAngularSector);
		writeMatrix(ofstreamRayleighFunctionMatrices, transferRayleighPolarizationMatrixPerAngularSector);
		ofstreamRayleighFunctionMatrices.close();
	}

	static void computeAndWritePolarizationMatrixFluidNotBinary(AirPhaseFunction & airPhaseFunction,
																const std::vector<DirectionCartesienne> & listDirections,
																const int & spectralBandIndex) {

		std::ostringstream val;
		val << airPhaseFunction.getIndex();
		std::string name;
		name = val.str() + "." + airPhaseFunction.getParametersFilePath();
		std::string pathRayleighFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator + name
				+ "_ScatteringFunctionMatrices.txt";
		std::fstream ofstreamRayleighFunctionMatrices;
		ofstreamRayleighFunctionMatrices.open(pathRayleighFunctionMatrices.c_str(), std::fstream::out | std::fstream::binary);

		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrix;
		transferRayleighPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrixPerAngularSector;
		transferRayleighPolarizationMatrixPerAngularSector.resize(PhaseProperty::getNumberOfAngularSectors() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferRayleighPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrix[directionIn][directionOut].resize(4, 4);
			}
		}
		for (int angularSector = 0; angularSector <= PhaseProperty::getNumberOfAngularSectors(); angularSector++) {
			transferRayleighPolarizationMatrixPerAngularSector[angularSector].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrixPerAngularSector[angularSector][directionOut].resize(4, 4);
			}
		}
		computePolarizationMatrixFluid(airPhaseFunction, listDirections, spectralBandIndex, transferRayleighPolarizationMatrix,
			transferRayleighPolarizationMatrixPerAngularSector);
		writeMatrixNotBinary(ofstreamRayleighFunctionMatrices, transferRayleighPolarizationMatrixPerAngularSector);
		ofstreamRayleighFunctionMatrices.close();
	}

	static void computeAndWritePolarizationMatrixAtmosphere(const std::vector<DirectionCartesienne> & listDirections,
															const int & spectralBandIndex) {
		std::string pathAtmosphereFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator
				+ "AtmospherePolarizedPhaseFunction";
		std::fstream ofstreamAtmosphereFunctionMatrices;
		ofstreamAtmosphereFunctionMatrices.open(pathAtmosphereFunctionMatrices.c_str(), std::fstream::out | std::fstream::binary);

		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrix;
		transferRayleighPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferAerosolPolarizationMatrix;
		transferAerosolPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferRayleighPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			transferAerosolPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrix[directionIn][directionOut].resize(4, 4);
				transferAerosolPolarizationMatrix[directionIn][directionOut].resize(4, 4);
			}
		}
		computeRayleighPolarizationMatrixAtmosphere(listDirections, spectralBandIndex, transferRayleighPolarizationMatrix);
		computeAerosolPolarizationMatrixAtmosphere(listDirections, spectralBandIndex, transferAerosolPolarizationMatrix);
		writeMatrix(ofstreamAtmosphereFunctionMatrices, transferRayleighPolarizationMatrix);
		writeMatrix(ofstreamAtmosphereFunctionMatrices, transferAerosolPolarizationMatrix);
		ofstreamAtmosphereFunctionMatrices.close();
	}

	static void computeAndWritePolarizationMatrixAtmosphereNotBinary(	const std::vector<DirectionCartesienne> & listDirections,
																		const int & spectralBandIndex) {
		std::string pathAtmosphereFunctionMatrices = DartEnv::getCurrentSimulationOutputPath() + PhaseUtils::getOutputFolder() + pathSeparator
				+ "AtmospherePolarizedPhaseFunction.txt";
		std::fstream ofstreamAtmosphereFunctionMatrices;
		ofstreamAtmosphereFunctionMatrices.open(pathAtmosphereFunctionMatrices.c_str(), std::fstream::out | std::fstream::binary);

		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferRayleighPolarizationMatrix;
		transferRayleighPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);
		std::vector<std::vector<boost::numeric::ublas::matrix<double> > > transferAerosolPolarizationMatrix;
		transferAerosolPolarizationMatrix.resize(PhaseProperty::getNumberOfDirection() + 1);

		for (int directionIn = 0; directionIn <= PhaseProperty::getNumberOfDirection(); directionIn++) {
			transferRayleighPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			transferAerosolPolarizationMatrix[directionIn].resize(PhaseProperty::getNumberOfDirection());
			for (int directionOut = 0; directionOut < PhaseProperty::getNumberOfDirection(); directionOut++) {
				transferRayleighPolarizationMatrix[directionIn][directionOut].resize(4, 4);
				transferAerosolPolarizationMatrix[directionIn][directionOut].resize(4, 4);
			}
		}
		computeRayleighPolarizationMatrixAtmosphere(listDirections, spectralBandIndex, transferRayleighPolarizationMatrix);
		computeAerosolPolarizationMatrixAtmosphere(listDirections, spectralBandIndex, transferAerosolPolarizationMatrix);
		writeMatrixNotBinary(ofstreamAtmosphereFunctionMatrices, transferRayleighPolarizationMatrix);
		writeMatrixNotBinary(ofstreamAtmosphereFunctionMatrices, transferAerosolPolarizationMatrix);
		ofstreamAtmosphereFunctionMatrices.close();
	}

	/* End of the functions to take into account the polarization */

};

#endif /* PHASESPECULARCOMPUTATION_HPP_ */
