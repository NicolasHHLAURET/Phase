/*
 * ObjectDatabase.cpp
 *
 *  Created on: 01 avril 2014  >))))°>
 *      Author: guilleuxj
 *
 * Implémentation de l'ORM
 */

#include "../../../../include/databases/lut/object/ObjectDatabase.hpp"

#include <cmath>
#include <iterator>

using namespace std;

vector<ObjectDatabase::SpectralBand> ObjectDatabase::_bandesSpectrales = vector<ObjectDatabase::SpectralBand>();
vector<ObjectDatabase::SpectralMode> ObjectDatabase::_modesSpectraux = vector<ObjectDatabase::SpectralMode>();

vector<ObjectDatabase::ViewAngle> ObjectDatabase::_anglesVisee = vector<ObjectDatabase::ViewAngle>();
vector<ObjectDatabase::SolarAngle> ObjectDatabase::_anglesSolaire = vector<ObjectDatabase::SolarAngle>();
vector<ObjectDatabase::Product> ObjectDatabase::_produits = vector<ObjectDatabase::Product>();
vector<ObjectDatabase::ParametreInversion> ObjectDatabase::_parametresInversion = vector<ObjectDatabase::ParametreInversion>();
vector<ObjectDatabase::Parameter> ObjectDatabase::_parametres = vector<ObjectDatabase::Parameter>();

vector<ObjectDatabase::Simulation> ObjectDatabase::_simulations = vector<ObjectDatabase::Simulation>();
double ObjectDatabase::DB_EPSILON = 1E-12;

/*vector<ObjectDatabase::EnregistrementCoefficientMultiplicatif> ObjectDatabase::_enregistrementsCoefficientMultiplicatif =
 vector<ObjectDatabase::EnregistrementCoefficientMultiplicatif>();*/

const double ObjectDatabase::getEpsilon() {
	return DB_EPSILON;
}

ObjectDatabase::ObjectDatabase() {
	// #Yolo
}

ObjectDatabase::~ObjectDatabase() {
	// #Yolo
}

void ObjectDatabase::addBandeSpectrale(ObjectDatabase::SpectralBand & bandeSpectrale) {
	vector<SpectralBand>::iterator itr;
	for (itr = ObjectDatabase::_bandesSpectrales.begin(); itr != ObjectDatabase::_bandesSpectrales.end(); ++itr) {
		if (itr->idSpectralBand == bandeSpectrale.idSpectralBand) {
			return;
		}
	}
	ObjectDatabase::_bandesSpectrales.push_back(bandeSpectrale);
}

void ObjectDatabase::addSpectralMode(ObjectDatabase::SpectralMode & spectralMode) {
	vector<SpectralMode>::iterator itr;
	for (itr = ObjectDatabase::_modesSpectraux.begin(); itr != ObjectDatabase::_modesSpectraux.end(); ++itr) {
		if (itr->idSpectralMode == spectralMode.idSpectralMode) {
			return;
		}
	}
	ObjectDatabase::_modesSpectraux.push_back(spectralMode);
}

void ObjectDatabase::addAngleVisee(ObjectDatabase::ViewAngle & angleVisee) {
	vector<ViewAngle>::iterator itr;
	for (itr = ObjectDatabase::_anglesVisee.begin(); itr != ObjectDatabase::_anglesVisee.end(); ++itr) {
		if (itr->idViewAngle == angleVisee.idViewAngle) {
			return;
		}
	}
	ObjectDatabase::_anglesVisee.push_back(angleVisee);
}

void ObjectDatabase::addAngleSolaire(ObjectDatabase::SolarAngle & angleSolaire) {
	vector<SolarAngle>::iterator itr;
	for (itr = ObjectDatabase::_anglesSolaire.begin(); itr != ObjectDatabase::_anglesSolaire.end(); ++itr) {
		if (itr->idSolarAngle == angleSolaire.idSolarAngle) {
			return;
		}
	}
	ObjectDatabase::_anglesSolaire.push_back(angleSolaire);
}

void ObjectDatabase::addProduit(ObjectDatabase::Product & produit) {
	vector<Product>::iterator itr;
	for (itr = ObjectDatabase::_produits.begin(); itr != ObjectDatabase::_produits.end(); ++itr) {
		if (itr->idProduct == produit.idProduct) {
			return;
		}
	}
	ObjectDatabase::_produits.push_back(produit);
}

