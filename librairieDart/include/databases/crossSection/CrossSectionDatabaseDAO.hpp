/*
 * CrossSectionDatabaseDAO.hpp
 *
 *  Created on: 27 nov. 2018
 *      Author: wangy
 */

#ifndef CROSSSECTIONDATABASEDAO_HPP_
#define CROSSSECTIONDATABASEDAO_HPP_

#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>
#include "../DartDatabaseManager.hpp"
#include <map>

using namespace std;

class CrossSectionDatabaseDAO {
private:
	//List of tables
	static const string _tableCS, _tableWL, _tableAltitude;

	//List of columns in each table
	static const string  _col_tableWL, _col_tableAltitude;

	//List of columns in cross-section table.
	static const string _colCS, _colIdWavel, _colIdAltitude;

	//static string _databaseFileName;

	//static DartDatabaseManager _croSecDBmanager;


public:

	//List of molecule

	static const vector<string> _molecule;

	static string const buildReadingQueryMultipleValuesCrossSection(const std::string & tableName,
	                                                                int const& idLamda,
                                                      			vector<int > const& idAltitude);

	static void getMaxIdSpectralBandIfEndingOfBeginingOfTable(	DartDatabaseManager & _croSecDBmanager,
	                                                          	const std::string & tableName,
																const double & lowerLambdaValue,
																int & returnedSpectralBandIdMin);

	static void getMaxIdSpectralBandIfEndingOfEndingOfTable(DartDatabaseManager & _croSecDBmanager,
	                                                        const std::string & tableName,
															const double & upperLambdaValue,
															int & returnedSpectralBandIdMax);

	static void getIdSpectralBand(	DartDatabaseManager & _croSecDBmanager,
	                              	const std::string & tableName,
									const double & lowerLambdaValue,
									const double & upperLambdaValue,
									int & returnedSpectralBandIdMin,
									int & returnedSpectralBandIdMax);

	static void getMinMaxIdSpectralBand(DartDatabaseManager & _croSecDBmanager,
	                                    const std::string & tableName,
										const double & lowerLambdaValue,
										const double & upperLambdaValue,
										int & returnedSpectralBandIdMin,
										int & returnedSpectralBandIdMax);

	static int getMaxIdSpectralBand(DartDatabaseManager & _croSecDBmanager,
	                                const std::string & tableName) ;

	static vector<double> getWavelength(DartDatabaseManager & _croSecDBmanager,
        								const std::string & tableName,
        								int& spectralBandIdMin,
        								int& spectralBandIdMax);

	static vector<int> getAltutudeId(DartDatabaseManager & _croSecDBmanager,
									double const &z_min,
	                                 double const &z_max);

	static void readAtmosphericParametersPerBand(	DartDatabaseManager & databaseManager,
	                                             	const std::string & gas_model,
	        										const double & userSpectrallowerBand,
	        										const double & userSpectralupperBand,
	        										std::vector<std::vector<std::vector<double> > >  & result);

};



#endif /* CROSSSECTIONDATABASEDAO_HPP_ */
