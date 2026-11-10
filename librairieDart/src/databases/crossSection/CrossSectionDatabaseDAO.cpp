/*
 * CrossSectionDatabaseDAO.cpp
 *
 *  Created on: 27 nov. 2018
 *      Author: wangy
 */

#include "../../../include/databases/crossSection/CrossSectionDatabaseDAO.hpp"

const vector<string> CrossSectionDatabaseDAO::_molecule = {"co2","h2o","o3","ch4","n2o"};

const string CrossSectionDatabaseDAO::_tableCS = "_crosec_";
const string CrossSectionDatabaseDAO::_tableWL = "_wavelength_";
const string CrossSectionDatabaseDAO::_tableAltitude = "altitude";

const string CrossSectionDatabaseDAO::_col_tableWL = "wavelength";
const string CrossSectionDatabaseDAO::_col_tableAltitude = "altitude";

const string CrossSectionDatabaseDAO::_colCS = "crosec";
const string CrossSectionDatabaseDAO::_colIdWavel = "IDlamda";
const string CrossSectionDatabaseDAO::_colIdAltitude = "IDz";

//string CrossSectionDatabaseDAO::_databaseFileName;

//DartDatabaseManager CrossSectionDatabaseDAO::_croSecDBmanager;

string const CrossSectionDatabaseDAO::buildReadingQueryMultipleValuesCrossSection(const std::string & tableName,
                                                                                  int const& idLamda,
                                                                    			vector<int > const& idAltitude) {

	vector<int >::const_iterator it;

	ostringstream stream_wlInd;
	stream_wlInd << "(";
	stream_wlInd << idLamda;
	stream_wlInd << ")";

	ostringstream stream_zInd;
	stream_zInd << "(";
	for (it = idAltitude.begin(); it < idAltitude.end(); ++it) {
		stream_zInd << (*it);
		if (it != idAltitude.end() - 1) {
			stream_zInd << ",";
		}
	}
	stream_zInd << ")";


	ostringstream tmp_querry;
	tmp_querry.str("");

	tmp_querry << "SELECT " << _colCS << " FROM " << tableName << " WHERE " << _colIdWavel << " in " << stream_wlInd.str() << " AND " << _colIdAltitude << " in "
			<< stream_zInd.str();

	return tmp_querry.str();
}

vector<double> CrossSectionDatabaseDAO::getWavelength(DartDatabaseManager & _croSecDBmanager,
                                                      const std::string & tableName,
                                                      int& spectralBandIdMin,
                                                      int& spectralBandIdMax){
	ostringstream tmp_querry;
	tmp_querry.str("");
	tmp_querry << "SELECT wavelength FROM " << tableName << " WHERE Id BETWEEN " << spectralBandIdMax<<" AND "<<spectralBandIdMin<<" order by "<<_col_tableWL;
	vector<double> result;
	_croSecDBmanager.executeSingleColumnMultipleValueReadingQuery(tmp_querry.str(), result);
	return result;
}

vector<int> CrossSectionDatabaseDAO::getAltutudeId(DartDatabaseManager & _croSecDBmanager,
                                                   double const &z_min,
                                                   double const &z_max){
	ostringstream tmp_querry;
	tmp_querry.str("");
	tmp_querry << "SELECT Id FROM " << _tableAltitude<< " WHERE " << _col_tableAltitude << ">= " << z_min << " AND " <<_col_tableAltitude<<"<= " << z_max ;
	vector<int > result;
	_croSecDBmanager.executeSingleColumnMultipleValueReadingQuery(tmp_querry.str(), result);
	return result;
}

void CrossSectionDatabaseDAO::getMaxIdSpectralBandIfEndingOfBeginingOfTable(	DartDatabaseManager & _croSecDBmanager,
                                                                            	const std::string & tableName,
                                                                            	const double & lowerLambdaValue,
                                                                            	int & returnedSpectralBandIdMin) {
	_croSecDBmanager.getSpectralBandId(tableName, lowerLambdaValue, true, true, returnedSpectralBandIdMin);
}

void CrossSectionDatabaseDAO::getMaxIdSpectralBandIfEndingOfEndingOfTable(DartDatabaseManager & _croSecDBmanager,
                                                                          const std::string & tableName,
                                                                          const double & upperLambdaValue,
                                                                          int & returnedSpectralBandIdMax) {
	_croSecDBmanager.getSpectralBandId(tableName, upperLambdaValue, false, true, returnedSpectralBandIdMax);
}

void CrossSectionDatabaseDAO::getIdSpectralBand(	DartDatabaseManager & _croSecDBmanager,
                                                	const std::string & tableName,
                                                	const double & lowerLambdaValue,
                                                	const double & upperLambdaValue,
                                                	int & returnedSpectralBandIdMin,
                                                	int & returnedSpectralBandIdMax) {

	_croSecDBmanager.getSpectralBandId(tableName, upperLambdaValue, true, false, returnedSpectralBandIdMax);
	_croSecDBmanager.getSpectralBandId(tableName, lowerLambdaValue, false, false, returnedSpectralBandIdMin);
}

