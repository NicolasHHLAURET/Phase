/*
 * BaseDeDonnees.cpp
 *
 *  Created on: 21 janv. 2011
 *      Authors : cortir, guilleuxj (2014-2018+)
 *
 *  WARNINGS ('Bug' SOCI !):
 *  Ne pas utiliser dans les into() des varables structurées incompletes mappées avec soci.
 *  -> ex: sql << "SELECT idProduit FROM Produit WHERE nomProduit='valeurTOA'",into(produit.idProduit);
 *  	tout les champs du produit n'étant pas sélectionnés, la requête va ... rien faire (pas d'erreur, et conserver la valeur initiale de produit.idProduit)
 *  -> Solutions, déclarer un "int idProduit = -42" et faire => sql << "SELECT idProduit FROM Produit WHERE nomProduit='valeurTOA'",into(idProduit);
 * 	   Ou bien récupérer tous les champs => sql << "SELECT idProduit, nomProduit, numIteration FROM Produit WHERE nomProduit='valeurTOA'",into(produit);
 *  -> Sanction en cas de non respect : des heures de déboggage ! :3
 */

/***********
 * En-tête *
 ***********/
#include "../include/databases/lut/BaseDeDonnees.hpp"
using namespace std;

/*****************************
 * Objets static & constants *
 *****************************/

const int BaseDeDonnees::CONNEXION_MAX_ESSAI = 600; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
const int BaseDeDonnees::CONNEXION_ATTENTE = 100; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)

const string BaseDeDonnees::NOM_SHORT_PARAMETRE_TAUX_COUVERTURE = "coverage";
const string BaseDeDonnees::NOM_PARAMETRE_TAUX_COUVERTURE = "maket.coverage";
const string BaseDeDonnees::NOM_TABLE_TAUX_COUVERTURE = "P_coverage";
const string BaseDeDonnees::NOM_COLUMN_TAUX_COUVERTURE = "id_coverage";

const string BaseDeDonnees::NOM_PARAMETRE_ATMOSPHERE_TOA = "Atmosphere_TOA";
const string BaseDeDonnees::NOM_PARAMETRE_ATMOSPHERE_TOA_ORDRE1 = "Atmosphere_TOA_Ordre1";

const string BaseDeDonnees::CONNEXION_ERREUR_VERROU = "database is locked";

/******************
 * Objects static *
 ******************/

string BaseDeDonnees::_valeurParametreTauxCouverture = "";
int BaseDeDonnees::_simulationIdCombinaison = -2;
ObjectDatabase BaseDeDonnees::_objectDatabase = ObjectDatabase();
map<pair<int, pair<double, double> >, double> BaseDeDonnees::_reflectancesAtmosphereTOA;
map<pair<int, pair<double, double> >, double> BaseDeDonnees::_luminancesAtmosphereTOA;
map<pair<int, pair<double, double> >, double> BaseDeDonnees::_reflectancesAtmosphereTOAOrdre1;
map<pair<int, pair<double, double> >, double> BaseDeDonnees::_luminancesAtmosphereTOAOrdre1;

/**********************
 * Méthodes statiques *
 **********************/

void BaseDeDonnees::setValeurTauxCouverture(const string &valeurParametreTauxCouverture) {
	BaseDeDonnees::_valeurParametreTauxCouverture = valeurParametreTauxCouverture;
}

/**
 * Sauvegarde le produit atmosphère pour enregistrement futur en base de données
 * La sauvegarde est due au fait qu'il est calculé à part des autres produits.
 */
void BaseDeDonnees::sauvegarderResultatAtmosphereTOA(	map<pair<int, pair<double, double> >, double> reflectances,
														map<pair<int, pair<double, double> >, double> luminances) {
	BaseDeDonnees::_reflectancesAtmosphereTOA = reflectances;
	BaseDeDonnees::_luminancesAtmosphereTOA = luminances;
}

void BaseDeDonnees::sauvegarderResultatAtmosphereTOAOrdre1(	map<pair<int, pair<double, double> >, double> reflectances,
															map<pair<int, pair<double, double> >, double> luminances) {
	BaseDeDonnees::_reflectancesAtmosphereTOAOrdre1 = reflectances;
	BaseDeDonnees::_luminancesAtmosphereTOAOrdre1 = luminances;
}

/*************************************
 * Getters génériques sur les tables *
 *************************************/

/**
 * Récupère l'id de l'angle de visée associée aux valeurs.
 * Le créé et l'enregistre en base si il n'existe pas.
 */
int BaseDeDonnees::getIdAngleViseeFromValues(	session & sql,
												const double &angleZenith,
												const double &angleAzimuth) {
	int idViewAngle = -1;
	ObjectDatabase::ViewAngle angleVisee;
	angleVisee.valueAzimuthAngle = angleAzimuth;
	angleVisee.valueZenithAngle = angleZenith;
	try {
		idViewAngle = _objectDatabase.getIdAngleVisee(angleVisee);
		if (idViewAngle < 1) {
			sql << "INSERT OR IGNORE INTO ViewAngle(valueZenithAngle, valueAzimuthAngle) VALUES(:valueZenithAngle, :valueAzimuthAngle)", use(
				angleVisee);
			sql << "SELECT idViewAngle FROM ViewAngle WHERE valueZenithAngle=:valueZenithAngle AND valueAzimuthAngle=:valueAzimuthAngle ;", use(
				angleVisee), into(idViewAngle);
			angleVisee.idViewAngle = idViewAngle;
			_objectDatabase.addAngleVisee(angleVisee);
		}
	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to get view angle id from values (generic method)" << endl;
		sql.close();
		throw se;
	}
	return idViewAngle;
}

/**
 * Retourne l'id du produit associé aux valeurs
 * le créé et l'enregistre en base si il n'existe pas.
 */
int BaseDeDonnees::getIdProduitFromValues(	session &sql,
											const string &nomProduit,
											const int &numIteration) {
	int idProduit = -1;
	ObjectDatabase::Product produit;
	produit.nameProduct = nomProduit;
	produit.numberIteration = numIteration;
	try {
		idProduit = _objectDatabase.getIdProduit(produit);
		if (idProduit < 1) {
			sql << "INSERT OR IGNORE INTO Product(nameProduct, numberIteration) VALUES (:nameProduct,:numberIteration)", use(produit);
			sql << "SELECT idProduct FROM Product WHERE nameProduct=:nameProduct AND numberIteration=:numberIteration ", use(produit), into(idProduit);
			produit.idProduct = idProduit;
			_objectDatabase.addProduit(produit);
		}
	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to get product id from values (generic method)" << endl;
		sql.close();
		throw se;
	}
	return idProduit;
}

/**
 * Récupère l'id du résultat scalaire associée aux valeurs.
 * Le créé et l'enregistre en base si il n'existe pas.
 * Note: il n'est pas enregistré en mémoire à cause du nombre de résultats qu'on peut obtenir
 * (coucou les simulations à 2000+ bandes spectrales ! :D)
 */
