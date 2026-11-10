/*
 * PhaseIODirections.cpp
 *
 *  Created on: 25 sept. 2026
 *      Author: lauretn
 */

#include <string>
#include <stdexcept>
#include <sstream>

#include "PhaseDAO.hpp"
#include "inputs/CSVReader.hpp"

void loadDirectionSetCSV(phsdata::DirectionSet & ds, const std::string & fileName, const std::string & name, unsigned int index) {
	CSVStreamReader reader(DartEnv::getFilePathForRead(OUTPUT_DIR, fileName), ';');

    std::vector<std::string> row;
	DirectionCartesienne dir;
    while (reader.nextRow(row)) {
    	dir.setBeginTheta(reader.hasColumn("thetaBegin") ? deg2rad(std::stod(row[reader.getColumnIndex("thetaBegin")])): 0.);
    	dir.setCenterTheta(reader.hasColumn("theta") ? deg2rad(std::stod(row[reader.getColumnIndex("theta")])): 0.);
    	dir.setEndTheta(reader.hasColumn("thetaEnd") ? deg2rad(std::stod(row[reader.getColumnIndex("thetaEnd")])): 0.);

    	dir.setBeginPhi(reader.hasColumn("phiBegin") ? deg2rad(std::stod(row[reader.getColumnIndex("phiBegin")])): 0.);
    	dir.setCenterPhi(reader.hasColumn("phi") ? deg2rad(std::stod(row[reader.getColumnIndex("phi")])): 0.);
    	dir.setEndPhi(reader.hasColumn("phiEnd") ? deg2rad(std::stod(row[reader.getColumnIndex("phiEnd")])): 0.);

    	dir.setSolidAngle(reader.hasColumn("solidAngle") ? std::stod(row[reader.getColumnIndex("solidAngle")]): 0.);

    	dir.setCosThetaDOmega(reader.hasColumn("cosThetaDOmega") ? std::stod(row[reader.getColumnIndex("cosThetaDOmega")]): 0.);

    	dir.setProperty(reader.hasColumn("prop") ? DirectionRoot::propertyOfString(row[reader.getColumnIndex("prop")]): DirectionRoot::VIRTUAL);

		dir.calculerCoordonneesCartesiennes();

    	ds._directions.push_back(dir);
    }
}

bool PhaseDAO::readAndStoreDirections(PhaseGeneralParametersDTO & genDTO) {
	FichierProperties directionProperty(DartEnv::getFilePathForRead(OUTPUT_DIR, SIMULATIONPROPERTIESFILE));

	const std::string dirSetHead = "direction.dirset.";
	const std::string dirSetFileTail = ".file";
	const std::string dirSetIndexTail = ".index";
	for (const auto & key_value: directionProperty.getPropertyList()) {
		if (boost::algorithm::starts_with(key_value.first, dirSetHead) && boost::algorithm::ends_with(key_value.first, dirSetFileTail)) {
			const std::string dirSetName = key_value.first.substr(dirSetHead.size(), key_value.first.size() - dirSetHead.size() - dirSetFileTail.size());
			const unsigned int dirSetIndex = directionProperty.valeurProperties<unsigned int>(dirSetHead + dirSetName + dirSetIndexTail);
			if (genDTO._directionSet.size() <= dirSetIndex)
				genDTO._directionSet.resize(dirSetIndex + 1);
			loadDirectionSetCSV(genDTO._directionSet[dirSetIndex], key_value.second, dirSetName, dirSetIndex);
		}

		// Todo: Read sun
	}

	return true;
}
