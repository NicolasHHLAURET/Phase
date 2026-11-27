/*
 * DiscreteBSDFMaterialWriter.hpp
 *
 *  Created on: 26 nov. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_DISCRETEBSDFMATERIALWRITER_HPP_
#define INCLUDE_MATERIALS_DISCRETEBSDFMATERIALWRITER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include "GeneralDef.hpp"
#include "PhaseDTOs.hpp"
#include "PhaseProperty.hpp"
#include "PhaseStructure.hpp"
#include "PhaseUtils.hpp"
#include "libDartEnv.h"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"
#include "containers/NearestDirections.hpp"
#include "containers/phaseModel/OpticalPropertyParameters.hpp"
#include "database/model/PhaseExternDatabaseParameters.hpp"

using namespace std;

/* Header-only computation core moved from phaseExtern/PhaseExternComputation
 * (item 33): the DiscreteBSDF (PhaseExtern) material is now a MaterialWriter. */
namespace phase {
class DiscreteBSDFMaterialWriter {
public:
	// LuxCore material name (the part after "scene.materials."): shared with
	// SurfaceMaterialProcessor so mixed/specular materials can reference this material.
	static std::string getMaterialName(unsigned int indexFunction);

	bool treat(	const phsdata::SurfaceFunction & sf,
				PhaseGeneralParametersDTO & genDTO,
				const std::vector<bool> & useSpectralIrradianceForReflectance,
				unsigned int materialIndex,
				PhaseKeyMappingGenerator & pkmg,
				std::ofstream & outCoeffSpecProperties) const;

	// Writes the LuxCore scene keys of a DiscreteBSDF (PhaseExtern) material:
	// type, dataBase (binary phase-function file) and generator.
	void writeLuxCoreMaterial(	const phsdata::DiscreteBSDF & d,
								PhaseGeneralParametersDTO & genDTO) const;

private:
	void computePhaseExternForBand(	const unsigned int& spectralBandIndex,
									shared_ptr<DirectionnalPhaseExternDatabaseParameters> & phaseExternPropertyParameters,
									const std::vector<DirectionCartesienne> & listDirections,
									PhaseExternProperty & phaseExternProperty) const {
		phaseExternProperty.thetaView = phaseExternPropertyParameters->getThetaView();
		phaseExternProperty.phiView = phaseExternPropertyParameters->getPhiView();
		phaseExternProperty.thetaIncident = phaseExternPropertyParameters->getThetaIncident();
		phaseExternProperty.phiIncident = phaseExternPropertyParameters->getPhiIncident();

		phaseExternProperty.reflectance = phaseExternPropertyParameters->getReflectanceForBand(spectralBandIndex);
	}

	void writePhaseExternProperties(const unsigned int& modelIndex,
								const std::string & modelName,
								const unsigned int& spectralBandIndex,
								PhaseKeyMappingGenerator & pkmg,
								std::ofstream & outCoeffSpecProperties) const {

		pkmg.setTypeFonctionDePhase(globalReferences::PHASE_EXTERN);
		pkmg.setNumeroFonctionDePhase(modelIndex);
		pkmg.setBandeSpectrale(spectralBandIndex);

		outCoeffSpecProperties << pkmg.clePhaseUtilisee() << "1" << endl;
		outCoeffSpecProperties << pkmg.cleNomFichier() << modelName << endl;
	}


