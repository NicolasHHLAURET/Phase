/*
 * BaseDeDonneesInversion.cpp
 *
 *  Created on: 5 mai 2014
 *      Author: guilleuxj
 */

/**
 * SOCI
 *
 * "SELECT P.idInt, P.stringNom From table"
 * avec => struct superTypeDeLaMort {int idInt; string stringNom; }
 * into(vector<superTypeDeLaMort>)  => ne marche pas (pas possible de ce sens avec SOCI ...)
 * into(vector<int>), into(vector<string>) => marche
 */

#include "../include/databases/inversion/BaseDeDonneesInversion.hpp"
#include "../../DartInversion/include/Exception/DonneeInversionParamException.hpp"

using namespace std;

ObjectDatabase BaseDeDonneesInversion::_objectDatabase = ObjectDatabase();
string BaseDeDonneesInversion::_reflectanceDatabase = "";

void BaseDeDonneesInversion::openConnexion() {
	_session.open(_sqlBackend, _cheminVersFichierBDD);
	_session << "PRAGMA journal_mode = OFF";
	_session << "PRAGMA synchronous = OFF";
	_session << "PRAGMA temp_store = MEMORY";
}

void BaseDeDonneesInversion::closeConnexion() {
	_session.close();
}

int BaseDeDonneesInversion::getIdModele(string modelName) {
	int idModele = -1;
	try {
		_session << "SELECT MC.idModeleCoefficient FROM ModeleCoefficient MC WHERE MC.nomModele = ? ;", use(modelName), into(idModele);
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get coefficient model (esteve, hapke ...)." << endl;
		_session.close();
		throw se;
	}
	return idModele;
}
int BaseDeDonneesInversion::getIdSequence() {
	return 1;
}

int BaseDeDonneesInversion::getNbBandesSpectrales() {
	int nbBandesSpectrales = 0;
	try {
		// Puis on recupere l'id
		_session
				<< "SELECT count(distinct BS.idBandeSpectrale) FROM BandeSpectrale BS, Coefficient C WHERE BS.idBandeSpectrale = C.idBandeSpectrale ;", into(
			nbBandesSpectrales);

	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get count of spectral band." << endl;
		_session.close();
		throw se;
	}
	return nbBandesSpectrales;
}

vector<ObjectDatabase::SpectralBand> BaseDeDonneesInversion::getBandesSpectrales() {
	if (_objectDatabase.getBandesSpectrales().size() < 1) {
		try {
			ObjectDatabase::SpectralBand bandeSpectrale;
			statement st =
					(_session.prepare
							<< "SELECT distinct BS.idBandeSpectrale, BS.centralWaveLength, BS.spectralBandwidth FROM BandeSpectrale BS, Coefficient C WHERE C.idBandeSpectrale=BS.idBandeSpectrale ; ", into(
						bandeSpectrale.idSpectralBand), into(bandeSpectrale.valueCentralWavelength), into(bandeSpectrale.valueSpectralBandwidth));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addBandeSpectrale(bandeSpectrale);
			}
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: when try to get spectral band." << endl;
			_session.close();
			throw se;
		}
	}
	return _objectDatabase.getBandesSpectrales();
}

int BaseDeDonneesInversion::getNbParametreInversion(string sequenceName) {
	int nbParametreInversion = 0;
	try {
		// Puis on recupere l'id
		_session << "SELECT count(distinct indexLut) FROM ParametreInversion PI ;", into(nbParametreInversion);

	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get inversion parameters" << endl;
		_session.close();
		throw se;
	}
	return nbParametreInversion;
}

vector<ObjectDatabase::ParametreInversion> BaseDeDonneesInversion::getParametresInversion(string sequenceName) {
	if (_objectDatabase.getParametresInversion().size() < 1) {
		try {
			ObjectDatabase::ParametreInversion parametreInversion;
			indicator ind; // Indicator, pour le traitement specifique de l'idBandeSpectrale qui peut être nul (cas d'un paramêtre non spectral).
			statement st =
					(_session.prepare << "SELECT distinct PI.indexLut, PI.nomParametre, PI.dartSpectralBandKey FROM ParametreInversion PI ; ", into(
						parametreInversion, ind));
			st.execute();
			while (st.fetch()) {
				_objectDatabase.addParametreInversion(parametreInversion);
			}
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: when try to get inversion parameters." << endl;
			cerr << "Try to re-generate lut and inversion database, if you have this error again, contact dart development team." << endl;
			_session.close();
			throw se;
		}
	}
	return _objectDatabase.getParametresInversion();
}