int BaseDeDonnees::getIdResultatScalaireFromValues(	session &sql,
													const int &idProduit,
													const int &idSimulation,
													const int &idSpectralBand,
													const int &idSpectralMode,
													const int &idSunAngle,
													const int &idCombinaison) {
	indicator soci_ind; // Indicator soci pour détecter les valeurs nulles
	int idResultatScalaire = -1;
	ObjectDatabase::ScalarResult resultatScalaire;
	resultatScalaire.idSolarAngle = idSunAngle;
	resultatScalaire.idSpectralBand = idSpectralBand;
	resultatScalaire.idSpectralMode = idSpectralMode;
	resultatScalaire.idCombination = idCombinaison;
	resultatScalaire.idSimulation = idSimulation;
	resultatScalaire.idProduct = idProduit;
	try {
		if (resultatScalaire.idCombination > 0) {
			sql << "SELECT DISTINCT idScalarResult FROM ScalarResult WHERE idSolarAngle=:idSolarAngle "
					<< " AND idSpectralBand=:idSpectralBand AND idSpectralMode=:idSpectralMode AND idCombination=:idCombination "
					<< " AND idSimulation=:idSimulation AND idProduct=:idProduct;", use(resultatScalaire, soci_ind), into(idResultatScalaire,
				soci_ind);
			if (idResultatScalaire < 1 || soci_ind == i_null) {
				sql << "INSERT OR IGNORE INTO ScalarResult(idSolarAngle, idSpectralBand,idSpectralMode, idCombination, idSimulation, idProduct) "
						<< "VALUES (:idSolarAngle,:idSpectralBand,:idSpectralMode,:idCombination,:idSimulation,:idProduct) ; ", use(resultatScalaire);
				sql << "SELECT DISTINCT idScalarResult FROM ScalarResult WHERE idSolarAngle=:idSolarAngle "
						<< " AND idSpectralBand=:idSpectralBand AND idSpectralMode=:idSpectralMode AND idCombination=:idCombination "
						<< " AND idSimulation=:idSimulation AND idProduct=:idProduct ;", use(resultatScalaire), into(idResultatScalaire);
			}
		} else {
			sql << "SELECT DISTINCT idScalarResult FROM ScalarResult WHERE idSolarAngle=:idSolarAngle "
					<< " AND idSpectralBand=:idSpectralBand AND idSpectralMode=:idSpectralMode "
					<< " AND idSimulation=:idSimulation AND idProduct=:idProduct;", use(resultatScalaire, soci_ind), into(idResultatScalaire,
				soci_ind);
			if (idResultatScalaire < 1 || soci_ind == i_null) {
				sql << "INSERT OR IGNORE INTO ScalarResult(idSolarAngle, idSpectralBand,idSpectralMode, idSimulation, idProduct) "
						<< "VALUES (:idSolarAngle,:idSpectralBand,:idSpectralMode,:idSimulation,:idProduct) ; ", use(resultatScalaire);
				sql << "SELECT DISTINCT idScalarResult FROM ScalarResult WHERE idSolarAngle=:idSolarAngle "
						<< " AND idSpectralBand=:idSpectralBand AND idSpectralMode=:idSpectralMode "
						<< " AND idSimulation=:idSimulation AND idProduct=:idProduct ;", use(resultatScalaire), into(idResultatScalaire);
			}
		}
	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to get scalar result id from values (generic method)" << endl;
		sql.close();
		throw se;
	}
	return idResultatScalaire;
}

int BaseDeDonnees::storeScalarFromValues(	session & sql,
											ObjectDatabase::Record & record) {
	try {
		int idScalarResult = getIdResultatScalaireFromValues(sql, record.resultatScalaire.idProduct, record.resultatScalaire.idSimulation,
			record.resultatScalaire.idSpectralBand, record.resultatScalaire.idSpectralMode, record.resultatScalaire.idSolarAngle,
			record.resultatScalaire.idCombination);
		for (unsigned int i = 0; i < record.scalarResults.size(); ++i) {
			record.scalarResults[i].idScalarResult = idScalarResult;
			sql << "INSERT OR IGNORE INTO " << record.scalarResults[i].tableName << "(idScalarResult, valueResult) " << " VALUES (:id, :val);", use(
				record.scalarResults[i].idScalarResult), use(record.scalarResults[i].valueResult);
		}
		return idScalarResult;
	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to store scalar result from values (generic method)" << endl;
		sql.close();
		throw se;
	}
}

int BaseDeDonnees::getIdDirectionalResultFromValues(session &sql,
													const int &idAngleVisee,
													const int &idResultatScalaire) {
	indicator soci_ind; // Indicator soci pour détecter les valeurs nulles
	int idDirectionalResult = -1;

	ObjectDatabase::DirectionalResult directionalResult;
	directionalResult.idScalarResult = idResultatScalaire;
	directionalResult.idViewAngle = idAngleVisee;

	try {
		sql << "INSERT OR IGNORE INTO DirectionalResult(idScalarResult, idViewAngle) " << " VALUES (:idScalarResult, :idViewAngle) ;", use(
			directionalResult);

		sql << "SELECT DISTINCT idDirectionalResult FROM DirectionalResult WHERE idViewAngle=:idViewAngle "
				<< " AND idScalarResult=:idScalarResult ;", use(directionalResult, soci_ind), into(idDirectionalResult, soci_ind);

	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to store directional result from values " << endl;
		sql.close();
		throw se;
	}

	return idDirectionalResult;
}

void BaseDeDonnees::storeResultatFromValues(session & sql,
											ObjectDatabase::RecordDirectionalResult record) {
	try {
		record.idDirectionalResult = getIdDirectionalResultFromValues(sql, record.angleVisee.idViewAngle, record.idScalarResult);
		sql << "INSERT OR IGNORE INTO " << record.tableName << "(idDirectionalResult, valueResult) " << " VALUES (:id, :res);", use(
			record.idDirectionalResult), use(record.valueResult);
	} catch (const soci_error &se) {
		cerr << sql.get_last_query() << endl;
		cerr << "LUT Error: when try to store result from values (generic method)" << endl;
		sql.close();
		throw se;
	}

}

void BaseDeDonnees::storeResultatFromValues(session & sql,
											const int &idAngleVisee,
											const int &idResultatScalaire,
											const bool &estLuminance,
											const double &valeurResultat) {
	try {
		int idDirectionalResult = getIdDirectionalResultFromValues(sql, idAngleVisee, idResultatScalaire);
		string tableName = "Reflectance";
		if (estLuminance) {
			tableName = "Radiance";
		}
		sql << "INSERT OR IGNORE INTO " << tableName << "(idDirectionalResult, valueResult) " << " VALUES (:id, :val);", use(idDirectionalResult), use(
			valeurResultat);
	} catch (const soci_error &se) {
		cerr << "LUT Error: when try to store result from values " << endl;
		sql.close();
		throw se;
	}

}

/************
 * Méthodes *
 ************/

bool BaseDeDonnees::estUnParametreRegulier(	const string &nomParametre,
											const bool &saveMaketCoverage) const {
	return !(boost::starts_with(nomParametre, "Phase.DartInputParameters.SpectralIntervals")
			|| boost::starts_with(nomParametre, "Directions.SunViewingAngles")
			|| boost::starts_with(nomParametre, "Directions.AddedDirections[0].ZenithAzimuth")
			|| boost::starts_with(nomParametre, "Directions.ExactDateHour") || estUnTreeParametre(nomParametre, saveMaketCoverage));
}

bool BaseDeDonnees::estUnTreeParametre(	const string &nomParametre,
										const bool &saveMaketCoverage) const {
	if (!saveMaketCoverage) {
		return false;
	}

	return ((boost::contains(nomParametre, "treePositionFileName") || boost::contains(nomParametre, "numberOfTreesInWholeScene")
			|| boost::contains(nomParametre, "HolesDistribution") || boost::contains(nomParametre, "LeavesDistribution")
			|| boost::contains(nomParametre, "CrownEllipsoid") || boost::contains(nomParametre, "CrownTrapezoid")
			|| boost::contains(nomParametre, "CrownTruncatedCone") || boost::contains(nomParametre, "CrownEllipsoidComposed")
			|| boost::contains(nomParametre, "CrownConeComp") || boost::contains(nomParametre, "probabilityOfTreesAtTheNodesOfTheGrid")
			|| boost::contains(nomParametre, "rawX") || boost::contains(nomParametre, "rawY") || boost::contains(nomParametre, "rayon")
			|| boost::contains(nomParametre, "Polygon2D") || boost::contains(nomParametre, "EllipsoidSubZone")
			|| boost::contains(nomParametre, "Polygon2D")) && boost::starts_with(nomParametre, "Trees."));
}

