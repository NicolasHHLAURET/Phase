/*
 * AirPhaseFunctionFileLoader.hpp
 *
 *  Created on: 4 juin 2012
 *      Author: lauretn
 */

#ifndef AIRPHASEFUNCTIONFILELOADER_HPP_
#define AIRPHASEFUNCTIONFILELOADER_HPP_

#include <bits/stl_pair.h>
#include "../../../../libSOCI/src/core/soci.h"
#include "../../../../libSOCI/src/backends/sqlite3/soci-sqlite3.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

using namespace soci;
class AirPhaseFunctionFileLoader {
private:
	string _law;
	vector<vector<double> > _valuesPerBand; // Value [0] is the spectral band value.
	unsigned int _numberOfColumns;
	unsigned int _numberOfValues;

public:
	AirPhaseFunctionFileLoader() {
		_numberOfColumns = 0;
		_numberOfValues = 0;
	}

	AirPhaseFunctionFileLoader(	const string & absoluteFilePath,
								const string & _ParametersFilePath) {
		loadFile(absoluteFilePath, _ParametersFilePath);
	}

	const string & getLaw() const {
		return _law;
	}

	const vector<vector<double> > & getValuesPerBand() const {
		return _valuesPerBand;
	}
	const unsigned int & getNumberOfColumns() const {
		return _numberOfColumns;
	}
	const unsigned int & getNumberOfValues() const {
		return _numberOfValues;
	}

	bool loadFile(	const string & absoluteFilePath,
					const string & _ParametersFilePath) {
		session sociSession;
		sociSession.open(*factory_sqlite3(), absoluteFilePath);
		string query;
		query = "SELECT Gas_type FROM _gas_type WHERE model = '" + _ParametersFilePath + "'";
		sociSession << query, into(_law);
		string columnNames;
		if (_law == "Rayleigh") {
			_numberOfColumns = 5;
			columnNames = "wavelength,sigma,albedo,a,c";
		} else if (_law == "HenyeyGreenstein") {
			_numberOfColumns = 6;
			columnNames = "wavelength,sigma,albedo,a,g1,g2";
		} else {
			_numberOfColumns = 0;
			cout << "type of gas unknown" << endl;
			return false;
		}
		query = "SELECT COUNT(Id) FROM " + _ParametersFilePath;
		sociSession << query, into(_numberOfValues);
		vector < vector<double> > newVector;
		newVector.resize(_numberOfColumns);
		for (unsigned int i = 0; i < _numberOfColumns; i++) {
			newVector[i].resize(_numberOfValues);
		}
		_valuesPerBand.resize(_numberOfValues);
		for (unsigned int i = 0; i < _numberOfValues; i++) {
			_valuesPerBand[i].resize(_numberOfColumns);
		}
		query = "SELECT wavelength FROM " + _ParametersFilePath;
		sociSession << query, into(newVector[0]);
		query = "SELECT sigma FROM " + _ParametersFilePath;
		sociSession << query, into(newVector[1]);
		query = "SELECT albedo FROM " + _ParametersFilePath;
		sociSession << query, into(newVector[2]);
		query = "SELECT a FROM " + _ParametersFilePath;
		sociSession << query, into(newVector[3]);
		if (_law == "Rayleigh") {
			query = "SELECT c FROM " + _ParametersFilePath;
			sociSession << query, into(newVector[4]);
		} else if (_law == "HenyeyGreenstein") {
			query = "SELECT g1 FROM " + _ParametersFilePath;
			sociSession << query, into(newVector[4]);
			query = "SELECT g2 FROM " + _ParametersFilePath;
			sociSession << query, into(newVector[5]);
		} else {
			cout << "type of gas unknown" << endl;
		}

		for (unsigned int i = 0; i < _numberOfColumns; i++) {
			for (unsigned int j = 0; j < _numberOfValues; j++) {
				_valuesPerBand[j][i] = newVector[i][j];
			}
		}
		return true;
	}

	const string toString() const;
	friend ostream & operator<<(ostream &stream,
								const AirPhaseFunctionFileLoader & apffl);

private:
	/**
	 * Skip slash-star and double slash commentaries.
	 * currentString contains the last read string in the stream.
	 * At the end of the method, currentString will be the first valid string read in the stream.
	 */
	void skipCommentaries(ifstream & inputData) {
		char c = inputData.get();
		bool correctData = false;
		bool inComment = false;
		while (!correctData) {
			if (c == ' ' || c == '\t' || c == '\n' || c == ',') {
				c = inputData.get();
			} else if (!inComment && c == '/') {
				c = inputData.get();
				if (c == '/') {
					do {
						c = inputData.get();
					} while (c != '\n');
				} else if (c == '*') {
					inComment = true;
				} else {
					inputData.unget();
					correctData = true;
				}
			} else if (inComment) {
				if (c == '*') {
					c = inputData.get();
					if (c == '/') {
						c = inputData.get();
						inComment = false;
					}
				} else {
					c = inputData.get();
				}
			} else {
				correctData = true;
			}
		}
		inputData.unget();
	}
};

inline ostream & operator<<(ostream &stream,
							const AirPhaseFunctionFileLoader & apffl) {
	stream << "AirPhaseFunctionFileLoader - Law : " << apffl._law << endl;
	stream << "   Data : " << endl;
	for (unsigned int i = 0; i < apffl._valuesPerBand.size(); ++i) {
		if (!apffl._valuesPerBand[i].empty()) {
			cout << apffl._valuesPerBand[i][0];
			for (unsigned int j = 1; j < apffl._valuesPerBand[i].size(); ++j) {
				cout << " " << apffl._valuesPerBand[i][j];
			}
			cout << endl;
		}
	}
	return stream;
}

inline const string AirPhaseFunctionFileLoader::toString() const {
	ostringstream os;
	os << *this;
	return os.str();
}

#endif /* AIRPHASEFUNCTIONFILELOADER_HPP_ */