ObjectDatabase::ParametreInversion BaseDeDonneesInversion::getParametreInversion(int indexLut) {
	ObjectDatabase::ParametreInversion parametreInversion;
	vector<ObjectDatabase::ParametreInversion>::iterator itr;
	for (itr = _objectDatabase.getParametresInversion().begin(); itr != _objectDatabase.getParametresInversion().end(); ++itr) {
		if (itr->indexLut == indexLut) {
			return (*itr);
		}
	}
	try {
		statement st =
				(_session.prepare
						<< "SELECT distinct PI.indexLut, PI.nomParametre, PI.dartSpectralBandKey FROM ParametreInversion PI WHERE PI.indexLut=:indexLut ; ", use(
					indexLut), into(parametreInversion));
		st.execute();
		if (st.fetch()) {
			_objectDatabase.addParametreInversion(parametreInversion);
		} else {
			throw soci_error("Parametre inversion introuvable");
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get a specific inversion parameter" << endl;
		_session.close();
		throw se;
	}
	return parametreInversion;
}

int BaseDeDonneesInversion::getIdParametre(const string &nomParametre) {
	int idParametre = -1;
	indicator soci_ind;
	try {
		idParametre = _objectDatabase.getIdParametre(nomParametre);
		if (idParametre < 1) {
			ObjectDatabase::Parameter parametre;
			parametre.nameShortParameter = nomParametre;
			_session << "SELECT idParametre FROM Parametre WHERE nomParametre=:nomParametre", use(parametre.nameShortParameter), into(idParametre, soci_ind);
			if (idParametre > 0 && soci_ind == i_ok) {
				parametre.idParameter = idParametre;
				_objectDatabase.addParametre(parametre);
			} else {
				cerr << "Inversion database Error: parameter not found when try to get interpolation parameters." << endl;
				idParametre = -1;
			}
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get lut parameter." << endl;
		throw se;
	}
	return idParametre;
}

void BaseDeDonneesInversion::init(const string &modelName) {
	_idSequence = getIdSequence();
	_idModele = getIdModele(modelName);
	_nombreCoefficients = getNbCoefficients(modelName);

	int nbRes = -1;
	indicator soci_ind;

	// Pour allouer la bonne taille de not' cache, il faut connaitre le nombre max d'elements
	_session << " SELECT count(*) " << " FROM Coefficient C ", into(nbRes, soci_ind);

	if (nbRes > 0 && soci_ind == i_ok) {
		_cacheParameters.reserve(nbRes);
		_cacheCoefficients.reserve(nbRes);
	}
}

void BaseDeDonneesInversion::getSunAngleFromId(	const int &idSunAngle,
												ObjectDatabase::SolarAngle &angleSolaire) {
	if (_objectDatabase.getAngleSolaireFromId(idSunAngle, angleSolaire)) {} else {
		try {
			_session << "SELECT SA.zenithal, SA.azimuthal FROM AngleSolaire SA WHERE SA.idAngleSolaire = :idAngleSolaire ;", use(idSunAngle,
				"idAngleSolaire"), into(angleSolaire.valueZenithAngle), into(angleSolaire.valueAzimuthAngle);
			_objectDatabase.addAngleSolaire(angleSolaire);
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: when try to get sun angle from id." << endl;
			throw se;
		}
	}
}

void BaseDeDonneesInversion::getBandeSpectraleFromId(	const int idBandeSpectrale,
														ObjectDatabase::SpectralBand &bandeSpectrale) {

	if (_objectDatabase.getBandeSpectraleFromId(idBandeSpectrale, bandeSpectrale)) {} else {
		try {
			_session << "SELECT BS.centralWavelength, BS.spectralBandwidth FROM BandeSpectrale BS WHERE BS.idBandeSpectrale=? ;", use(
				idBandeSpectrale), into(bandeSpectrale.valueCentralWavelength), into(bandeSpectrale.valueSpectralBandwidth);
			_objectDatabase.addBandeSpectrale(bandeSpectrale);
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: error when try to get a specific spectral band" << endl;
			throw se;
		}
	}
}

void BaseDeDonneesInversion::getLutPourInterpolation(	Lut &lut,
														const int &idBandeSpectrale,
														const int &idAngleSolaire,
														const vector<int> &vectorCombinaisons) {
	if (_idSequence < 1 || _idModele < 1 || _nombreCoefficients < 1) {
		throw soci_error("Inversion database Error: when try to get lut for interpolation, database are not initialized");
	}

	int indexLut, nbResultats = -1;
	indicator soci_ind;

	// récupération de la bande spectrale
	ObjectDatabase::SpectralBand spectralBand;
	getBandeSpectraleFromId(idBandeSpectrale, spectralBand);

	// et de l'angle solaire
	ObjectDatabase::SolarAngle sunAngle;
	getSunAngleFromId(idAngleSolaire, sunAngle);

	// Pour allouer la bonne taille de not' liste, il faut connaitre le nombre d'elements
	_session << "SELECT count(*) FROM Coefficient C ;", into(nbResultats, soci_ind);
	if (nbResultats < 1 || soci_ind != i_ok) {
		throw soci_error("Inversion database Error: when try to get lut for interpolation, no results found");
	}
	lut.reserve(nbResultats);

	ostringstream requete;
	requete << " SELECT VC.valeurCoefficient ";
	requete << " FROM Coefficient C, ValeurCoefficient VC ";
	requete << " WHERE VC.idCoefficient = C.idCoefficient ";
	requete << " AND C.idModeleCoefficient = :idModeleCoefficient ";
	requete << " AND C.idBandeSpectrale = :idBandeSpectrale ";
	requete << " AND C.idAngleSolaire = :idAngleSolaire ";
	requete << " AND C.idCombinaison = :idCombinaison ";
	requete << " AND C.idSequence = :idSequence ";
	requete << " ORDER BY VC.numCoefficient ASC ";

	vector<int>::const_iterator itrCombinaison, itrEndCombinaison = vectorCombinaisons.end();
	vector<int> idIndexLuts;
	vector<double> vectorCoefficients;
	vector<string> valNomParametres, valValeurParametres;
	vectorCoefficients.resize(_nombreCoefficients + 1);

	shared_ptr<pair<shared_ptr<InversionParameter>, double> > bandeSpectraleInterpole(
		new pair<shared_ptr<InversionParameter>, double>(_inversionParameters->getSpectralBands(), idBandeSpectrale));
	shared_ptr<pair<shared_ptr<InversionParameter>, double> > sunPhiInterpole(
		new pair<shared_ptr<InversionParameter>, double>(_inversionParameters->getSunPhiAngles(), sunAngle.valueAzimuthAngle));
	shared_ptr<pair<shared_ptr<InversionParameter>, double> > SunTethaInterpole(
		new pair<shared_ptr<InversionParameter>, double>(_inversionParameters->getSunTethaAngles(), sunAngle.valueZenithAngle));

	for (itrCombinaison = vectorCombinaisons.begin(); itrCombinaison != itrEndCombinaison; ++itrCombinaison) {
		Combinaison combinaison;
		indexLut = -1;

		try {
			if (_cacheCoefficients.getInterpolationCoefficients(idBandeSpectrale, idAngleSolaire, (*itrCombinaison), vectorCoefficients,
				valNomParametres, valValeurParametres, idIndexLuts)) {
				_debug.printConsole("Utilisation du cache coefficients");
				// utilisation du cache, pas de requête a faire !
			} else {
				_debug.printConsole("Non utilisation du cache coefficients");

				_session << requete.str(), use(_idModele, "idModeleCoefficient"), use(_idSequence, "idSequence"), use(idBandeSpectrale,
					"idBandeSpectrale"), use(idAngleSolaire, "idAngleSolaire"), use(_idSequence, "idSequence"), use((*itrCombinaison),
					"idCombinaison"), into(vectorCoefficients);

				_debug.printConsole("requete:" + _session.get_last_query());
				_debug.printConsole(
					"params|idModel=" + lcs(_idModele) + "|idBS=" + lcs(idBandeSpectrale) + "|idAS=" + lcs(idAngleSolaire) + "|idCom="
							+ lcs((*itrCombinaison)) + "|idSeq=" + lcs(_idSequence));

				getParametersValgrind((*itrCombinaison), valValeurParametres, valNomParametres, idIndexLuts);

				_cacheCoefficients.addInterpolationCoefficients(idBandeSpectrale, spectralBand.valueCentralWavelength, spectralBand.valueSpectralBandwidth,
					idAngleSolaire, sunAngle.valueZenithAngle, sunAngle.valueAzimuthAngle, (*itrCombinaison), _nombreCoefficients, vectorCoefficients,
					valNomParametres, valValeurParametres, idIndexLuts);

				// Traitement de la spectral band, et des angles;
				SpectralBand& spectralsBands = static_cast<SpectralBand &>(*_inversionParameters->getSpectralBands());
				SunPhiAngle& sunPhiAngles = static_cast<SunPhiAngle &>(*_inversionParameters->getSunPhiAngles());
				SunTethaAngle& sunTethaAngles = static_cast<SunTethaAngle &>(*_inversionParameters->getSunTethaAngles());

				spectralsBands.addSpectralBand(idBandeSpectrale, spectralBand.valueCentralWavelength, spectralBand.valueSpectralBandwidth);
				spectralsBands.majInterpolationValue(idBandeSpectrale);
				sunPhiAngles.majInterpolationValue(sunAngle.valueAzimuthAngle);
				sunTethaAngles.majInterpolationValue(sunAngle.valueZenithAngle);
			}

			// Recuperation des parametres (et de leurs valeurs) de la combinaison.
			for (unsigned int numParametre = 0; numParametre < valValeurParametres.size(); numParametre++) {
				string valeurEnString = valValeurParametres[numParametre];

				indexLut = idIndexLuts[numParametre];
				string nomParametre = valNomParametres[numParametre];
				shared_ptr<InversionParameter> inversionParameter = _inversionParameters->getParameter(indexLut);

				double valeurParametre;
				if (!isNotDoubleValue(valeurEnString)) {
					valeurParametre = std::atof(valValeurParametres[numParametre].c_str());
				} else {
					// Ici, il faut aller chercher bonheur dans la base de donnees reflectance !
					valeurParametre = LireDatabase2D::getReflectanceDansDatabase2DForInterval(valValeurParametres[numParametre],
						getReflectanceDatabase(), ObjectDatabase::getLambdaMinRounded(spectralBand),
						ObjectDatabase::getLambdaMaxRounded(spectralBand));
				}

				combinaison.add(inversionParameter, valeurParametre);
			}

			combinaison.add(bandeSpectraleInterpole);
			combinaison.add(sunPhiInterpole);
			combinaison.add(SunTethaInterpole);

			Coefficients coefficients(vectorCoefficients);

			// Ajout au lut de coefficients
			shared_ptr<pair<Combinaison, Coefficients> > elementLut(new pair<Combinaison, Coefficients>(combinaison, coefficients));
			lut.push_back(elementLut);

		} catch (DonneeInversionParamException &e) {
			cerr << "Inversion database Error: when try to get parameters combinaison" << endl;
			cerr << e.what() << endl;
			throw;
		} catch (...) {
			cerr << "Inversion database Error: when try to get parameters combinaison" << endl;
			throw;
		}
	}
}

const string& BaseDeDonneesInversion::getReflectanceDatabase() {
	if (_reflectanceDatabase == "") {
		throw soci_error("Reflectance database error: path to reflectance database are not defined.");
	}
	return _reflectanceDatabase;
}

void BaseDeDonneesInversion::getParametersValgrind(	const int &idCombinaison,
													vector<string> &valValeurParametres,
													vector<string> &valNomParametres,
													vector<int> &idIndexLuts) {
	try {
		int nbParametres = -1;
		indicator soci_ind;

		_session << " SELECT count(*) FROM ValeurParametre ; ", into(nbParametres, soci_ind);
		if (nbParametres < 1 || soci_ind != i_ok) {
			throw soci_error("Inversion database Error: when try to get lut for interpolation, no parameters found");
		}
		valValeurParametres.resize(nbParametres + 1);
		valNomParametres.resize(nbParametres + 1);
		idIndexLuts.resize(nbParametres + 1);
		_session << "SELECT VP.valeurParametre , P.nomParametre, PI.indexLut "
				" FROM CombinaisonValeurParametre CVP, ValeurParametre VP, Parametre P, ParametreInversion PI "
				" WHERE CVP.idValeurParametre=VP.idValeurParametre "
				" AND CVP.idCombinaison=? "
				" AND VP.idParametre = P.idParametre "
				" AND PI.nomParametre = P.nomParametre "
				" ORDER BY VP.idParametre", use(idCombinaison), into(valValeurParametres), into(valNomParametres), into(idIndexLuts);
	} catch (soci_error &se) {
		cerr << "Inversion database Error: when try to get lut for interpolation, can not find parameters" << endl;
		throw se;
	}
}

int BaseDeDonneesInversion::getIdSunAngle(	const double &valeurZenithal,
											const double &valeurAzimuthal) {
	indicator soci_ind;
	ObjectDatabase::SolarAngle sunAngle;
	sunAngle.valueZenithAngle = fabs(180 - valeurZenithal);
	sunAngle.valueAzimuthAngle = fmod((180 + valeurAzimuthal), 360);
	if ((sunAngle.idSolarAngle = _objectDatabase.getIdAngleSolaire(sunAngle)) < 1) {

		double minZenithal = sunAngle.valueZenithAngle - ObjectDatabase::getEpsilon();
		double maxZenithal = sunAngle.valueZenithAngle + ObjectDatabase::getEpsilon();
		double minAzimuthal = sunAngle.valueAzimuthAngle - ObjectDatabase::getEpsilon();
		double maxAzimuthal = sunAngle.valueAzimuthAngle + ObjectDatabase::getEpsilon();

		try {
			_session << " SELECT SA.idAngleSolaire " << " FROM AngleSolaire SA " << " WHERE SA.zenithal > :minZenithal "
					<< " AND SA.zenithal < :maxZenithal " << " AND SA.azimuthal > :minAzimuthal " << " AND SA.azimuthal < :maxAzimuthal ", use(
				minZenithal, "minZenithal"), use(maxZenithal, "maxZenithal"), use(minAzimuthal, "minAzimuthal"), use(maxAzimuthal, "maxAzimuthal"), into(
				sunAngle.idSolarAngle, soci_ind);
			_objectDatabase.addAngleSolaire(sunAngle);
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: when try to get sun angle." << endl;
			throw se;
		}
		if (sunAngle.idSolarAngle < 1 || !(soci_ind == i_ok)) {
			throw soci_error("Inversion database Error: when try to get sun angle, result not found\nPlease check if sun angles of your images is valid ?");
		}
	}
	return sunAngle.idSolarAngle;
}

/**
 * Retourne les valeur de paramètres d'un paramètre pour une combinaison partielle donnée.
 * @param: parametreAInterpoler: Le paramètre dont on veut les valeurs
 * @param: lutParametres: Le lut contenant les paramètres/valeur qu'on a déjà pour l'interpolation
 * -> Exemple, pour la spectral band 0.6 nm, un coeff' k de 0.4 et 0.5 à été défini, et pour la
 * spectral band 0.8nm, le coeff' k est de 0.6 et 0.7, on doit donc extraire paramètres par paramètres
 * pour l'interpolation (et prendre en compte les paramètres déjà extraits)
 */
void BaseDeDonneesInversion::getParametresCombinaisonInterpolation(	const Combinaison::Parametre &parametreAInterpoler,
																	vector<int> &vectorCombinaisonExtraites,
																	const int &spectralBandKey,
																	const int &nbPointsInterpolation) {

	int idParametre = getIdParametre(parametreAInterpoler->first->getName());
	int nbCombinaison = 0;
	vector<int> combinaisonsParametre;
	indicator soci_ind;
	try {
		if (parametreAInterpoler->first->getIndexLut() > 0) {
			if (_cacheParameters.getInterpolationParameterCombinaison(spectralBandKey, 0, idParametre, parametreAInterpoler->second,
				combinaisonsParametre)) {
				// Nothing to do ...
			} else {
				_session << " SELECT count(CVP.idCombinaison) " << " FROM CombinaisonValeurParametre CVP ;", into(nbCombinaison, soci_ind);
				if (nbCombinaison > 0 && soci_ind == i_ok) {
					vector<int> idParametrePrint;
					vector<string> valeurParametrePrint;
					idParametrePrint.resize(nbCombinaison + 1);
					valeurParametrePrint.resize(nbCombinaison + 1);
					combinaisonsParametre.resize(nbCombinaison + 1);

					stringstream requete; // Juste pour feinter le formateur automatique et indenter la requête CORRECTEMENT !! *Rire sadique*
					requete << " SELECT DISTINCT VP.idParametre, VP.valeurParametre, CVP.idCombinaison ";
					requete << " FROM CombinaisonValeurParametre CVP, ValeurParametre VP ";
					requete << " WHERE CVP.idValeurParametre = VP.idValeurParametre ";
					requete << " AND (CVP.idValeurParametre IN ";
					requete << " (SELECT DISTINCT VP1.idValeurParametre ";
					requete << "  FROM ValeurParametre VP1, CombinaisonValeurParametre CVP1 ";
					requete << "  WHERE VP1.valeurParametre < :valP ";
					requete << "  AND CVP1.idValeurParametre = VP1.idValeurParametre ";
					requete << "  AND VP1.idParametre = :idP ";
					requete << "  AND CVP1.idCombinaison IN (SELECT idCombinaison FROM Coefficient WHERE idBandeSpectrale = :idBS) ";
					requete << "  ORDER BY VP1.valeurParametre DESC ";
					requete << "  LIMIT :lim) ";
					requete << " OR CVP.idValeurParametre in ";
					requete << " (SELECT DISTINCT VP3.idValeurParametre ";
					requete << "  FROM ValeurParametre VP3, CombinaisonValeurParametre CVP3 ";
					requete << "  WHERE VP3.valeurParametre = :valP ";
					requete << "  AND CVP3.idValeurParametre = VP3.idValeurParametre ";
					requete << "  AND VP3.idParametre = :idP ";
					requete << "  AND CVP3.idCombinaison IN (SELECT idCombinaison FROM Coefficient WHERE idBandeSpectrale = :idBS)) ";
					requete << " OR CVP.idValeurParametre in ";
					requete << " (SELECT DISTINCT VP2.idValeurParametre ";
					requete << "  FROM ValeurParametre VP2, CombinaisonValeurParametre CVP2 ";
					requete << "  WHERE VP2.valeurParametre > :valP ";
					requete << "  AND CVP2.idValeurParametre = VP2.idValeurParametre ";
					requete << "  AND VP2.idParametre = :idP ";
					requete << "  AND CVP2.idCombinaison IN (SELECT idCombinaison FROM Coefficient WHERE idBandeSpectrale = :idBS) ";
					requete << "  ORDER BY VP2.valeurParametre ASC ";
					requete << "  LIMIT :lim)) ";
					requete << " ORDER BY VP.idParametre ASC, VP.valeurParametre ASC";

					_session << requete.str(), use(idParametre, "idP"), use(parametreAInterpoler->second, "valP"), use(spectralBandKey, "idBS"), use(
						nbPointsInterpolation, "lim"), into(idParametrePrint), into(valeurParametrePrint), into(combinaisonsParametre);

					// Ajout au cache
					int nbTest = -1;
					_session << " SELECT count(VP.idValeurParametre) " << " FROM ValeurParametre VP " << " WHERE VP.idParametre = :idParametre "
							<< " AND VP.valeurParametre = :valeurParametre ", use(idParametre, "idParametre"), use(parametreAInterpoler->second,
						"valeurParametre"), into(nbTest, soci_ind);
					if (nbTest == 0 && soci_ind == i_ok) {
						double min = -4269;
						double max = -4269;
						_session << " SELECT VP1.valeurParametre, VP2.valeurParametre " << " FROM ValeurParametre VP1, ValeurParametre VP2 "
								<< " WHERE VP1.idParametre = :idParametre " << " AND VP2.idParametre = :idParametre "
								<< " AND VP1.valeurParametre < :valeurParametre " << " AND VP2.valeurParametre > :valeurParametre "
								<< " ORDER BY VP1.valeurParametre DESC, VP2.valeurParametre ASC " << " LIMIT 1 ", use(idParametre, "idParametre"), use(
							parametreAInterpoler->second, "valeurParametre"), into(min, soci_ind), into(max, soci_ind);
						_cacheParameters.addInterpolationParameter(spectralBandKey, 0, idParametre, 42, min, max, combinaisonsParametre);
					} else {
						// Cas particulier (paramètre "constant")
						if (nbTest == 1 && soci_ind == i_ok) {
							_cacheParameters.addInterpolationParameter(spectralBandKey, 0, idParametre, 42, parametreAInterpoler->second,
								parametreAInterpoler->second, combinaisonsParametre);
						}
					}
				} else {
					throw soci_error("Inversion database Error: when try to get count of coefficient for a specific model.");
				}
			}
			makeIntersectionInversionCombinaison(vectorCombinaisonExtraites, combinaisonsParametre);
		}
	} catch (const soci_error & se) {
		cerr << "idParametre: " << idParametre << endl;
		cerr << "parametre a interpoler: " << parametreAInterpoler->second << endl;
		cerr << "Inversion database Error: when try to get count of coefficient for a specific model." << endl;
		throw se;
	}
}

/**
 * Réalise l'intersection de combinaisons appartenant à un set de paramètres.
 * Fonction mise en place à cause des limitations d'SQLite au niveau de la taille des requêtes, jointures, ...
 * Afin d'éviter de devoir faire "trop" de requêtes, on traite par informatique.
 * On retourne toutes les combinaisons en commun
 * On n'utilise pas la fonction de la stl, car elle necessite des vecteurs d'éléments trié (sort) et on
 * veut conserver l'order by de la base de données.
 * */
void BaseDeDonneesInversion::makeIntersectionInversionCombinaison(	vector<int> &baseVector,
																	const vector<int> &intersectVector) const {
	if (intersectVector.empty()) {
		cerr << "Inversion Error: when try to make combinaison intersection : combinaison empty" << endl;
	} else {
		if (baseVector.empty()) {
			baseVector.assign(intersectVector.begin(), intersectVector.end());
		} else {
			vector<int> resultVector;
			vector<int>::const_iterator itrBase, itrEndBase = baseVector.end();
			vector<int>::const_iterator itrInter, itrEndInter = intersectVector.end();

			resultVector.reserve(intersectVector.size());
			for (itrInter = intersectVector.begin(); itrInter != itrEndInter; ++itrInter) {
				for (itrBase = baseVector.begin(); itrBase != itrEndBase; ++itrBase) {
					if ((*itrBase) == (*itrInter)) {
						resultVector.push_back(*itrInter);
						break;
					}
				}
			}
			baseVector.swap(resultVector);
		}
	}
}

int BaseDeDonneesInversion::getNbCoefficients(string modelName) {
	int nbCoeff = -1;
	try {
		_session << "SELECT MC.nombreDeCoefficients FROM ModeleCoefficient MC WHERE MC.nomModele=:nomModele", use(modelName), into(nbCoeff);
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: when try to get count of coefficient for a specific model." << endl;
		throw se;
	}
	return nbCoeff;
}

bool BaseDeDonneesInversion::isAnInversionDatabase() {
	try {
		_session << "SELECT 42 FROM ModeleCoefficient, Coefficient, BandeSpectrale, ParametreInversion";
	} catch (const soci_error & se) {
		return false; //On cherche pas à savoir, ça marche pas, il y a un problème avec la babase
	}
	return true;
}

int BaseDeDonneesInversion::compterCoefficients(const string &modelName) {
	int idSequence = getIdSequence();
	int idModele = -1;
	int nbCoefficients = -1;
	try {
		_session << "SELECT MC.idModeleCoefficient FROM ModeleCoefficient MC WHERE MC.nomModele=:nomModele ;", use(modelName), into(idModele);
		_session
				<< "SELECT count(distinct C.idCoefficient) FROM Coefficient C WHERE C.idModeleCoefficient=:idModeleCoefficient AND C.idSequence=:idSequence ;", use(
			idModele), use(idSequence), into(nbCoefficients);

	} catch (const soci_error & se) {
		cerr << "Inversion database Error: error when try to get count of stored coefficients" << endl;
		throw se;
	}
	return nbCoefficients;
}

int BaseDeDonneesInversion::getIdParametreInversion(string parametreName) {

	ObjectDatabase::ParametreInversion parametreInversion;
	parametreInversion.nomParametre = parametreName;
	parametreInversion.indexLut = _objectDatabase.getIdParametreInversion(parametreInversion);
	if (parametreInversion.indexLut < 1) {
		try {
			_session
					<< "SELECT PI.indexLut, PI.nomParametre, PI.dartSpectralBandKey FROM ParametreInversion PI WHERE PI.nomParametre=:nomParametre ;", use(
				parametreName), into(parametreInversion);
			if (parametreInversion.indexLut == -1) {
				throw soci_error("Inversion database Error: Inversion parameter not found: " + parametreName);
			}
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: Inversion parameters not found" << endl;
			throw se;
		}
	}
	return parametreInversion.indexLut;
}

void BaseDeDonneesInversion::getBandeSpectrale(	const int idBandeSpectrale,
												double &centralWavelength,
												double &spectralBandwidth) {

	ObjectDatabase::SpectralBand bandeSpectrale;
	if (_objectDatabase.getBandeSpectraleFromId(idBandeSpectrale, bandeSpectrale)) {
		centralWavelength = bandeSpectrale.valueCentralWavelength;
		spectralBandwidth = bandeSpectrale.valueSpectralBandwidth;
	} else {
		try {
			_session << "SELECT BS.centralWavelength, BS.spectralBandwidth FROM BandeSpectrale BS WHERE BS.idBandeSpectrale=? ;", use(
				idBandeSpectrale), into(centralWavelength), into(spectralBandwidth);
			bandeSpectrale.idSpectralBand = idBandeSpectrale;
			bandeSpectrale.valueCentralWavelength = centralWavelength;
			bandeSpectrale.valueSpectralBandwidth = spectralBandwidth;
			_objectDatabase.addBandeSpectrale(bandeSpectrale);
		} catch (const soci_error & se) {
			cerr << "Inversion database Error: error when try to get a specific spectral band" << endl;
			throw se;
		}
	}
}

/**
 * Remplace les noms de sols par leurs valeurs en fonction de la bande spectrale associée
 * en base de données afin de permettre l'interpolation
 * Todo : pas testé !
 */
void BaseDeDonneesInversion::majLambertianReflectanceBySpectralBands() {
	bool majEffectuee;
	do {
		majEffectuee = true;
		ObjectDatabase::ValueParameter valeursParametres;
		getAllValeursParametresInterpolable(valeursParametres);
		for (unsigned int numValeurParametre = 0; numValeurParametre < valeursParametres.idValueParameter.size(); ++numValeurParametre) {
			if (isNotDoubleValue(valeursParametres.valueParameter[numValeurParametre])) {
				vector<int> vectorCombinaisons;
				getCombinaisonsForValeurParametre(vectorCombinaisons, valeursParametres.idValueParameter[numValeurParametre]);
				vector<int>::const_iterator itrVectorCombinaisons, itrEndVectorCombinaisons = vectorCombinaisons.end();
				for (itrVectorCombinaisons = vectorCombinaisons.begin(); itrVectorCombinaisons != itrEndVectorCombinaisons; ++itrVectorCombinaisons) {
					ObjectDatabase::get_BandesSpectrales spectralsBands;
					getSpectralBandsForCombinaison(spectralsBands, *itrVectorCombinaisons);
					for (unsigned int numBandeSpectrale = 0; numBandeSpectrale < spectralsBands.idBandeSpectrale.size(); ++numBandeSpectrale) {
						ObjectDatabase::SpectralBand bs;
						bs.valueCentralWavelength = spectralsBands.centralWavelength[numBandeSpectrale];
						bs.valueSpectralBandwidth = spectralsBands.spectralBandwidth[numBandeSpectrale];
						double lambertianValueForBand = LireDatabase2D::getReflectanceDansDatabase2DForInterval(
							valeursParametres.valueParameter[numValeurParametre], getReflectanceDatabase(), ObjectDatabase::getLambdaMinRounded(bs),
							ObjectDatabase::getLambdaMaxRounded(bs));
						int combiWithLambValue = createOrGetCombinaison((*itrVectorCombinaisons), valeursParametres.idValueParameter[numValeurParametre],
							valeursParametres.idValueParameter[numValeurParametre], lambertianValueForBand);
						majCombinaisonForCoefficients((*itrVectorCombinaisons), spectralsBands.idBandeSpectrale[numBandeSpectrale],
							combiWithLambValue);
					}
				}
				// On break le premier 'for' pour update le vecteur des valeurs avec celles qu'on viens d'inserer;
				majEffectuee = false;
				break;
			}
		}
	} while (!majEffectuee);
}

void BaseDeDonneesInversion::getAllValeursParametresInterpolable(ObjectDatabase::ValueParameter &parametres) {
	try {
		int nbValeurs = -1;
		indicator soci_ind;
		_session << " SELECT count(*) FROM ValeurParametre; ", into(nbValeurs, soci_ind);
		if (nbValeurs > 0 && soci_ind == i_ok) {
			parametres.idValueParameter.resize(nbValeurs + 1);
			parametres.valueParameter.resize(nbValeurs + 1);
			_session << " SELECT DISTINCT VP.idValeurParametre, VP.valeurParametre, VP.idParametre "
					<< " FROM ValeurParametre VP, CombinaisonValeurParametre CVP, Coefficient C "
					<< " WHERE VP.idValeurParametre = CVP.idValeurParametre " << " AND CVP.idCombinaison = C.idCombinaison", into(
				parametres.idValueParameter), into(parametres.valueParameter), into(parametres.idValueParameter);
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: update with lambertian values : error when try to get parameters for interpolation" << endl;
		throw se;
	}
}

bool BaseDeDonneesInversion::isNotDoubleValue(const string &value) {
	for (unsigned int numChar = 0; numChar < value.length(); numChar++) {
		// Si on rencontre un caractere qui n'est pas un chiffre (ou un point)
		// C'est pas un double, mais un nom de quelque chose qu'il faudra traiter
		// (Base de donnees lambertian, toussa toussa (et compote banane !))
		if ((value.at(numChar) < '0' || value.at(numChar) > '9') && value.at(numChar) != '.') {
			return true;
		}
	}
	return false;
}

void BaseDeDonneesInversion::getCombinaisonsForValeurParametre(	vector<int> &combinaisons,
																const int &idValeurParametre) {
	try {
		int nbValeurs = -1;
		indicator soci_ind;
		_session << " SELECT count(*) " << " FROM CombinaisonValeurParametre " << " WHERE idValeurParametre=:idValeurParametre; ", use(
			idValeurParametre, "idValeurParametre"), into(nbValeurs, soci_ind);
		if (nbValeurs > 0 && soci_ind == i_ok) {
			combinaisons.resize(nbValeurs + 1);
			_session << " SELECT DISTINCT CVP.idCombinaison " << " FROM CombinaisonValeurParametre CVP"
					<< " WHERE idValeurParametre=:idValeurParametre; ", use(idValeurParametre, "idValeurParametre"), into(combinaisons);
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: update with lambertian values : error when try to get conbinaisons for lambertian parameter" << endl;
		throw se;
	}
}

void BaseDeDonneesInversion::getSpectralBandsForCombinaison(ObjectDatabase::get_BandesSpectrales& spectralBands,
															const int &idCombinaison) {
	try {
		int nbValeurs = -1;
		indicator soci_ind;
		_session << " SELECT count(*) " << " FROM BandeSpectrale ", into(nbValeurs, soci_ind);
		if (nbValeurs > 0 && soci_ind == i_ok) {
			spectralBands.idBandeSpectrale.resize(nbValeurs + 1);
			spectralBands.centralWavelength.resize(nbValeurs + 1);
			spectralBands.spectralBandwidth.resize(nbValeurs + 1);

			_session << " SELECT DISTINCT BS.idBandeSpectrale, BS.centralWavelength, BS.spectralBandwidth "
					<< " FROM BandeSpectrale BS, Coefficient C " << " WHERE C.idBandeSpectrale=BS.idBandeSpectrale "
					<< " AND C.idCombinaison=:idCombinaison ; ", use(idCombinaison, "idCombinaison"), into(spectralBands.idBandeSpectrale), into(
				spectralBands.centralWavelength), into(spectralBands.spectralBandwidth);
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: update with lambertian values : error when try to get spectral bands for combinaison interpolation"
				<< endl;
		throw se;
	}
}

/**
 * Récupère (et créé si elle n'existe pas) une combinaison associée à une valeur lambertienne à
 * partir d'une combinaison contenant les autres paramètres;
 */
int BaseDeDonneesInversion::createOrGetCombinaison(	const int &idCombinaison,
													const int &idParametre,
													const int idValeurParametre,
													const double &lambertianValueForBand) {
	int idNewCombinaison = -1;
	int idLambertianValue;
	vector<int> idValeurParametres;
	vector<int>::const_iterator itrIdValeurParametres;
	indicator soci_ind;
	try {
		// On zyeute si la valeur lambertienne calculée est déjà présente en base de données;
		_session << " SELECT DISTINT VP.idValeurParametre " << " FROM ValeurParametre VP " << " WHERE VP.idParametre=:idParametre "
				<< " AND VP.valeurParametre=:valeurParametre ", use(idParametre, "idParametre"), use(lambertianValueForBand, "valeurParametre"), into(
			idLambertianValue, soci_ind);

		// La valeur lambertienne n'existe pas en base, on l'insère;
		if (!(soci_ind == i_ok)) {
			_session << "INSERT INTO ValeurParametre(idParametre, valeurParametre) " << " VALUES(:idParametre, :valeurParametre);", use(idParametre,
				"idParametre"), use(lambertianValueForBand, "valeurParametre");
			_session << " SELECT DISTINT VP.idValeurParametre " << " FROM ValeurParametre VP " << " WHERE VP.idParametre=:idParametre "
					<< " AND VP.valeurParametre=:valeurParametre ", use(idParametre, "idParametre"), use(lambertianValueForBand, "valeurParametre"), into(
				idLambertianValue, soci_ind);
		}

		// On zyeute si la combinaison existe en base de données;
		_session << " SELECT DISTINCT CVP.idValeurParametre " << " FROM CombinaisonValeurParametre CVP "
				<< " WHERE CVP.idCombinaison = :idCombinaison " << " AND CVP.idValeurParametre != :idValeurParametre ", use(idCombinaison,
			"idCombinaison"), use(idValeurParametre, "idValeurParametre"), into(idValeurParametres);
		stringstream requete;
		requete << " SELECT DISTINCT CVP.idCombinaison " << " FROM CombinaisonValeurParametre CVP "
				<< " WHERE CVP.idValeurParametre=:idValeurParametre ";
		for (itrIdValeurParametres = idValeurParametres.begin(); itrIdValeurParametres != idValeurParametres.end(); ++itrIdValeurParametres) {
			requete << " AND CVP.idCombinaison EXISTS (" << " SELECT idCombinaison " << " FROM CombinaisonValeurParametre "
					<< " WHERE idValeurParametre = " << (*itrIdValeurParametres) << " ) ";
		}
		_session << requete.str(), use(idLambertianValue, "idValeurParametre"), into(idNewCombinaison, soci_ind);

		// La combinaison n'existe pas, on l'insère en base de données;
		if (idNewCombinaison < 1 || !(soci_ind == i_ok)) {
			_session << " SELECT MAX(idCombinaison) " << " FROM Combinaison C ", into(idNewCombinaison, soci_ind);
			if (!(soci_ind == i_ok)) {
				idNewCombinaison = 0;
			}
			idNewCombinaison++;
			_session << " INSERT INTO Combinaison(idCombinaison, cardinal) VALUES (:idCombinaison, :cardinal);", use(idNewCombinaison,
				"idCombinaison"), use(idValeurParametres.size() + 1, ":cardinal");
			_session << " INSERT INTO CombinaisonValeurParametre(idCombinaison, idValeurParametre) "
					<< " VALUES (:idCombinaison, :idValeurParametre);", use(idNewCombinaison, "idCombinaison"), use(idLambertianValue,
				":idValeurParametre");
			for (itrIdValeurParametres = idValeurParametres.begin(); itrIdValeurParametres != idValeurParametres.end(); ++itrIdValeurParametres) {
				_session << " INSERT INTO CombinaisonValeurParametre(idCombinaison, idValeurParametre) "
						<< " VALUES (:idCombinaison, :idValeurParametre);", use(idNewCombinaison, "idCombinaison"), use((*itrIdValeurParametres),
					":idValeurParametre");
			}
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: update with lambertian values : error when try to get or create a new combinaison" << endl;
		throw se;
	}
	return idNewCombinaison;
}

void BaseDeDonneesInversion::majCombinaisonForCoefficients(	const int &idOldCombinaison,
															const int &idBandeSpectrale,
															const int &idNewCombinaison) {
	try {
		_session << " UPDATE Coefficient " << " SET idCombinaison=:newCombinaison " << " WHERE idCombinaison=:oldCombinaison "
				<< " AND idBandeSpectrale=:idBandeSpectrale ", use(idOldCombinaison, "oldCombinaison"), use(idNewCombinaison, "newCombinaison"), use(
			idBandeSpectrale, "idBandeSpectrale");
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: update with lambertian values : error when try to update coefficients" << endl;
		throw se;
	}
}

void BaseDeDonneesInversion::getMinMaxParameter(const string &nomParametre,
												double &min,
												double &max) {
	try {
		indicator soci_ind_min, soci_ind_max;
		_session << " SELECT MIN(valeurParametre), MAX(valeurParametre) " << " FROM ValeurParametre " << " WHERE idParametre = :idParametre ;", use(
			getIdParametre(nomParametre), "idParametre"), into(min, soci_ind_min), into(max, soci_ind_max);
		if (soci_ind_min != i_ok || soci_ind_max != i_ok) {
			throw soci_error("indicator error");
		}
	} catch (const soci_error & se) {
		cerr << "Inversion database Error: error when try to get interpolation min and max values" << endl;
		throw se;
	}
}

void BaseDeDonneesInversion::setReflectanceDatabase(const string &nomDatabase) {
	_reflectanceDatabase = nomDatabase;
}

/**
 * Bloc de mappinge de la base de donnees avec l'interface SOCI.
 * http://soci.sourceforge.net/doc/3.2/exchange.html#data_states
 */
namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::ParametreInversion> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator & ind,
								ObjectDatabase::ParametreInversion & parametreInversion) {

			parametreInversion.indexLut = value.get<int>("indexLut");
			parametreInversion.nomParametre = value.get<string>("nomParametre");
			if (value.get_indicator("dartSpectralBandKey") == i_null) {
				parametreInversion.dartSpectralBandKey = -1;
			} else {
				parametreInversion.dartSpectralBandKey = value.get<int>("dartSpectralBandKey");
			}
		}

		static void to_base(const ObjectDatabase::ParametreInversion & parametreInversion,
							values & value,
							indicator & ind) {
			value.set("indexLut", parametreInversion.indexLut);
			value.set("nomParametre", parametreInversion.nomParametre);
			value.set("dartSpectralBandKey", parametreInversion.dartSpectralBandKey);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::ModeleCoefficient> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::ModeleCoefficient & modeleCoefficient) {
			modeleCoefficient.idModeleCoefficient = value.get<int>("idModeleCoefficient");
			modeleCoefficient.nomModele = value.get<string>("nomModele");
			modeleCoefficient.nombreDeCoefficients = value.get<int>("nombreDeCoefficients");
		}

		static void to_base(const ObjectDatabase::ModeleCoefficient & modeleCoefficient,
							values & value,
							indicator & ind) {
			value.set("idModeleCoefficient", modeleCoefficient.idModeleCoefficient);
			value.set("nomModele", modeleCoefficient.nomModele);
			value.set("nombreDeCoefficients", modeleCoefficient.nombreDeCoefficients);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::Coefficient> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::Coefficient & coefficient) {
			coefficient.idCoefficient = value.get<int>("idCoefficient");
			coefficient.idModeleCoefficient = value.get<int>("idModeleCoefficient");
			coefficient.rmse = value.get<double>("rmse");
			coefficient.estLuminance = value.get<int>("estLuminance");
			coefficient.idCombinaison = value.get<int>("idCombinaison");
			coefficient.idAngleSolaire = value.get<int>("idAngleSolaire");
			coefficient.idBandeSpectrale = value.get<int>("idBandeSpectrale");
			coefficient.idSequence = value.get<int>("idSequence");
			coefficient.positionDuCapteur = value.get<int>("positionDuCapteur");
		}

		static void to_base(const ObjectDatabase::Coefficient & coefficient,
							values & value,
							indicator & ind) {
			value.set("idCoefficient", coefficient.idCoefficient);
			value.set("estLuminance", coefficient.estLuminance);
			value.set("rmse", coefficient.rmse);
			value.set("estLuminance", coefficient.estLuminance);
			value.set("idCombinaison", coefficient.idCombinaison);
			value.set("idAngleSolaire", coefficient.idAngleSolaire);
			value.set("idBandeSpectrale", coefficient.idBandeSpectrale);
			value.set("idSequence", coefficient.idSequence);
			value.set("positionDuCapteur", coefficient.positionDuCapteur);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::ValeurCoefficient> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::ValeurCoefficient & valeurCoefficient) {
			valeurCoefficient.idCoefficient = value.get<int>("idCoefficient");
			valeurCoefficient.numCoefficient = value.get<int>("numCoefficient");
			valeurCoefficient.valeurCoefficient = value.get<double>("valeurCoefficient");
		}

		static void to_base(const ObjectDatabase::ValeurCoefficient & valeurCoefficient,
							values & value,
							indicator & ind) {
			value.set("idCoefficient", valeurCoefficient.idCoefficient);
			value.set("numCoefficient", valeurCoefficient.numCoefficient);
			value.set("valeurCoefficient", valeurCoefficient.valeurCoefficient);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::SpectralBand> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::SpectralBand & spectralBand) {
			spectralBand.idSpectralBand = value.get<int>("idSpectralBand");
			spectralBand.valueCentralWavelength = value.get<double>("valueCentralWavelength");
			spectralBand.valueSpectralBandwidth = value.get<double>("valueSpectralBandwidth");
		}

		static void to_base(const ObjectDatabase::SpectralBand & spectralBand,
							values & value,
							indicator & ind) {
			value.set("idSpectralBand", spectralBand.idSpectralBand);
			value.set("valueCentralWavelength", spectralBand.valueCentralWavelength);
			value.set("valueSpectralBandwidth", spectralBand.valueSpectralBandwidth);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::SolarAngle> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::SolarAngle & angle) {
			angle.idSolarAngle = value.get<int>("idSolarAngle");
			angle.valueZenithAngle = value.get<double>("valueZenithAngle");
			angle.valueAzimuthAngle = value.get<double>("valueAzimuthAngle");
		}

		static void to_base(const ObjectDatabase::SolarAngle & angle,
							values & value,
							indicator & ind) {
			value.set("idSolarAngle", angle.idSolarAngle);
			value.set("valueZenithAngle", angle.valueZenithAngle);
			value.set("valueAzimuthAngle", angle.valueAzimuthAngle);
			ind = i_ok;
		}
	};
}

namespace soci {
	template<>
	struct type_conversion<ObjectDatabase::Simulation> {
		typedef values base_type;

		static void from_base(	values const & value,
								indicator,
								ObjectDatabase::Simulation & simulation) {
			simulation.idSimulation = value.get<int>("idSimulation");
			simulation.nameSimulation = value.get<string>("nameSimulation");
		}

		static void to_base(const ObjectDatabase::Simulation & simulation,
							values & value,
							indicator & ind) {
			value.set("idSimulation", simulation.idSimulation);
			value.set("nameSimulation", simulation.nameSimulation);
			ind = i_ok;
		}
	};
}