// Les parametres a inserer sont ceux presents dans DartProprietes::valeurProprieteSequence()
int BaseDeDonnees::insertionCombinaisonParametres(	session & sql,
													FichierProperties & proprietesDeLaSequence,
													FichierProperties & proprietesDeLaMaket,
													const bool & onVeutSauvegarderParametresIndirects,
													const int & numSpectralBand,
													const bool & saveMaketCoverage) const {
	int idCombinaison = -1;
	if (!(_simulationIdCombinaison == -2)) {
		return _simulationIdCombinaison;
	} else {
		int compteurParam = 0;
		map<int, pair<string, string> > listeParametres;
		int compteurTreeParametre = 0;
		map<int, pair<string, string> > listeTreeSpeciesParametres;

		// On recupere d'abord toutes les paires nom/valeur du fichier dart.sequenceur.properties
		try {
			int i = 0;
			stringstream clef;
			stringstream valeur;
			while (1) {
				clef.str("");
				clef << "sequenceur.parametre" << i << ".nom";
				valeur.str("");
				valeur << "sequenceur.parametre" << i << ".valeur";
				string nomParam = proprietesDeLaSequence.valeurProperties<string>(clef.str());

				// On stock le couple nom/valeur du parametre i sous la clef i (s'il ne s'agit pas de la bande spectrale, car geree independemment des autres Parametres)
				if (estUnTreeParametre(nomParam, saveMaketCoverage)) {
					listeTreeSpeciesParametres[compteurTreeParametre++] = pair<string, string>(nomParam,
						proprietesDeLaSequence.valeurProperties<string>(valeur.str()));
				} else {
					if (estUnParametreRegulier(nomParam, saveMaketCoverage)) {
						listeParametres[compteurParam++] = pair<string, string>(nomParam,
							proprietesDeLaSequence.valeurProperties<string>(valeur.str()));
					}
				}
				i++;
			}
		} catch (const exception & e) {
			//Utilise pour detecter que tous les parametres ont etes parcourus et donc quitter la boucle
		}

		//On recupere ensuite, si necessaire, les paires nom/valeur du fichier maket.properties
		if (onVeutSauvegarderParametresIndirects) {
			if (proprietesDeLaMaket.exists("maket.LAI.trees")) {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.trees",
					proprietesDeLaMaket.valeurProperties<string>("maket.LAI.trees"));
			} else {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.trees","0");
			}

			if (proprietesDeLaMaket.exists("maket.LAI.object3D")) {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.object3D",
					proprietesDeLaMaket.valeurProperties<string>("maket.LAI.object3D"));
			} else {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.object3D","0");
			}

			if (proprietesDeLaMaket.exists("maket.LAI.plot")) {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.plot",
					proprietesDeLaMaket.valeurProperties<string>("maket.LAI.plot"));
			} else {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.plot","0");
			}

			if (proprietesDeLaMaket.exists("maket.LAI.total")) {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.total",
					proprietesDeLaMaket.valeurProperties<string>("maket.LAI.total"));
			} else {
				listeParametres[compteurParam++] = pair<string, string>("maket.LAI.total","0");
			}
		}

		// Si le taux de couverture à été défini, on l'ajoute aux paramètres.
		if (_valeurParametreTauxCouverture != "" && (saveMaketCoverage || listeTreeSpeciesParametres.size() > 0)) {
			int idValueCoverage = ajouterTauxRecouvrementDansLutBaseDeDonnees(sql);
			listeParametres[compteurParam++] = pair<string, string>(NOM_PARAMETRE_TAUX_COUVERTURE, _valeurParametreTauxCouverture);

			// Il y a un tree param ! <3
			if (_valeurParametreTauxCouverture != "" && listeTreeSpeciesParametres.size() > 0) {
				string requeteInsertP1 = " INSERT OR IGNORE INTO CombinationTreeSpecies ( " + NOM_COLUMN_TAUX_COUVERTURE;
				string requeteInsertP2 = " ) VALUES ( " + boost::lexical_cast<std::string>(idValueCoverage);

				for (unsigned int numTSP = 0; numTSP < listeTreeSpeciesParametres.size(); numTSP++) {
					int idParametreTreeSpecies = -1;
					int idValeurParametreTreeSpecies = -1;
					int nbParameter = -1;
					string tableName = "";
					string columnName = "";

					// Insertion du tree paramètre
					try {
						sql << "SELECT idParameter FROM TranslationParameterTreeSpecies WHERE nameCompleteParameter='"
								<< listeTreeSpeciesParametres.at(numTSP).first << "' ;", into(idParametreTreeSpecies);
						if (idParametreTreeSpecies < 1) {
							sql << "SELECT count(*) FROM TranslationParameterTreeSpecies ", into(nbParameter);
							tableName = "PTS_" + nbParameter;
							columnName = "id_" + nbParameter;

							// Pour le groupe, on en créé un nouveau, à ce stade on a perdu l'info du groupe depuis longtemps.
							// Cette partie est seulement en backup du sequenceur Java qui gère ça normalement
							int numGroup = -1;
							sql << "SELECT count(*) FROM GroupParameter ", into(numGroup);
							numGroup++;
							string groupName = "GroupTree_" + boost::lexical_cast<std::string>(numGroup);
							sql << " INSERT OR IGNORE INTO GroupParameter(idGroupParameter, nameGroupParameter) VALUES (" << numGroup << ", '"
									<< groupName << "');";
							sql
									<< " INSERT INTO TranslationParameterTreeSpecies (nameShortParameter, nameCompleteParameter, nameTable, nameColumnCombination, idGroupParameter) VALUES ( '"
									<< listeTreeSpeciesParametres.at(numTSP).first << "','" << listeTreeSpeciesParametres.at(numTSP).first << "','"
									<< tableName << "','" << columnName << "', " << numGroup << ");";
							sql << "SELECT idParameter FROM TranslationParameterTreeSpecies WHERE nameCompleteParameter='"
									<< listeTreeSpeciesParametres.at(numTSP).first << "' ;", into(idParametreTreeSpecies);
							sql << "CREATE TABLE IF NOT EXISTS " << tableName
									<< "(idValueParameter INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, valueParameter TEXT NOT NULL, unique(valueParameter))";
							sql << "ALTER TABLE Combination ADD COLUMN " << columnName << " INTEGER REFERENCES " << tableName << "(idValueParameter)";
						} else {
							sql << "SELECT nameTable FROM TranslationParameterTreeSpecies WHERE idParameter=" << idParametreTreeSpecies << ";", into(
								tableName);
							sql << "SELECT nameColumnCombination FROM TranslationParameterTreeSpecies WHERE idParameter=" << idParametreTreeSpecies
									<< ";", into(columnName);
						}

						sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='"
								<< listeTreeSpeciesParametres.at(numTSP).second << "' ;", into(idValeurParametreTreeSpecies);
						if (idValeurParametreTreeSpecies < 1) {
							sql << "INSERT INTO " << tableName << " (valueParameter) VALUES ('" << listeTreeSpeciesParametres.at(numTSP).second
									<< "');";
							sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='"
									<< listeTreeSpeciesParametres.at(numTSP).second << "' ;", into(idValeurParametreTreeSpecies);
						}
					} catch (const soci_error & se) {
						sql.close();
						throw se;
					}

					// Construction de la requete de la combinaison ...
					requeteInsertP1 += "," + columnName;
					requeteInsertP2 += "," + boost::lexical_cast<std::string>(idValeurParametreTreeSpecies);
				}

				// Insertion de la combinaison de tree parametre
				sql << requeteInsertP1 << requeteInsertP2 << ")";
			}
		}
		// Si pas de param dans listeParametres, alors retourner -1 (-> idcombinaison=NULL), car on fait varier la Bande Spectrale dans le sequenceur, ce qui n'apparait pas ici (Bande Spectrale traitee separemment)
		if (listeParametres.size() == 0) {
			_simulationIdCombinaison = -1;
		} else {
			try {
				// Ceci est de la duplication de code (mais j'ai rename les variables pour cacher la misère).
				string requeteInsertP1 = " INSERT OR IGNORE INTO Combination ( ";
				string requeteInsertP2 = " ) VALUES ( ";

				string requeteSelect = " SELECT idCombination FROM Combination ";

				for (unsigned int numParam = 0; numParam < listeParametres.size(); numParam++) {
					int idParameter = -1;
					int idValueParameter = -1;
					int nbParameter = -1;
					string tableName = "";
					string columnName = "";

					// Traitement du paramètre
					sql << "SELECT idParameter FROM TranslationParameter WHERE nameCompleteParameter='" << listeParametres.at(numParam).first
							<< "' ;", into(idParameter);
					if (idParameter < 1) {
						sql << "SELECT count(*) FROM TranslationParameter ", into(nbParameter);
						tableName = "P_" + boost::lexical_cast<std::string>(nbParameter);
						columnName = "id_" + boost::lexical_cast<std::string>(nbParameter);
						// Pour le groupe, on en créé un nouveau, à ce stade on a perdu l'info du groupe depuis longtemps.
						// Cette partie est seulement en backup du sequenceur Java qui gère ça normalement
						int numGroup = -1;
						sql << "SELECT count(*) FROM GroupParameter ", into(numGroup);
						numGroup++;
						string groupName = "Group_" + boost::lexical_cast<std::string>(numGroup);
						sql << " INSERT OR IGNORE INTO GroupParameter(idGroupParameter, nameGroupParameter) VALUES (" << numGroup << ", '"
								<< groupName << "');";
						sql
								<< " INSERT INTO TranslationParameter (nameShortParameter, nameCompleteParameter, nameTable, nameColumnCombination, idGroupParameter) VALUES ( '"
								<< listeParametres.at(numParam).first << "','" << listeParametres.at(numParam).first << "','" << tableName << "','"
								<< columnName << "', " << numGroup << ");";
						sql << "SELECT idParameter FROM TranslationParameter WHERE nameCompleteParameter='" << listeParametres.at(numParam).first
								<< "' ;", into(idParameter);
						sql << "CREATE TABLE IF NOT EXISTS " << tableName
								<< "(idValueParameter INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, valueParameter TEXT NOT NULL, unique(valueParameter))";
						sql << "ALTER TABLE Combination ADD COLUMN " << columnName << " INTEGER REFERENCES " << tableName << "(idValueParameter)";
					} else {
						sql << "SELECT nameTable FROM TranslationParameter WHERE idParameter=" << idParameter << ";", into(tableName);
						sql << "SELECT nameColumnCombination FROM TranslationParameter WHERE idParameter=" << idParameter << ";", into(columnName);
					}

					sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='" << listeParametres.at(numParam).second << "' ;", into(
						idValueParameter);
					if (idValueParameter < 1) {
						sql << "INSERT INTO " << tableName << " (valueParameter) VALUES ('" << listeParametres.at(numParam).second << "');";
						sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='" << listeParametres.at(numParam).second
								<< "' ;", into(idValueParameter);
					}
					// Construction des requetes de la combinaison ...
					if (numParam > 0) {
						requeteInsertP1 += ",";
						requeteInsertP2 += ",";
					}
					requeteInsertP1 += columnName;
					requeteInsertP2 += boost::lexical_cast<std::string>(idValueParameter);

					if (numParam == 0) {
						requeteSelect += " WHERE " + columnName + "=" + boost::lexical_cast<std::string>(idValueParameter);
					} else {
						requeteSelect += " AND " + columnName + "=" + boost::lexical_cast<std::string>(idValueParameter);
					}
				}

				// Insertion de la combinaison de parametre
				sql << requeteSelect, into(idCombinaison);
				if (idCombinaison == -1) {
					sql << requeteInsertP1 << requeteInsertP2 << ")";
					sql << requeteSelect, into(idCombinaison);
				}
				sql << requeteSelect, into(idCombinaison);
				if (idCombinaison == -1) {
					throw soci_error("LUT Error: parameter combinaison not found. (insert parameters combinaison)");
				}

			} catch (const soci_error & se) {
				throw se;
			}
		}
		_simulationIdCombinaison = idCombinaison;
		return idCombinaison;
	}
}

