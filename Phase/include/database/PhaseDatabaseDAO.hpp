/*
 * PhaseDatabaseDAO.hpp
 *
 *  Created on: 5 oct. 2015
 *      Author: guilleuxj
 */

#ifndef PHASEDATABASEDAO_HPP_
#define PHASEDATABASEDAO_HPP_

#include "model/PhaseDatabaseParameters.hpp"

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

class PhaseDatabaseDAO {
private:
	/**
	 * Attention, certains inversent le min et le max
	 */
	static const std::string buildReadingQueryPerBand(	const std::string & columnChoice,
														const std::string & tableName,
														const std::string & betweenIdMin,
														const std::string & betweenIdMax,
														const bool & isDesc) {

		std::ostringstream oss117_querry;
		oss117_querry.str("");

		oss117_querry << "SELECT " << columnChoice << " FROM " << tableName << " WHERE Id " << " BETWEEN " << betweenIdMin << " AND " << betweenIdMax
				<< " ORDER BY Id ";
		if (isDesc) {
			oss117_querry << " DESC ";
		}

		return oss117_querry.str();
	}

	/**
	 * Attention, certains inversent le min et le max
	 * On redeveloppe la lecture pour phaseExtern, youpi
	 */
	static const std::string buildReadingQueryPerBandPerDirection(	const std::string & columnChoice,
																	const std::string & tableName,
																	const std::string & wavelengthMin,
																	const std::string & wavelengthMax,
																	const bool & isDesc,
																	const int & idAngle) {

		std::ostringstream oss117_querry;
		oss117_querry.str("");

		oss117_querry << "SELECT " << columnChoice << " FROM " << tableName << " WHERE wavelength " << " BETWEEN " << wavelengthMin << " AND "
				<< wavelengthMax << " AND idAngle=" << idAngle << " ORDER BY wavelength ";
		if (isDesc) {
			oss117_querry << " DESC ";
		}

		return oss117_querry.str();
	}

	static const std::string buildReadingQuery(	const std::string & columnChoice,
												const std::string & tableName) {

		std::ostringstream oss117_querry;
		oss117_querry.str("");
		oss117_querry << "SELECT " << columnChoice << " FROM " << tableName;

		return oss117_querry.str();
	}

public:
	/**
	 * L'irradiance n'est pas codee comme le reste de la base de donnees atmospherique ...
	 */
	static void readIrradianceAtmosphericParametersPerBand(	DartDatabaseManager & databaseManager,
															const std::string & solarIrradianceTableName,
															const std::string & irradianceColumnName,
															const UserSpectralBand & userSpectralBand,
															std::vector<double> & resultWavelength,
															std::vector<double> & resultColumnIrradiance) {

		std::string completeTableName, spectralBandIdMin, spectralBandIdMax, query;

		completeTableName = solarIrradianceTableName /*+ irradianceExtension*/;
		getStringIdSpectralBand(databaseManager, completeTableName, userSpectralBand, spectralBandIdMin, spectralBandIdMax);

		query = buildReadingQueryPerBand(" wavelength ", completeTableName, spectralBandIdMax, spectralBandIdMin, true);

		databaseManager.executeSingleColumnMultipleValueReadingQuery(query, resultWavelength);

		query = buildReadingQueryPerBand(irradianceColumnName, completeTableName, spectralBandIdMax, spectralBandIdMin, true);

		databaseManager.executeSingleColumnMultipleValueReadingQuery(query, resultColumnIrradiance);
	}

