/*
 * LireDatabase2D.hpp
 *
 *  Created on: 15 sept. 2010
 *      Author: goulierv
 */

#ifndef LIREDATABASE2D_HPP_
#define LIREDATABASE2D_HPP_

#include "libDartEnv.h"
#include "libDartBase.h"
#include <string>
#include "BaseDeDonneesReflectance.hpp"
#include <iostream>

class LireDatabase2D {
private:
	struct Key {
		std::string databaseName;
		double lambdaMin;
		double lambdaMax;
	};

	struct compare {
		bool operator() (const Key& key1, const Key& key2) const
		{
			if (key1.databaseName != key2.databaseName) {
				return key1.databaseName < key2.databaseName;
			}
			if (key1.lambdaMin != key2.lambdaMin) {
				return key1.lambdaMin < key2.lambdaMin;
			}
			return key1.lambdaMax < key2.lambdaMax;
		}
	};

	static std::map<Key,double,compare> _reflectances;

	static bool hasReflectanceInMemory(const Key &key);
	static double getReflectanceInMemory(const Key &key);
	static double setReflectanceInMemory(const Key &key, const double &value);

public:

	static double getReflectanceDansDatabase2DForInterval(const std::string &tableName,
	                                                      	const std::string &databaseName,
															const double &lambdaMin,
															const double &lambdaMax) {

		// Gestion d'un cache (Pour éviter le spam requête en base qui ralenti fortement le processus)
		Key key;
		key.databaseName = tableName;
		key.lambdaMin = lambdaMin;
		key.lambdaMax = lambdaMax;
		if (hasReflectanceInMemory(key)) {
			return getReflectanceInMemory(key);
		}

		std::vector<double> wavelength, reflectances;
		double reflectance = 0;
		std::string pathToDatabaseReflectance = DartEnv::getDatabasePath(databaseName) + databaseName;
		BaseDeDonneesReflectance dbReflectance(pathToDatabaseReflectance);
		dbReflectance.openConnexion();
		dbReflectance.checkIntegrity(tableName); //Histoire de détecter et de gueuler si quelqu'un modifie la base :3



		int nbli_spectral = dbReflectance.getNbLignesTable(tableName);
		double lambda_min_data = dbReflectance.getLambdaMin(tableName);
		double lambda_max_data = dbReflectance.getLambdaMax(tableName);

		if (lambda_min_data > lambdaMin || lambda_max_data < lambdaMax) {
			std::cerr << "Spectral interval out of database range for database !" << std::endl;
			return 0;
		}

		wavelength.resize(nbli_spectral);
		reflectances.resize(nbli_spectral);

		dbReflectance.getWavelengthForTable(tableName,wavelength);
		dbReflectance.getReflectanceForTable(tableName,reflectances);

		double **Tab_spectral_data = dmatrix(0, nbli_spectral + 1, 0, 1);
		double **Tab_spectral = dmatrix(0, nbli_spectral + 1, 0, 1);

		for (int i = 0; i < nbli_spectral; i++) {
			Tab_spectral_data[i][0] = wavelength[i];
			Tab_spectral_data[i][1] = reflectances[i];
		}

		Interpolation_Trapeze(lambdaMin, lambdaMax, nbli_spectral, 2, Tab_spectral_data, Tab_spectral);
		reflectance = Tab_spectral[nbli_spectral + 1][1];
		free_dmatrix(Tab_spectral_data, 0, 0);
		free_dmatrix(Tab_spectral, 0, 0);

		// Enregistrement en cache local
		setReflectanceInMemory(key,reflectance/100);

		std::cout << " \t -> Ground spectra found : " << tableName << "("<<lambdaMin<<";"<<lambdaMax <<") -> reflectance : " << (reflectance / 100) << std::endl;
		return reflectance / 100;
	}

	static double getTransmittanceDirecteDansDatabase2DForInterval(const std::string &nomDatabase,
																	const double &lambdaMin,
																	const double &lambdaMax) {
		double transmittance = 0;
		std::string cheminDatabase = DartEnv::getDatabasePath(nomDatabase) + nomDatabase;
		std::ifstream in_data(cheminDatabase.c_str(), std::ifstream::in);
		if (in_data) {
			double lambda_min_data, lambda_max_data;
			int nbli_spectral;
			echappeLectureCommentaireFichier(in_data, FOUR_CHAR, '*');
			in_data >> nbli_spectral;
			echappeLectureCommentaireFichier(in_data, TWO_CHAR, '*');
			in_data >> lambda_min_data >> lambda_max_data;
			//			cout << lambda_min_data<< " "<<lambdaMin<<" "<<lambda_max_data<<" "<<lambdaMax<<endl;
			if (lambda_min_data > lambdaMin || lambda_max_data < lambdaMax) {
				std::cerr << "Spectral interval out of database range" << std::endl;
				return 0;
			}
			double **Tab_spectral_data = dmatrix(0, nbli_spectral + 1, 0, 1);
			double **Tab_spectral = dmatrix(0, nbli_spectral + 1, 0, 1);
			double B0, h, b1, c1, b2, c2, lambda, Ro, n, A1, Alpha1, w, A2, Alpha2;
			double dTauDirect = 0.0;
			double dTauDiffus = 0.0;

			echappeLectureCommentaireFichier(in_data, TWO_CHAR, '*');
			in_data >> B0 >> h >> b1 >> c1 >> b2 >> c2;
			echappeLectureCommentaireFichier(in_data, TWO_CHAR, '*');
			for (int i = 0; i < nbli_spectral; i++) {
				if (!(in_data >> lambda >> Ro >> n >> A1 >> Alpha1 >> w >> A2 >> Alpha2 >> dTauDirect >> dTauDiffus)) {
					std::cerr << "Bad format of the database : " << nomDatabase << std::endl;
					return 0;
				}
				Tab_spectral_data[i][0] = lambda;
				Tab_spectral_data[i][1] = dTauDirect;
			}
			Interpolation_Trapeze(lambdaMin, lambdaMax, nbli_spectral, 2, Tab_spectral_data, Tab_spectral);
			transmittance = Tab_spectral[nbli_spectral + 1][1];
			free_dmatrix(Tab_spectral_data, 0, 0);
			free_dmatrix(Tab_spectral, 0, 0);
			in_data.close();
		} else {
			std::cerr << "Impossible to open database : " << cheminDatabase << std::endl;
		}
		return transmittance * 0.01;
	}

	// Source : http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
	/**
	 * Découpe un string (&s) selon un caractère (delim)
	 * Retourne un vecteur de string contenant la découpe.
	 */
private:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while (getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}
	static std::vector<std::string> split(const std::string &s, char delim) {
	    std::vector<std::string> elems;
	    split(s, delim, elems);
	    return elems;
	}


};

#endif /* LIREDATABASE2D_HPP_ */