void ObjectDatabase::addSimulation(ObjectDatabase::Simulation & simulation) {
	vector<Simulation>::iterator itr;
	for (itr = ObjectDatabase::_simulations.begin(); itr != ObjectDatabase::_simulations.end(); ++itr) {
		if (itr->idSimulation == simulation.idSimulation) {
			return;
		}
	}
	ObjectDatabase::_simulations.push_back(simulation);
}

void ObjectDatabase::addParametreInversion(ObjectDatabase::ParametreInversion & parametreInversion) {
	vector<ParametreInversion>::iterator itr;
	for (itr = ObjectDatabase::_parametresInversion.begin(); itr != ObjectDatabase::_parametresInversion.end(); ++itr) {
		if (itr->indexLut == parametreInversion.indexLut) {
			return;
		}
	}
	ObjectDatabase::_parametresInversion.push_back(parametreInversion);
}

void ObjectDatabase::addParametre(ObjectDatabase::Parameter & parametre) {
	vector<Parameter>::iterator itr;
	for (itr = ObjectDatabase::_parametres.begin(); itr != ObjectDatabase::_parametres.end(); ++itr) {
		if (itr->idParameter == parametre.idParameter) {
			return;
		}
	}
	ObjectDatabase::_parametres.push_back(parametre);
}

int ObjectDatabase::getIdBandeSpectrale(SpectralBand & bandeSpectrale) {
	vector<SpectralBand>::iterator itr;
	for (itr = ObjectDatabase::_bandesSpectrales.begin(); itr != ObjectDatabase::_bandesSpectrales.end(); ++itr) {
		if (isDoubleEqual(itr->valueCentralWavelength, bandeSpectrale.valueCentralWavelength)
				&& isDoubleEqual(itr->valueSpectralBandwidth, bandeSpectrale.valueSpectralBandwidth)) {

			return itr->idSpectralBand;
		}
	}
	return -1;
}

int ObjectDatabase::getIdSpectralMode(SpectralMode & spectralMode) {
	vector<SpectralMode>::iterator itr;
	for (itr = ObjectDatabase::_modesSpectraux.begin(); itr != ObjectDatabase::_modesSpectraux.end(); ++itr) {
		if (itr->idSpectralMode == spectralMode.idSpectralMode) {

			return itr->idSpectralMode;
		}
	}
	return -1;
}

int ObjectDatabase::getIdAngleVisee(ViewAngle & angleVisee) {
	vector<ViewAngle>::iterator itr;
	for (itr = ObjectDatabase::_anglesVisee.begin(); itr != ObjectDatabase::_anglesVisee.end(); ++itr) {
		if (isDoubleEqual(itr->valueZenithAngle, angleVisee.valueZenithAngle)
				&& isDoubleEqual(itr->valueAzimuthAngle, angleVisee.valueAzimuthAngle)) {
			return itr->idViewAngle;
		}
	}
	return -1;
}

int ObjectDatabase::getIdAngleSolaire(SolarAngle & angleSolaire) {
	vector<SolarAngle>::iterator itr;
	for (itr = ObjectDatabase::_anglesSolaire.begin(); itr != ObjectDatabase::_anglesSolaire.end(); ++itr) {
		if (isDoubleEqual(itr->valueZenithAngle, angleSolaire.valueZenithAngle)
				&& isDoubleEqual(itr->valueAzimuthAngle, angleSolaire.valueAzimuthAngle)) {
			return itr->idSolarAngle;
		}
	}
	return -1;
}

int ObjectDatabase::getIdProduit(ObjectDatabase::Product & produit) {
	vector<Product>::iterator itr;
	for (itr = ObjectDatabase::_produits.begin(); itr != ObjectDatabase::_produits.end(); ++itr) {
		if (itr->nameProduct == produit.nameProduct && itr->numberIteration == produit.numberIteration) {
			return itr->idProduct;
		}
	}
	return -1;
}

int ObjectDatabase::getIdSimulation(ObjectDatabase::Simulation & simulation) {
	vector<Simulation>::iterator itr;
	for (itr = ObjectDatabase::_simulations.begin(); itr != ObjectDatabase::_simulations.end(); ++itr) {
		if (itr->nameSimulation == simulation.nameSimulation) {
			return itr->idSimulation;
		}
	}
	return -1;
}

