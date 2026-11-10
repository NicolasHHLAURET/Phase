/*
 * waterVaporDatabaseDAO.cpp
 *
 *  Created on: Jun 4, 2016
 *      Author: ctpp
 */
#include "../../../include/databases/waterVapor/WaterVaporDatabaseDAO.hpp"

bool WaterVaporDatabaseDAO::_ifAlreadyInit = false;

vector<double> WaterVaporDatabaseDAO::_listDistance = vector<double>();
vector<double> WaterVaporDatabaseDAO::_listPressure = vector<double>();
vector<double> WaterVaporDatabaseDAO::_listTemperature = vector<double>();
vector<double> WaterVaporDatabaseDAO::_listRH = vector<double>();
vector<double> WaterVaporDatabaseDAO::_listWavelength = vector<double>();

const string WaterVaporDatabaseDAO::_tableCS = "CrossSection";
const string WaterVaporDatabaseDAO::_tableWL = "Wavelength";
const string WaterVaporDatabaseDAO::_tableTemp = "Temperature";
const string WaterVaporDatabaseDAO::_tableRH = "RelHumidity";
const string WaterVaporDatabaseDAO::_tableDist = "Distance";
const string WaterVaporDatabaseDAO::_tablePres = "Pressure";

const string WaterVaporDatabaseDAO::_col_tableWL = "wavelength";
const string WaterVaporDatabaseDAO::_col_tableTemp = "temperature";
const string WaterVaporDatabaseDAO::_col_tableRH = "rh";
const string WaterVaporDatabaseDAO::_col_tableDist = "distance";
const string WaterVaporDatabaseDAO::_col_tablePres = "pressure";

const string WaterVaporDatabaseDAO::_colCS = "croSec";
const string WaterVaporDatabaseDAO::_colIdWavel = "idWavel";
const string WaterVaporDatabaseDAO::_colIdRH = "idRH";
const string WaterVaporDatabaseDAO::_colIdTemp = "idTemp";
const string WaterVaporDatabaseDAO::_colIdPres = "idPres";
const string WaterVaporDatabaseDAO::_colIdDist = "idDist";

string WaterVaporDatabaseDAO::_databaseFileName;

DartDatabaseManager WaterVaporDatabaseDAO::_h2oDBmanager;

map<unsigned int, map<unsigned int, map<unsigned int, map<unsigned int, map<unsigned int, double> > > > > WaterVaporDatabaseDAO::_preComputedCrossSections;

bool WaterVaporDatabaseDAO::_ifLUTcreated = false;

string const WaterVaporDatabaseDAO::buildReadingQueryMultipleValuesCrossSection(vector<unsigned int> const& idDist,
																				vector<unsigned int> const& idPressure,
																				vector<unsigned int> const& idTemp,
																				vector<unsigned int> const& idRH,
																				vector<unsigned int> const& idWavelength) {

	vector<unsigned int>::const_iterator it;

	ostringstream stream_distInd;
	stream_distInd << "(";
	for (it = idDist.begin(); it < idDist.end(); ++it) {
		stream_distInd << (*it);
		if (it != idDist.end() - 1) {
			stream_distInd << ",";
		}
	}
	stream_distInd << ")";

	ostringstream stream_presInd;
	stream_presInd << "(";
	for (it = idPressure.begin(); it < idPressure.end(); ++it) {
		stream_presInd << (*it);
		if (it != idPressure.end() - 1) {
			stream_presInd << ",";
		}
	}
	stream_presInd << ")";

	ostringstream stream_tempInd;
	stream_tempInd << "(";
	for (it = idTemp.begin(); it < idTemp.end(); ++it) {
		stream_tempInd << (*it);
		if (it != idTemp.end() - 1) {
			stream_tempInd << ",";
		}
	}
	stream_tempInd << ")";

	ostringstream stream_rhInd;
	stream_rhInd << "(";
	for (it = idRH.begin(); it < idRH.end(); ++it) {
		stream_rhInd << (*it);
		if (it != idRH.end() - 1) {
			stream_rhInd << ",";
		}
	}
	stream_rhInd << ")";

	ostringstream stream_wlInd;
	stream_wlInd << "(";
	for (it = idWavelength.begin(); it < idWavelength.end(); ++it) {
		stream_wlInd << (*it);
		if (it != idWavelength.end() - 1) {
			stream_wlInd << ",";
		}
	}
	stream_wlInd << ")";

	ostringstream tmp_querry;
	tmp_querry.str("");

	tmp_querry << "SELECT " << _colCS << " FROM " << _tableCS << " WHERE " << _colIdDist << " in " << stream_distInd.str() << " AND " << _colIdPres
			<< " in " << stream_presInd.str() << " AND " << _colIdTemp << " in " << stream_tempInd.str() << " AND " << _colIdRH << " in "
			<< stream_rhInd.str() << " AND " << _colIdWavel << " in " << stream_wlInd.str();

	return tmp_querry.str();
}