	static void readAtmosphericParametersPerBand(	DartDatabaseManager & databaseManager,
													const std::string & tableName,
													const AtmosphericTableType & tableStructure,
													const UserSpectralBand & userSpectralBand,
													std::vector<std::vector<double> > & result) {
		std::string completeTableName, spectralBandIdMin, spectralBandIdMax, query;

		completeTableName = tableName + tableStructure.getTableExtension();

//		cout<<"completeTableName: "<<completeTableName<<endl;

		getStringIdSpectralBand(databaseManager, completeTableName, userSpectralBand, spectralBandIdMin, spectralBandIdMax);

		// Les ids dans la base de donnees atmosphere sont inverses, si le fix de nicolas dans
		// getStringIdSpectralBand s'amuse a me les remettre dans l'ordre alors je dois les
		// remettre dans le desordre.
		int idMin = std::stoi(spectralBandIdMin);
		int idMax = std::stoi(spectralBandIdMax);
		if (idMin < idMax) {
			std::string temp = spectralBandIdMax;
			spectralBandIdMax = spectralBandIdMin;
			spectralBandIdMin = temp;
		}

//		cout<<"tableStructure.getCollumnChoice(): "<<tableStructure.getCollumnChoice()<<endl;
//		cout<<"tableStructure.getCollumnNumber(): "<<tableStructure.getCollumnNumber()<<endl;
//		cout<<"tableStructure.getTableExtension(): "<<tableStructure.getTableExtension()<<endl;

		query = buildReadingQueryPerBand(tableStructure.getCollumnChoice(), completeTableName, spectralBandIdMax, spectralBandIdMin, true);

//		cout<<"getCollumnNumber(): "<<tableStructure.getCollumnNumber()<<endl;

		databaseManager.executeMultipleColumnReadingQuery(query, result, tableStructure.getCollumnNumber());
	}

	static void readAtmosphericParameters(	DartDatabaseManager & databaseManager,
											const std::string & tableName,
											const AtmosphericTableType & tableStructure,
											std::vector<std::vector<double> > & result) {

		std::string completeTableName, query;

		completeTableName = tableName + tableStructure.getTableExtension();

		query = buildReadingQuery(tableStructure.getCollumnChoice(), completeTableName);

		databaseManager.executeMultipleColumnReadingQuery(query, result, tableStructure.getCollumnNumber());
	}

	/**
	 * La difference avec la methode readAtmosphericParametersPerBand ? spectralBandIdMin et spectralBandIdMax sont dans l'ordre :3
	 */
	static void readHgParametersPerBand(DartDatabaseManager & databaseManager,
										const std::string & tableName,
										const AtmosphericTableType & tableStructure,
										const UserSpectralBand & userSpectralBand,
										std::vector<std::vector<double> > & result) {

		std::string completeTableName, spectralBandIdMin, spectralBandIdMax, query;

		completeTableName = tableName + tableStructure.getTableExtension();

		getStringIdSpectralBand(databaseManager, completeTableName, userSpectralBand, spectralBandIdMin, spectralBandIdMax);

		query = buildReadingQueryPerBand(tableStructure.getCollumnChoice(), completeTableName, spectralBandIdMin, spectralBandIdMax, false);

		databaseManager.executeMultipleColumnReadingQuery(query, result, tableStructure.getCollumnNumber());
	}

