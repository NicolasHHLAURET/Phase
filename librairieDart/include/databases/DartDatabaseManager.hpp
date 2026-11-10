/*
 * DartDatabaseManager.hpp
 *
 *  Created on: Jun 15, 2016
 *      Author: guilleuxj
 */

#ifndef INCLUDE_DARTDATABASEMANAGER_HPP_
#define INCLUDE_DARTDATABASEMANAGER_HPP_

#include <sstream>
#include <iostream>
#include <stdio.h>

#include <boost/thread.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

using namespace std;
using namespace soci;

class DartDatabaseManager {
private:
	static const string CONNEXION_ERREUR_VERROU;
	static const int CONNEXION_MAX_ESSAI; // nombre limite d'essai avant d'abandonner subjectif. L'idee est seulement d'eviter la boucle infinie en cas de probleme inattendu
	static const int CONNEXION_ATTENTE; // valeur d'attente empirique en ms (= magique. si trop basse on augmente la concurrence)

	static const int MAX_WARNING_MESSAGE; // anti spam

	static const double DBEPSILON; //

	int _nbDisplayedWarningMessages;

	string _completePathToDatabase;
	session _currentSession;
	bool _isDatabaseConnected;

public:

	DartDatabaseManager();
	DartDatabaseManager(const string & completePathToDatabase);
	virtual ~DartDatabaseManager();

	void reinit(const string & completePathToDatabase);

	bool connectDatabase();
	void deconnectDatabase();

	const string & getCompletePathToDatabase() const {
		return _completePathToDatabase;
	}