	void computePhaseExternPerDirections(	const unsigned int & spectralBandIndex,
											vector<PhaseExternProperty> & peProperties,
											PhaseKeyMappingGenerator & pkmg,
											std::ofstream & outCoeffSpecProperties,
											PhaseGeneralParametersDTO & genDTO,
											const std::vector<DirectionCartesienne> & listDirections) const {

		// Le vector PhaseExternProperty contient, pour la bande courante toute les directions computees et interpolees par bandes.
//		vector<PhaseExternProperty> temp;
//		temp.resize(peProperties.size());
//		temp = peProperties;

		for (unsigned int numDirIncident = 0; numDirIncident < listDirections.size(); numDirIncident++) {
			for (unsigned int numDirVisee = 0; numDirVisee < listDirections.size(); numDirVisee++) {

				// Index des la direction de visee la plus proche ...
				NearestDirections nearDirVisee;
				for (unsigned int numPhaseExternDirection = 0; numPhaseExternDirection < peProperties.size(); ++numPhaseExternDirection) {
					DirectionCartesienne dartDirVis = listDirections[numDirVisee];
					checkIfIsNearDirection(listDirections[numDirVisee], numPhaseExternDirection, nearDirVisee, peProperties[numPhaseExternDirection]);
				}
				nearDirVisee.cleanDoubles();

				// Interpolated on the numDirInc and numDirVisee
				PhaseExternProperty interpolatedProperty;
				computeInterpolationDirectionIncident(listDirections, numDirIncident, numDirVisee, peProperties, nearDirVisee, interpolatedProperty);


				// Write in binary file
				writeInterpolatedPhaseFunctionToBinaryFile(interpolatedProperty, numDirVisee, numDirIncident);
			}
		}
	}

	void computeInterpolationDirectionIncident(	const std::vector<DirectionCartesienne> & listDirections,
												const int numDirIncident,
												const int numDirVisee,
												vector<PhaseExternProperty> & peProperties,
												NearestDirections & nearDirVisee,
												PhaseExternProperty & interpolatedPhaseExtern) const
	{
		// Conteneur temporaire pour ne pas modifier les donnees de la base externe -> detruit pour chaque dir Inc differente
//		vector<PhaseExternProperty> temp,
//		temp = peProperties;

		// Pour chaque direction voisine de le direction de visee
		for (unsigned int nDirIndex = 0; nDirIndex < nearDirVisee.getNearestCorners().size(); ++nDirIndex) {

			int nearestDirIndex = nearDirVisee.getNearestCorners()[nDirIndex].first;

			// Creates local container with the incident directions for the selected viewing direction
			vector<PhaseExternProperty> incidentProperties;
			PhaseExternProperty cornerProperty = peProperties[nearestDirIndex];
			getVectorOfAllIncidentDirectionForView(cornerProperty, peProperties, incidentProperties);

			// Pour chaque direction de visee recupere la direction incident la plus proche ....
			NearestDirections nearDirIncident;
			for (unsigned int numPropIncident = 0; numPropIncident < incidentProperties.size(); ++numPropIncident) {
				checkIfIsNearDirectionIncident(listDirections[numDirIncident], numPropIncident, nearDirIncident, incidentProperties[numPropIncident]);
			}
			nearDirIncident.cleanDoubles();

//			temp[nearestDirIndex].thetaView = cornerProperty.thetaView;
//			temp[nearestDirIndex].phiView = cornerProperty.phiView;
//
//			// Reconvert to degreeees
//			temp[nearestDirIndex].thetaView /= M_PI / 180.0;
//			temp[nearestDirIndex].phiView /= M_PI / 180.0;

			// Interpolation des directions incidentes du corner d'incidence et stockage dans le container temporaire
			interpoleDartDirectionWithNearDirectionIncident(listDirections[numDirIncident], incidentProperties, nearDirIncident, peProperties[nearestDirIndex]);
		}

		// Interpolation du corner de visee selon les directions de visee
		interpolatedPhaseExtern.thetaView = listDirections[numDirVisee].getCenterTheta();
		interpolatedPhaseExtern.phiView = listDirections[numDirVisee].getCenterPhi();
		interpolatedPhaseExtern.thetaIncident = listDirections[numDirIncident].getCenterTheta();
		interpolatedPhaseExtern.phiIncident = listDirections[numDirIncident].getCenterPhi();


		interpoleDartDirectionWithNearDirection(listDirections[numDirVisee], peProperties, nearDirVisee, interpolatedPhaseExtern);

		// Reconvert to degreeees
		interpolatedPhaseExtern.thetaView /= M_PI / 180.0;
		interpolatedPhaseExtern.phiView /= M_PI / 180.0;
		interpolatedPhaseExtern.thetaIncident /= M_PI / 180.0;
		interpolatedPhaseExtern.phiIncident /= M_PI / 180.0;

//		cout << "("<<interpolatedPhaseExtern.thetaIncident<<", "<<interpolatedPhaseExtern.phiIncident<<") ("<<interpolatedPhaseExtern.thetaView<<", "
//				<<interpolatedPhaseExtern.phiView<<") -> "<< interpolatedPhaseExtern.reflectance <<endl;

	}