/**
 * Ajoute le taux de couverture en base de données
 * Fait le lien entre ce taux, et les paramètres des arbres associés.
 * @param coverageValue : le taux de couverture calculé par DART (lu dans le maket.properties, ouais ...)
 */
int BaseDeDonnees::ajouterTauxRecouvrementDansLutBaseDeDonnees(session &sql) const {
	// Insertion en base -> Attention à la concurence
	string nomParametre = NOM_PARAMETRE_TAUX_COUVERTURE;
	string tableName = NOM_TABLE_TAUX_COUVERTURE;
	string columnName = NOM_COLUMN_TAUX_COUVERTURE;
	string userName = NOM_SHORT_PARAMETRE_TAUX_COUVERTURE;

	int idParametre = -1;
	int idValeurParametre = -1;
	if (_valeurParametreTauxCouverture != "") {
		try {
			// Création du paramètre (si non existant)
			try {
				sql << "SELECT idParameter FROM TranslationParameter WHERE nameCompleteParameter='" << nomParametre << "' ;", into(idParametre);

				// N'existe pas
				if (idParametre < 1) {
					sql << "CREATE TABLE IF NOT EXISTS " << tableName
							<< " (idValueParameter INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, valueParameter TEXT NOT NULL);";
					sql << "INSERT INTO TranslationParameter(nameShortParameter, nameCompleteParameter, nameTable, nameColumnCombination) VALUES ('"
							<< userName << "','" << nomParametre << "','" << tableName << "','" << columnName << "');";
					sql << "ALTER TABLE Combination ADD COLUMN " << columnName << " INTEGER REFERENCES " << tableName << "(idValueParameter)";
					sql << "SELECT idParameter FROM TranslationParameter WHERE nameCompleteParameter='" << nomParametre << "' ;", into(idParametre);
				}
			} catch (const soci_error & se) {
				cerr << "LUT Error: error when try to create maket coverage table parameter." << endl;
				sql.close();
				throw se;
			}

			// Récupération de l'id de la valeur du taux de couverture
			try {
				sql << "SELECT PC.idValueParameter FROM " << tableName << " PC " << "WHERE PC.valueParameter = '" << _valeurParametreTauxCouverture
						<< "';", into(idValeurParametre);
				if (idValeurParametre < 1) {
					sql << "INSERT INTO " << tableName << "(valueParameter) " << "VALUES(" << _valeurParametreTauxCouverture << ");";
					sql << "SELECT PC.idValueParameter FROM " << tableName << " PC " << "WHERE PC.valueParameter = '"
							<< _valeurParametreTauxCouverture << "';", into(idValeurParametre);
					if (idValeurParametre < 1) {
						throw soci_error("LUT Error : can not store id value coverage parameter");
					}
				}
			} catch (const soci_error & se) {
				cerr << "LUT Error: can't find id value coverage parameter" << endl;
				sql.close();
				throw se;
			}
		} catch (const soci_error &se) {
			cerr << "LUT Error: when try to store maket coverage value" << endl;
			throw se;
		}
	}
	return idValeurParametre;
}

/**
 *
 */