void CrossSectionDatabaseDAO::getMinMaxIdSpectralBand(DartDatabaseManager & _croSecDBmanager,
                                                      const std::string & tableName,
                                                      const double & lowerLambdaValue,
                                                      const double & upperLambdaValue,
                                                      int & returnedSpectralBandIdMin,
                                                      int & returnedSpectralBandIdMax) {
	int spectralBandIdMin = 0, spectralBandIdMax = 0;
	std::ostringstream ossMin, ossMax;

	getIdSpectralBand( _croSecDBmanager,tableName, lowerLambdaValue, upperLambdaValue, spectralBandIdMin, spectralBandIdMax);
	// Fix_à_l'arrache_dans_le_code += 1
	// process phase has ended and returned an error: Overflow error [Fix_à_l'arrache_dans_le_code]
	if (spectralBandIdMin < 1) {
		getMaxIdSpectralBandIfEndingOfBeginingOfTable( _croSecDBmanager,tableName, lowerLambdaValue, spectralBandIdMin);
	}

	// Deux mois plus tard ...
	// Encore_Un_Fix_à_l'arrache_dans_le_code += 1
	if (spectralBandIdMax < 1) {
		getMaxIdSpectralBandIfEndingOfEndingOfTable( _croSecDBmanager,tableName, upperLambdaValue, spectralBandIdMax);
	}

	// Dans deux mois, ici, il y aura un autre fix :3
	// On est ~5 mois plus tard et je suis la :D (En date du 07/02/2017 zoulou)
	// Bon c'était un problème de précision, donc le fix n'est pas ici, juste dans la méthode en dessous.

	// Loupé, c'était 3 mois plus tard! xD
	if (spectralBandIdMin == spectralBandIdMax) {
		if (spectralBandIdMax == 1) {
			spectralBandIdMax = 0;
		} else {
			++spectralBandIdMin;
		}
	}
	returnedSpectralBandIdMin = spectralBandIdMin;
	returnedSpectralBandIdMax = spectralBandIdMax;
}

int CrossSectionDatabaseDAO::getMaxIdSpectralBand(DartDatabaseManager & _croSecDBmanager,
                                                      const std::string & tableName) {
	ostringstream tmp_querry;
	tmp_querry.str("");
	tmp_querry << "SELECT Id FROM " << tableName;
	vector<int > result;
	_croSecDBmanager.executeSingleColumnMultipleValueReadingQuery(tmp_querry.str(), result);
	return result.size();
}

void CrossSectionDatabaseDAO::readAtmosphericParametersPerBand(  DartDatabaseManager & _croSecDBmanager,
                                                                 const std::string & gas_model,
                                                                 const double & lowerLambdaValue,
                                                                 const double & upperLambdaValue,
                                                                 std::vector<std::vector<std::vector<double> > > & result) {

	std::string completeTableName, query;
	int spectralBandIdMin, spectralBandIdMax;
	vector<double> wavelength;
	int maxID;
	int idBand;
	double z_min = 0.0, z_max = 100.0;
	vector<int > idAltitude = getAltutudeId(_croSecDBmanager,z_min,z_max);

	for(unsigned int idMolecule = 0;idMolecule < _molecule.size();idMolecule++){
		completeTableName =  gas_model + _tableWL + _molecule[idMolecule];
		getMinMaxIdSpectralBand(_croSecDBmanager, completeTableName, lowerLambdaValue, upperLambdaValue, spectralBandIdMin, spectralBandIdMax);
		maxID = getMaxIdSpectralBand(_croSecDBmanager, completeTableName);
		wavelength = getWavelength(_croSecDBmanager,completeTableName,spectralBandIdMin, spectralBandIdMax);
//		cout<<idMolecule<<"\t"<<spectralBandIdMin<<" "<<spectralBandIdMax<<endl;
//		for(int i = 0;i<wavelength.size();i++){
//			cout<<wavelength[i]<<"\t";
//		}
//		cout<<endl;
		vector< vector <double> >  croSec_perband = vector< vector <double> > ();
		for(unsigned int idLamda = 0; idLamda<wavelength.size();idLamda++){
			vector<double> temp_res = vector<double>();
			completeTableName =  gas_model + _tableCS + _molecule[idMolecule];
			if(spectralBandIdMin > maxID){//if the defined band is inferior to minimum value in the database, we choose the cross section of minimum value in the database
				idBand = spectralBandIdMin - idLamda - 1;
			}else{
				idBand = spectralBandIdMin - idLamda;
			}
			query = buildReadingQueryMultipleValuesCrossSection( completeTableName,idBand,idAltitude);
//			cout<<maxID<<query<<endl;
			_croSecDBmanager.executeSingleColumnMultipleValueReadingQuery(query, temp_res);
			temp_res.insert(temp_res.begin(),wavelength[idLamda]);
			croSec_perband.push_back(temp_res);
		}
		result.push_back(croSec_perband);
	}
}