string const WaterVaporDatabaseDAO::buildReadingQuerySingleValueCrossSection(	unsigned int const& idDist,
																				unsigned int const& idPressure,
																				unsigned int const& idTemp,
																				unsigned int const& idRH,
																				unsigned int const& idWavelength) {
	ostringstream tmp_querry;
	tmp_querry.str("");

	tmp_querry << "SELECT " << _colCS << " FROM " << _tableCS << " WHERE " << _colIdDist << "=" << idDist << " AND " << _colIdPres << "="
			<< idPressure << " AND " << _colIdTemp << "=" << idTemp << " AND " << _colIdRH << "=" << idRH << " AND " << _colIdWavel << "="
			<< idWavelength;

	return tmp_querry.str();
}

string const WaterVaporDatabaseDAO::buildReadingQueryMultiValueCrossSection(unsigned int const& idDist,
																			unsigned int const& idPressure,
																			unsigned int const& idTemp,
																			unsigned int const& idRH,
																			unsigned int const& idWavelengthMin,
																			unsigned int const& idWavelengthMax) {
	ostringstream tmp_querry;
	tmp_querry.str("");

	tmp_querry << "SELECT " << _colCS << " FROM " << _tableCS << " WHERE " << _colIdDist << "=" << idDist << " AND " << _colIdPres << "="
			<< idPressure << " AND " << _colIdTemp << "=" << idTemp << " AND " << _colIdRH << "=" << idRH << " AND " << _colIdWavel << " BETWEEN "
			<< idWavelengthMin << " AND " << idWavelengthMax << " ORDER BY Id ";

	return tmp_querry.str();
}

void WaterVaporDatabaseDAO::init() {

	_databaseFileName = DartEnv::getDatabasePath("H2Oground.db") + "H2Oground.db";
	_h2oDBmanager.reinit(_databaseFileName);
	if (_h2oDBmanager.connectDatabase()) {
		cout << "    Water Vapor database: " << _databaseFileName << endl;

		//Reading the distance list
		ostringstream querry_table_dist;
		querry_table_dist.str("");
		querry_table_dist << "SELECT " << _col_tableDist << " FROM " << _tableDist;
		_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_table_dist.str(), _listDistance);

		//Reading the pressure list
		ostringstream querry_table_pressure;
		querry_table_pressure.str("");
		querry_table_pressure << "SELECT " << _col_tablePres << " FROM " << _tablePres;
		_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_table_pressure.str(), _listPressure);

		//Reading the temperature list
		ostringstream querry_table_temperature;
		querry_table_temperature.str("");
		querry_table_temperature << "SELECT " << _col_tableTemp << " FROM " << _tableTemp;
		_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_table_temperature.str(), _listTemperature);

		//Reading the relative humidity list
		ostringstream querry_table_RH;
		querry_table_RH.str("");
		querry_table_RH << "SELECT " << _col_tableRH << " FROM " << _tableRH;
		_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_table_RH.str(), _listRH);

		//Reading the wavelength list
		ostringstream querry_table_wavelength;
		querry_table_wavelength.str("");
		querry_table_wavelength << "SELECT " << _col_tableWL << " FROM " << _tableWL;
		_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_table_wavelength.str(), _listWavelength);
		_ifAlreadyInit = true;
	} else {
		cerr << "Error: Water Vapor database cannot be connected..." << endl;
		abort();
	}
}

