/*
 * LibDartException.h
 *
 *  Created on: 18 aout 2009
 *
 */

#ifndef LIBDARTEXCEPTION_H_
#define LIBDARTEXCEPTION_H_
#include <exception>
#include <string>
class LibDartException: public std::exception {
public:
	LibDartException(const std::string & clef = "erreur.generique") throw () :
			_clef(clef) {}
	const char* what() const throw () {
		return _clef.c_str();
	}
	virtual ~LibDartException() throw () {}
private:
	std::string _clef;       // Description de l'erreur.
};
/*const char* LibDartException::what() const throw()
 {
 return _clef.c_str();
 }*/
#endif /* LIBDARTEXCEPTION_H_ */
