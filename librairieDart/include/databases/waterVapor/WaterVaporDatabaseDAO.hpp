/*
 * WaterVaporDatabaseDAO.hpp
 *
 *  Created on: Jun 4, 2016
 *      Author: ctpp
 */

#ifndef INCLUDE_WATERVAPOR_WATERVAPORDATABASEDAO_HPP_
#define INCLUDE_WATERVAPOR_WATERVAPORDATABASEDAO_HPP_

#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>
#include "../DartDatabaseManager.hpp"
#include "DartEnv.h"
#include <map>

using namespace std;

class WaterVaporDatabaseDAO {
private:

	static bool _ifAlreadyInit;

	//List of distances, pressures, temperatures, relative humidities, and wavelengths
	static vector<double> _listDistance,
	_listPressure,
	_listTemperature,
	_listRH,
	_listWavelength;

	//List of tables
	static const string _tableCS, _tableWL, _tableTemp, _tableRH, _tableDist, _tablePres;

	//List of columns in each table
	static const string _col_tableWL, _col_tableTemp, _col_tableRH, _col_tableDist, _col_tablePres;

	//List of columns in cross-section table.
	static const string _colCS, _colIdWavel, _colIdRH, _colIdTemp, _colIdPres, _colIdDist;

	static string _databaseFileName;

	static DartDatabaseManager _h2oDBmanager;

	static map<unsigned int, map<unsigned int, map<unsigned int, map<unsigned int, map<unsigned int, double> > > > > _preComputedCrossSections;

	static bool _ifLUTcreated;

public:
	static string const buildReadingQuerySingleValueCrossSection(unsigned int const& idDist,
			unsigned int const& idPressure, unsigned int const& idTemp,
			unsigned int const& idRH, unsigned int const& idWavelength);

	static string const buildReadingQueryMultipleValuesCrossSection(vector<unsigned int> const& idDist,
			vector<unsigned int> const& idPressure, vector<unsigned int> const& idTemp,
			vector<unsigned int> const& idRH, vector<unsigned int> const& idWavelength);

	static string const buildReadingQueryMultiValueCrossSection(unsigned int const& idDist,
				unsigned int const& idPressure, unsigned int const& idTemp,
				unsigned int const& idRH, unsigned int const& idWavelengthMin, unsigned int const& idWavelengthMax);

	static void init();

	static void createLocalLUTForWaterVaporDatabaseReading(vector<double> const& allDists, vector<double> const& allPressures, vector<double> const& allTempratures, vector<double> const& allRHs, vector<double> const& allWavelengths);

	static double getCrossSection(double const& distance, double const& pressure, double const& temperature, double const& rh, double const& wl, bool const& isFromLUT=true);

	static void getInterpLinearParametersFromList(vector<double> const& col, double const& inp, unsigned int & ind0, unsigned int & ind1, double & separation, double & proportion0);

};

#endif /* INCLUDE_WATERVAPOR_WATERVAPORDATABASEDAO_HPP_ */
