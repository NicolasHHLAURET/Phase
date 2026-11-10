/*
 * AtmosphericDatabaseStructure.cpp
 *
 *  Created on: 4 mars 2016
 *      Author: guilleuxj
 */

#include "../../../include/database/atmosphereModel/AtmosphericDatabaseStructure.hpp"

std::map<AtmosphericDatabaseStructure::AtmosphericType, AtmosphericTableType> AtmosphericDatabaseStructure::_atmosphericDatabaseStructure = std::map<AtmosphericDatabaseStructure::AtmosphericType, AtmosphericTableType>();
AtmosphericDatabaseStructure AtmosphericDatabaseStructure::_singleton = AtmosphericDatabaseStructure();
