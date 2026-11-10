/*
 * ObjectDatabase.hpp
 *
 *  Created on: 01 avril 2014
 *      Author: guilleuxj
 *
 *  Cette classe contient le mapping object-database (ORM)
 *  L'objectif est de diminuer le nombre d'accès à la base de données (couteux car sur disque)
 *  en conservant les données 'utiles' en mémoire
 *  En gros, à chaque requêtes en DB, une copie est enregistrée en tant qu'objet ici
 *  Lors de requêtes futures, on check d'abord ici, puis on fait la requête si on trouve pas
 *  (En gros, on implémente ce que java 'optimise' avec JDBC ...)
 */

#ifndef OBJECTDATABASE_HPP_
#define OBJECTDATABASE_HPP_

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "FichierProperties.hpp"
#include "../../libSOCI/src/core/soci.h"
#include "../../libSOCI/src/backends/sqlite3/soci-sqlite3.h"

class ObjectDatabase: soci::values {
public:
	struct Parameter {
		int idParameter;
		std::string nameShortParameter;
		std::string nameCompleteParameter;
		std::string nameTable;
		std::string nameColumnCombination;
	};

	/**
	 * Del ???
	 */
	struct ValueParameter {
		std::vector<int> idValueParameter;
		std::vector<std::string> valueParameter;
	};

	/**
	 * Del ???
	 */
	struct Combination {
		int idCombination;
	};

	struct ScalarResult {
		int idScalarResult;
		int idCombination;
		int idSolarAngle;
		int idSpectralBand;
		int idSpectralMode;
		int idSimulation;
		int idProduct;
	};

	struct DirectionalResult {
		int idDirectionalResult;
		int idScalarResult;
		int idViewAngle;
	};

	struct Simulation {
		int idSimulation;
		std::string nameSimulation;
//		double tauxRecouvrement; ??? ?? !!
	};

	struct SpectralBand {
		int idSpectralBand;
		double valueCentralWavelength;
		double valueSpectralBandwidth;
	};

	struct SpectralMode {
		int idSpectralMode;
		std::string nameSpectralMode;
	};

	struct SolarAngle {
		int idSolarAngle;
		double valueZenithAngle;
		double valueAzimuthAngle;
	};

	struct ViewAngle {
		int idViewAngle;
		double valueZenithAngle;
		double valueAzimuthAngle;
	};

	struct Product {
		int idProduct;
		std::string nameProduct;
		int numberIteration;
	};

	struct TreeSpecies {
		int idCombinaisonTreeSpecies;
		int idValeurParametreTreeSpecies;
	};

	struct get_BandesSpectrales {
		std::vector<int> idBandeSpectrale;
		std::vector<double> centralWavelength;
		std::vector<double> spectralBandwidth;
		std::vector<int> typeBande;
	};

	// Les structures suivantes ne sont pas dans la base de données.
	// Elles servent à construire "l'objet" pour enregistrer les données

	struct ParametreCombinaison {
		bool sauvegarderParametresIndirects;
		FichierProperties maketProperties;
		FichierProperties sequenceProperties;
	};

	struct ParametresFixes {
		Simulation simulation;
		ParametreCombinaison parametreCombinaison;
		SolarAngle angleSoleil;
	};

	// Reflectance | Luminance
	struct RecordDirectionalResult {
		int idDirectionalResult; // PK & FK
		int idScalarResult;
		ViewAngle angleVisee;
		std::string tableName;
		double valueResult;
	};

	// Albedo | Irradiance | Exitance | DirecteIrradiance
	struct RecordScalarResult {
		int idScalarResult; // PK & FK
		std::string tableName;
		double valueResult;
	};

	struct Record {
		ScalarResult resultatScalaire;
		SpectralBand bandeSpectrale;
		SpectralMode modeSpectral;
		Product produit;
		int spectralBandNumber; //Numéro de la bande spectrale dans les fichier proprietes (dans DART)
		std::vector<RecordDirectionalResult> directionalResults;
		std::vector<RecordScalarResult> scalarResults;
	};