	void interpoleDartDirectionWithNearDirection(	const DirectionCartesienne & dartDirection,
													vector<PhaseExternProperty> & peProperties,
													const NearestDirections & nearDirVisee,
													PhaseExternProperty & interpolatedDirection) const {

		double distanceSum = 0., distance = 0., invertedDistance = 0.;
		double interpalotedReflectance = 0.;
		Vecteur<double> carthesianDirection;
		PhaseExternProperty pe;
		for (unsigned int nDirIndex = 0; nDirIndex < nearDirVisee.getNearestCorners().size(); ++nDirIndex) {
			pe = peProperties[nearDirVisee.getNearestCorners()[nDirIndex].first];

			carthesianDirection = convertiDirectionPolaireEnCartesien(pe.thetaView, pe.phiView, DirectionRoot::DEGREES);
			distance = carthesianDirection.scalaire(dartDirection.getDirectionCartesienne());

			if (distance < 0.999999) {
				invertedDistance = 1. / (1. - distance);
				interpalotedReflectance += pe.interpolatedReflectance * invertedDistance; //pe.reflectance * invertedDistance;
				distanceSum += invertedDistance;
			} else {
				interpalotedReflectance = pe.interpolatedReflectance; //pe.reflectance;
				distanceSum = 1.;
				break;
			}
		}
		interpolatedDirection.reflectance = interpalotedReflectance / distanceSum;
	}


	void interpoleDartDirectionWithNearDirectionIncident(	const DirectionCartesienne & dartDirection,
															const vector<PhaseExternProperty> & peProperties,
															const NearestDirections & nearDirVisee,
															PhaseExternProperty & interpolatedDirection) const {
		double distanceSum = 0., distance = 0., invertedDistance = 0.;
		double interpalotedReflectance = 0.;

		Vecteur<double> carthesianDirection;
		PhaseExternProperty pe;
		for (unsigned int nDirIndex = 0; nDirIndex < nearDirVisee.getNearestCorners().size(); ++nDirIndex) {
			pe = peProperties[nearDirVisee.getNearestCorners()[nDirIndex].first];

			carthesianDirection = convertiDirectionPolaireEnCartesien(pe.thetaIncident, pe.phiIncident, DirectionRoot::DEGREES);
			distance = carthesianDirection.scalaire(dartDirection.getDirectionCartesienne());

			if (distance < 0.999999) {
				invertedDistance = 1. / (1. - distance);
				interpalotedReflectance += pe.reflectance * invertedDistance;
				distanceSum += invertedDistance;
			} else {
				interpalotedReflectance = pe.reflectance;
				distanceSum = 1.;
				break;
			}
		}

		interpolatedDirection.interpolatedReflectance = interpalotedReflectance / distanceSum;
	}


	/**
	 * Recupere toutes les directions incidentes correspondant a une direction de visee
	 */
	void getVectorOfAllIncidentDirectionForView(const PhaseExternProperty & dirView,
												const vector<PhaseExternProperty> & peProperties,
												vector<PhaseExternProperty> & incidentDirectionsForView) const {
		for (vector<PhaseExternProperty>::const_iterator it = peProperties.begin(); it < peProperties.end(); ++it) {
			if (it->thetaView ==dirView.thetaView && it->phiView == dirView.phiView) {
				incidentDirectionsForView.push_back(*it);
//				cout<<it->thetaIncident<<", "<<it->phiIncident<<") , ("<<it->thetaView<<", "<<it->phiView<<") -> "<<it->reflectance<<endl;
			}
		}
	}

	/**
		 * Recupere toutes les directions visee correspondant a une direction incidente
		 */
	void getVectorOfAllViewDirectionForIncident(const PhaseExternProperty & dirIncident,
												const vector<PhaseExternProperty> & peProperties,
												vector<PhaseExternProperty> & ViewDirectionsForIncident) const {
		for (vector<PhaseExternProperty>::const_iterator it = peProperties.begin(); it < peProperties.end(); ++it) {
			if (it->thetaIncident == dirIncident.thetaIncident && it->phiIncident == dirIncident.phiIncident) {
				ViewDirectionsForIncident.push_back(*it);
			}
		}
	}