string BaseDeDonnees::tool_getNomProduit(	const string & typeLayerName,
											const bool & perTypeLayer,
											const BaseDeDonnees::produitEnergyType & energyType,
											const BaseDeDonnees::produitLevel & level) {
	string nomDuChampResultat;

	// Type layer
	if (perTypeLayer) {
		nomDuChampResultat = typeLayerName;
		for (string::size_type i = 0; (i = nomDuChampResultat.find(";", i)) != string::npos;) {
			nomDuChampResultat.replace(i, 1, "_");
			i += 1;
		}
	} else {
		nomDuChampResultat = "All";
	}
	nomDuChampResultat = nomDuChampResultat + ";";

	// Energy type
	switch (energyType) {
		case BaseDeDonnees::BRF:
			nomDuChampResultat = nomDuChampResultat + "BRF";
			break;
		case BaseDeDonnees::FluoPSI:
			nomDuChampResultat = nomDuChampResultat + "FluoPSI";
			break;
		case BaseDeDonnees::FluoPSII:
			nomDuChampResultat = nomDuChampResultat + "FluoPSII";
			break;
		default:
			nomDuChampResultat = nomDuChampResultat + "Unknown";
			break;
	}
	nomDuChampResultat = nomDuChampResultat + ";";

	// Energy level
	switch (level) {
		case BaseDeDonnees::BOA:
			nomDuChampResultat = nomDuChampResultat + "BOA";
			break;
		case BaseDeDonnees::BOAIter1:
			nomDuChampResultat = nomDuChampResultat + "BOAIter1";
			break;
		case BaseDeDonnees::BOAIterMax:
			nomDuChampResultat = nomDuChampResultat + "BOAIterMax";
			break;
		case BaseDeDonnees::BOASensor:
			nomDuChampResultat = nomDuChampResultat + "BOASensor";
			break;
		case BaseDeDonnees::TOA:
			nomDuChampResultat = nomDuChampResultat + "TOA";
			break;
		case BaseDeDonnees::Coupl:
			nomDuChampResultat = nomDuChampResultat + "Coupl";
			break;
		case BaseDeDonnees::BOASensorDiffus:
			nomDuChampResultat = nomDuChampResultat + "BOASensorDiffus";
			break;
		case BaseDeDonnees::BOASensorDirect:
			nomDuChampResultat = nomDuChampResultat + "BOASensorDirect";
			break;
		case BaseDeDonnees::BOASensorPureAtm:
			nomDuChampResultat = nomDuChampResultat + "BOASensorPureAtm";
			break;
		case BaseDeDonnees::BOABeforeCoupl:
			nomDuChampResultat = nomDuChampResultat + "BOA_Before_Coupling";
			break;
		default:
			nomDuChampResultat = nomDuChampResultat + "Unknown";
			break;
	}

	return nomDuChampResultat;
}

/* Traite le cas si, l'utilisateur à saisi la direction solaire sous la forme temporelle et l'a fait varier
 * Dans ce cas, on insere le temps associé à l'angle solaire courant
 * */
void BaseDeDonnees::traiterTemps(	session & sql,
									const int & idAngleSolaire,
									const FichierProperties & parametresDeLaSequence) {
	bool foundDateParam = false;
	int i = 0;
	stringstream clef;
	stringstream valeur;

	string requestSelect = "SELECT idCombination FROM CombinationTime WHERE idSolarAngle=:sa ";
	string requestInsertP1 = "INSERT INTO CombinationTime (idSolarAngle ";
	string requestInsertP2 = " ) VALUES (:sa";
	try {
		while (1) {
			clef.str("");
			clef << "sequenceur.parametre" << i << ".nom";
			valeur.str("");
			valeur << "sequenceur.parametre" << i << ".valeur";
			string nomParam = parametresDeLaSequence.valeurProperties<string>(clef.str());
			if (boost::starts_with(nomParam, "Directions.ExactDateHour")) {
				int idParameter = -1;
				int nbParameter = 0;
				int idValueParameter = -1;
				string tableName = "";
				string columnName = "";
				string valeurParametre = parametresDeLaSequence.valeurProperties<string>(valeur.str());

				// Et voici un fix à l'arrache, crû 2019, si le LUT est créé sans le séquenceur, cette table n'existe PAS
//				sql << " CREATE TABLE IF NOT EXISTS TranslationParameterTime (idParameter INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,nameShortParameter TEXT NOT NULL,nameCompleteParameter TEXT NOT NULL,nameTable TEXT NOT NULL,nameColumnCombination TEXT NOT NULL,idGroupParameter INTEGER REFERENCES GroupParameter(idGroupParameter),UNIQUE (nameShortParameter), UNIQUE (nameCompleteParameter), UNIQUE (nameTable), UNIQUE (nameColumnCombination)); ";
//				sql << " CREATE TABLE IF NOT EXISTS CombinationTime (idCombination INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,idSolarAngle INTEGER REFERENCES SolarAngle (idSolarAngle) ); ";

				// On vérifie déjà que le paramètre est en base
				sql << "SELECT idParameter FROM TranslationParameterTime WHERE nameCompleteParameter=:name ", use(nomParam), into(idParameter);
				if (idParameter < 1) {
					sql << "SELECT count(*) FROM TranslationParameterTime ", into(nbParameter);
					tableName = "PTime_" + nbParameter;
					columnName = "id_" + nbParameter;

					// Pour le groupe, on en créé un nouveau, à ce stade on a perdu l'info du groupe depuis longtemps.
					// Cette partie est seulement en backup du sequenceur Java qui gère ça normalement
					int numGroup = -1;
					sql << "SELECT count(*) FROM GroupParameter ", into(numGroup);
					numGroup++;
					string groupName = "GroupTime_" + boost::lexical_cast<std::string>(numGroup);
					sql << " INSERT OR IGNORE INTO GroupParameter(idGroupParameter, nameGroupParameter) VALUES (" << numGroup << ", '" << groupName
							<< "');";

					sql
							<< "INSERT INTO TranslationParameterTime(nameShortParameter, nameCompleteParameter, nameTable, nameColumnCombination, idGroupParameter) VALUES (:s, :c, :t, :l, :g)", use(
						nomParam), use(nomParam), use(tableName), use(columnName), use(numGroup);
					sql << "SELECT idParameter FROM TranslationParameterTime WHERE nameCompleteParameter=:name ", use(nomParam), into(idParameter);
				} else {
					sql << "SELECT nameTable FROM TranslationParameterTime WHERE idParameter=" << idParameter << ";", into(tableName);
					sql << "SELECT nameColumnCombination FROM TranslationParameterTime WHERE idParameter=" << idParameter << ";", into(columnName);
				}

				sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='" << valeurParametre << "' ;", into(idValueParameter);
				if (idValueParameter < 1) {
					sql << "INSERT INTO " << tableName << " (valueParameter) VALUES ('" << valeurParametre << "';";
					sql << "SELECT idValueParameter FROM " << tableName << " WHERE valueParameter='" << valeurParametre << "' ;", into(
						idValueParameter);
				}

				requestSelect += " AND ";
				requestInsertP1 += ",";
				requestInsertP2 += ",";

				requestSelect += columnName + "=" + boost::lexical_cast<std::string>(idValueParameter);
				requestInsertP1 += columnName;
				requestInsertP2 += boost::lexical_cast<std::string>(idValueParameter);
				foundDateParam = true;
			}
			i++;
		}
	} catch (const soci_error & se) {
		sql.close();
		throw se;
	} catch (const exception & e) {
		//Utilise pour detecter que tous les parametres ont etes parcourus et donc quitter la boucle
		//(Il doit clairement y avoir une meilleure solution de développement ....)
	}

	try {
		// Un paramètre DATE à été trouvé
		if (foundDateParam) {
			int idCombination = -1;
			sql << requestSelect, use(idAngleSolaire), into(idCombination);
			if (idCombination < 1) {
				sql << requestInsertP1 << requestInsertP2 << ");", use(idAngleSolaire);
			}
		}
	} catch (const soci_error & se) {
		sql.close();
		throw se;
	}
}

