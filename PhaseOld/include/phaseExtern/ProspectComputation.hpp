/*
 * ProspectComputation.hpp
 *
 *  Created on: 19 mai 2022
 *      Author: guilleuxj
 */

#include <boost/algorithm/string/replace.hpp>
using namespace std;

#ifndef INCLUDE_PHASEEXTERN_PROSPECTCOMPUTATION_HPP_
#define INCLUDE_PHASEEXTERN_PROSPECTCOMPUTATION_HPP_

class ProspectComputation {
	private:
		std::string _preparationFile;

	public:
		ProspectComputation() : _preparationFile(""){
		}

		ProspectComputation(const std::string & preparationFile) :
			_preparationFile(preparationFile) {
		}

		~ProspectComputation() {}

		void prepareProspect(const std::string dbName, const ProspectParameters & prospectParameters)
		{
			std::ofstream prospectPreparationCsv;
			prospectPreparationCsv.open(_preparationFile, std::ios_base::app);
			prospectPreparationCsv << DartEnv::getFilePath(INPUT_DIR, prospectParameters.inputProspectFile) << ",";
			prospectPreparationCsv << DartEnv::getOutputTempDatabasePath(true) << dbName << ",";
			prospectPreparationCsv << prospectParameters.chlorophyll << ",";
			prospectPreparationCsv << prospectParameters.carotenoid << ",";
			prospectPreparationCsv << prospectParameters.brownPigment << ",";
			prospectPreparationCsv << prospectParameters.waterThickness << ",";
			if (prospectParameters.useNewCm) {
				prospectPreparationCsv << 0 << ",";
			} else {
				prospectPreparationCsv << prospectParameters.leafMassPerArea << ",";
			}
			prospectPreparationCsv << prospectParameters.structureCoeff << ",";
			prospectPreparationCsv << 0 << ",";
			prospectPreparationCsv << prospectParameters.v2z << ",";
			prospectPreparationCsv << prospectParameters.anthocyanin << ",";
			prospectPreparationCsv << prospectParameters.Cbc << ",";
			prospectPreparationCsv << prospectParameters.Cp << ",";
			prospectPreparationCsv << "-prospectDatabase" << "\n";

		    prospectPreparationCsv.close();
		}

		void runAllProspects()
		{
			std::string pythonScript = "Batch_Fluspect_B_CX_PRO.py";
			ostringstream commandLineRoot;
			commandLineRoot << DartEnv::getPythonExecutable() << " ";
			commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "Fluspect" << pathSeparator << "src" << pathSeparator << pythonScript << " ";
			commandLineRoot << _preparationFile << " ";
			std::cout << "    Run python cmd: " << commandLineRoot.str() << endl;
			std::system(commandLineRoot.str().c_str());

			// All prospect are terminated, i can delete the csv file, useless now
			if (boost::filesystem::exists(_preparationFile)) {
				boost::filesystem::remove(_preparationFile);
			}
		}

		void runProspectDatabaseGeneration(const string dbName, const ProspectParameters & prospectParameters) {
			ostringstream commandLineRoot;
			commandLineRoot << DartEnv::getPythonExecutable() << " ";
			commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "Fluspect" << pathSeparator << "src" << pathSeparator << prospectParameters.pythonScript << " ";

			commandLineRoot << DartEnv::getFilePath(INPUT_DIR, prospectParameters.inputProspectFile) << " ";
			commandLineRoot << DartEnv::getLocalTempDatabasePath() << dbName << " ";

			commandLineRoot << "\" " << prospectParameters.chlorophyll << "\" ";
			commandLineRoot << "\" " << prospectParameters.carotenoid << "\" ";
			commandLineRoot << "\" " << prospectParameters.brownPigment << "\" ";
			commandLineRoot << "\" " << prospectParameters.waterThickness << "\" ";
			if (prospectParameters.useNewCm) {
				commandLineRoot << 0 << " ";
			} else {
				commandLineRoot << "\" " << prospectParameters.leafMassPerArea << "\" ";
			}
			commandLineRoot << "\" " << prospectParameters.structureCoeff << "\" ";
			commandLineRoot << 0 << " ";
			commandLineRoot << "\" " << prospectParameters.v2z << "\" ";
			commandLineRoot << "\" " << prospectParameters.anthocyanin << "\" ";
			commandLineRoot << "\" " << prospectParameters.Cbc << "\" ";
			commandLineRoot << "\" " << prospectParameters.Cp << "\" ";

			commandLineRoot << "-prospectDatabase" << " ";
			std::cout << "    Run python cmd: " << commandLineRoot.str() << endl;
			std::system(commandLineRoot.str().c_str());
		}

		void runProspectOutputFileGeneration(const ProspectParameters & prospectParameters) {
			ostringstream commandLineRoot;
			commandLineRoot << DartEnv::getPythonExecutable() << " ";
			commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "Fluspect" << pathSeparator << "src" << pathSeparator << prospectParameters.pythonScript << " ";

			commandLineRoot << DartEnv::getFilePath(INPUT_DIR, prospectParameters.inputProspectFile) << " ";
			commandLineRoot << DartEnv::getCurrentSimulationOutputPath() << " ";

			commandLineRoot << "\" " << prospectParameters.chlorophyll << "\" ";
			commandLineRoot << "\" " << prospectParameters.carotenoid << "\" ";
			commandLineRoot << "\" " << prospectParameters.brownPigment << "\" ";
			commandLineRoot << "\" " << prospectParameters.waterThickness << "\" ";
			if (prospectParameters.useNewCm) {
				commandLineRoot << 0 << " ";
			} else {
				commandLineRoot << "\" " << prospectParameters.leafMassPerArea << "\" ";
			}
			commandLineRoot << "\" " << prospectParameters.structureCoeff << "\" ";
			commandLineRoot << 0 << " ";
			commandLineRoot << "\" " << prospectParameters.v2z << "\" ";
			commandLineRoot << "\" " << prospectParameters.anthocyanin << "\" ";
			commandLineRoot << "\" " << prospectParameters.Cbc << "\" ";
			commandLineRoot << "\" " << prospectParameters.Cp << "\" ";
			commandLineRoot << "-prospect" << " ";
			std::cout << "    Run python cmd: " << commandLineRoot.str() << endl;
			std::system(commandLineRoot.str().c_str());
		}
};

#endif /* INCLUDE_PHASEEXTERN_PROSPECTCOMPUTATION_HPP_ */

