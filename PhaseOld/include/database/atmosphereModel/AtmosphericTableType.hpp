/*
 * AtmosphericTableType.hpp
 *
 *  Created on: 3 mars 2016
 *      Author: guilleuxj
 */

#ifndef ATMOSPHERICTABLETYPE_HPP_
#define ATMOSPHERICTABLETYPE_HPP_

using namespace std;
class AtmosphericTableType {
private:
	string _tableExtension;
	string _tableColumns;
	int _numberOfColumns;

public:

	AtmosphericTableType() {
		_tableExtension = "";
		_tableColumns = "";
		_numberOfColumns = 0;
	}


	AtmosphericTableType(	const string tableExtension,
							const string collumnChoice,
							const int collumnNumber) {
		_tableExtension = tableExtension;
		_tableColumns = collumnChoice;
		_numberOfColumns = collumnNumber;
	}

	const string & getCollumnChoice() const {
		return _tableColumns;
	}

	const int & getCollumnNumber() const {
		return _numberOfColumns;
	}

	const string & getTableExtension() const {
		return _tableExtension;
	}
};

#endif /* ATMOSPHERICTABLETYPE_HPP_ */