int ObjectDatabase::getIdParametreInversion(ObjectDatabase::ParametreInversion & parametreInversion) {
	vector<ParametreInversion>::iterator itr;
	for (itr = ObjectDatabase::_parametresInversion.begin(); itr != ObjectDatabase::_parametresInversion.end(); ++itr) {
		if (itr->nomParametre == parametreInversion.nomParametre) {
			return itr->indexLut;
		}
	}
	return -1;
}

int ObjectDatabase::getIdParametre(const string &nomParametre) {
	vector<Parameter>::iterator itr;
	for (itr = ObjectDatabase::_parametres.begin(); itr != ObjectDatabase::_parametres.end(); ++itr) {
		if (itr->nameCompleteParameter == nomParametre) {
			return itr->idParameter;
		}
	}
	return -1;
}

bool ObjectDatabase::getAngleSolaireFromId(	const int &idAngleSolaire,
											ObjectDatabase::SolarAngle &angleSolaire) {
	vector<SolarAngle>::iterator itr, itrEnd = _anglesSolaire.end();
	for (itr = ObjectDatabase::_anglesSolaire.begin(); itr != itrEnd; ++itr) {
		if (idAngleSolaire == (*itr).idSolarAngle) {
			angleSolaire = (*itr);
			return true;
		}
	}
	return false;
}

bool ObjectDatabase::getBandeSpectraleFromId(	const int &idBandeSpectrale,
												ObjectDatabase::SpectralBand &bandeSpectrale) {
	vector<SpectralBand>::iterator itr, itrEnd = _bandesSpectrales.end();
	for (itr = ObjectDatabase::_bandesSpectrales.begin(); itr != itrEnd; ++itr) {
		if (idBandeSpectrale == (*itr).idSpectralBand) {
			bandeSpectrale = (*itr);
			return true;
		}
	}
	return false;
}

vector<ObjectDatabase::SpectralBand> ObjectDatabase::getBandesSpectrales() {
	return ObjectDatabase::_bandesSpectrales;
}

vector<ObjectDatabase::ViewAngle> ObjectDatabase::getAnglesVisee() {
	return ObjectDatabase::_anglesVisee;
}

vector<ObjectDatabase::SolarAngle> ObjectDatabase::getAnglesSolaire() {
	return ObjectDatabase::_anglesSolaire;
}

vector<ObjectDatabase::Product> ObjectDatabase::getProduits() {
	return ObjectDatabase::_produits;
}

vector<ObjectDatabase::Simulation> ObjectDatabase::getSimulations() {
	return ObjectDatabase::_simulations;
}

vector<ObjectDatabase::ParametreInversion> ObjectDatabase::getParametresInversion() {
	return ObjectDatabase::_parametresInversion;
}

vector<ObjectDatabase::Parameter> ObjectDatabase::getParametre() {
	return ObjectDatabase::_parametres;
}

void ObjectDatabase::resizeVectorBandesSpectrales(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_bandesSpectrales.reserve(newLength);
	}
}

void ObjectDatabase::resizeVectorSpectralMode(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_modesSpectraux.reserve(newLength);
	}
}

void ObjectDatabase::resizeVectorAnglesVisee(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_anglesVisee.reserve(newLength);
	}
}

void ObjectDatabase::resizeVectorAnglesSolaire(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_anglesSolaire.reserve(newLength);
	}
}

void ObjectDatabase::resizeVectorProduits(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_produits.reserve(newLength);
	}
}

void ObjectDatabase::resizeVectorSimulations(int newLength) {
	if (newLength > 0) {
		ObjectDatabase::_simulations.reserve(newLength);
	}
}

/* . . . */
inline bool ObjectDatabase::isDoubleEqual(	double x,
											double y) {
	return std::abs(x - y) <= getEpsilon() * std::abs(x);
}

double ObjectDatabase::getLambdaMinRounded(const SpectralBand & bs) {
	return rounding2FloatingPoint((bs.valueCentralWavelength - (bs.valueSpectralBandwidth / 2)) * pow(10.0, 6.0));
}

double ObjectDatabase::getLambdaMaxRounded(const SpectralBand & bs) {
	return rounding2FloatingPoint((bs.valueCentralWavelength + (bs.valueSpectralBandwidth / 2)) * pow(10.0, 6.0));
}

// Arrondi un double à 2 chiffres après la virgule.
double ObjectDatabase::rounding2FloatingPoint(double num) {
	num *= 100;
	num = round(num);
	num /= 100;
	return num;
}