	/**
	 * Lis les reflectances du modele bottom en base de donnees, et reecrit au bon endroit sur le vecteur de resultats les valeurs pour le modele bottom (dans le cas où il n'y a pas de bottom, alors top = bottom)
	 * Les reflectances du modele TOP doivent etre lues AVANT.
	 */
	static void readBottomReflectanceParametersPerBand(	DartDatabaseManager & databaseManager,
														const unsigned int & spectralBandIndex,
														const std::string & modelName,
														const std::vector<std::string> & reflectanceParametersColumnChoice,
														const std::string & databasePath,
														const std::vector<int> & overridePos,
														const std::vector<SolarIrradianceValues> & solarIrradianceValues,
														std::vector<std::vector<double> > & results,
														unsigned int & numberOfSpectralValues,
														const double & lowerLambdaString,
														const double & upperLambdaString) {
		std::vector<std::vector<double> > bottomResults;

		readReflectanceParametersPerBand(databaseManager, spectralBandIndex, modelName, reflectanceParametersColumnChoice, databasePath,
			solarIrradianceValues, bottomResults, numberOfSpectralValues, lowerLambdaString, upperLambdaString);

		unsigned int topIterator = 0;
		unsigned int bottomIterator = 0;

		// En gros, on a le vecteur topResult qui contient les valeurs top et les valeurs bottom duplique a partir du top.
		// Donc, si top = bottom alors il y a besoin de rien faire, sinon on reecrit les valeurs bottom dans le vecteur top au bon endoit.
		// MAIS il se peut les intervalles spectraux soient differents entre top et bottom, dans ce cas on interpole, c'est l'algo ci dessous :)
		// On va parcourir les deux tableaux, puis en fonction de l'intervalle spectral :
		// Si wavelength top = bottom alors on reecrit simplement les valeurs du vecteur bottom dans le vecteur top.
		// Si wavelength top < bottom alors top est en "retard", on peut simplement interpoler les valeurs de bottom.
		// 		Si on est au debut de bottom -> on ne peut pas interpoler,
		// 			on admet donc que la valeur est constante et egale a la premiere valeur disponible
		//		Sinon on interpole les valeurs de bottom en fonction de bottom-1 et bottom +0
		// Si wavelength top > bottom alors top est en "avance"
		//		On doit creer une nouvelle entree dans top qui correspond a l'intervalle bottom (vecteur newLine)
		//		Et on interpole les valeurs de top dans ce vecteur en fonction de top-1 et top +0
		//		Si on est au debut de top, on ne peut pas interpoler, et on prend la valeur de top+0 comme constante.
		while (topIterator < results.at(0).size() && bottomIterator < bottomResults.at(0).size()) {
			if (results[0][topIterator] == bottomResults[0][bottomIterator]) {
				// La wavelength top = bottom, on reecrit simplement les valeurs bottom dans le top
				for (unsigned int i = 1; i < bottomResults.size(); ++i) {
					results[overridePos[i]][topIterator] = bottomResults[i][bottomIterator];
				}

				// La condition du while fait qu'au moins un des deux iterator avance
				if (topIterator < results.at(0).size()) {
					topIterator++;
				}
				if (bottomIterator < bottomResults.at(0).size()) {
					bottomIterator++;
				}
			} else {
				if (results[0][topIterator] < bottomResults[0][bottomIterator]) {
					// wavelength top < bottom
					if (bottomIterator == 0) {
						for (unsigned int i = 1; i < bottomResults.size(); ++i) {
							results[overridePos[i]][topIterator] = bottomResults[i][bottomIterator];
						}
					} else {
						for (unsigned int i = 1; i < bottomResults.size(); ++i) {
							double borneMin = bottomResults[0][bottomIterator - 1];
							double borneMax = bottomResults[0][bottomIterator];

							double valMin = bottomResults[i][bottomIterator - 1];
							double valMax = bottomResults[i][bottomIterator];

							double borneInt = results[0][topIterator];

							double interpolatedValue = Interpolation_lineaire(borneMin, borneMax, borneInt, valMin, valMax);

							results[overridePos[i]][topIterator] = interpolatedValue;
						}
					}

					if (topIterator < results.at(0).size()) {
						topIterator++;
					} else {
						break; // Cas impossible, mais on evite l'infinite loop
					}
				} else {
					// wavelength top > bottom
					std::vector<double> newLine;
					for (unsigned int i = 0; i < results.size(); ++i) {
						if (i == 0) {
							newLine.push_back(bottomResults[i][bottomIterator]);
						} else {
							if (topIterator == 0) {
								newLine.push_back(results[i][topIterator]);
							} else {
								double borneMin = results[0][topIterator - 1];
								double borneMax = results[0][topIterator];

								double valMin = results[i][topIterator - 1];
								double valMax = results[i][topIterator];

								double borneInt = bottomResults[0][bottomIterator];

								double interpolatedValue = Interpolation_lineaire(borneMin, borneMax, borneInt, valMin, valMax);

								newLine.push_back(interpolatedValue);
							}
						}
					}

					for (unsigned int i = 1; i < bottomResults.size(); ++i) {
						newLine[overridePos[i]] = bottomResults[i][bottomIterator];
					}

					for (unsigned int i = 0; i < newLine.size(); ++i) {
						results.at(i).insert(results.at(i).begin() + topIterator, newLine.at(i));
					}

					if (bottomIterator < bottomResults.at(0).size()) {
						bottomIterator++;
					} else {
						break; // Cas impossible, mais on evite l'infinite loop
					}
				}
			}
		}
		if (results[0][results.at(0).size() - 1] > bottomResults[0][bottomResults.at(0).size() - 1]) {
			for (unsigned int i = 1; i < bottomResults.size(); ++i) {
				results[overridePos[i]][results.at(0).size() - 1] = bottomResults[i][bottomResults.at(0).size() - 1];
			}
		}
	}