void BaseDeDonnees::enregistrerResultats(	ObjectDatabase::ParametresFixes & parametresFixes,
											vector<ObjectDatabase::Record> & enregistrements,
											const bool & saveAtmosphereToa,
											const bool & saveAtmosphereToaOrdre1,
											const bool & saveMaketCoverage) {
	time_t t0 = time(&t0);

	int idCombinaison = -1;
	int nbEssai = 0;
	bool verrouObtenu = false;
	do {
		try {
			session sql(_sqlBackend, _cheminVersFichierBDD);
			sql << "PRAGMA journal_mode = OFF";
			sql << "PRAGMA synchronous = OFF";
			sql << "PRAGMA temp_store = MEMORY";
			/*
			 * Pose d'un verrou exclusif sur la base de données avant le processus d'enregistrement.
			 * Afin d'éviter les problèmes liés au lancement parallèle de plusieures instances de DART
			 * qui faisaient des tests sur une base non verouillée et pouvaient avoir des données modifiées
			 * avant de pouvoir enregistrer.
			 */
			sql << "BEGIN EXCLUSIVE TRANSACTION"; // <=> au sql.begin(); mais avec le verrou exclusif.
			verrouObtenu = true;

			/* Traitement de la simulation */
			int idSimulation = _objectDatabase.getIdSimulation(parametresFixes.simulation);
			if (idSimulation < 1) {
				try {
					sql << "INSERT OR IGNORE INTO Simulation(nameSimulation) VALUES (:nameSimulation) ;", use(parametresFixes.simulation);

					sql << "SELECT idSimulation FROM Simulation WHERE nameSimulation=:nameSimulation ;", use(parametresFixes.simulation), into(
						idSimulation);

					parametresFixes.simulation.idSimulation = idSimulation;
					_objectDatabase.addSimulation(parametresFixes.simulation);
				} catch (const soci_error & se) {
					cerr << "LUT Error: when try to store simulation" << endl;
					sql.close();
					throw se;
				}
			}

			/* Traitement de l'angle solaire */
			int idAnglesSoleil = _objectDatabase.getIdAngleSolaire(parametresFixes.angleSoleil);
			if (idAnglesSoleil < 1) {
				try {
					sql << "INSERT OR IGNORE INTO SolarAngle(valueZenithAngle, valueAzimuthAngle) VALUES (:valueZenithAngle, :valueAzimuthAngle) ;", use(
						parametresFixes.angleSoleil);
					sql << "SELECT idSolarAngle FROM SolarAngle WHERE valueZenithAngle=:valueZenithAngle AND valueAzimuthAngle=:valueAzimuthAngle ;", use(
						parametresFixes.angleSoleil), into(idAnglesSoleil);

					parametresFixes.angleSoleil.idSolarAngle = idAnglesSoleil;
					_objectDatabase.addAngleSolaire(parametresFixes.angleSoleil);
				} catch (const soci_error & se) {
					cerr << "LUT Error: when try to store sun angle" << endl;
					sql.close();
					throw se;
				}
			}

			try {
				this->traiterTemps(sql, idAnglesSoleil, parametresFixes.parametreCombinaison.sequenceProperties);
			} catch (const soci_error & se) {
				cout << "[Warning] LUT can not store date and time. To work with date you must use the sequencer by varying at least one date parameter. Solar angle is still functional" << endl;
				//throw se;
			}

			// Les paramètres invariants de la simulation ont été traités, et leurs id maintenant
			// connus pour pouvoir enregistrer les résultats associées (Enregistrements).
			vector<ObjectDatabase::Record>::iterator itr;
			for (itr = enregistrements.begin(); itr != enregistrements.end(); ++itr) {

				/* Traitement de la bande spectrale */
				int idBandeSpectrale = _objectDatabase.getIdBandeSpectrale(itr->bandeSpectrale);

				if (idBandeSpectrale < 1) {
					try {
						sql
								<< "SELECT idSpectralBand FROM SpectralBand WHERE valueCentralWavelength=:valueCentralWavelength AND valueSpectralBandwidth=:valueSpectralBandwidth ;", use(
							itr->bandeSpectrale), into(idBandeSpectrale);
						if (idBandeSpectrale < 1) {
							sql
									<< "INSERT OR IGNORE INTO SpectralBand(valueCentralWavelength, valueSpectralBandwidth) VALUES (:valueCentralWavelength, :valueSpectralBandwidth);", use(
								itr->bandeSpectrale);
							sql
									<< "SELECT idSpectralBand FROM SpectralBand WHERE valueCentralWavelength=:valueCentralWavelength AND valueSpectralBandwidth=:valueSpectralBandwidth ;", use(
								itr->bandeSpectrale), into(idBandeSpectrale);
						}
						itr->bandeSpectrale.idSpectralBand = idBandeSpectrale;
						_objectDatabase.addBandeSpectrale(itr->bandeSpectrale);
					} catch (const soci_error & se) {
						cerr << "LUT Error: when try to store spectral band" << endl;
						sql.close();
						throw se;
					}
				}

				/* Traitement de du le mode spectrale */
				int idModeSpectral = _objectDatabase.getIdSpectralMode(itr->modeSpectral);
				if (idModeSpectral < 1) {
					try {
						sql << "SELECT idSpectralMode FROM SpectralMode WHERE idSpectralMode=:idSpectralMode ;", use(itr->modeSpectral), into(
							idModeSpectral);
						if (idModeSpectral < 1) {
							sql
									<< "INSERT OR IGNORE INTO SpectralMode(idSpectralMode, nameSpectralMode) VALUES (:idSpectralMode, :nameSpectralMode);", use(
								itr->modeSpectral);
							sql << "SELECT idSpectralMode FROM SpectralMode WHERE idSpectralMode=:idSpectralMode ;", use(itr->modeSpectral), into(
								idModeSpectral);
						}
						itr->modeSpectral.idSpectralMode = idModeSpectral;
						_objectDatabase.addSpectralMode(itr->modeSpectral);
					} catch (const soci_error & se) {
						cerr << "LUT Error: when try to store spectral mode" << endl;
						sql.close();
						throw se;
					}
				}

				/* Traitement de la combinaison */
				try {
					idCombinaison = insertionCombinaisonParametres(sql, parametresFixes.parametreCombinaison.sequenceProperties,
						parametresFixes.parametreCombinaison.maketProperties, parametresFixes.parametreCombinaison.sauvegarderParametresIndirects,
						itr->spectralBandNumber, saveMaketCoverage);
				} catch (const soci_error & se) {
					cerr << "LUT Error: fail to store link between results and parameters combinaison" << endl;
					cerr << sql.get_last_query() << endl;
					sql.close();
					throw se;
				}

				/* Traitement du produit */
				int idProduit = _objectDatabase.getIdProduit(itr->produit);
				if (idProduit < 1) {
					try {
						sql << "INSERT OR IGNORE INTO Product(nameProduct, numberIteration) VALUES (:nameProduct, :numberIteration) ;", use(
							itr->produit);
						sql << "SELECT idProduct FROM Product WHERE nameProduct=:nameProduct AND numberIteration=:numberIteration ;", use(
							itr->produit), into(idProduit);

						itr->produit.idProduct = idProduit;
						_objectDatabase.addProduit(itr->produit);
					} catch (const soci_error & se) {
						cerr << "LUT Error: when try to store -standard- product (all except maket coverage, indirect LAI, atmospheric products) "
								<< endl;
						sql.close();
						throw se;
					}
				}

				/* Récupération des id traités pour enregistrement dans la BD */
				itr->resultatScalaire.idCombination = idCombinaison;
				itr->resultatScalaire.idSimulation = idSimulation;
				itr->resultatScalaire.idSolarAngle = idAnglesSoleil;
				itr->resultatScalaire.idSpectralBand = idBandeSpectrale;
				itr->resultatScalaire.idSpectralMode = idModeSpectral;
				itr->resultatScalaire.idProduct = idProduit;

				/* Traitement du résultat scalaire */
				int idResultatScalaire = storeScalarFromValues(sql, *itr);

				/* Traitement des résultats */
				vector<ObjectDatabase::RecordDirectionalResult>::iterator itrEnr;
				for (itrEnr = itr->directionalResults.begin(); itrEnr != itr->directionalResults.end(); ++itrEnr) {
					itrEnr->idScalarResult = idResultatScalaire;

					/* Traitement de la direction*/
					int idAnglesVisee = _objectDatabase.getIdAngleVisee(itrEnr->angleVisee);
					if (idAnglesVisee < 1) {
						try {
							sql
									<< "INSERT OR IGNORE INTO ViewAngle(valueZenithAngle, valueAzimuthAngle) VALUES (:valueZenithAngle, :valueAzimuthAngle) ;", use(
								itrEnr->angleVisee);
							sql
									<< "SELECT idViewAngle FROM ViewAngle WHERE valueZenithAngle=:valueZenithAngle AND valueAzimuthAngle=:valueAzimuthAngle ;", use(
								itrEnr->angleVisee), into(idAnglesVisee);
							itrEnr->angleVisee.idViewAngle = idAnglesVisee;
							_objectDatabase.addAngleVisee(itrEnr->angleVisee);
						} catch (const soci_error & se) {
							cerr << "LUT Error: when try to store view direction." << endl;
							sql.close();
							throw se;
						}
					}
					itrEnr->angleVisee.idViewAngle = idAnglesVisee;

					/* Traitement du résultat*/
					storeResultatFromValues(sql, (*itrEnr));
				}

				// C'est ici qu'on enregistrera nos produits purement atmosphériques
				// Une session d'enregistrement à chaque bande spectrale (spectralbandnumber)
				try {
					if (hasAtmosphericTOAtoStore(true) && saveAtmosphereToaOrdre1) {
						enregistrerProduitAtmosphereTOA(sql, itr->spectralBandNumber, itr->resultatScalaire.idSimulation,
							itr->resultatScalaire.idSpectralBand, itr->resultatScalaire.idSpectralMode, itr->resultatScalaire.idSolarAngle,
							itr->resultatScalaire.idCombination, true);
					}
					if (hasAtmosphericTOAtoStore(false) && saveAtmosphereToa) {
						enregistrerProduitAtmosphereTOA(sql, itr->spectralBandNumber, itr->resultatScalaire.idSimulation,
							itr->resultatScalaire.idSpectralBand, itr->resultatScalaire.idSpectralMode, itr->resultatScalaire.idSolarAngle,
							itr->resultatScalaire.idCombination, false);
					}
				} catch (const soci_error & se) {
					sql.close();
					cerr << "LUT Error: when try to store results and/or product of TOA atmosphere" << endl;
					throw se;
				}
			}
			sql.commit();
			sql.close();
		} catch (const soci_error & se) {
			if (nbEssai > CONNEXION_MAX_ESSAI) {
				cerr << endl << "Concurrency saturation: try reducing the number of simulation launched in parallel" << endl;
				throw se;
			} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
				boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
			} else {
				cerr << "LUT Error: recording failed" << endl;
				throw se;
			}
		}
	} while (!verrouObtenu);
	time_t t1 = time(&t1);
	cout << "LUT results stored in " << (t1 - t0) << "second(s)" << endl;
}

