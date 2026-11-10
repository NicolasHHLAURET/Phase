/*
 * IndexErrorException.hpp
 *
 *  Created on: 23 nov. 2009
 *      Author: goulierv
 */

#ifndef INDEXERROREXCEPTION_HPP_
#define INDEXERROREXCEPTION_HPP_

#include <stdexcept>
#include <string>

class IndexErrorException: public std::exception {
public:
	IndexErrorException(std::string message);
	virtual ~IndexErrorException() {}
	;
	std::string what();
private:
	std::string _message;
};

inline IndexErrorException::IndexErrorException(std::string message) {
	_message = message;
}

inline std::string IndexErrorException::what() {
	return _message;
}

#endif /* INDEXERROREXCEPTION_HPP_ */