	static void readDirectionnalReflectanceParametersPerBand(	DartDatabaseManager & databaseManager,
																const unsigned int & spectralBandIndex,
																const std::string & modelName,
																const std::vector<std::string> & reflectanceParametersColumnChoice,
																const std::string & databasePath,
																const std::vector<SolarIrradianceValues> & solarIrradianceValues,
																std::vector<std::vector<double> > & results,
																unsigned int & numberOfSpectralValues,
																const double & lowerLambdaString,
																const double & upperLambdaString,
																const int & idAngle) {
		double wavelengthMin = -1.0;
		double wavelengthMax = -1.0;
		numberOfSpectralValues = 0;

		databaseManager.getNearWavelengthPerDirection(modelName, upperLambdaString, true, false, wavelengthMax, idAngle);
		databaseManager.getNearWavelengthPerDirection(modelName, lowerLambdaString, false, false, wavelengthMin, idAngle);
		if (wavelengthMax < wavelengthMin || wavelengthMax < 0) {
			databaseManager.getNearWavelengthPerDirection(modelName, upperLambdaString, false, true, wavelengthMax, idAngle);
		}
		if (wavelengthMin < 0) {
			databaseManager.getNearWavelengthPerDirection(modelName, lowerLambdaString, true, true, wavelengthMin, idAngle);
		}
		databaseManager.getNumberOfSpectralValuesPerDirection(modelName, wavelengthMin, wavelengthMax, numberOfSpectralValues, idAngle);

		results.clear();
		for (unsigned int nbColumnChoice = 0; nbColumnChoice < reflectanceParametersColumnChoice.size(); ++nbColumnChoice) {
			std::vector<double> choiceResults;
			stringstream ss;
			ss << idAngle;
			if (databaseManager.readSingleColumnBetweenWavelength(modelName, reflectanceParametersColumnChoice[nbColumnChoice], wavelengthMin,
				wavelengthMax, idAngle, choiceResults)) {
				results.push_back(choiceResults);
			}
		}
	}

	static void readReflectanceParametersPerBand(	DartDatabaseManager & databaseManager,
													const unsigned int & spectralBandIndex,
													const std::string & modelName,
													const std::vector<std::string> & reflectanceParametersColumnChoice,
													const std::string & databasePath,
													const std::vector<SolarIrradianceValues> & solarIrradianceValues,
													std::vector<std::vector<double> > & results,
													unsigned int & numberOfSpectralValues,
													const double & lowerLambdaString,
													const double & upperLambdaString) {
		int min = 1, max = -1;
		numberOfSpectralValues = 0;
		std::string stMin, stMax, reflectanceQuery;

		databaseManager.getSpectralBandId(modelName, upperLambdaString, true, false, max);
		databaseManager.getSpectralBandId(modelName, lowerLambdaString, false, false, min);

		// Faut vraiment recoder la logique .... ça a ete partiellement fait avec phase extern ... un jour
		if (max < 0) {
			databaseManager.getMaxSpectralBandId(modelName, max);
		}
		if (max < min || max < 0) {
			databaseManager.getSpectralBandId(modelName, upperLambdaString, false, false, max);
		}

		if (min > max) {
			min = 1;
		}

		numberOfSpectralValues = max - min + 1;
		std::ostringstream ossMin;
		ossMin << min;
		stMin = ossMin.str();
		std::ostringstream ossMax;
		ossMax << max;
		stMax = ossMax.str();

		results.clear();
		for (unsigned int nbColumnChoice = 0; nbColumnChoice < reflectanceParametersColumnChoice.size(); ++nbColumnChoice) {
			std::vector<double> choiceResults;
			reflectanceQuery = buildReadingQueryPerBand(reflectanceParametersColumnChoice[nbColumnChoice], modelName, stMin, stMax, false);

			if (numberOfSpectralValues == 1) {
				reflectanceQuery = " SELECT " + reflectanceParametersColumnChoice[nbColumnChoice] + " FROM " + modelName + " WHERE Id = " + stMin;
			}
			databaseManager.executeSingleColumnMultipleValueReadingQuery(reflectanceQuery, choiceResults);
			results.push_back(choiceResults);
		}
	}