void BaseDeDonnees::loadObjectsFromDatabase() {
	session sql(_sqlBackend, _cheminVersFichierBDD);
	sql << "PRAGMA journal_mode = OFF";
	sql << "PRAGMA synchronous = OFF";
	sql << "PRAGMA temp_store = MEMORY";
	try {
		sql.begin();
		//Fist step : dimentionner nos vectors
		int lengthSpectralsBands = 0;
		int lengthSpectralsModes = 0;
		int lengtthSolarAngles = 0;
		int lengtthViewAngles = 0;
		int lengthProducts = 0;
		int lengthSimul = 0;

		sql << "SELECT count(*) FROM SpectralBand", into(lengthSpectralsBands);
		sql << "SELECT count(*) FROM SpectralMode", into(lengthSpectralsModes);
		sql << "SELECT count(*) FROM SolarAngle", into(lengtthSolarAngles);
		sql << "SELECT count(*) FROM ViewAngle", into(lengtthViewAngles);
		sql << "SELECT count(*) FROM Product", into(lengthProducts);
		sql << "SELECT count(*) FROM Simulation", into(lengthSimul);

		ObjectDatabase::resizeVectorBandesSpectrales(lengthSpectralsBands);
		ObjectDatabase::resizeVectorSpectralMode(lengthSpectralsModes);
		ObjectDatabase::resizeVectorAnglesSolaire(lengtthSolarAngles);
		ObjectDatabase::resizeVectorAnglesVisee(lengtthViewAngles);
		ObjectDatabase::resizeVectorProduits(lengthProducts);
		ObjectDatabase::resizeVectorSimulations(lengthSimul);

		if (lengthSpectralsBands > 0) {
			ObjectDatabase::SpectralBand bandeSpectrale;
			statement st = (sql.prepare << "SELECT idSpectralBand, valueCentralWaveLength, valueSpectralBandwidth FROM SpectralBand ; ", into(
				bandeSpectrale.idSpectralBand), into(bandeSpectrale.valueCentralWavelength), into(bandeSpectrale.valueSpectralBandwidth));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addBandeSpectrale(bandeSpectrale);
			}
		}

		if (lengthSpectralsModes > 0) {
			ObjectDatabase::SpectralMode modeSpectral;
			statement st = (sql.prepare << "SELECT idSpectralMode, nameSpectralMode FROM SpectralMode ; ", into(modeSpectral.idSpectralMode), into(
				modeSpectral.nameSpectralMode));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addSpectralMode(modeSpectral);
			}
		}

		if (lengtthSolarAngles > 0) {
			ObjectDatabase::SolarAngle angle;
			statement st =
					(sql.prepare << "SELECT idSolarAngle, valueZenithAngle, valueAzimuthAngle FROM SolarAngle ; ", into(angle.idSolarAngle), into(
						angle.valueZenithAngle), into(angle.valueAzimuthAngle));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addAngleSolaire(angle);
			}
		}

		if (lengtthViewAngles > 0) {
			ObjectDatabase::ViewAngle angle;
			statement st = (sql.prepare << "SELECT idViewAngle, valueZenithAngle, valueAzimuthAngle FROM ViewAngle ; ", into(angle.idViewAngle), into(
				angle.valueZenithAngle), into(angle.valueAzimuthAngle));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addAngleVisee(angle);
			}
		}

		if (lengthProducts > 0) {
			ObjectDatabase::Product produit;
			statement st = (sql.prepare << "SELECT idProduct, nameProduct, numberIteration FROM Product ; ", into(produit.idProduct), into(
				produit.nameProduct), into(produit.numberIteration));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addProduit(produit);
			}
		}

		if (lengthSimul > 0) {
			ObjectDatabase::Simulation sim;
			statement st = (sql.prepare << "SELECT idSimulation, nameSimulation FROM Simulation ; ", into(sim.idSimulation), into(sim.nameSimulation));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addSimulation(sim);
			}
		}
		sql.close();

	} catch (const soci_error & se) {
		cerr << "LUT Error: when try to loading objects from database: " << _cheminVersFichierBDD << endl;
		sql.close();
		throw se;
	}
}

bool BaseDeDonnees::hasAtmosphericTOAtoStore(const bool &isOrdre1) const {
	return isOrdre1 ? !(BaseDeDonnees::_luminancesAtmosphereTOAOrdre1.empty() && BaseDeDonnees::_reflectancesAtmosphereTOAOrdre1.empty()) : !(BaseDeDonnees::_luminancesAtmosphereTOA.empty()
			&& BaseDeDonnees::_reflectancesAtmosphereTOA.empty());
}

