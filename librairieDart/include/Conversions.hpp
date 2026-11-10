#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "../include/BadFormatException.hpp"

class Conversions {
public:
	Conversions();
	virtual ~Conversions();
	/*
	 * Converti quoi que ce soit en string...
	 */
	template<typename T>
	static std::string convertitEnString(const T & t) {
		std::ostringstream os;
		os.setf(std::ios::fixed);
		os << t;
		if (os.fail()) {
			throw BadFormatException("echec conversion");
		}
		return os.str();
	}
	
	/*
	 * Converti quoi que ce soit en string...
	 */
	template<typename T>
	static std::string convertitEnString(T & t) {
		std::ostringstream os;
		os.setf(std::ios::fixed);
		os << t;
		if (os.fail()) {
			throw BadFormatException("echec conversion");
		}
		return os.str();
	}
	
	/*
	 * Converti un string en quoi que ce soit ...
	 */
	template<typename T>
	static T convertitStringVersAutre(const std::string & t) {
		T retour;
		std::istringstream os(t);
		os >> retour;
		if (os.fail()) {
			throw BadFormatException("echec conversion");
		}
		return retour;
	}
	/*
	 * Converti un string en quoi que ce soit ...
	 */
	template<typename T>
	static T convertitStringVersAutre(std::string & t) {
		T retour;
		std::istringstream os(t);
		os >> retour;
		if (os.fail()) {
			throw BadFormatException("echec conversion");
		}
		return retour;
	}
	/*
	 * Decoupe une string et retourne le resultat dans un vecteur
	 */
	static void tokenize(	const std::string & str,
							std::vector<std::string> & chaineDecoupee,
							const std::string& delimiteur = " ") {
		// Skip delimiters at beginning.
		std::string::size_type lastPos = str.find_first_not_of(delimiteur, 0);
		// Find first "non-delimiter".
		std::string::size_type pos = str.find_first_of(delimiteur, lastPos);
		
		while (std::string::npos != pos || std::string::npos != lastPos) {
			// Found a token, add it to the vector.
			chaineDecoupee.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiteur, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiteur, lastPos);
		}
	}
};

#endif /* CONVERSIONS_H_ */