	static void readTemperatureParameters(const shared_ptr<PhaseDatabaseParameters> & modelProperties) {

		std::string completeNameDatabase = modelProperties.get()->getDatabasePath() + modelProperties.get()->getDatabaseName();
		DartDatabaseManager databaseManager(completeNameDatabase);

		std::string query = "", stMin = "", stMax = "";
		double wavelengthForTemperature = 0.55;
		double directTransmittanceBeforeBandWavelength = 0;
		double directTransmittanceAfterBandWavelength = 0;
		double lowerLimitMultiplicativeCoefficient = 0;
		double upperLimitMultiplicativeCoefficient = 0;
		double intervalBetweenSuccesiveWavelength = 0;
		std::vector<double> directTransmittanceForTemperature(1);
		directTransmittanceForTemperature[0] = 0;
		double directTransmittanceAfter = 0;
		double directTransmittanceBefore = 0;
		int min = 1, max = 1;
		std::ostringstream ossMin, ossMax;

		if (databaseManager.connectDatabase()) {
			databaseManager.getSpectralBandId(modelProperties.get()->getDatabaseModel(), wavelengthForTemperature, true, true, max);
			databaseManager.getSpectralBandId(modelProperties.get()->getDatabaseModel(), wavelengthForTemperature, false, true, min);

			if (max - min == 1) {
				ossMin << min;
				stMin = ossMin.str();
				ossMax << max;
				stMax = ossMax.str();

				std::vector<double> resultWavelength;
				query = buildReadingQueryPerBand(modelProperties.get()->getColumnChoices().at(0), modelProperties.get()->getDatabaseModel(), stMin,
					stMax, false);
				databaseManager.executeSingleColumnMultipleValueReadingQuery(query, resultWavelength);

				std::vector<double> resultTemperature;
				query = buildReadingQueryPerBand(modelProperties.get()->getColumnChoices().at(1), modelProperties.get()->getDatabaseModel(), stMin,
					stMax, false);
				databaseManager.executeSingleColumnMultipleValueReadingQuery(query, resultTemperature);

				directTransmittanceBeforeBandWavelength = resultWavelength.at(0);
				directTransmittanceBefore = resultTemperature.at(0);

				directTransmittanceAfterBandWavelength = resultWavelength.at(1);
				directTransmittanceAfter = resultTemperature.at(1);

				lowerLimitMultiplicativeCoefficient = wavelengthForTemperature - directTransmittanceBeforeBandWavelength;
				upperLimitMultiplicativeCoefficient = directTransmittanceAfterBandWavelength - wavelengthForTemperature;
				intervalBetweenSuccesiveWavelength = directTransmittanceAfterBandWavelength - directTransmittanceBeforeBandWavelength;
				directTransmittanceForTemperature[0] = directTransmittanceBefore * upperLimitMultiplicativeCoefficient
						+ directTransmittanceAfter * lowerLimitMultiplicativeCoefficient;
				if (intervalBetweenSuccesiveWavelength != 0) {
					directTransmittanceForTemperature[0] /= intervalBetweenSuccesiveWavelength;
				}
			} else {
				min += 1;
				std::ostringstream ossMin;
				ossMin << min;
				stMin = ossMin.str();
				query = "SELECT " + modelProperties.get()->getColumnChoices().at(1) + " FROM " + modelProperties.get()->getDatabaseModel()
						+ " WHERE Id = " + stMin;
				databaseManager.executeSingleColumnSingleValueReadingQuery(query, directTransmittanceForTemperature[0]);
			}
			databaseManager.deconnectDatabase();
		}
		directTransmittanceForTemperature[0] = directTransmittanceForTemperature[0] * 0.01;
		modelProperties->setParametersValuesForBand(0, directTransmittanceForTemperature);
	}