void WaterVaporDatabaseDAO::createLocalLUTForWaterVaporDatabaseReading(	vector<double> const& allDists,
																		vector<double> const& allPressures,
																		vector<double> const& allTempratures,
																		vector<double> const& allRHs,
																		vector<double> const& allWavelengths) {
	vector<unsigned int> indicesDist, indicesPressure, indicesTemperature, indicesRH, indicesWavelength;

	unsigned int tmpIndex0, tmpIndex1;
	double tmpSep, tmpProp;
	vector<double>::const_iterator it;

	for (it = allDists.begin(); it < allDists.end(); ++it) {
		getInterpLinearParametersFromList(_listDistance, *it, tmpIndex0, tmpIndex1, tmpSep, tmpProp);
		if (std::find(indicesDist.begin(), indicesDist.end(), tmpIndex0) == indicesDist.end()) {
			indicesDist.push_back(tmpIndex0);
		}
		if (std::find(indicesDist.begin(), indicesDist.end(), tmpIndex1) == indicesDist.end()) {
			indicesDist.push_back(tmpIndex1);
		}
	}

	for (it = allPressures.begin(); it < allPressures.end(); ++it) {
		getInterpLinearParametersFromList(_listPressure, *it, tmpIndex0, tmpIndex1, tmpSep, tmpProp);
		if (std::find(indicesPressure.begin(), indicesPressure.end(), tmpIndex0) == indicesPressure.end()) {
			indicesPressure.push_back(tmpIndex0);
		}
		if (std::find(indicesPressure.begin(), indicesPressure.end(), tmpIndex1) == indicesPressure.end()) {
			indicesPressure.push_back(tmpIndex1);
		}
	}

	for (it = allTempratures.begin(); it < allTempratures.end(); ++it) {
		getInterpLinearParametersFromList(_listTemperature, *it, tmpIndex0, tmpIndex1, tmpSep, tmpProp);
		if (std::find(indicesTemperature.begin(), indicesTemperature.end(), tmpIndex0) == indicesTemperature.end()) {
			indicesTemperature.push_back(tmpIndex0);
		}
		if (std::find(indicesTemperature.begin(), indicesTemperature.end(), tmpIndex1) == indicesTemperature.end()) {
			indicesTemperature.push_back(tmpIndex1);
		}
	}

	for (it = allRHs.begin(); it < allRHs.end(); ++it) {
		getInterpLinearParametersFromList(_listRH, *it, tmpIndex0, tmpIndex1, tmpSep, tmpProp);
		if (std::find(indicesRH.begin(), indicesRH.end(), tmpIndex0) == indicesRH.end()) {
			indicesRH.push_back(tmpIndex0);
		}
		if (std::find(indicesRH.begin(), indicesRH.end(), tmpIndex1) == indicesRH.end()) {
			indicesRH.push_back(tmpIndex1);
		}
	}

	for (it = allWavelengths.begin(); it < allWavelengths.end(); ++it) {
		getInterpLinearParametersFromList(_listWavelength, *it, tmpIndex0, tmpIndex1, tmpSep, tmpProp);
		if (std::find(indicesWavelength.begin(), indicesWavelength.end(), tmpIndex0) == indicesWavelength.end()) {
			indicesWavelength.push_back(tmpIndex0);
		}
		if (std::find(indicesWavelength.begin(), indicesWavelength.end(), tmpIndex1) == indicesWavelength.end()) {
			indicesWavelength.push_back(tmpIndex1);
		}
	}

	vector<unsigned int>::const_iterator it_int;

	string querry_multiple = buildReadingQueryMultipleValuesCrossSection(indicesDist, indicesPressure, indicesTemperature, indicesRH,
		indicesWavelength);
	vector<double> cs_selected;
	_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_multiple, cs_selected);

	//Create the map for reference
	unsigned int indVectorCS = 0;
	for (unsigned int ind0 = 0; ind0 < indicesDist.size(); ++ind0) {
		map<unsigned int, map<unsigned int, map<unsigned int, map<unsigned int, double> > > > map1;
		for (unsigned int ind1 = 0; ind1 < indicesPressure.size(); ++ind1) {
			map<unsigned int, map<unsigned int, map<unsigned int, double> > > map2;
			for (unsigned int ind2 = 0; ind2 < indicesTemperature.size(); ++ind2) {
				map<unsigned int, map<unsigned int, double> > map3;
				for (unsigned int ind3 = 0; ind3 < indicesRH.size(); ++ind3) {
					map<unsigned int, double> map4;
					for (unsigned int ind4 = 0; ind4 < indicesWavelength.size(); ++ind4) {
						map4[indicesWavelength[ind4]] = cs_selected[indVectorCS];
						++indVectorCS;
					}
					map3[indicesRH[ind3]] = map4;
				}
				map2[indicesTemperature[ind2]] = map3;
			}
			map1[indicesPressure[ind1]] = map2;
		}
		_preComputedCrossSections[indicesDist[ind0]] = map1;
	}
	if (indVectorCS == indicesDist.size() * indicesPressure.size() * indicesTemperature.size() * indicesRH.size() * indicesWavelength.size()) {
		_ifLUTcreated = true;
	} else {
		cerr << "The created look up table is not in correct size with the input dimensions" << endl;
		abort();
	}

}