	// Structures pour la database inversion
	// La BD inversion étant une copie légèrement modifiée du LUT,
	// On évite de duppliquer les 3/4 du code de cette classe.
	// Note, la BD inversion ne peut pas utiliser les structures résultat, résultatScalaire,
	// produit, Enregistrement, ParametresFixes, EnregistrementScalaire.
	// Et la BD Lut ne peut pas utiliser les structures suivante.
	struct ParametreInversion {
		int indexLut;
		std::string nomParametre;
		int dartSpectralBandKey;
	};

	struct ModeleCoefficient {
		int idModeleCoefficient;
		std::string nomModele;
		int nombreDeCoefficients;
	};

	struct Coefficient {
		int idCoefficient;
		int idModeleCoefficient;
		double rmse;
		int estLuminance;
		int idCombinaison;
		int idAngleSolaire;
		int idBandeSpectrale;
		int idSequence;
		int positionDuCapteur;
	};

	struct ValeurCoefficient {
		int idCoefficient;
		int numCoefficient;
		double valeurCoefficient;
	};

private:
	static std::vector<ViewAngle> _anglesVisee;
	static std::vector<SolarAngle> _anglesSolaire;
	static std::vector<SpectralBand> _bandesSpectrales;
	static std::vector<SpectralMode> _modesSpectraux;
	static std::vector<Product> _produits;
	static std::vector<Simulation> _simulations;
	static std::vector<ParametreInversion> _parametresInversion;
	static std::vector<Parameter> _parametres;
	static double DB_EPSILON;

public:
	ObjectDatabase();
	~ObjectDatabase();

	const static double getEpsilon();

	void addBandeSpectrale(SpectralBand & bandeSpectrale);
	void addSpectralMode(SpectralMode & spectralMode);
	void addAngleVisee(ViewAngle & angleVisee);
	void addAngleSolaire(SolarAngle & angleSolaire);
	void addProduit(Product & produit);
	void addSimulation(Simulation & simulation);
	void addParametreInversion(ParametreInversion & parametreInversion);
	void addParametre(Parameter & parametre);

	int getIdBandeSpectrale(SpectralBand & bandeSpectrale);
	int getIdSpectralMode(SpectralMode & spectralMode);
	int getIdAngleVisee(ViewAngle & angleVisee);
	int getIdAngleSolaire(SolarAngle & angleSolaire);
	int getIdProduit(Product & produit);
	int getIdSimulation(Simulation & simulation);
	int getIdParametreInversion(ParametreInversion & parametreInversion);
	int getIdParametre(const std::string &nomParametre);

	bool getAngleSolaireFromId(	const int &idAngleSolaire,
								SolarAngle &angleSolaire);
	bool getBandeSpectraleFromId(	const int &idBandeSpectrale,
									SpectralBand &bandeSpectrale);

	std::vector<SpectralBand> getBandesSpectrales();
	std::vector<ViewAngle> getAnglesVisee();
	std::vector<SolarAngle> getAnglesSolaire();
	std::vector<Product> getProduits();
	std::vector<Simulation> getSimulations();
	std::vector<ParametreInversion> getParametresInversion();
	std::vector<Parameter> getParametre();

	static void resizeVectorBandesSpectrales(int newLength);
	static void resizeVectorSpectralMode(int newLength);
	static void resizeVectorAnglesVisee(int newLength);
	static void resizeVectorAnglesSolaire(int newLength);
	static void resizeVectorProduits(int newLength);
	static void resizeVectorSimulations(int newLength);

	inline bool isDoubleEqual(	double x,
								double y);

	static double getLambdaMinRounded(const SpectralBand & bandeSpectrale);
	static double getLambdaMaxRounded(const SpectralBand & bandeSpectrale);
	static double rounding2FloatingPoint(double A_num);
};

#endif /* OBJECTDATABASE_HPP_ */
