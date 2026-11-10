/*
 * FichierProperties.hpp
 *
 *  Created on: 23 oct. 2009
 *      Author: crosj
 */

#ifndef FICHIERPROPERTIES_HPP_
#define FICHIERPROPERTIES_HPP_

#include "AccesFichiers.hpp"
#include "LibDartException.hpp"
#include <vector>

class FichierProperties {
protected:
	std::map<std::string, std::string> listeProprietes;
private:
	std::string cheminAbsolu;
	std::string separateur;

public:
	FichierProperties(const std::string & cheminAbsolu, const std::string & separateur = ":");
	FichierProperties();
	virtual ~FichierProperties();
	template<typename T>
	T valeurProperties(const std::string & clef) const;
	template<typename T>
	bool valeurProperties(	const std::string & clef,
							T & valeur) const;
	bool exists(const std::string & clef) const;
	bool existsLignesCommencantParClef(const std::string & clef) const;
	void changerFichierProperties(const std::string & cheminAbsolu, const std::string & separateur = ":");
	void effacerLignesCommencantParClef(const std::string & clef);
	void effacerLignesCommencantParClefs(const std::vector<std::string> & clefs);
	void effacerLignesCommencantParClefEtReecrire(const std::string & clef);
	void writeFile(std::string absoluteFilePath = "");
	void appendToFile(std::string absoluteFilePath = "");

	std::map<std::string, std::string>& getListeProprietes();

	void ecrireLigne(const std::string & str);

	void ajouterCle(const std::string & cle,
					const std::string & valeur) {
		listeProprietes[cle] = valeur;
	}

	const std::map<std::string, std::string> & getPropertyList() const {
		return listeProprietes;
	}
	std::map<std::string, std::string> & getPropertyList() {
		return listeProprietes;
	}
};

inline FichierProperties::FichierProperties(const std::string & _cheminAbsolu, const std::string & separateur) {
	AccesFichiers af(separateur);
	try {
		listeProprietes = af.lireFichierProperties(_cheminAbsolu);
	} catch (LibDartException & ex) {
		std::cerr << ex.what();
	}
	cheminAbsolu = _cheminAbsolu;
	this->separateur = separateur;
}

inline FichierProperties::FichierProperties() {}

inline FichierProperties::~FichierProperties() {
	// TODO Auto-generated destructor stub
}
inline std::map<std::string, std::string>& FichierProperties::getListeProprietes() {
	return listeProprietes;
}

inline void FichierProperties::changerFichierProperties(const std::string & cheminAbsolu, const std::string & separateur) {
	AccesFichiers af(separateur);
	listeProprietes = af.lireFichierProperties(cheminAbsolu);
	this->cheminAbsolu = cheminAbsolu;
	this->separateur = separateur;
}

template<typename T>
inline T FichierProperties::valeurProperties(const std::string & clef) const {
	T valeurRetour;
	std::istringstream iss;
	const auto iterateur = listeProprietes.find(clef);
	if (iterateur != listeProprietes.end()) {
		iss.str(iterateur->second);
		iss >> valeurRetour;
	} else {
		throw LibDartException("Key (" + clef + ") not found.");
	}
	return valeurRetour;
}

template<typename T>
inline bool FichierProperties::valeurProperties(const std::string & clef,
												T & valeur) const {
	std::istringstream iss;
	const auto iterateur = listeProprietes.find(clef);
	if (iterateur != listeProprietes.end()) {
		iss.str(iterateur->second);
		iss >> valeur;
		return true;
	} else {
		return false;
	}
}

inline bool FichierProperties::exists(const std::string & clef) const {
	return listeProprietes.find(clef) != listeProprietes.end();
}

inline bool FichierProperties::existsLignesCommencantParClef(const std::string & clef) const {
	auto iterator = listeProprietes.begin();
	const auto endIterator = listeProprietes.end();
	while (iterator != endIterator) {
		if (iterator->first.rfind(clef, 0) == 0)
			return true;
		++iterator;
	}
	return false;
}

inline void FichierProperties::effacerLignesCommencantParClef(const std::string & clef) {
	auto iterator = listeProprietes.begin();
	const auto endIterator = listeProprietes.end();
	while (iterator != endIterator) {
		if (iterator->first.rfind(clef, 0) == 0) {
			// All iterators pointing to an erased element become invalid.
			// So we have to move the iterator to a valid element BEFORE doing the erase operation.
			// The next element is the only one we are certain exists.
			// If we are on the last element in the map, the next iterator is simply the "end" iterator.
			++iterator;
			listeProprietes.erase(std::prev(iterator, 1));
		} else {
			++iterator;
		}
	}
}

inline void FichierProperties::effacerLignesCommencantParClefs(const std::vector<std::string> & clefs) {
	auto iterator = listeProprietes.begin();
	const auto endIterator = listeProprietes.end();
	bool found = false;
	unsigned int i = 0;
	while (iterator != endIterator) {
		found = false;
		i = 0;
		while (!found && i < clefs.size()) {
			if (iterator->first.rfind(clefs[i], 0) == 0) {
				// All iterators pointing to an erased element become invalid.
				// So we have to move the iterator to a valid element BEFORE doing the erase operation.
				// The next element is the only one we are certain exists.
				// If we are on the last element in the map, the next iterator is simply the "end" iterator.
				++iterator;
				listeProprietes.erase(std::prev(iterator, 1));
			} else {
				++iterator;
			}
		}
	}
}

inline void FichierProperties::effacerLignesCommencantParClefEtReecrire(const std::string & clef) {
	effacerLignesCommencantParClef(clef);
	writeFile();
}

inline void FichierProperties::writeFile(std::string absoluteFilePath) {
	if (absoluteFilePath == "") {
		absoluteFilePath = cheminAbsolu;
	}
	std::ofstream fichierProperties(absoluteFilePath.c_str(), std::ios::trunc);
	for (const auto & keyValue : listeProprietes) {
		fichierProperties << keyValue.first << this->separateur << keyValue.second << std::endl;
	}
	fichierProperties.close();
}

inline void FichierProperties::appendToFile(std::string absoluteFilePath) {
	if (absoluteFilePath == "") {
		absoluteFilePath = cheminAbsolu;
	}
	std::ofstream fichierProperties(absoluteFilePath.c_str(), std::ios::app);
	for (const auto & keyValue : listeProprietes) {
		fichierProperties << keyValue.first << this->separateur << keyValue.second << std::endl;
	}
	fichierProperties.close();
}

#endif /* FICHIERPROPERTIES_HPP_ */