	void checkIfIsNearDirection(const DirectionCartesienne & dartDirection,
								const unsigned int & numPhaseExternDirection,
								NearestDirections & nearDirVisee,
								const PhaseExternProperty & pe) const {
		double dotProduct;
		Vecteur<double> carthesianDirection;
		NearestDirections::Corners corner;

		// Compute carthesian direction
		carthesianDirection = convertiDirectionPolaireEnCartesien(pe.thetaView, pe.phiView, DirectionRoot::DEGREES);
		dotProduct = carthesianDirection.scalaire(dartDirection.getDirectionCartesienne());
		if (dotProduct > 0.) {
			corner = getCorner(dartDirection.getCenterTheta(), dartDirection.getCenterPhi(), pe.thetaView, pe.phiView);
			nearDirVisee.setNearestCorner(numPhaseExternDirection, dotProduct, corner);
		}
	}

	void checkIfIsNearDirectionIncident(const DirectionCartesienne & dartDirection,
										const unsigned int & numPhaseExternDirection,
										NearestDirections & nearDirIncident,
										const PhaseExternProperty & pe) const {
		double dotProduct;
		Vecteur<double> carthesianDirection;
		NearestDirections::Corners corner;

		// Compute carthesian direction
		carthesianDirection = convertiDirectionPolaireEnCartesien(pe.thetaIncident, pe.phiIncident, DirectionRoot::DEGREES);
		dotProduct = carthesianDirection.scalaire(dartDirection.getDirectionCartesienne());
		if (dotProduct > 0.) {
			corner = getCorner(dartDirection.getCenterTheta(), dartDirection.getCenterPhi(), pe.thetaIncident, pe.phiIncident);
			nearDirIncident.setNearestCorner(numPhaseExternDirection, dotProduct, corner);
		}
	}

	NearestDirections::Corners getCorner(	const double & dartTheta,
											const double & dartPhi,
											const double & dbTheta,
											const double & dbPhi) const {
		if (dartTheta < dbTheta) {
			// Upper
			if ((dbPhi - dartPhi >= 0. && dbPhi - dartPhi <= M_PI05) || (dbPhi - dartPhi + M_PI2 >= 0. && dbPhi - dartPhi + M_PI2 <= M_PI05)) {
				// Left
				return NearestDirections::UPPER_LEFT;
			} else {
				// Right
				return NearestDirections::UPPER_RIGHT;
			}
		} else {
			// Lower
			if ((dbPhi - dartPhi >= 0. && dbPhi - dartPhi <= M_PI05) || (dbPhi - dartPhi + M_PI2 >= 0. && dbPhi - dartPhi + M_PI2 <= M_PI05)) {
				// Left
				return NearestDirections::LOWER_LEFT;
			} else {
				// Right
				return NearestDirections::LOWER_RIGHT;
			}
		}
	}

	Vecteur<double> convertiDirectionPolaireEnCartesien(const double & theta,
														const double & phi,
														const DirectionRoot::Unit & unit) const {
		Vecteur<double> resultsVector;
		static const double epsilon = 1E-10;
		double thetaInRad = theta;
		double phiInRad = phi;
		if (unit == DirectionRoot::DEGREES) {
			thetaInRad *= M_PI / 180.;
			phiInRad *= M_PI / 180.;
		}
		double cosTheta = cos(thetaInRad);
		if (fabs(cosTheta) < epsilon) {
			cosTheta = 0.0;
		}
		double cosPhi = cos(phiInRad);
		if (fabs(cosPhi) < epsilon) {
			cosPhi = 0.0;
		}

		double sinTheta = sin(thetaInRad);
		if (fabs(sinTheta) < epsilon) {
			sinTheta = 0.0;
		}

		double sinPhi = sin(phiInRad);
		if (fabs(sinPhi) < epsilon) {
			sinPhi = 0.0;
		}

		resultsVector.setZ(cosTheta);
		resultsVector.setX(sinTheta * cosPhi);
		resultsVector.setY(sinTheta * sinPhi);
		resultsVector.normaliserEgal();
		return resultsVector;
	}

	mutable ofstream _binaryFile;
	//ofstream _textFile; //debug