//pair<numBande,numDirection>
void BaseDeDonnees::enregistrerProduitAtmosphereTOA(session &sql,
													const int &spectralBandNumber,
													const int &idSimulation,
													const int &idSpectralBand,
													const int &idSpectralMode,
													const int &idSunAngle,
													const int &idCombinaison,
													const bool &isOrdre1) {
	int idProduit = getIdProduitFromValues(sql, isOrdre1 ? NOM_PARAMETRE_ATMOSPHERE_TOA_ORDRE1 : NOM_PARAMETRE_ATMOSPHERE_TOA, 0);
	int idResultatScalaire = getIdResultatScalaireFromValues(sql, idProduit, idSimulation, idSpectralBand, idSpectralMode, idSunAngle, idCombinaison);

	map<pair<int, pair<double, double> >, double>::iterator itrRef;
	map<pair<int, pair<double, double> >, double>::iterator itrLum;

	if (isOrdre1) {
		for (itrRef = _reflectancesAtmosphereTOAOrdre1.begin(); itrRef != _reflectancesAtmosphereTOAOrdre1.end(); itrRef++) {
			if (itrRef->first.first == spectralBandNumber) {
				int idAngleVisee = getIdAngleViseeFromValues(sql, itrRef->first.second.first, itrRef->first.second.second);
				storeResultatFromValues(sql, idAngleVisee, idResultatScalaire, false, itrRef->second);
			}
		}

		for (itrLum = _luminancesAtmosphereTOAOrdre1.begin(); itrLum != _luminancesAtmosphereTOAOrdre1.end(); itrLum++) {
			if (itrLum->first.first == spectralBandNumber) {
				int idAngleVisee = getIdAngleViseeFromValues(sql, itrLum->first.second.first, itrLum->first.second.second);
				storeResultatFromValues(sql, idAngleVisee, idResultatScalaire, true, itrLum->second);
			}
		}
	} else {
		for (itrRef = _reflectancesAtmosphereTOA.begin(); itrRef != _reflectancesAtmosphereTOA.end(); itrRef++) {
			if (itrRef->first.first == spectralBandNumber) {
				int idAngleVisee = getIdAngleViseeFromValues(sql, itrRef->first.second.first, itrRef->first.second.second);
				storeResultatFromValues(sql, idAngleVisee, idResultatScalaire, false, itrRef->second);
			}
		}

		for (itrLum = _luminancesAtmosphereTOA.begin(); itrLum != _luminancesAtmosphereTOA.end(); itrLum++) {
			if (itrLum->first.first == spectralBandNumber) {
				int idAngleVisee = getIdAngleViseeFromValues(sql, itrLum->first.second.first, itrLum->first.second.second);
				storeResultatFromValues(sql, idAngleVisee, idResultatScalaire, true, itrLum->second);
			}
		}
	}
}

/*****************
 * Mappinge SOCI *
 *****************/

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::DirectionalResult> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::DirectionalResult & resultat) {
			resultat.idDirectionalResult = value.get<int>("idDirectionalResult");
			resultat.idScalarResult = value.get<int>("idScalarResult");
			resultat.idViewAngle = value.get<int>("idViewAngle");
		}

		static void to_base(const ObjectDatabase::DirectionalResult & resultat,
							values & value,
							indicator & ind) {
			value.set("idDirectionalResult", resultat.idDirectionalResult);
			value.set("idScalarResult", resultat.idScalarResult);
			value.set("idViewAngle", resultat.idViewAngle);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::ScalarResult> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::ScalarResult & resultatScalaire) {
			resultatScalaire.idScalarResult = value.get<int>("idScalarResult");
			resultatScalaire.idSpectralBand = value.get<int>("idSpectralBand");
			resultatScalaire.idSpectralMode = value.get<int>("idSpectralMode");
			resultatScalaire.idCombination = value.get<int>("idCombination", -1);
			resultatScalaire.idSolarAngle = value.get<int>("idSolarAngle");
			resultatScalaire.idSimulation = value.get<int>("idSimulation");
			resultatScalaire.idProduct = value.get<int>("idProduct");
		}

		static void to_base(const ObjectDatabase::ScalarResult & resultatScalaire,
							values & value,
							indicator & ind) {
			value.set("idScalarResult", resultatScalaire.idScalarResult);
			value.set("idSpectralBand", resultatScalaire.idSpectralBand);
			value.set("idSpectralMode", resultatScalaire.idSpectralMode);
			value.set("idCombination", resultatScalaire.idCombination, resultatScalaire.idCombination == -1 ? i_null : i_ok);
			value.set("idSolarAngle", resultatScalaire.idSolarAngle);
			value.set("idSimulation", resultatScalaire.idSimulation);
			value.set("idProduct", resultatScalaire.idProduct);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::SpectralBand> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::SpectralBand & spectralBand) {
			spectralBand.idSpectralBand = value.get<int>("idSpectralBand");
			spectralBand.valueCentralWavelength = value.get<double>("valueCentralWavelength");
			spectralBand.valueSpectralBandwidth = value.get<double>("valueSpectralBandwidth");
		}

		static void to_base(const ObjectDatabase::SpectralBand & spectralBand,
							values & value,
							indicator & ind) {
			value.set("idSpectralBand", spectralBand.idSpectralBand);
			value.set("valueCentralWavelength", spectralBand.valueCentralWavelength);
			value.set("valueSpectralBandwidth", spectralBand.valueSpectralBandwidth);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::SpectralMode> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::SpectralMode & spectralBand) {
			spectralBand.idSpectralMode = value.get<int>("idSpectralMode");
			spectralBand.nameSpectralMode = value.get<string>("nameSpectralMode");
		}

		static void to_base(const ObjectDatabase::SpectralMode & spectralBand,
							values & value,
							indicator & ind) {
			value.set("idSpectralMode", spectralBand.idSpectralMode);
			value.set("nameSpectralMode", spectralBand.nameSpectralMode);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::SolarAngle> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::SolarAngle & angle) {
			angle.idSolarAngle = value.get<int>("idSolarAngle");
			angle.valueAzimuthAngle = value.get<double>("valueAzimuthAngle");
			angle.valueZenithAngle = value.get<double>("valueZenithAngle");
		}

		static void to_base(const ObjectDatabase::SolarAngle & angle,
							values & value,
							indicator & ind) {
			value.set("idSolarAngle", angle.idSolarAngle);
			value.set("valueAzimuthAngle", angle.valueAzimuthAngle);
			value.set("valueZenithAngle", angle.valueZenithAngle);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::ViewAngle> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::ViewAngle & angle) {
			angle.idViewAngle = value.get<int>("idViewAngle");
			angle.valueAzimuthAngle = value.get<double>("valueAzimuthAngle");
			angle.valueZenithAngle = value.get<double>("valueZenithAngle");
		}

		static void to_base(const ObjectDatabase::ViewAngle & angle,
							values & value,
							indicator & ind) {
			value.set("idViewAngle", angle.idViewAngle);
			value.set("valueAzimuthAngle", angle.valueAzimuthAngle);
			value.set("valueZenithAngle", angle.valueZenithAngle);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::Product> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::Product & produit) {
			produit.idProduct = value.get<int>("idProduct");
			produit.nameProduct = value.get<string>("nameProduct");
			produit.numberIteration = value.get<int>("numberIteration");
		}

		static void to_base(const ObjectDatabase::Product & produit,
							values & value,
							indicator & ind) {
			value.set("idProduct", produit.idProduct);
			value.set("nameProduct", produit.nameProduct);
			value.set("numberIteration", produit.numberIteration);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::Simulation> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::Simulation & simulation) {
			simulation.idSimulation = value.get<int>("idSimulation");
			simulation.nameSimulation = value.get<string>("nameSimulation");
		}

		static void to_base(const ObjectDatabase::Simulation & simulation,
							values & value,
							indicator & ind) {
			value.set("idSimulation", simulation.idSimulation);
			value.set("nameSimulation", simulation.nameSimulation);
			ind = i_ok;
		}
	};
}