	static void getStringIdSpectralBand(DartDatabaseManager & databaseManager,
										const std::string & tableName,
										const UserSpectralBand & userSpectralBand,
										std::string & returnedSpectralBandIdMin,
										std::string & returnedSpectralBandIdMax) {
		int spectralBandIdMin = 0, spectralBandIdMax = 0;
		std::ostringstream ossMin, ossMax;

		getIdSpectralBand(databaseManager, tableName, userSpectralBand, spectralBandIdMin, spectralBandIdMax);

		// Fix_a_l'arrache_dans_le_code += 1
		// process phase has ended and returned an error: Overflow error [Fix_a_l'arrache_dans_le_code]
		if (spectralBandIdMin < 1) {
			getMaxIdSpectralBandIfEndingOfBeginingOfTable(databaseManager, tableName, userSpectralBand, spectralBandIdMin);
		}

		// Deux mois plus tard ...
		// Encore_Un_Fix_a_l'arrache_dans_le_code += 1
		if (spectralBandIdMax < 1) {
			getMaxIdSpectralBandIfEndingOfEndingOfTable(databaseManager, tableName, userSpectralBand, spectralBandIdMax);
		}

		// Dans deux mois, ici, il y aura un autre fix :3
		// On est ~5 mois plus tard et je suis la :D (En date du 07/02/2017 zoulou)
		// Bon c'etait un probleme de precision, donc le fix n'est pas ici, juste dans la methode en dessous.

		// Loupe, c'etait 3 mois plus tard! xD
		if (spectralBandIdMin == spectralBandIdMax) {
			if (spectralBandIdMin == 1) {
				spectralBandIdMax = 2;
			} else {
				--spectralBandIdMin;
			}
		}

		ossMin.str("");
		ossMin << spectralBandIdMin;
		returnedSpectralBandIdMin = ossMin.str();
		ossMax.str("");
		ossMax << spectralBandIdMax;
		returnedSpectralBandIdMax = ossMax.str();
	}

	static void getIdSpectralBand(	DartDatabaseManager & databaseManager,
									const std::string & tableName,
									const UserSpectralBand & userSpectralBand,
									int & returnedSpectralBandIdMin,
									int & returnedSpectralBandIdMax) {
		databaseManager.getSpectralBandId(tableName, userSpectralBand.upperLambdaValue, true, false, returnedSpectralBandIdMax);
		databaseManager.getSpectralBandId(tableName, userSpectralBand.lowerLambdaValue, false, false, returnedSpectralBandIdMin);
	}

	/**
	 * Attention, requete specifique a la base de donnees atmospherique :)
	 * Attention, risque de chute de gros rochers
	 *
	 * Requete a executer quand la bande spectrale de l'utilisateur se situe "avant" les bandes spectrales dans la base de donnees
	 */
	static void getMaxIdSpectralBandIfEndingOfBeginingOfTable(	DartDatabaseManager & databaseManager,
																const std::string & tableName,
																const UserSpectralBand & userSpectralBand,
																int & returnedSpectralBandIdMin) {
		databaseManager.getSpectralBandId(tableName, userSpectralBand.lowerLambdaValue, true, true, returnedSpectralBandIdMin);
	}

	/**
	 * Attention, requete specifique a la base de donnees atmospherique :)
	 * Attention, risque de chute de gros rochers
	 *
	 * Requete a executer quand la bande spectrale de l'utilisateur se situe "apres" les bandes spectrales dans la base de donnees
	 */
	static void getMaxIdSpectralBandIfEndingOfEndingOfTable(DartDatabaseManager & databaseManager,
															const std::string & tableName,
															const UserSpectralBand & userSpectralBand,
															int & returnedSpectralBandIdMax) {
		databaseManager.getSpectralBandId(tableName, userSpectralBand.upperLambdaValue, false, true, returnedSpectralBandIdMax);
	}

	/**
	 * Retourne la liste des directions pour un modele de phase externe.
	 */
	static void readPhaseExternAnglesForModel(	DartDatabaseManager & databaseManager,
												const std::string & tableName,
												vector<int> & anglesId,
												vector<double> & thetaView,
												vector<double> & phiView,
												vector<double> & thetaIncident,
												vector<double> & phiIncident) {
		string queryInt = " SELECT idAngle FROM " + tableName;
		databaseManager.executeSingleColumnMultipleValueReadingQuery(queryInt, anglesId);
		for (vector<int>::const_iterator it = anglesId.begin(); it != anglesId.end(); it++) {
			stringstream ss;
			ss << *it;
			string queryDouble = " SELECT thetaView, phiView, thetaIncident, phiIncident FROM Angles WHERE id=" + ss.str();
			vector < vector<double> > result;
			databaseManager.executeMultipleColumnReadingQuery(queryDouble, result, 4);
			thetaView.push_back(result[0][0]);
			phiView.push_back(result[0][1]);
			thetaIncident.push_back(result[0][2]);
			phiIncident.push_back(result[0][3]);
		}
	}
};

#endif /* PHASEDATABASEDAO_HPP_ */
