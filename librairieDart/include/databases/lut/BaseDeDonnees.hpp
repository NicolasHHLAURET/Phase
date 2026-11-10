/*
 * BaseDeDonnees.hpp
 *
 *  Created on: 21 janv. 2011
 *      Author: cortir
 */

//Database.hpp
#ifndef BASEDEDONNEES_HPP_
#define BASEDEDONNEES_HPP_

#include <map>
#include <math.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stack>

#include <boost/algorithm/string/predicate.hpp>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include "FichierProperties.hpp"
#include "Conversions.hpp"
#include "object/ObjectDatabase.hpp"
#include "../../libDartEnv/include/DartEnv.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

using namespace soci;

class BaseDeDonnees: values {
private:
	static int _simulationIdCombinaison;
	std::string _cheminVersFichierBDD;
	backend_factory const &_sqlBackend;
	static std::string _valeurParametreTauxCouverture;
	static ObjectDatabase _objectDatabase;
	static std::map<std::pair<int, std::pair<double, double> >, double> _reflectancesAtmosphereTOA;
	static std::map<std::pair<int, std::pair<double, double> >, double> _luminancesAtmosphereTOA;
	static std::map<std::pair<int, std::pair<double, double> >, double> _reflectancesAtmosphereTOAOrdre1;
	static std::map<std::pair<int, std::pair<double, double> >, double> _luminancesAtmosphereTOAOrdre1;

	int makeIntersectionCombinaison(std::vector<std::vector<int> > vectorsToIntersect) const;

	/* Méthodes relative au temps */
	void traiterTemps(	session & sql,
						const int & idAngleSolaire,
						const FichierProperties & parametresDeLaSequence);

	/* Traitement des paramètres */
	bool estUnParametreRegulier(const std::string &nomParametre,
								const bool &saveMaketCoverage) const;
	bool estUnTreeParametre(const std::string &nomParametre,
							const bool &saveMaketCoverage) const;
	bool estUnParametreCoefficientMultiplicatif(const std::string & nomParametre) const;
	bool estUnParametreCoefficientMultiplicatif(const std::string & nomParametre,
												const int& numBande) const;

	/*Narjess:  Add maket.coverage to database */
	int ajouterTauxRecouvrementDansLutBaseDeDonnees(session &sql) const;

	/* Insère une combinaison de paramètres en base de données */
	int insertionCombinaisonParametres(	session & sql,
										FichierProperties & proprietesDeLaSequence,
										FichierProperties & proprietesDeLaMaket,
										const bool & onVeutSauvegarderParametresIndirects,
										const int & numSpectralBand,
										const bool & saveMaketCoverage) const;

	/* Traitement du produit atmosphérique (TOA) */
	bool hasAtmosphericTOAtoStore(const bool &isOrdre1) const;
	void enregistrerProduitAtmosphereTOA(	session &sql,
											const int &spectralBandNumber,
											const int &idSimulation,
											const int &idSpectralBand,
											const int &idSpectralMode,
											const int &idSunAngle,
											const int &idCombinaison,
											const bool &estOrdre1);

	/* Méthodes génériques d'accès à la base */
	int getIdAngleViseeFromValues(	session & sql,
									const double &angleZenith,
									const double &angleAzimuth);
	int getIdProduitFromValues(	session &sql,
								const std::string &nomProduit,
								const int &numIteration);
	int getIdResultatScalaireFromValues(session &sql,
										const int &idProduit,
										const int &idSimulation,
										const int &idSpectralBand,
										const int &idSpectralMode,
										const int &idSunAngle,
										const int &idCombinaison);
	int getIdDirectionalResultFromValues(	session &sql,
											const int &idAngleVisee,
											const int &idResultatScalaire);

	int storeScalarFromValues(	session & sql,
								ObjectDatabase::Record & record);
	void storeResultatFromValues(	session & sql,
									ObjectDatabase::RecordDirectionalResult record);
	void storeResultatFromValues(	session & sql,
									const int &idAngleVisee,
									const int &idResultatScalaire,
									const bool &estLuminance,
									const double &valeurResultat);
public:
	static const std::string NOM_SHORT_PARAMETRE_TAUX_COUVERTURE;
	static const std::string NOM_PARAMETRE_TAUX_COUVERTURE;
	static const std::string NOM_TABLE_TAUX_COUVERTURE;
	static const std::string NOM_COLUMN_TAUX_COUVERTURE;

	static const std::string NOM_PARAMETRE_ATMOSPHERE_TOA;
	static const std::string NOM_PARAMETRE_ATMOSPHERE_TOA_ORDRE1;
	static const std::string CONNEXION_ERREUR_VERROU;
	static const int CONNEXION_MAX_ESSAI; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
	static const int CONNEXION_ATTENTE; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)

	static void setValeurTauxCouverture(const std::string &valeurParametreTauxCouverture);

	BaseDeDonnees(const std::string & cheminVersFichierBDD) :
			_sqlBackend(*soci::factory_sqlite3()) {
		_cheminVersFichierBDD = std::string("dbname=\"") + cheminVersFichierBDD + std::string("\" timeout=600");
	}

	/* Load database à l'ouverture */
	void loadObjectsFromDatabase();

	void enregistrerResultats(	ObjectDatabase::ParametresFixes & parametresFixes,
								std::vector<ObjectDatabase::Record> & enregistrements,
								const bool & saveAtmosphereToa,
								const bool & saveAtmosphereToaOrdre1,
								const bool & saveMaketCoverage);

	enum produitEnergyType {
		BRF, FluoPSI, FluoPSII
	};

	enum produitLevel {
		BOA, BOAIter1, BOAIterMax, BOASensor, TOA, Coupl, BOASensorDiffus, BOASensorDirect, BOASensorPureAtm, BOABeforeCoupl
	};

	std::string tool_getNomProduit(	const std::string & typeLayerName,
									const bool & perTypeLayer,
									const BaseDeDonnees::produitEnergyType & energyType,
									const BaseDeDonnees::produitLevel & level);

	/* Traitement du produit atmosphérique (TOA) */
	static void sauvegarderResultatAtmosphereTOA(	std::map<std::pair<int, std::pair<double, double> >, double> reflectances,
													std::map<std::pair<int, std::pair<double, double> >, double> luminances);
	static void sauvegarderResultatAtmosphereTOAOrdre1(	std::map<std::pair<int, std::pair<double, double> >, double> reflectances,
														std::map<std::pair<int, std::pair<double, double> >, double> luminances);

};

#endif /* BASEDEDONNEES_HPP_ */
