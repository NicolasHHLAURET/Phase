/*
 * WriteJobIlwis.hpp
 *
 *  Created on: 19 avr. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_LIBILWIS_WRITEJOBILWIS_HPP_
#define INCLUDE_LIBILWIS_WRITEJOBILWIS_HPP_

#include "WriteIlwis.hpp"

class WriteJobIlwis: JobWriter, WriteIlwis {
private:
	~WriteJobIlwis() {}

	bool _grdiezePrepare;
	string _grdiezerepertoireImage;
	string _grdiezenomImage;
	vector<vector<vector<double> > > _grdiezecoordonneesGeoreferentiel;
	unsigned int _grdiezenombreLignes;
	unsigned int _grdiezenombreColonnes;

	bool _grfPrepare;
	string _grfrepertoireImage;
	string _grfnomImage;
	unsigned int _grfnombreLignes;
	unsigned int _grfnombreColonnes;

	bool _mprPrepare;
	string _mprrepertoireImage;
	string _mprnomImage;
	unsigned int _mprnombreLignes;
	unsigned int _mprnombreColonnes;
	float _mprdeltaX;
	float _mprdeltaY;
	double _mprvalMinImage;
	double _mprvalMaxImage;
	bool _mprcreateGeo;

public:

	void prepareGrDieze(const string & repertoireImage,
						const string & nomImage,
						const vector<vector<vector<double> > > & coordonneesGeoreferentiel,
						const unsigned int & nombreLignes,
						const unsigned int & nombreColonnes) {
		_grdiezerepertoireImage = repertoireImage;
		_grdiezenomImage = nomImage;
		_grdiezecoordonneesGeoreferentiel = coordonneesGeoreferentiel;
		_grdiezenombreLignes = nombreLignes;
		_grdiezenombreColonnes = nombreColonnes;
		_grdiezePrepare = true;
	}

	void prepareGrf(const string & repertoireImage,
					const string & nomImage,
					const unsigned int & nombreLignes,
					const unsigned int & nombreColonnes) {
		_grfrepertoireImage = repertoireImage;
		_grfnomImage = nomImage;
		_grfnombreLignes = nombreLignes;
		_grfnombreColonnes = nombreColonnes;
		_grfPrepare = true;
	}

	void prepareMpr(const string & repertoireImage,
					const string & nomImage,
					const unsigned int & nombreLignes,
					const unsigned int & nombreColonnes,
					const float & deltaX,
					const float & deltaY,
					const double & valMinImage,
					const double & valMaxImage,
					bool createGeo = false) {
		_mprrepertoireImage = repertoireImage;
		_mprnomImage = nomImage;
		_mprnombreLignes = nombreLignes;
		_mprnombreColonnes = nombreColonnes;
		_mprdeltaX = deltaX;
		_mprdeltaY = deltaY;
		_mprvalMinImage = valMinImage;
		_mprvalMaxImage = valMaxImage;
		_mprcreateGeo = createGeo;
		_mprPrepare = true;
	}

	void write() {
		if (_grdiezePrepare) {
			WriteIlwis::ecrireFichierGRdiese(_grdiezerepertoireImage, _grdiezenomImage, _grdiezecoordonneesGeoreferentiel, _grdiezenombreLignes,
				_grdiezenombreColonnes);
		}
		if (_grfPrepare) {
			WriteIlwis::ecrireFichierGRF(_grfrepertoireImage, _grfnomImage, _grfnombreLignes, _grfnombreColonnes);
		}
		if (_mprPrepare) {
			WriteIlwis::ecrireFichierMPR(_mprrepertoireImage, _mprnomImage, _mprnombreLignes, _mprnombreColonnes, _mprdeltaX, _mprdeltaY, _mprvalMinImage,
				_mprvalMaxImage, _mprcreateGeo);
		}
	}
};

#endif /* INCLUDE_LIBILWIS_WRITEJOBILWIS_HPP_ */
