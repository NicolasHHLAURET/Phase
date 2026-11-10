/*
 * BadFormatException.hpp
 *
 *  Created on: 10 nov. 2009
 *      Author: goulierv
 */

#ifndef BADFORMATEXCEPTION_HPP_
#define BADFORMATEXCEPTION_HPP_

#include <stdexcept>
#include <string>

class BadFormatException: public std::exception {
public:
	BadFormatException(std::string message);
	virtual ~BadFormatException() throw () {}
	;
	std::string what();
private:
	std::string _message;
};

inline BadFormatException::BadFormatException(std::string message) {
	_message = message;
}

inline std::string BadFormatException::what() {
	return _message;
}

#endif /* BADFORMATEXCEPTION_HPP_ */
