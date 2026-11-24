/*
 * SphereDiscretizationMap.hpp
 *
 *  Created on: Feb 16, 2012
 *      Author: yint
 */

#ifndef SPHEREDISCRETIZATIONMAP_HPP_
#define SPHEREDISCRETIZATIONMAP_HPP_

#define PHI_MAX 360
using namespace std;

class SphereDiscretizationMap {
private:
	vector<Doublet<double> > mapSphere;       //Spherical Coordinate Map
	vector<Vecteur<double> > mapCartesian;    //Cartesian Coordinate Map

	double deltaTheta;
	double deltaPhi;

	unsigned int sizeTheta;
	unsigned int sizePhi;

public:
	SphereDiscretizationMap(const double & deltaT,
							const double & deltaP) :
			deltaTheta(deltaT), deltaPhi(deltaP) {
		generateMaps();
	}

    void generateMaps();
    double getDeltaPhi() const;
    double getDeltaTheta() const;
//    vector<vector<Vecteur<double> > > getMapCartesian() const;
//    vector<vector<Doublet<double> > > getMapSphere() const;
    void setDeltaPhi(double deltaPhi);
    void setDeltaTheta(double deltaTheta);
    unsigned int getSizePhi() const;
    unsigned int getSizeTheta() const;
    void setSizePhi(unsigned int sizePhi);

//    vector<vector<double> > generateScalerProductMap(Vecteur<double> const& a) const;
    void generateScalerProductMap(vector<double> & res, Vecteur<double> const& a) const;
};

inline double SphereDiscretizationMap::getDeltaPhi() const
{
    return deltaPhi;
}

inline double SphereDiscretizationMap::getDeltaTheta() const
{
    return deltaTheta;
}

//inline vector<vector<Vecteur<double> > > SphereDiscretizationMap::getMapCartesian() const
//{
//    return mapCartesian;
//}
//
//inline vector<vector<Doublet<double> > > SphereDiscretizationMap::getMapSphere() const
//{
//    return mapSphere;
//}

inline void SphereDiscretizationMap::setDeltaPhi(double deltaPhi)
{
    this->deltaPhi = deltaPhi;
}

inline unsigned int SphereDiscretizationMap::getSizePhi() const
{
    return sizePhi;
}

inline unsigned int SphereDiscretizationMap::getSizeTheta() const
{
    return sizeTheta;
}

inline void SphereDiscretizationMap::setDeltaTheta(double deltaTheta)
{
    this->deltaTheta = deltaTheta;
}

inline void SphereDiscretizationMap::generateMaps() {
	mapSphere.clear();
	mapCartesian.clear();

	if (deltaTheta <= 0 || deltaPhi <= 0) {
		cerr << "Error: deltaTheta or deltaPhi is less than 0." << endl;
		abort();
	}

	sizeTheta = NBMAX_TETAF / deltaTheta;
	sizePhi = PHI_MAX / deltaPhi;

	mapSphere.resize(sizeTheta * sizePhi);
	mapCartesian.resize(sizeTheta * sizePhi);

	unsigned int thetaIndex, phiIndex;

	for (thetaIndex = 0; thetaIndex < sizeTheta; thetaIndex++) {
		for (phiIndex = 0; phiIndex < sizePhi; phiIndex++) {
			mapSphere[phiIndex * sizeTheta + thetaIndex] = Doublet<double>(deg2rad(thetaIndex * deltaTheta + 0.5), deg2rad(phiIndex * deltaPhi));
			mapCartesian[phiIndex * sizeTheta + thetaIndex] = PhaseUtils::conv_direction(thetaIndex * deltaTheta + 0.5, phiIndex * deltaPhi);
		}
	}
}

//inline vector<vector<double> > SphereDiscretizationMap::generateScalerProductMap(Vecteur<double> const& a) const {
//	unsigned int thetaIndex, phiIndex;
//	vector<vector<double> > scalerProduct;
//
//	scalerProduct.resize(sizeTheta, vector<double>(sizePhi));
//
//	for (thetaIndex = 0; thetaIndex < sizeTheta; thetaIndex++) {
//		for (phiIndex = 0; phiIndex < sizePhi; phiIndex++) {
//			scalerProduct[thetaIndex][phiIndex]=a.scalaire(mapCartesian[thetaIndex][phiIndex]);
//		}
//	}
//	return scalerProduct;
//}

inline void SphereDiscretizationMap::generateScalerProductMap(vector<double> & res, Vecteur<double> const& a) const {
	res.resize(sizeTheta * sizePhi);
	for (unsigned int thetaIndex = 0; thetaIndex < sizeTheta; thetaIndex++) {
		for (unsigned int phiIndex = 0; phiIndex < sizePhi; phiIndex++) {
			res[phiIndex * sizeTheta + thetaIndex] = a.scalaire(mapCartesian[phiIndex * sizeTheta + thetaIndex]);
		}
	}
}

//.scaler

#endif /* SPHEREDISCRETIZATIONMAP_HPP_ */