	/**
	 * 1 fichier par bande, comme les autres ?
	 */
	void openPhaseExternWriteFileForModel(	const unsigned int & modelIndex,
											const string & modelName) const {
		ostringstream filePathStream;
		filePathStream << DartEnv::getCurrentSimulationOutputPath() << PhaseUtils::getOutputFolder() << DartEnv::getSeparator() << modelIndex << ".phaseExternData.bin";
		_binaryFile.open(filePathStream.str(), ios::out | ios::binary);
		//_textFile.open(filePathStream.str()+".txt", ios::out);
	}

	/* BODY
	 * DE LA FORME
	 * [directionViewIndex, directionIncidentIndex, reflectance, directTransmittacne, diffuseTransmittance]
	 * CE QUI DONNE
	 * [unsigned int, unsigned int, double, double, double]
	 */
	void writeInterpolatedPhaseFunctionToBinaryFile(	const PhaseExternProperty & phaseProperty,
																				const unsigned int & numDirVisee,
																				const unsigned int & numDirIncident) const {
		_binaryFile.write((char *) &phaseProperty.reflectance, sizeof(double));
		//_textFile << phaseProperty.reflectance << " ";
	}

	/**
	 * HEADER:
	 * [modelIndex, spectralBandIndex, numberOfValues]
	 * [unsigned int, unsigned int, unsigned int]
	 */
	void writeBinaryHeader(const unsigned int & numberOfValues) const {
		_binaryFile.write((char *) &numberOfValues, sizeof(unsigned int));
		//_textFile << numberOfValues << " ";
	}

	void computePhaseExtern_MultiDirection(	OpticalPropertyParameters & phaseExternPropertyParameters,
											PhaseKeyMappingGenerator & pkmg,
											std::ofstream & outCoeffSpecProperties,
											PhaseGeneralParametersDTO & genDTO,
											const std::vector<DirectionCartesienne> & listDirections) const {

		shared_ptr<PhaseExternDatabaseParameters> phaseExternDatabaseParameters = ::static_pointer_cast<PhaseExternDatabaseParameters>(phaseExternPropertyParameters.getDatabaseParameters(PHASE_EXTERN_DATABASE));
		vector<shared_ptr<PhaseDatabaseParameters> > directionnalPhaseExtern = phaseExternDatabaseParameters->getDirectionnalPhaseExternDatabaseParametersVector();

		openPhaseExternWriteFileForModel(phaseExternPropertyParameters.getModelIndex(), phaseExternDatabaseParameters->getDatabaseModel());
		writeBinaryHeader(genDTO.numberOfSpectralBands * listDirections.size() * listDirections.size());
		cout << " -> Phase extern: interpolate directions (view + incident) for " << phaseExternDatabaseParameters->getDatabaseModel() << endl;

		for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
			vector<PhaseExternProperty> listOfComputedPhaseExternProperty;
			for (unsigned int i = 0; i < directionnalPhaseExtern.size(); ++i) {
				PhaseExternProperty phaseExternProperty;
				shared_ptr<DirectionnalPhaseExternDatabaseParameters> directionnalPhaseExternDatabaseParameters = ::static_pointer_cast<DirectionnalPhaseExternDatabaseParameters>(directionnalPhaseExtern.at(i));
				computePhaseExternForBand(spectralBandIndex, directionnalPhaseExternDatabaseParameters, listDirections, phaseExternProperty);
				listOfComputedPhaseExternProperty.push_back(phaseExternProperty);

			}

			writePhaseExternProperties(phaseExternPropertyParameters.getModelIndex(), phaseExternPropertyParameters.getModelName(), spectralBandIndex,
				pkmg, outCoeffSpecProperties);
			// Potentiellement un traitement par les directions ici :)


			computePhaseExternPerDirections(spectralBandIndex, listOfComputedPhaseExternProperty, pkmg, outCoeffSpecProperties, genDTO,
				listDirections);
		}
		_binaryFile.close();
		//_textFile.close();
		//cerr << "[END] Interpolate phase extern on directions (view + incident)"<<endl;

	}
};
} // namespace phase

#endif /* INCLUDE_MATERIALS_DISCRETEBSDFMATERIALWRITER_HPP_ */
