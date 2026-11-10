/*
 * DartDatabaseManager.cpp
 *
 *  Created on: Jun 15, 2016
 *      Author: guilleuxj
 */

#include "../include/databases/DartDatabaseManager.hpp"

#include <string>
/*****************************
 * Objets static & constants *
 *****************************/

const int DartDatabaseManager::CONNEXION_MAX_ESSAI = 20; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
const int DartDatabaseManager::CONNEXION_ATTENTE = 100; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)
const string DartDatabaseManager::CONNEXION_ERREUR_VERROU = "database is locked";
const int DartDatabaseManager::MAX_WARNING_MESSAGE = 10; // anti spam
const double DartDatabaseManager::DBEPSILON = 0.000001; //

/**
 * Gestionnaire de bases de données manipulées par phase.
 * Supporte uniquement les opérations de lecture sur une base de données pouvant être "temporairement" verouillée (coucou Prospect).
 * Toute execution de requête ne passant pas par les méthodes executeXXX définies ici peut se faire jeter en cas d'accès concurent avec un écrivain.
 *
 * template<typename TypeColumn> supporte les types primitifs+string des colonnes (pour retourner qu'un résultat), ainsi que les vectors
 * de type UNIQUEMENT primitifs pour retourner plusieurs résultats (string N'EST PAS un type primitif, pour le bureau des pleurs c'est chez SOCI)
 *
 * executeSingleColumnReadingQuery() supporte les requetes sur UNE SEULE colonne (+rapide)
 * executeMultipliColumnReadingQuery() supporte les requetes sur PLUSIEURS colonne qui doivent avoir le même TypeColumn
 */
DartDatabaseManager::DartDatabaseManager(const string & completePathToDatabase) {
	_nbDisplayedWarningMessages = 0;
	_completePathToDatabase = completePathToDatabase;
	_isDatabaseConnected = false;
}

void DartDatabaseManager::reinit(const string & completePathToDatabase) {
	_nbDisplayedWarningMessages = 0;
	_completePathToDatabase = completePathToDatabase;
	_isDatabaseConnected = false;
}

DartDatabaseManager::DartDatabaseManager(){
	_nbDisplayedWarningMessages = 0;
	_completePathToDatabase = "";
	_isDatabaseConnected = false;
}

/***
 * /!\ DON'T FORGET TO sociSession.close() AT END OF USING DATABASE /!\
 * @param sociSession : empty session, contain key of session if connect sucessfull
 * @param completePathToDatabase : complete path to connected database
 * @return true if connect successfull, false + cerr trace else.
 */
bool DartDatabaseManager::connectDatabase() {
	int nbEssai = 0;
	if (!_isDatabaseConnected) {
		do {
			try {
				_currentSession.open(*factory_sqlite3(), _completePathToDatabase);
				_currentSession << "PRAGMA journal_mode = OFF";
				_currentSession << "PRAGMA synchronous = OFF";
				_currentSession << "PRAGMA temp_store = MEMORY";

				_isDatabaseConnected = true;
			} catch (const soci_error & se) {
				if (nbEssai > CONNEXION_MAX_ESSAI) {
					if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
						_nbDisplayedWarningMessages++;
						cerr << endl << "[Warning] Database access concurrency saturation: try reducing the number of simulation launched in parallel"
								<< endl;
					}
					break;
				} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
					++nbEssai;
					boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
				} else if (boost::ends_with(se.what(), "Cannot open already connected session.")) {
					_currentSession.close();
					++nbEssai;
					boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
				} else {
					if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
						_nbDisplayedWarningMessages++;
						cerr << endl << "[Warning] Unknow error when try to connect database, Is this database exists or is valid : "
								<< _completePathToDatabase << " ? " << endl;
					}
					break;
				}
			}
		} while (!_isDatabaseConnected);
	}
	return _isDatabaseConnected;
}

void DartDatabaseManager::deconnectDatabase() {
	if (_isDatabaseConnected) {
		_currentSession.close();
		_isDatabaseConnected = false;
	}
}

DartDatabaseManager::~DartDatabaseManager() {
	// TODO Auto-generated destructor stub
}

bool DartDatabaseManager::executePragmaInfo(	const string & tableName,
												vector<string> & resultColumnsName) {
	if (_isDatabaseConnected) {
		for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {

			try {
				string query = "PRAGMA table_info(" + tableName + ");";
//				cout<<"query in executeProgramaInfo: "<<query<<endl;
				resultColumnsName.clear();
				rowset<row> resultSet = _currentSession.prepare << query;
				for (rowset<row>::const_iterator itResultSet = resultSet.begin(); itResultSet != resultSet.end(); ++itResultSet) {
					const row & rowResult = *itResultSet;
					resultColumnsName.push_back(rowResult.get<string>(1));
//					cout<<"rowResult.get<string>(1): "<<rowResult.get<string>(1)<<endl;
				}
				return true;
			} catch (const soci_error & se) {

				if (nbEssai > CONNEXION_MAX_ESSAI) {
					if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
						_nbDisplayedWarningMessages++;
						cerr << endl << "[Warning] Database access concurrency saturation: try reducing the number of simulation launched in parallel"
								<< endl;
					}
					break;
				} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
					boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
				} else {
					if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
						_nbDisplayedWarningMessages++;
						cerr << endl << "[Error] Unknow error when try to reading data in SQLITE PRAGMA, Is this database exists or is valid : "
								<< _completePathToDatabase << " ? " << endl;
					}
					break;
				}
			}
		}
	}
	return false;
}