	template<typename TypeColumn>
	bool executeSingleColumnSingleValueReadingQuery(const string & query,
													TypeColumn & result) {
		if (_isDatabaseConnected) {

			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {

				try {
					_currentSession << query, into(result);
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try reducing the number of parallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << se.what() << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	template<typename TypeColumn>
	bool executeSingleColumnMultipleValueReadingQuery(	const string & query,
														vector<TypeColumn> & result) {
		if (_isDatabaseConnected) {
			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					result.clear();
					rowset<row> resultSet = (_currentSession.prepare << query);
					for (rowset<row>::const_iterator itResultSet = resultSet.begin(); itResultSet != resultSet.end(); ++itResultSet) {
						const row & rowResult = *itResultSet;
						result.push_back(rowResult.get<TypeColumn>(0));
					}
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading a vector of data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try to reducing number of parrallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	template<typename TypeColumn>
	bool executeMultipleColumnReadingQuery(	const string & query,
											vector<vector<TypeColumn> > & result,
											const int & nbColumn) {
		if (_isDatabaseConnected) {
			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					result.clear();
					rowset<row> resultSet = (_currentSession.prepare << query);
					for (rowset<row>::const_iterator itResultSet = resultSet.begin(); itResultSet != resultSet.end(); ++itResultSet) {
						const row & rowResult = *itResultSet;
						vector<TypeColumn> line;
						for (int numColumn = 0; numColumn < nbColumn; ++numColumn) {
							line.push_back(rowResult.get<TypeColumn>(numColumn));
						}
						result.push_back(line);
					}
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	/**
	 * Récupère l'id de la bande spectrale se rapprochant de la wavelength la plus proche
	 * @param tableName nom de la table contenant les données
	 * @param wavelength valeur de la wavelength
	 * @param getUpperWavelength true si on veut l'id de la bande spectrale correspondant à la wavelength directement supérieure, false si on veut celle directement inférieure
	 * @param isEqualValid true si on accepte une valeur strictement identique
	 * @param spectralBandId retour de l'id de la bande spectrale correspondante
	 * @return true si la requête s'est déroulée avec succès.
	 */
	bool getMaxSpectralBandId(	const string & tableName,
								int & spectralBandId) {
		if (_isDatabaseConnected) {
			string query = "";
			query = "SELECT  Id  FROM " + tableName + " ORDER BY wavelength DESC LIMIT 1";
			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					_currentSession << query, into(spectralBandId);
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	bool getSpectralBandId(	const string & tableName,
							const double & wavelength,
							const bool & getUpperWavelength,
							const bool & isEqualValid,
							int & spectralBandId) {
		if (_isDatabaseConnected) {
			string query = "";

			if (getUpperWavelength) {
				if (isEqualValid) {
					query = "SELECT  Id  FROM " + tableName + " WHERE wavelength >= :upperLambda ORDER BY wavelength LIMIT 1";
				} else {
					query = "SELECT  Id  FROM " + tableName + " WHERE wavelength > :upperLambda ORDER BY wavelength LIMIT 1";
				}
			} else {
				if (isEqualValid) {
					query = "SELECT  Id  FROM " + tableName + " WHERE wavelength <= :lowerLambda ORDER BY wavelength DESC LIMIT 1";
				} else {
					query = "SELECT  Id  FROM " + tableName + " WHERE wavelength < :lowerLambda ORDER BY wavelength DESC LIMIT 1";
				}
			}

			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					_currentSession << query, use(wavelength), into(spectralBandId);
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	/**
	 * On développe le cas où les données en base ne sont pas contigues, on se base sur la wavelength au lieu de l'id de la bande.
	 * @param tableName nom de la table contenant les données
	 * @param wavelength valeur de la wavelength
	 * @param getUpperWavelength true si on veut l'id de la bande spectrale correspondant à la wavelength directement supérieure, false si on veut celle directement inférieure
	 * @param isEqualValid true si on accepte une valeur strictement identique
	 * @param nearWavelength retour la wavelength en base la plus proche selon les critères utilisés.
	 * @return true si la requête s'est déroulée avec succès.
	 */
	bool getNearWavelengthPerDirection(	const string & tableName,
										const double & wavelength,
										const bool & getUpperWavelength,
										const bool & isEqualValid,
										double & nearWavelength,
										const int & idAngle) {
		if (_isDatabaseConnected) {
			string query = "";
			stringstream ss;
			ss << idAngle;

			if (getUpperWavelength) {
				if (isEqualValid) {
					query = "SELECT  wavelength  FROM " + tableName + " WHERE wavelength >= :upperLambda AND idAngle=" + ss.str()
							+ " ORDER BY wavelength LIMIT 1";
				} else {
					query = "SELECT  wavelength  FROM " + tableName + " WHERE wavelength > :upperLambda  AND idAngle=" + ss.str()
							+ " ORDER BY wavelength LIMIT 1";
				}
			} else {
				if (isEqualValid) {
					query = "SELECT  wavelength  FROM " + tableName + " WHERE wavelength <= :lowerLambda  AND idAngle=" + ss.str()
							+ " ORDER BY wavelength DESC LIMIT 1";
				} else {
					query = "SELECT  wavelength  FROM " + tableName + " WHERE wavelength < :lowerLambda  AND idAngle=" + ss.str()
							+ " ORDER BY wavelength DESC LIMIT 1";
				}
			}

			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					_currentSession << query, use(wavelength), into(nearWavelength);
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	/**
	 * On développe le cas où les données en base ne sont pas contigues, on se base sur la wavelength au lieu de l'id de la bande.
	 * @param tableName nom de la table contenant les données
	 * @param wavelength valeur de la wavelength
	 * @param getUpperWavelength true si on veut l'id de la bande spectrale correspondant à la wavelength directement supérieure, false si on veut celle directement inférieure
	 * @param isEqualValid true si on accepte une valeur strictement identique
	 * @param nearWavelength retour la wavelength en base la plus proche selon les critères utilisés.
	 * @return true si la requête s'est déroulée avec succès.
	 */
	bool getNumberOfSpectralValuesPerDirection(	const string & tableName,
												const double & wavelengthMin,
												const double & wavelengthMax,
												unsigned int & numberOfSpectralValues,
												const int & idAngle) {
		if (_isDatabaseConnected) {
			string query = "";
			stringstream ss;
			ss << idAngle;

			query = "SELECT  count(*)  FROM " + tableName + " WHERE wavelength BETWEEN :lowerLambda AND :upperLambda AND idAngle=" + ss.str() + " ";

			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					const double lowerLambda = wavelengthMin - DBEPSILON;
					const double upperLambda = wavelengthMax + DBEPSILON;
					_currentSession << query, use(lowerLambda), use(upperLambda), into(numberOfSpectralValues);
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	/**
	 * On est encore plus déguelasse qu'avant, aller c'est un peu générique là :P
	 * Normalement les données sont censées avoir des id ...
	 */
	template<typename TypeColumn>
	bool readSingleColumnBetweenWavelength(	const string & tableName,
											const string & columnName,
											const double & wavelengthMin,
											const double & wavelengthMax,
											const int & idAngle,
											vector<TypeColumn> & result) {
		if (_isDatabaseConnected) {
			string query = "";
			stringstream ss;
			ss << idAngle;
			query = "SELECT " + columnName + " FROM " + tableName + " WHERE wavelength BETWEEN :lowerLambda AND :upperLambda AND idAngle=" + ss.str()
					+ " ";
			for (int nbEssai = 0; nbEssai < CONNEXION_MAX_ESSAI; ++nbEssai) {
				try {
					result.clear();
				const double lowerLambda = wavelengthMin - DBEPSILON;
				const double upperLambda = wavelengthMax + DBEPSILON;
				rowset<row> resultSet = (_currentSession.prepare << query, use(lowerLambda), use(upperLambda));
					for (rowset<row>::const_iterator itResultSet = resultSet.begin(); itResultSet != resultSet.end(); ++itResultSet) {
						const row & rowResult = *itResultSet;
						result.push_back(rowResult.get<TypeColumn>(0));
					}
					return true;
				} catch (const soci_error & se) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not read in, try to reducing number of parrallel threads" << endl;
						}
						break;
					} else if (boost::ends_with(se.what(), CONNEXION_ERREUR_VERROU)) {
						boost::this_thread::sleep(boost::posix_time::milliseconds(CONNEXION_ATTENTE));
					} else {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[Error] Unknow error when try to reading multiple data, Is this database exists or is valid : "
									<< _completePathToDatabase << " ? " << endl;
							cerr << se.what() << endl;
							cerr << "Last database query: " << query << endl;
							exit(-69);
						}
						break;
					}
				} catch (const std::exception & bc) {
					if (nbEssai > CONNEXION_MAX_ESSAI) {
						if (_nbDisplayedWarningMessages < MAX_WARNING_MESSAGE) {
							_nbDisplayedWarningMessages++;
							cerr << endl << "[ERROR] Phase failure: database (" << _completePathToDatabase
									<< ") is locked and phase can not be read correctly, try reducing the number of parallel threads" << endl;
							cerr << bc.what() << endl;
						}
						break;
					}
				}
			}
		}
		return false;
	}

	bool executePragmaInfo(	const string & tableName,
							vector<string> & resultColumnsName);
};

#endif /* INCLUDE_DARTDATABASEMANAGER_HPP_ */
