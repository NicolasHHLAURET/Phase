/*
 * BaseDeDonneesInversion.hpp
 *
 *  Created on: 5 mai 2014
 *      Author: guilleuxj
 */

#ifndef BASEDEDONNEESINVERSION_HPP_
#define BASEDEDONNEESINVERSION_HPP_

#include <map>
#include <math.h>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
//#include <boost/regex.hpp> // Pas incluse dans cette version :(
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include "FichierProperties.hpp"
#include "Conversions.hpp"
#include "../lut/object/ObjectDatabase.hpp"
#include "../../libDartEnv/include/DartEnv.h"

#include "../../DartInversion/include/LutCoefficients/object/Lut.hpp"

#include "../../DartInversion/include/InversionParameters/InversionParameters.hpp"
#include "../../DartInversion/include/InversionParameters/ParametersType/SunPhiAngle.hpp"
#include "../../DartInversion/include/InversionParameters/ParametersType/SunTethaAngle.hpp"
#include "../../DartInversion/include/InversionParameters/ParametersType/SpectralBand.hpp"

#include "cache/CacheInterpolationParameters.hpp"
#include "cache/CacheInterpolationCoefficients.hpp"
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #include <memory>
    using namespace std;
#else
    #include <boost/shared_ptr.hpp>
    using namespace boost;
#endif

using namespace soci;

class BaseDeDonneesInversion: values {
private:
	std::string _cheminVersFichierBDD;
	backend_factory const &_sqlBackend;
	session _session;
	shared_ptr<InversionParameters> _inversionParameters;
	int _idSequence;
	int _idModele;
	int _nombreCoefficients;
	Debug _debug;
	CacheInterpolationParameters _cacheParameters;
	CacheInterpolationCoefficients _cacheCoefficients;

	/**
	 * Objets statiques
	 */
	static ObjectDatabase _objectDatabase;
	static std::string _reflectanceDatabase;

	int getIdParametre(const std::string &nomParametre);
	void makeIntersectionInversionCombinaison(	std::vector<int> &baseVector,
												const std::vector<int> &intersectVector) const;

public:

	void init(const std::string &modelName);

	BaseDeDonneesInversion() :
			_sqlBackend(*soci::factory_sqlite3()) {
		_idSequence = 0;
		_idModele = 0;
		_nombreCoefficients = 0;
	}

	BaseDeDonneesInversion(	const std::string & cheminVersFichierBDD,
							shared_ptr<InversionParameters> &inversionParameters) :
			_sqlBackend(*soci::factory_sqlite3()) {
		_cheminVersFichierBDD = std::string("dbname=\"") + cheminVersFichierBDD + std::string("\" timeout=600");
		_inversionParameters = inversionParameters;
		_idSequence = 0;
		_idModele = 0;
		_nombreCoefficients = 0;
	}

	BaseDeDonneesInversion(const std::string & cheminVersFichierBDD) :
			_sqlBackend(*soci::factory_sqlite3()) {
		_cheminVersFichierBDD = std::string("dbname=\"") + cheminVersFichierBDD + std::string("\" timeout=600");
	}

	~BaseDeDonneesInversion() {
		this->closeConnexion();
	}

	void setPathToDbInversion(const std::string & cheminVersFichierBDD) {
		_cheminVersFichierBDD = std::string("dbname=\"") + cheminVersFichierBDD + std::string("\" timeout=600");
	}

	/**
	 * Retourne le path vers la base de donnees reflectance
	 */
	static const std::string& getReflectanceDatabase();

	/**
	 * Ouvre une connexion vers la base de données.
	 */
	void openConnexion();

	/**
	 * Ferme la connexion vers la base de données.
	 */
	void closeConnexion();

	/**
	 * Récupère l'ID de la séquence dont le nom est passe en paramètre
	 * @param sequenceName: nom de la séquence dont on récupère l'ID.
	 */
	int getIdSequence();

	int getIdModele(std::string modelName);

	/**
	 * Récupère le nombre de bandes spectrales d'une séquence dont le nom est passe en paramètre.
	 */
	int getNbBandesSpectrales();

	/**
	 * Check si la base de données est une base inversion
	 * @return true si c'est le cas, false sinon
	 */
	bool isAnInversionDatabase();

	/**
	 * Récupère les bandes spectrales d'une séquence dont le nom est passe en paramètre.
	 * @return les bandes spectrales sous la forme d'un vecteur de bandes spectrales (ObjectDatabase).
	 */
	std::vector<ObjectDatabase::SpectralBand> getBandesSpectrales();