double WaterVaporDatabaseDAO::getCrossSection(	double const& distance,
												double const& pressure,
												double const& temperature,
												double const& rh,
												double const& wl,
												bool const& isFromLUT) {

	bool ifOutputForDebug = false;

	if (isFromLUT && (!_ifLUTcreated)) {
		cerr << "Error: LUT was not previously created! " << endl;
		abort();
	}

	if (_ifAlreadyInit) {
		//dimentions: 1: distance, 2: pressure, 3: temperature, 4: rh, 5: wl
		const unsigned int numDim = 5;
		const unsigned int numValueForIntPerDim = 2; //Just 2 values for linear interpolation

		vector<vector<unsigned int> > indices(numDim, vector<unsigned int>(numValueForIntPerDim, 999));
		vector<double> seps(numDim, 0);
		vector<double> pros(numDim, 0);

		getInterpLinearParametersFromList(_listDistance, distance, indices[0][0], indices[0][1], seps[0], pros[0]);
		getInterpLinearParametersFromList(_listPressure, pressure, indices[1][0], indices[1][1], seps[1], pros[1]);
		getInterpLinearParametersFromList(_listTemperature, temperature, indices[2][0], indices[2][1], seps[2], pros[2]);
		getInterpLinearParametersFromList(_listRH, rh, indices[3][0], indices[3][1], seps[3], pros[3]);
		getInterpLinearParametersFromList(_listWavelength, wl, indices[4][0], indices[4][1], seps[4], pros[4]);

		if (ifOutputForDebug) {
			cout << "Distance Selected Indices: " << indices[0][0] << " " << indices[0][1] << "; Separation: " << seps[0] << "; Proportion: "
					<< pros[0] << "." << endl;
			cout << "Pressure Selected Indices: " << indices[1][0] << " " << indices[1][1] << "; Separation: " << seps[1] << "; Proportion: "
					<< pros[1] << "." << endl;
			cout << "Temperature Selected Indices: " << indices[2][0] << " " << indices[2][1] << "; Separation: " << seps[2] << "; Proportion: "
					<< pros[2] << "." << endl;
			cout << "RH Selected Indices: " << indices[3][0] << " " << indices[3][1] << "; Separation: " << seps[3] << "; Proportion: " << pros[3]
					<< "." << endl;
			cout << "Wavelength Selected Indices: " << indices[4][0] << " " << indices[4][1] << "; Separation: " << seps[4] << "; Proportion: "
					<< pros[4] << "." << endl;
		}
		unsigned int ind0, ind1, ind2, ind3, ind4;
		unsigned int ind_list_0, ind_list_1, ind_list_2, ind_list_3, ind_list_4;
		double interpValues0[numValueForIntPerDim], interpValues1[numValueForIntPerDim], interpValues2[numValueForIntPerDim],
				interpValues3[numValueForIntPerDim], interpValues4[numValueForIntPerDim];

		vector<double> cs_selected;
		//Using buildReadingQueryMultipleValuesCrossSection for multiple value reading would be much faster than executeSingleColumnSingleValueReadingQuery for single value reading.
		if (!isFromLUT) {
			string querry_multiple = buildReadingQueryMultipleValuesCrossSection(indices[0], indices[1], indices[2], indices[3], indices[4]);
			_h2oDBmanager.executeSingleColumnMultipleValueReadingQuery(querry_multiple, cs_selected);
			if (cs_selected.size() != pow(numValueForIntPerDim, numDim)) {
				cerr << "Error: The size of multi-column output values are not in consistent with the defined dimentions" << endl;
				abort();
			}
		}

		unsigned int indSelectedCS = 0;

		string querry_tmp;
		//Linear interpolation in 5 dimensions
		for (ind0 = 0; ind0 < numValueForIntPerDim; ++ind0) {
			ind_list_0 = indices[0][ind0];
			for (ind1 = 0; ind1 < numValueForIntPerDim; ++ind1) {
				ind_list_1 = indices[1][ind1];
				for (ind2 = 0; ind2 < numValueForIntPerDim; ++ind2) {
					ind_list_2 = indices[2][ind2];
					for (ind3 = 0; ind3 < numValueForIntPerDim; ++ind3) {
						ind_list_3 = indices[3][ind3];
						for (ind4 = 0; ind4 < numValueForIntPerDim; ++ind4) {
							ind_list_4 = indices[4][ind4];
							if (isFromLUT) {
								interpValues4[ind4] = _preComputedCrossSections[ind_list_0][ind_list_1][ind_list_2][ind_list_3][ind_list_4];
							} else {
								interpValues4[ind4] = cs_selected[indSelectedCS];
							}
							indSelectedCS += 1;
						}
						interpValues3[ind3] = interpValues4[0] + (interpValues4[1] - interpValues4[0]) * pros[4];
					}
					interpValues2[ind2] = interpValues3[0] + (interpValues3[1] - interpValues3[0]) * pros[3];
				}
				interpValues1[ind1] = interpValues2[0] + (interpValues2[1] - interpValues2[0]) * pros[2];
			}
			interpValues0[ind0] = interpValues1[0] + (interpValues1[1] - interpValues1[0]) * pros[1];
		}
		double res = interpValues0[0] + (interpValues0[1] - interpValues0[0]) * pros[0];
		return res;

	} else {
		cerr << "The database manager has not been initiated. " << endl;
		abort();
	}

	return 0.0;
}