	/**
	 * Récupère le nombre de paramètres d'inversion d'une séquence dont le nom est passe en paramètre.
	 * @param sequenceName: nom de la séquence dont on récupère le nombre de paramètres d'inversion.
	 */
	int getNbParametreInversion(std::string sequenceName);

	/**
	 * Récupère les paramètres d'inversion d'une séquence dont le nom est passe en paramètre.
	 * @param sequenceName: nom de la séquence dont on récupère les paramètres d'inversion.
	 * @return les paramètres d'inversion sous la forme d'un vecteur de paramètres d'inversion (ObjectDatabase).
	 */
	std::vector<ObjectDatabase::ParametreInversion> getParametresInversion(std::string sequenceName);

	void getSunAngleFromId(	const int &idSunAngle,
							ObjectDatabase::SolarAngle &angleSolaire);
	void getBandeSpectraleFromId(	const int idBandeSpectrale,
									ObjectDatabase::SpectralBand &bandeSpectrale);

	/**
	 * Récupère un paramètre d'inversion identifié par son indexLUT
	 * @param: indexLut : l'index du paramètre d'inversion
	 * @return le paramètre d'inversion sous la forme d'une structure paramètre d'inversion (ObjectDatabase).
	 */
	ObjectDatabase::ParametreInversion getParametreInversion(int indexLut);

	/**
	 * Retourne le nombre de coefficients du modèle passé en paramètre.
	 * @param modelName: nom du modèle dont on récupère le nombre de coefficients
	 * @return un entier contenant le nombre de coefficients.
	 */
	int getNbCoefficients(std::string modelName);

	/**
	 *	Lit une ligne de coefficients
	 *	@author pasmoi :3
	 */
	int lireCoefficients(	std::vector<double> &vParameters,
							const int & indiceDeLecture,
							const std::string & modeleCoefficient,
							const std::string & nomSequence);

	/**
	 * Retourne le nombre de lignes de coefficients (càd le nombre de lignes de coefficients) pour un modèle d'une séquence
	 * donnée.
	 * Une ligne de coefficients contient X coeficients, X = nombre de coefficients du modèle (retourné par getNbCoefficients).
	 * @param modelName
	 * @param sequenceName
	 * @return nombre de lignes de coefficients.
	 */
	int compterCoefficients(const std::string &modelName);

	int getIdParametreInversion(std::string parametreName);

	void getBandeSpectrale(	const int idBandeSpectrale,
							double &centralWavelength,
							double &spectralBandwidth);

	void loadLutParameters(	std::string sequenceName,
							InversionParameters &inversionParameters);

	void lireCoefficients(	std::string modelName,
							std::string sequenceName,
							std::vector<shared_ptr<std::pair<Combinaison, Coefficients> > > &lutCoefficients);

	void getParametresCombinaisonInterpolation(const Combinaison::Parametre &parametreAInterpoler,
												std::vector<int> &vectorCombinaisonExtraites,
												const int &spectralBandKey,
												const int &nbPointsInterpolation);

	//Màj de la base
	int getIdSunAngle(	const double &valeurZenithal,
						const double &valeurAzimuthal);
	void majLambertianReflectanceBySpectralBands();
	void getAllValeursParametresInterpolable(ObjectDatabase::ValueParameter &parametres);
	bool isNotDoubleValue(const std::string &value);
	void getCombinaisonsForValeurParametre(	std::vector<int> &combinaisons,
											const int &idValeurParametre);
	void getSpectralBandsForCombinaison(ObjectDatabase::get_BandesSpectrales& spectralBands,
										const int &idCombinaison);
	int createOrGetCombinaison(	const int &idCombinaison,
								const int &idParametre,
								const int idValeurParametre,
								const double &lambertianValueForBand);

	void majCombinaisonForCoefficients(	const int &idOldCombinaison,
										const int &idBandeSpectrale,
										const int &idNewCombinaison);

	void getLutPourInterpolation(	Lut &lut,
									const int &idBandeSpectrale,
									const int &idSunAngle,
									const std::vector<int> &vectorCombinaisons);

	void getParametersValgrind(	const int &idCombinaison,
								std::vector<std::string> &valValeurParametres,
								std::vector<std::string> &valNomParametres,
								std::vector<int> &idIndexLuts);



	void getMinMaxParameter(const std::string &nomParametre,
							double &min,
							double &max);

	static void setReflectanceDatabase(const std::string &nomDatabase);
}
;

#endif /* BASEDEDONNEESINVERSION_HPP_ */