void WaterVaporDatabaseDAO::getInterpLinearParametersFromList(	vector<double> const& col,
																double const& inp,
																unsigned int & ind0,
																unsigned int & ind1,
																double & separation,
																double & proportion0) {
	bool valueFound = false;

	if (inp < min(col[0], col[col.size() - 1])) {
		// Value below minimum of the range: set to min
		if (col[0] < col[col.size() - 1]) {
			ind0 = 1;
			ind1 = 2;

			separation = col[1] - col[0];
			proportion0 = 0;
		} else {
			ind0 = col.size() - 1;
			ind1 = col.size();

			separation = col[col.size() - 1] - col[col.size() - 2];
			proportion0 = 1;
		}
		return;
	} else if (inp > max(col[0], col[col.size() - 1])) {
		// Value above the maximum of the range: set to max
		if (col[0] > col[col.size() - 1]) {
			ind0 = 1;
			ind1 = 2;

			separation = col[1] - col[0];
			proportion0 = 0;
		} else {
			ind0 = col.size() - 1;
			ind1 = col.size();

			separation = col[col.size() - 1] - col[col.size() - 2];
			proportion0 = 1;
		}
		return;
	}

	for (unsigned int i = 0; i < col.size() - 1; ++i) {
		if (inp >= min(col[i], col[i + 1]) && inp <= max(col[i], col[i + 1])) {
			ind0 = i + 1; // one plus for one additional index value for SQL database
			ind1 = i + 2; // one plus for one additional index value for SQL database

			separation = col[i + 1] - col[i];
			proportion0 = (inp - col[i]) / separation;
			valueFound = true;
			break;
		}
	}

	if (!valueFound) {
		cerr << "Error: Interpolation parameter not found!!! " << inp << endl;
		abort();
	}

}
